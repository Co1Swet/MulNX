#include "MQTTClient.hpp"
#include <WS2tcpip.h> // 添加到文件顶部的include区域

#pragma comment(lib, "ws2_32.lib")

#include "../../../Core/Core.hpp"
#include "../../../Systems/Systems.hpp"

using namespace MulNX::Extensions::MQTT;

bool MQTTClient::MQTTPublish(const std::string& topic, const std::string& message, uint8_t publish_flags) {
	std::unique_lock lock(this->MyThreadMutex);
    this->err = mqtt_publish(&this->client, topic.c_str(), message.c_str(), message.length(), publish_flags);
    if (this->err != MQTT_OK) {
        this->IDebugger->AddError("发布消息失败: " + this->GetCurrentError());
        return false;
    }
    this->IDebugger->AddInfo("已发布消息到主题 " + topic + ": " + message);
    return true;
}
bool MQTTClient::MQTTSubscribe(const std::string& topic, std::function<void(const std::string& message)> callback, int qos) {
    std::unique_lock lock(this->MyThreadMutex);
    // 订阅主题
    this->err = mqtt_subscribe(&this->client, topic.c_str(), qos);
    if (this->err != MQTT_OK) {
        this->IDebugger->AddError("订阅失败: " + this->GetCurrentError());
        return false;
    }
    this->IDebugger->AddInfo("已订阅主题: " + topic);
    // 存储回调函数
    this->TopicMessageMap[topic] = callback;
    return true;
}


void MQTTClient::CStylePublishCallback(void** state, struct mqtt_response_publish* published) {
    MQTTClient* This = static_cast<MQTTClient*>(*state);
	This->PublishCallback(published);
}
void MQTTClient::PublishCallback(struct mqtt_response_publish* published) {
    std::unique_lock lock(this->MyThreadMutex);
	// 转换主题和消息为std::string
    std::string topic(static_cast<const char*>(published->topic_name), published->topic_name_size);
    std::string message(static_cast<const char*>(published->application_message), published->application_message_size);
    
	// 记录收到的消息
    this->IDebugger->AddInfo("收到消息 - 主题: " + topic + ", 消息: " + message);

	// 查找主题对应的回调函数
	auto it = this->TopicMessageMap.find(topic);
    if (it != this->TopicMessageMap.end()) {
        // 调用回调函数
        it->second(message);
    }
    else {
        this->IDebugger->AddWarning("未找到主题的回调函数: " + topic);
	}
}

std::string MQTTClient::GetCurrentError()const {
    return std::to_string(*mqtt_error_str(this->err));
}

std::string MQTTClient::GetServerIP()const {
    return this->ServerIP;
}
std::string MQTTClient::GetServerPort()const {
    return std::to_string(this->ServerPort);
}
void MQTTClient::SetServerIP(const std::string& ip) {
    std::unique_lock lock(this->MyThreadMutex);
    this->ServerIP = ip;
}
void MQTTClient::SetServerPort(uint16_t port) {
    std::unique_lock lock(this->MyThreadMutex);
    this->ServerPort = port;
}

bool MQTTClient::Init() {
	std::unique_lock lock(this->MyThreadMutex);
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
    this->server_addr.sin_port = htons(this->ServerPort); // MQTT默认端口

    // 使用本地地址
    int ptonResult = inet_pton(AF_INET, this->ServerIP.c_str(), &this->server_addr.sin_addr);
    if (ptonResult != 1) {
        this->IDebugger->AddError("地址解析失败");
        closesocket(this->sockfd);
        WSACleanup();
        return -1;
    }

    this->IDebugger->AddInfo("尝试连接到本地MQTT服务器 (" +
        this->GetServerIP() + " :" + this->GetServerPort() + ")...");
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
        this->sendbuf, sizeof(this->sendbuf), this->recvbuf, sizeof(this->recvbuf), this->CStylePublishCallback);

    // 绑定this指针，用于跳转到类函数
    this->client.publish_response_callback_state = this;

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

	lock.unlock();

    // 订阅主题
    this->MQTTSubscribe("test/topic", [this](const std::string& message) {
        this->IDebugger->AddInfo("回调处理收到的消息: " + message);
        });

    this->IDebugger->AddInfo("客户端运行中...");

    return true;
}

void MQTTClient::VirtualMain() {
    static int counter = 0;
    // 处理MQTT通信
    this->err = mqtt_sync(&this->client);
    if (this->err != MQTT_OK) {
        this->IDebugger->AddError("MQTT同步错误: " + this->GetCurrentError());
    }

    // 定期发布消息
    if (++counter % 200 == 0) {
        std::string message = "本地测试消息 #" + std::to_string(counter / 200);
		this->MQTTPublish("test/topic", message, MQTT_PUBLISH_QOS_0);
    }
}