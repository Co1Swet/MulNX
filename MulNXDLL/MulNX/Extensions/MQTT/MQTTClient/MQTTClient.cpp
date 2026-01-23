#include "MQTTClient.hpp"
#include <WS2tcpip.h> // 添加到文件顶部的include区域

#pragma comment(lib, "ws2_32.lib")

#include "../../../Core/Core.hpp"
#include "../../../Systems/Systems.hpp"

using namespace MulNX::Extensions::MQTT;

// 发布消息回调函数
void publish_callback(void** state, struct mqtt_response_publish* published) {
    /*std::cout << "收到消息 - 主题: ";
    std::cout.write(static_cast<const char*>(published->topic_name), published->topic_name_size);
    std::cout << ", 内容: ";
    std::cout.write(static_cast<const char*>(published->application_message), published->application_message_size);
    std::cout << std::endl;*/
    MulNX::Core::Core::pCore()->IDebugger().AddInfo(
        "收到消息 - 主题: " +
        std::string(static_cast<const char*>(published->topic_name), published->topic_name_size) +
        ", 内容: " +
        std::string(static_cast<const char*>(published->application_message), published->application_message_size)
    );
}

bool MQTTClient::Init() {
	if (this->Inited) {
		return true;
	}
    this->IDebugger->AddInfo("尝试创建MQTT-C 本地客户端");

    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0) {
        this->IDebugger->AddError("Winsock初始化失败");
        return -1;
    }

    // 创建socket - 连接到本地服务器
    this->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->sockfd == INVALID_SOCKET) {
        this->IDebugger->AddError("创建socket失败");
        return -1;
    }

    // 连接本地MQTT服务器
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_port = htons(1883); // MQTT默认端口

    // 使用本地地址
    int ptonResult = InetPtonA(AF_INET, "127.0.0.1", &this->server_addr.sin_addr);
    if (ptonResult != 1) {
        this->IDebugger->AddError("地址解析失败");
        closesocket(this->sockfd);
        WSACleanup();
        return -1;
    }

    this->IDebugger->AddInfo("尝试连接到本地MQTT服务器 (127.0.0.1:1883)...");
    if (connect(this->sockfd, reinterpret_cast<sockaddr*>(&this->server_addr), sizeof(this->server_addr)) == SOCKET_ERROR) {
        this->IDebugger->AddError("连接本地MQTT服务器失败: " + std::to_string(WSAGetLastError()));
        this->IDebugger->AddError("请确保本地MQTT服务器已启动");
        closesocket(this->sockfd);
        WSACleanup();
        return -1;
    }

    // 设置socket为非阻塞模式
    this->mode = 1;
    ioctlsocket(this->sockfd, FIONBIO, &this->mode);

    this->IDebugger->AddSucc("成功连接到本地MQTT服务器");

    mqtt_init(&this->client, this->sockfd,
        this->sendbuf, sizeof(this->sendbuf), this->recvbuf, sizeof(this->recvbuf), publish_callback);

    // 连接到MQTT代理
    this->err = mqtt_connect(&this->client, "LocalClient",
        nullptr, nullptr, 0,  // 无遗嘱消息
        nullptr, nullptr,     // 无用户名密码
        MQTT_CONNECT_CLEAN_SESSION, 400);

    if (this->err != MQTT_OK) {
        this->IDebugger->AddError("MQTT连接失败: " + this->GetCurrentError());
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    // 订阅主题
    this->err = mqtt_subscribe(&this->client, "test/topic", 0);
    if (this->err != MQTT_OK) {
        this->IDebugger->AddError("订阅失败: " + this->GetCurrentError());
    }
    else {
        this->IDebugger->AddInfo("已订阅主题: test/topic");
    }

    this->IDebugger->AddInfo("客户端运行中...");

    return true;
}

void MQTTClient::VirtualMain() {
    static int counter = 0;
    // 处理MQTT通信
    err = mqtt_sync(&client);
    if (err != MQTT_OK) {
        this->IDebugger->AddError("MQTT同步错误: " + *mqtt_error_str(err));
    }

    // 定期发布消息
    if (++counter % 200 == 0) { // 每2秒发布一次
        std::string message = "本地测试消息 #" + std::to_string(counter / 200);
        mqtt_publish(&client, "test/topic", message.c_str(), message.length(), MQTT_PUBLISH_QOS_0);
        this->IDebugger->AddInfo("已发布: " + message);
    }

    //Sleep(1); // 100ms间隔
}

std::string MQTTClient::GetCurrentError()const {
    return std::to_string(*mqtt_error_str(this->err));
}