#include "CoreImpl.hpp"
#include "CoreStarterBase/CoreStarterBase.hpp"

// 构造函数和析构函数需要管理pImpl的生命周期
Core::Core() {
	// 创建Impl实例
	this->pImpl = std::make_unique<CoreImpl>();
}

Core::~Core() {
	return;
}

bool Core::SetCoreStarter(std::unique_ptr<CoreStarterBase> Starter) {
	if (!Starter) {
		return false;
	}
	this->pCoreStarter = std::move(Starter);
	return true;
}

bool Core::RegisteModule(std::unique_ptr<ModuleBase>Module, std::string&& Name) {
	return this->pImpl->ModuleManager.RegisteModule(std::move(Module), std::move(Name));
}
MulNX::ModuleBase* Core::FindModule(const std::string& Name) {
	return this->pImpl->ModuleManager.FindModule(Name);
}

//获取子模块的接口实现
MulNX::IHandleSystem&				Core::IHandleSystem() { return this->pImpl->HandleSystem; }
MulNX::IUISystem&					Core::IUISystem() { return this->pImpl->UISystem; }
MulNX::IPCer&						Core::IPCer() { return this->pImpl->IPCer; }
MulNX::Messaging::IMessageManager&	Core::IMessageManager() { return this->pImpl->MessageManager; }

CSController&						Core::CS() { return this->pImpl->CS; }
ConsoleManager&						Core::ConsoleManager() { return this->pImpl->ConsoleManager; }
GameSettingsManager&				Core::GameSettingsManager() { return this->pImpl->GameSettingsManager; }
ICameraSystem&						Core::ICameraSystem() { return this->pImpl->CameraSystem; }
VirtualUser&						Core::VirtualUser() { return this->pImpl->VirtualUser; }GameCfgManager&						Core::GameCfgManager() { return this->pImpl->GameCfgManager; }

// 专用初始化函数
void Core::Init() {
	// 核心启动器初始化
	this->pCoreStarter->EntryInit(this);
	// 通过核心启动器进行系统初始化
	this->pCoreStarter->SystemInit(this->pImpl.get(), this);

	// 逆向层初始化
	this->pImpl->CS.InitInterface();
	this->pImpl->CS.EntryInit(this);
	this->pImpl->CS.EntryCreateThread();// 包含线程创建
	this->pImpl->CS.SetMyThreadDelta(3);
	this->pImpl->AL3D.EntryInit(this);

	// 功能模块初始化
	this->pImpl->ConsoleManager.EntryInit(this);
	this->pImpl->GameSettingsManager.EntryInit(this);
	this->pImpl->CameraSystem.EntryInit(this);
	this->pImpl->GameCfgManager.EntryInit(this);
	this->pImpl->DemoHelper.EntryInit(this);
	// 虚拟用户层初始化（用于自动化任务）
	this->pImpl->VirtualUser.EntryInit(this);

	// 注册主窗口UI上下文
	auto SContext = MulNX::Base::make_any_unique<MulNXSingleUIContext>();
	MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
	SContextPtr->name = "MainDraw";
	SContextPtr->MyMsgChannel = this->pImpl->UISystem.ICreateAndGetMessageChannel();


	SContextPtr->MyFunc = [](MulNXSingleUIContext* This)->void {
		ImGui::Begin("主窗口");
		if (ImGui::BeginTabBar("主标签页集")) {
			if (ImGui::BeginTabItem("Demo助手")) {
				This->CallSingleUIContext("DemoHelper");
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("游戏设置")) {
				This->CallSingleUIContext("GameSettings");
				ImGui::EndTabItem();
			}


			ImGui::EndTabBar();
		}

		ImGui::End();
		};
	MulNXHandle hContext = this->IHandleSystem().RegisteHandle(std::move(SContext));

	MulNX::Messaging::Message Msg(MulNX::Messaging::MsgType::UISystem_ModulePush); 
	Msg.Handle = hContext;
	this->pImpl->MessageManager.Publish(std::move(Msg));


	this->pImpl->GlobalVars.SystemReady = true;

	// 初始化注册模块
	this->pImpl->ModuleManager.PackedInit();

	this->pCoreStarter->StartAll();

	this->pCoreStarter->InitEndCall();

	return;
}

// 主窗口
void Core::MulNXiMainWindow() {
	ImGui::Begin(MulNXiGlobalVarsOnlyRead::FullName);//窗口标题

	// Tab Bar
	if (ImGui::BeginTabBar("MainTabBar")) {
		if (ImGui::BeginTabItem("总控台")) {
			this->pImpl->Page = Page::Control;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("调试")) {
			this->pImpl->Page = Page::Debug;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("摄像机系统")) {
			this->pImpl->Page = Page::CameraSystem;
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	// 页面内容
	switch (this->pImpl->Page) {
	case Page::Control: {
		// 两个模块，上下分栏，各占一半
		this->pImpl->ConsoleManager.Menu();
		this->pImpl->VirtualUser.Menu();
		break;
	}
	case Page::Debug: {
		this->pImpl->Debugger.Menu();
		break;
	}
	case Page::CameraSystem: {
		this->pImpl->CameraSystem.Menu();
		break;
	}
	default:
		break;
	}	

	ImGui::End();
	return;
}

// 主逻辑
void Core::VirtualMain() {
	this->pImpl->GlobalVars.EntryVirtualMain();
	this->pImpl->AL3D.EntryVirtualMain();
	this->pImpl->VirtualUser.EntryVirtualMain();
	this->pImpl->CameraSystem.EntryVirtualMain();
	this->pImpl->ConsoleManager.EntryVirtualMain();
	this->pImpl->GameSettingsManager.EntryVirtualMain();
	
	this->pImpl->DemoHelper.EntryVirtualMain();

	// 包装的，所有的模块的VirtualMain
	this->pImpl->ModuleManager.EntryVirtualMain();

	this->MulNXiMainWindow();
	this->pImpl->Debugger.EntryWindows();
	this->pImpl->CameraSystem.EntryWindows();
	this->pImpl->GameCfgManager.EntryWindows();
	
	this->pImpl->ModuleManager.EntryWindows();

	return;
}