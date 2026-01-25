#include "DllMain.hpp"

#include "../../../MulNX/MulNX.hpp"
#include "../../../MulNX/Extensions/MQTT/MQTTClient/MQTTClient.hpp"
#include "../../../MulNX/Extensions/CS2/HookManager/HookManager.hpp"

#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		HANDLE hThread = CreateThread(NULL, 0, MulNX_CS2_Start, NULL, 0, NULL);
		break;
	}
	case DLL_THREAD_ATTACH: {
		break;
	}
	case DLL_THREAD_DETACH: {
		break;
	}
	case DLL_PROCESS_DETACH: {
		break;
	}
	default: {
		break;
	}
	}
	return TRUE;
}

DWORD MulNX_CS2_Start(void*) {
	// 创建核心实例
	MulNX::Core::Core* Core = new MulNX::Core::Core();

	// 创建核心启动器实例
	std::unique_ptr<MulNX::Core::CoreStarterBase> starter =
		std::make_unique<HookManager>();
	// 设置初始化完成回调
	starter->InitEndCall = [Core]() {
		CoreImpl* pImpl = Core->GetImpl();
		// 初始化额外任务（对系统无影响）
		pImpl->Debugger.AddSucc("注入成功！");
		pImpl->Debugger.AddSucc("各模块初始化完成！");
		pImpl->Debugger.AddWarning("您正在使用测试版本！！");
		pImpl->AL3D.ExecuteCommand("playdemo 111");
		std::thread([]() {
			MessageBoxW(NULL, L"MulNXDLL 注入成功！", L"MulNX", MB_OK | MB_ICONINFORMATION);
			}).detach();
		};

	// 设置核心启动器
	Core->SetCoreStarter(std::move(starter));

	// 创建MQTT模块
	std::unique_ptr<MulNX::Extensions::MQTT::MQTTClient> mqttClient =
		std::make_unique<MulNX::Extensions::MQTT::MQTTClient>();
	// 配置MQTT服务器地址和端口
	mqttClient->SetServerIP("127.0.0.1");
	mqttClient->SetServerPort(1883);
	// 注册模块
	Core->RegisteModule(std::move(mqttClient), "MQTTClient");

	std::unique_ptr<MiniMap>minimap =
		std::make_unique<MiniMap>();
	Core->RegisteModule(std::move(minimap), "MiniMap");

	// 启动核心
	Core->Init();

	return 0;
}