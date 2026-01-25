#include "Core.hpp"

#include "ModuleManager/ModuleManager.hpp"

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

using namespace MulNX::Core;

// 页面枚举
enum class Page :int {
    Control,
    Debug,
    CameraSystem
};

// 实现类，掌握所有的模块实例
class CoreImpl {
public:
    // 初始化页面
    Page Page = Page::Control;

    // 地基模块
    MulNX::Core::ModuleManager ModuleManager;
    MulNX::KeyTracker KT;
    MulNX::HandleSystem HandleSystem;
    MulNX::Messaging::MessageManager MessageManager;
    MulNX::GlobalVars GlobalVars;
    MulNX::UISystem UISystem;
    MulNX::IPCer IPCer;
    MulNX::Debugger Debugger;
    // 基础服务模块

    CSController CS;
    AbstractLayer3D AL3D;

    // 上层建筑
    ConsoleManager ConsoleManager;
    GameSettingsManager GameSettingsManager;
    DemoHelper DemoHelper;
    CameraSystem CameraSystem;
    VirtualUser VirtualUser;
    GameCfgManager GameCfgManager;

public:
    // 构造函数
    CoreImpl() = default;
};