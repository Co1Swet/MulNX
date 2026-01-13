#pragma once
//整个系统的核心，负责所有子模块的生命周期管理，初始化，主循环调用等
//同时提供各子模块的接口访问
#include"../MulNXB/MulNXB.hpp"

//前向声明实现类
class MulNXiCoreImpl;

class MulNXiCore {
private:
    //数据存储：

    //Impl的指针
    std::unique_ptr<MulNXiCoreImpl> pImpl;
public:
    //系统启动点
    static DWORD CoreStart(void*);
    //获取核心实例
    static MulNXiCore& GetInstance();
private:
    //构造函数
    MulNXiCore();
public:
    //析构函数
    ~MulNXiCore();
    //删除拷贝构造
    MulNXiCore(const MulNXiCore&) = delete;
    //删除拷贝赋值
    MulNXiCore& operator=(const MulNXiCore&) = delete;

    //初始化
    void Init();
    //主循环
    void VirtualMain();
    //窗口逻辑
    void MulNXiMainWindow();
    
    //获取子模块的接口
    CSController& CS();
    IPCer& IPCer();
    KeyTracker& KT();
    ConsoleManager& ConsoleManager();
    GameSettingsManager& GameSettingsManager();
    HandleSystem& HandleSystem();
    IDebugger& IDebugger();
    ICameraSystem& ICameraSystem();
    VirtualUser& VirtualUser();
    IMessageManager& IMessageManager();
    IMulNXUISystem& IUISystem();
    HookManager& HookManager();
    MulNXiGlobalVars& MulNXiGlobalVars();
    IAbstractLayer3D& IAbstractLayer3D();
    MiniMap& MiniMap();
	GameCfgManager& GameCfgManager();
};