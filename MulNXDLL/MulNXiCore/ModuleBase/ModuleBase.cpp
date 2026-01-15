#include"ModuleBase.hpp"

#include"../MulNXiCore.hpp"

#include"../MessageManager/IMessageManager.hpp"


//构造与析构函数，线程自动析构
ModuleBase::ModuleBase(MulNXiCore* MulNXi) {
	this->MulNXi = MulNXi;
}
ModuleBase::~ModuleBase() {
    this->CloseMyThread();
}
void ModuleBase::CloseMyThread() {
    std::unique_lock lock(this->MyThreadMutex);
    if (this->MyThreadRunning) {
        this->MyThreadRunning = false;
        this->MyThread.join();
    }
    return;
}

//模块自用
void ModuleBase::SetMyThreadDelta(int Delta) {
    this->MyThreadDelta = Delta;
}

//默认虚函数实现

void ModuleBase::VirtualMain() {
	return;
}
void ModuleBase::ThreadMain() {
    return;
}
void ModuleBase::ProcessMsg(MulNXMessage* Msg) {
	return;
}
void ModuleBase::Menu() {
	return;
}
void ModuleBase::Windows() {
	return;
}


//窗口控制

void ModuleBase::OpenWindow() {
    this->ShowWindow = true;
}
void ModuleBase::CloseWindow() {
    this->ShowWindow = false;
}
bool ModuleBase::IsWindowOpen()const {
    return this->ShowWindow;
}

//基本函数
bool ModuleBase::BaseInit() {
	this->IMsgManager = &this->MulNXi->IMessageManager();
	this->IDebugger = &this->MulNXi->IDebugger();
    this->MulNXiGlobalVars = &this->MulNXi->MulNXiGlobalVars();
    this->AL3D = &this->MulNXi->IAbstractLayer3D();

    static uint32_t Begin = 1;
    this->HModule.Value = Begin;
    ++Begin;

    this->IRegiste();

    return true;
}
void ModuleBase::BaseVirtualMain() {
    return;
}
void ModuleBase::BaseProcessMsg() {
    IMessageChannel* Channel = this->MainMsgChannel;
    if (Channel != nullptr) {
        MulNXMessage Msg{ MsgType::Null };
        while (Channel->PullMessage(Msg)) {
            this->ProcessMsg(&Msg);
        }
    }
    return;
}
void ModuleBase::BaseMenu() {
    return;
}
void ModuleBase::BaseWindows() {
    return;
}


//各类入口点函数
bool ModuleBase::EntryInit() {
    if (!this->BaseInit()) {
        return false;
    }
    if (!this->Init()) {
		return false;
    }
	return true;
}
void ModuleBase::EntryVirtualMain() {
    this->BaseVirtualMain();
    this->VirtualMain();
}
bool ModuleBase::EntryCreateThread() {
    for (;;) {
        //如果线程已经创建，则立即返回
        if (this->MyThreadRunning) return true;
        //尝试创建线程
        try {
            //先设置线程处于启动状态
            this->MyThreadRunning = true;
            //打开线程
            this->MyThread = std::thread([this]() {
                while (this->MyThreadRunning) {
                    this->ThreadMain();
                    std::this_thread::sleep_for(std::chrono::milliseconds(this->MyThreadDelta));
                }
                });
            //如果成功返回成功
            return true;
        }
        //捕获所有创建异常
        catch (...) {
            //先把运行置空
            this->MyThreadRunning = false;
            //重新尝试，直到成功
            continue;
        }
    }
}
void ModuleBase::EntryProcessMsg() {
    this->BaseProcessMsg();
    auto pMsg = this->CurrentMsg.load();
    if (pMsg) {
        this->ProcessMsg(pMsg);
        this->CurrentMsg.store(nullptr);
		this->IMsgManager->Release();
    }
}
void ModuleBase::EntryMenu() {
    this->BaseMenu();
    this->Menu();
}
void ModuleBase::EntryWindows() {
    this->BaseWindows();
    this->Windows();
}

void ModuleBase::IRegiste() {
    this->IMsgManager->Registe(this);
}
void ModuleBase::ISubscribe(MsgType MsgType) {
    this->IMsgManager->Subscribe(MsgType, this);
}
void ModuleBase::IPublish(MulNXMessage&& Msg) {
    this->IMsgManager->Publish(std::move(Msg));
}
void ModuleBase::IPublish(MsgType Type) {
    //消息管理器会负责消息生命周期
    MulNXMessage Msg(Type);
    this->IMsgManager->Publish(std::move(Msg));
}
IMessageChannel* ModuleBase::ICreateAndGetMessageChannel() {
    return this->IMsgManager->GetMessageChannel(this->IMsgManager->CreateMessageChannel());
}

AutoChild::AutoChild(ModuleBase* Module, const float HeightRatio, const float WidthRatio)
    :Module(Module) {
    ImVec2 Available = ImGui::GetContentRegionAvail();
    ImGui::BeginChild(Module->GetName().c_str(), ImVec2(Available.x * WidthRatio, Available.y * HeightRatio), true);
}
AutoChild::~AutoChild() {
    ImGui::EndChild();
}