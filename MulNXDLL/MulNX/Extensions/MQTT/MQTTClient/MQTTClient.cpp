#include "MQTTClient.hpp"
#include <WS2tcpip.h> // 添加到文件顶部的include区域

#pragma comment(lib, "ws2_32.lib")

#include "../../../Core/Core.hpp"
#include "../../../Systems/Systems.hpp"
#include "../../../../ThirdParty/All_ImGui.hpp"
#include "../../../../ThirdParty/protobuf/messages_down.pb.h"
#include "../../../../ThirdParty/protobuf/messages_up.pb.h"

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
void MQTTClient::SetClientName(const std::string& name) {
    std::unique_lock lock(this->MyThreadMutex);
    this->ClientName = name;
}

// UI绘制函数
static void MyDraw(MulNXSingleUIContext* This) {
	ImGui::Text("MQTT Client 配置界面");
	if (ImGui::Button("连接到MQTT服务器")) {
        auto Msg = This->CreateMsg(0x101);
		This->SendToOwner(std::move(Msg));
    }
    static std::string serverIP;
	ImGui::InputText("服务器IP", &serverIP);
    ImGui::SameLine();
    if (ImGui::Button("设置IP")) {
        auto Msg = This->CreateMsg(0x102);
        Msg.Handle = This->CreateStringHandle(std::move(serverIP));
        This->SendToOwner(std::move(Msg));
    }
	static int serverPort = 1883;
    ImGui::InputInt("服务器端口", &serverPort);
    ImGui::SameLine();
    if (ImGui::Button("设置端口")) {
        auto Msg = This->CreateMsg(0x103);
        Msg.ParamInt = serverPort;
        This->SendToOwner(std::move(Msg));
	}
}

bool MQTTClient::Init() {
	std::unique_lock lock(this->MyThreadMutex);
    this->MainMsgChannel = this->ICreateAndGetMessageChannel();
    (*this->MainMsgChannel)
        .Subscribe(MulNX::MsgType::UISystem_UICommand);

    auto SingleContext = MulNXSingleUIContext::Create(this);
    auto* SContextPtr = SingleContext.get<MulNXSingleUIContext>();
    SContextPtr->name = "MQTTClientCfg";
    //SContextPtr->pBuffer = MulNX::Base::make_any_unique<MulNX::Base::TripleBuffer<DemoHelperPrivateData>>();
    SContextPtr->MyFunc = MyDraw;

    //this->hContext = this->Core->IHandleSystem().RegisteHandle(std::move(SingleContext));

    MulNX::Message Msg(MulNX::MsgType::UISystem_ModulePush);
    Msg.Handle = this->Core->IHandleSystem().RegisteHandle(std::move(SingleContext));
    this->IPublish(std::move(Msg));

    return true;
}

bool MQTTClient::CreateMQTTConnect() {
    std::unique_lock lock(this->MyThreadMutex);
    this->IDebugger->AddInfo("尝试创建MQTT-C 本地客户端");

    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0) {
        this->IDebugger->AddError("Winsock初始化失败");
        return false;
    }

    // 创建socket - 连接到本地服务器
    this->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->sockfd == INVALID_SOCKET) {
        this->IDebugger->AddError("创建socket失败");
        return false;
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
        return false;
    }

    this->IDebugger->AddInfo("尝试连接到本地MQTT服务器 (" +
        this->GetServerIP() + " :" + this->GetServerPort() + ")...");
    if (connect(this->sockfd, reinterpret_cast<sockaddr*>(&this->server_addr), sizeof(this->server_addr)) == SOCKET_ERROR) {
        this->IDebugger->AddError("连接本地MQTT服务器失败: " + std::to_string(WSAGetLastError()));
        this->IDebugger->AddError("请确保本地MQTT服务器已启动");
        closesocket(this->sockfd);
        WSACleanup();
        return false;
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
    this->err = mqtt_connect(&this->client, this->ClientName.c_str(),
        nullptr, nullptr, 0,  // 无遗嘱消息
        nullptr, nullptr,     // 无用户名密码
        MQTT_CONNECT_CLEAN_SESSION, 400);

    if (this->err != MQTT_OK) {
        this->IDebugger->AddError("MQTT连接失败: " + this->GetCurrentError());
        closesocket(sockfd);
        WSACleanup();
        return false;
    }

    lock.unlock();

    // 订阅主题
    this->MQTTSubscribe("GameStatus", [this](const std::string& message) {
		rm_client_down::GameStatus status;
        if (status.ParseFromString(message)) {
            this->IDebugger->AddInfo("游戏状态更新 - 当前回合: " + std::to_string(status.current_round()) +
                ", 总回合数: " + std::to_string(status.total_rounds()) +
                ", 红队得分: " + std::to_string(status.red_score()) +
                ", 蓝队得分: " + std::to_string(status.blue_score()));
        }
        });
	return true;
}

void MQTTClient::VirtualMain() {
    this->EntryProcessMsg();
    if (!this->Inited) {
        return;
    }
    static int counter = 0;
    // 处理MQTT通信
    this->err = mqtt_sync(&this->client);
    if (this->err != MQTT_OK) {
        this->IDebugger->AddError("MQTT同步错误: " + this->GetCurrentError());
    }
	rm_client_up::RemoteControl controlMsg;
	controlMsg.set_mouse_x(100);
	controlMsg.set_mouse_y(-100);
	controlMsg.set_mouse_z(0);
	controlMsg.set_left_button_down(true);
	controlMsg.set_right_button_down(false);
	controlMsg.set_keyboard_value(65); // 例如，按下'A'键
	controlMsg.set_mid_button_down(false);
	std::string serializedMsg;
	controlMsg.SerializeToString(&serializedMsg);
	this->MQTTPublish("RemoteControl", serializedMsg, MQTT_PUBLISH_QOS_0);
    // 定期发布消息
    if (++counter % 200 == 0) {
        std::string message = "本地测试消息 #" + std::to_string(counter / 200);
		//this->MQTTPublish("test/topic", message, MQTT_PUBLISH_QOS_0);
    }
}

template<typename Func>
auto WinExceptionWrapper(Func func) -> decltype(func()) {
    __try {
        return func();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // 处理异常，返回默认值
        using ReturnType = decltype(func());
        if constexpr (!std::is_same_v<ReturnType, void>) {
            return ReturnType{}; // 返回类型的默认值
        }
    }
}

void MQTTClient::ProcessMsg(MulNX::Messaging::Message* Msg) {
    switch (Msg->Type) {
    case MulNX::MsgType::UISystem_UICommand: {
		this->HandleUICommand(Msg);
        Msg->pMsgChannel->PushMessage(MulNX::Message(MulNX::MsgType::UISystem_ModuleRespose));
        break;
    }
    default:
        break;
    }
}

void MQTTClient::HandleUICommand(MulNX::Messaging::Message* Msg) {
    // 处理UI命令的逻辑
    switch (Msg->SubType) {
    case 0x101: { // 连接到MQTT服务器命令
        if (!this->Inited) {
            bool r = WinExceptionWrapper([this]() {
                if (this->CreateMQTTConnect()) {
                    return true;
                }
                return false;
                });
            if (r) {
                this->Inited = true;
            }
        }
        else {
            this->IDebugger->AddWarning("MQTT客户端已初始化，无需重复连接");
        }
        break;
    }
    case 0x102: { // 设置服务器IP命令
        auto p = this->Core->IHandleSystem().ReleaseHandle(Msg->Handle);
        std::string* ip = p.get<std::string>();
        if (ip) {
            this->SetServerIP(*ip);
            this->IDebugger->AddInfo("已设置服务器IP为: " + *ip);
        }
        break;
    }
    case 0x103: { // 设置服务器端口命令
        int port = Msg->ParamInt;
        this->SetServerPort(static_cast<uint16_t>(port));
        this->IDebugger->AddInfo("已设置服务器端口为: " + std::to_string(port));
        break;
    }
    }
}