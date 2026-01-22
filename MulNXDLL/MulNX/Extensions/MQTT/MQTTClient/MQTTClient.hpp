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
				// 错误码
				enum MQTTErrors err;

				// 是否已初始化
				bool Inited = false;
			public:
				bool Init()override;
				void VirtualMain()override;

				std::string GetCurrentError()const;
			};
		}
	}
}