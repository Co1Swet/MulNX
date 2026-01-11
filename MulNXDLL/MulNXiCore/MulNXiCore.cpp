#include"MulNXiCore.hpp"

#include"AbstractLayer3D/AbstractLayer3D.hpp"
#include"CameraSystem/CameraSystem.hpp"
#include"ConsoleManager/ConsoleManager.hpp"
#include"CSController/CSController.hpp"
#include"Debugger/Debugger.hpp"
#include"DemoHelper/DemoHelper.hpp"
#include"GameCfgManager/GameCfgManager.hpp"
#include"GameSettingsManager/GameSettingsManager.hpp"
#include"MulNXiGlobalVars/MulNXiGlobalVars.hpp"
#include"HookManager/HookManager.hpp"
#include"IPCer/IPCer.hpp"
#include"KeyTracker/KeyTracker.hpp"
#include"MessageManager/MessageManager.hpp"
#include"MiniMap/MiniMap.hpp"
#include"MulNXUISystem/MulNXUISystem.hpp"
#include"VirtualUser/VirtualUser.hpp"

//页面枚举
enum class Page :int {
	Control,
	GameSettings,
	Debug,
	CameraSystem
};

//实现类，掌握所有的模块实例
class MulNXiCoreImpl {	
private:
	MulNXiCore* MulNXi;
public:
	//初始化页面
	Page Page;

    //地基模块
    KeyTracker KT;
    //基础服务模块
    CSController CS;
    AbstractLayer3D AL3D;
    IPCer IPCer;
    HookManager HookManager;
    MessageManager MessageManager;
    MulNXiGlobalVars MulNXiGlobalVars;
    //上层建筑
    ConsoleManager ConsoleManager;
    GameSettingsManager GameSettingsManager;
    Debugger Debugger;
    DemoHelper DemoHelper;
    CameraSystem CameraSystem;
    VirtualUser VirtualUser;
    MiniMap MiniMap;
    GameCfgManager GameCfgManager;
    MulNXUISystem MulNXUISystem;
public:
	//构造函数，绑定MulNXi指针
	MulNXiCoreImpl(MulNXiCore* MulNXi)
		: MulNXi(MulNXi)
		, Page(Page::CameraSystem)
		, KT(MulNXi)
		, CS(MulNXi)
		, AL3D(MulNXi)
		, IPCer(MulNXi)
		, HookManager(MulNXi)
		, MessageManager(MulNXi)
		, MulNXiGlobalVars(MulNXi)
		, ConsoleManager(MulNXi)
		, GameSettingsManager(MulNXi)
		, Debugger(MulNXi)
		, DemoHelper(MulNXi)
		, CameraSystem(MulNXi)
		, VirtualUser(MulNXi)
		, MiniMap(MulNXi)
		, GameCfgManager(MulNXi)
		, MulNXUISystem(MulNXi) {

	}
};

//构造函数和析构函数需要管理pImpl的生命周期
MulNXiCore::MulNXiCore() {
	//创建Impl实例
	this->pImpl = std::make_unique<MulNXiCoreImpl>(this);//绑定指针
}

MulNXiCore::~MulNXiCore() {
	return;
}

//获取子模块的接口实现
CSController& MulNXiCore::CS() { return this->pImpl->CS; }
IAbstractLayer3D& MulNXiCore::IAbstractLayer3D() { return this->pImpl->AL3D; }
IPCer& MulNXiCore::IPCer() { return this->pImpl->IPCer; }
KeyTracker& MulNXiCore::KT() { return this->pImpl->KT; }
ConsoleManager& MulNXiCore::ConsoleManager() { return this->pImpl->ConsoleManager; }
GameSettingsManager& MulNXiCore::GameSettingsManager() { return this->pImpl->GameSettingsManager; }
IDebugger& MulNXiCore::IDebugger() { return this->pImpl->Debugger; }
ICameraSystem& MulNXiCore::ICameraSystem() { return this->pImpl->CameraSystem; }
VirtualUser& MulNXiCore::VirtualUser() { return this->pImpl->VirtualUser; }
IMessageManager& MulNXiCore::IMessageManager() { return this->pImpl->MessageManager; }
HookManager& MulNXiCore::HookManager() { return this->pImpl->HookManager; }
MulNXiGlobalVars& MulNXiCore::MulNXiGlobalVars() { return this->pImpl->MulNXiGlobalVars; }
MiniMap& MulNXiCore::MiniMap() { return this->pImpl->MiniMap; }
IMulNXUISystem& MulNXiCore::IUISystem() { return this->pImpl->MulNXUISystem; }
GameCfgManager& MulNXiCore::GameCfgManager() { return this->pImpl->GameCfgManager; }

DWORD MulNXiCore::CoreStart(void*) {
	MH_Initialize();
	MulNXiCore::GetInstance().Init();
	return 0;
}
MulNXiCore& MulNXiCore::GetInstance() {
	static MulNXiCore MulNXiCoreInstance;
	return MulNXiCoreInstance;
}

//专用初始化函数
void MulNXiCore::Init() {
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
	this->pImpl->MulNXiGlobalVars.EntryInit();

	//钩子层初始化
	this->pImpl->HookManager.ReHook = true;
	this->pImpl->HookManager.EntryInit();
	this->pImpl->HookManager.EntryCreateThread();
	this->pImpl->HookManager.SetMyThreadDelta(250);

	while (!this->MulNXiGlobalVars().CoreReady) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	//UI模块初始化
	this->pImpl->MulNXUISystem.EntryInit();

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
	//打开主窗口
	this->OpenMainWindow = true;
	
	this->pImpl->MulNXiGlobalVars.SystemReady = true;
	return;
}

//主窗口
void MulNXiCore::MulNXiMainWindow() {
	if (!this->OpenMainWindow)return;
	ImGui::Begin(MulNXiGlobalVarsOnlyRead::FullName, &this->OpenMainWindow);//窗口标题

	//Tab Bar
	if (ImGui::BeginTabBar("MainTabBar")) {
		if (ImGui::BeginTabItem("总控台")) {
			this->pImpl->Page = Page::Control;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("游戏设置")) {
			this->pImpl->Page = Page::GameSettings;
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
	case Page::GameSettings: {
		this->pImpl->GameSettingsManager.Menu();
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

//主逻辑，在DllMain中被调用
void MulNXiCore::VirtualMain() {
	if (this->pImpl->KT.CheckComboClick(VK_INSERT, 1)) {
		this->OpenMainWindow = !this->OpenMainWindow;
	}

	this->pImpl->MulNXiGlobalVars.EntryVirtualMain();
	this->pImpl->AL3D.EntryVirtualMain();
	this->pImpl->VirtualUser.EntryVirtualMain();
	this->pImpl->CameraSystem.EntryVirtualMain();
	this->pImpl->ConsoleManager.EntryVirtualMain();
	this->pImpl->GameSettingsManager.EntryVirtualMain();
	this->pImpl->MiniMap.EntryVirtualMain();
	
	this->pImpl->DemoHelper.EntryVirtualMain();

	if (this->OpenMainWindow) {
		this->MulNXiMainWindow();
		this->pImpl->Debugger.EntryWindows();
		this->pImpl->CameraSystem.EntryWindows();
		this->pImpl->GameCfgManager.EntryWindows();
	}
	this->pImpl->MiniMap.EntryWindows();

	

	return;
}