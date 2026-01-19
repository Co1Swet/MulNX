#include "Core.hpp"

#include "../Systems/Debugger/Debugger.hpp"
#include "../Systems/HandleSystem/HandleSystem.hpp"
#include "../Systems/IPCer/IPCer.hpp"
#include "../Systems/KeyTracker/KeyTracker.hpp"
#include "../Systems/MessageManager/MessageManager.hpp"
#include "../Systems/MulNXiGlobalVars/MulNXiGlobalVars.hpp"
#include "../Systems/MulNXUISystem/MulNXUISystem.hpp"

#include "../Extensions/AbstractLayer3D/AbstractLayer3D.hpp"
#include "../Extensions/CameraSystem/CameraSystem.hpp"
#include "../Extensions/MiniMap/MiniMap.hpp"
#include "../Extensions/VirtualUser/VirtualUser.hpp"

#include "../Extensions/CS2/CSController/CSController.hpp"
#include "../Extensions/CS2/ConsoleManager/ConsoleManager.hpp"
#include "../Extensions/CS2/DemoHelper/DemoHelper.hpp"
#include "../Extensions/CS2/GameCfgManager/GameCfgManager.hpp"
#include "../Extensions/CS2/GameSettingsManager/GameSettingsManager.hpp"
#include "../Extensions/CS2/HookManager/HookManager.hpp"

//页面枚举
enum class Page :int {
	Control,
	Debug,
	CameraSystem
};

//实现类，掌握所有的模块实例
class CoreImpl {	
private:
	MulNX::Core* Core;
public:
	//初始化页面
	Page Page;

    //地基模块
    MulNX::KeyTracker KT;
	MulNX::HandleSystem HandleSystem;
	MulNX::Messaging::MessageManager MessageManager;
	MulNX::GlobalVars GlobalVars;
	MulNX::UISystem UISystem;
	MulNX::IPCer IPCer;
    //基础服务模块

    CSController CS;
    AbstractLayer3D AL3D;
    
    HookManager HookManager;
	
    //上层建筑
    ConsoleManager ConsoleManager;
    GameSettingsManager GameSettingsManager;
    MulNX::Debugger Debugger;
    DemoHelper DemoHelper;
    CameraSystem CameraSystem;
    VirtualUser VirtualUser;
    MiniMap MiniMap;
    GameCfgManager GameCfgManager;
    
public:
	//构造函数，绑定Core指针
	CoreImpl(MulNX::Core* Core)
		: Core(Core)
		, Page(Page::CameraSystem)
		, KT(Core)
		, CS(Core)
		, AL3D(Core)
		, IPCer(Core)
		, HookManager(Core)
		, HandleSystem(Core)
		, MessageManager(Core)
		, GlobalVars(Core)
		, ConsoleManager(Core)
		, GameSettingsManager(Core)
		, Debugger(Core)
		, DemoHelper(Core)
		, CameraSystem(Core)
		, VirtualUser(Core)
		, MiniMap(Core)
		, GameCfgManager(Core)
		, UISystem(Core) {

	}
};

//构造函数和析构函数需要管理pImpl的生命周期
MulNX::Core::Core() {
	//创建Impl实例
	this->pImpl = std::make_unique<CoreImpl>(this);//绑定指针
}

MulNX::Core::~Core() {
	return;
}

//获取子模块的接口实现
MulNX::IHandleSystem&				MulNX::Core::IHandleSystem() { return this->pImpl->HandleSystem; }
MulNX::IDebugger&					MulNX::Core::IDebugger() { return this->pImpl->Debugger; }
MulNX::IUISystem&					MulNX::Core::IUISystem() { return this->pImpl->UISystem; }
MulNX::IPCer&						MulNX::Core::IPCer() { return this->pImpl->IPCer; }
MulNX::KeyTracker&					MulNX::Core::KT() { return this->pImpl->KT; }
MulNX::Messaging::IMessageManager&	MulNX::Core::IMessageManager() { return this->pImpl->MessageManager; }
MulNX::GlobalVars&					MulNX::Core::GlobalVars() { return this->pImpl->GlobalVars; }

CSController&							MulNX::Core::CS() { return this->pImpl->CS; }
IAbstractLayer3D&						MulNX::Core::IAbstractLayer3D() { return this->pImpl->AL3D; }
ConsoleManager&							MulNX::Core::ConsoleManager() { return this->pImpl->ConsoleManager; }
GameSettingsManager&					MulNX::Core::GameSettingsManager() { return this->pImpl->GameSettingsManager; }
ICameraSystem&							MulNX::Core::ICameraSystem() { return this->pImpl->CameraSystem; }
VirtualUser&							MulNX::Core::VirtualUser() { return this->pImpl->VirtualUser; }
HookManager&							MulNX::Core::HookManager() { return this->pImpl->HookManager; }
MiniMap&								MulNX::Core::MiniMap() { return this->pImpl->MiniMap; }
GameCfgManager&							MulNX::Core::GameCfgManager() { return this->pImpl->GameCfgManager; }

DWORD MulNX::Core::CoreStart(void*) {
	MH_Initialize();
	MulNX::Core::GetInstance().Init();
	return 0;
}
MulNX::Core& MulNX::Core::GetInstance() {
	static MulNX::Core MulNXiCoreInstance;
	return MulNXiCoreInstance;
}

//专用初始化函数
void MulNX::Core::Init() {
	std::thread([]() {
		MessageBoxW(NULL, L"MulNXDLL 注入成功！", L"MulNX", MB_OK | MB_ICONINFORMATION);
		}).detach();

	//无依赖核心基础模块初始化
	this->pImpl->MessageManager.EntryInit();
	this->pImpl->MessageManager.EntryCreateThread();//包含线程创建
	this->pImpl->MessageManager.SetMyThreadDelta(10);//注意，此模块内部动态调整频率
	this->pImpl->KT.EntryCreateThread();//包含线程创建
	this->pImpl->KT.SetMyThreadDelta(3);
	this->pImpl->IPCer.EntryInit();
	this->pImpl->Debugger.EntryInit();
	this->pImpl->GlobalVars.EntryInit();

	//钩子层初始化
	this->pImpl->HookManager.ReHook = true;
	this->pImpl->HookManager.EntryInit();
	this->pImpl->HookManager.EntryCreateThread();
	this->pImpl->HookManager.SetMyThreadDelta(250);

	while (!this->GlobalVars().CoreReady) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	//UI模块初始化
	this->pImpl->UISystem.EntryInit();

	//逆向层初始化
	this->pImpl->CS.InitInterface();
	this->pImpl->CS.EntryInit();
	this->pImpl->CS.EntryCreateThread();//包含线程创建
	this->pImpl->CS.SetMyThreadDelta(3);
	this->pImpl->AL3D.EntryInit();

	
	
	//功能模块初始化
	this->pImpl->ConsoleManager.EntryInit();
	this->pImpl->GameSettingsManager.EntryInit();
	this->pImpl->CameraSystem.EntryInit();
	this->pImpl->GameCfgManager.EntryInit();
	this->pImpl->DemoHelper.EntryInit();
	//虚拟用户层初始化（用于自动化任务）
	this->pImpl->VirtualUser.EntryInit();
	//小地图模块初始化
	this->pImpl->MiniMap.EntryInit();

	//初始化额外任务（对系统无影响）
	this->pImpl->Debugger.AddSucc("注入成功！");
	this->pImpl->Debugger.AddSucc("各模块初始化完成！");
	this->pImpl->Debugger.AddWarning("您正在使用测试版本！！");
	this->pImpl->AL3D.ExecuteCommand("playdemo 111");

	
	//UI系统主界面初始化
	auto StartString = MulNXB::make_any_unique<std::string>("MainDraw");
	MulNXHandle hStr = this->IHandleSystem().RegisteHandle(std::move(StartString));
	MulNX::Messaging::Message StartMsg(MulNX::Messaging::MsgType::UISystem_Start);
	StartMsg.Handle = hStr;
	this->pImpl->MessageManager.Publish(std::move(StartMsg));


	//注册主窗口UI上下文
	auto SContext = MulNXB::make_any_unique<MulNXSingleUIContext>();
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
	return;
}

//主窗口
void MulNX::Core::MulNXiMainWindow() {
	ImGui::Begin(MulNXiGlobalVarsOnlyRead::FullName);//窗口标题

	//Tab Bar
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

	//页面内容
	switch (this->pImpl->Page) {
	case Page::Control: {
		//两个模块，上下分栏，各占一半
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

//主逻辑
void MulNX::Core::VirtualMain() {
	this->pImpl->GlobalVars.EntryVirtualMain();
	this->pImpl->AL3D.EntryVirtualMain();
	this->pImpl->VirtualUser.EntryVirtualMain();
	this->pImpl->CameraSystem.EntryVirtualMain();
	this->pImpl->ConsoleManager.EntryVirtualMain();
	this->pImpl->GameSettingsManager.EntryVirtualMain();
	this->pImpl->MiniMap.EntryVirtualMain();
	
	this->pImpl->DemoHelper.EntryVirtualMain();


	this->MulNXiMainWindow();
	this->pImpl->Debugger.EntryWindows();
	this->pImpl->CameraSystem.EntryWindows();
	this->pImpl->GameCfgManager.EntryWindows();
	this->pImpl->MiniMap.EntryWindows();

	return;
}