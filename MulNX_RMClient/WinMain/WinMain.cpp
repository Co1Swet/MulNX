#include "WinMain.hpp"

#include "../../MulNX/MulNX/MulNX.hpp"
#include "../../MulNX/MulNX/Extensions/CS2/MulNXCS2Ext.hpp"
#include "../../MulNX/MulNX/Extensions/MQTT/MQTTClient/MQTTClient.hpp"
#include "../../MulNX/MulNX/Extensions/RMClient/RMClient.hpp"
#include "../../MulNX/MulNX/Extensions/Win32Starter/Win32Starter.hpp"

static void MainDraw(MulNXSingleUIContext* This) {
	ImGui::Begin("主窗口");
	if (ImGui::BeginTabBar("主标签页集")) {
		if (ImGui::BeginTabItem("MQTTClientCfg")) {
			This->CallSingleUIContext("MQTTClientCfg");
			// 实时帧率显示
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("FPS: %.1f", io.Framerate);
			ImGui::Text("Frame Time: %.3f ms", 1000.0f / io.Framerate);
			ImGui::Text("Delta Time: %.6f s", io.DeltaTime);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("RM客户端")) {
			This->CallSingleUIContext("RMClient");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// 创建核心实例
	MulNX::Core::Core* Core = new MulNX::Core::Core();

	// 创建核心启动器实例
	std::unique_ptr<MulNX::Core::CoreStarterBase> starter = std::make_unique<Win32Starter>();
	// 设置初始化完成回调
	starter->InitEndCall = [Core]() {
		MulNX::Core::CoreStarterBase* starter = Core->GetStarter();
		// 初始化额外任务（对系统无影响）
		starter->IDebugger->AddSucc("您正在使用MulNX_RMClient");
		starter->IDebugger->AddSucc("版权所有：Co1Swet");
		starter->IDebugger->AddWarning("本项目为独立项目，授权开发者所在RM战队使用");
		std::thread([]() {
			MessageBoxW(NULL, L"欢迎使用RM自定义客户端！", L"MulNX", MB_OK | MB_ICONINFORMATION);
			}).detach();
		// 注册主绘制函数
		starter->RegisteMainDrawWith(MainDraw);
		// 启动UI系统
		starter->StartUIWith("MainDraw");
		};

	// 设置核心启动器
	Core->SetCoreStarter(std::move(starter));

	// MQTT模块
	std::unique_ptr<MulNX::Extensions::MQTT::MQTTClient> mqttClient = std::make_unique<MulNX::Extensions::MQTT::MQTTClient>();
	Core->ModuleManager()->RegisteModule(std::move(mqttClient), "MQTTClient", 102);

	// RMClient模块
	std::unique_ptr<RMClient> rmClient = std::make_unique<RMClient>();
	Core->ModuleManager()->RegisteModule(std::move(rmClient), "RMClient", 103);

	// 启动
	Core->Init();

	while (true) {
		Core->IUISystem().Render();
	}
}