#pragma once

#include "../../../Core/ModuleBase/ModuleBase.hpp"

#include "../ThirdParty/mqtt.h"

namespace MulNX {
	namespace Extensions {
		namespace MQTT {
			// MQTT客户端封装类
			class MQTTClient final :public MulNX::ModuleBase {
			private:
				// 内部MQTT客户端结构体
				struct mqtt_client mqtt_client;
				// WinSock数据结构体
				WSADATA wsaData;
				// socket句柄
				SOCKET sockfd;
				// 服务器地址结构体
				sockaddr_in server_addr{};
				// 连接模式
				u_long mode;

				// 初始化MQTT客户端
				struct mqtt_client client;
				// 发送和接收缓冲区
				uint8_t sendbuf[2048];
				// 接收缓冲区
				uint8_t recvbuf[1024];

				// IP地址，默认本地地址
				std::string ServerIP = "127.0.0.1";
				// 端口号，默认1883
				uint16_t ServerPort = 1883;

				// 包装回调函数，提供给C风格的接口使用
				static void CStylePublishCallback(void** state, struct mqtt_response_publish* published);
				// 回调函数，处理发布的消息
				void PublishCallback(struct mqtt_response_publish* published);
				// 错误码
				enum MQTTErrors err;

				// 是否已初始化
				bool Inited = false;

				// 主题到回调函数的映射
				std::unordered_map<std::string, std::function<void(const std::string& message)>> TopicMessageMap;
			public:
				bool Init()override;
				void VirtualMain()override;

				std::string GetCurrentError()const;
				
				std::string GetServerIP()const;
				std::string GetServerPort()const;

				void SetServerIP(const std::string& ip);
				void SetServerPort(uint16_t port);

				// 发布消息到指定主题
				bool MQTTPublish(const std::string& topic, const std::string& message, uint8_t publish_flags);
				// 订阅消息
				bool MQTTSubscribe(const std::string& topic, std::function<void(const std::string& message)> callback, int qos = 0);
			};
		}
	}
}