#include"MulNXB.hpp"

std::string Module_EnumToName(ModuleType Type) {
    switch (Type) {
    case ModuleType::ModuleBase:
        return "ModuleBase";
    case ModuleType::AbstractLayer3D:
        return "AbstractLayer3D";
    case ModuleType::CameraSystem:
        return "CameraSystem";
    case ModuleType::ConsoleManager:
        return "ConsoleManager";
    case ModuleType::CSController:
        return "CSController";
    case ModuleType::Debugger:
        return "Debugger";
    case ModuleType::DemoHelper:
        return "DemoHelper";
    case ModuleType::GameCfgManager:
        return "GameCfgManager";
    case ModuleType::GameSettingsManager:
        return "GameSettingsManager";
    case ModuleType::MulNXiGlobalVars:
        return "MulNXiGlobalVars";
    case ModuleType::HookManager:
        return "HookManager";
    case ModuleType::IPCer:
        return "IPCer";
    case ModuleType::KeyTracker:
        return "KeyTracker";
    case ModuleType::MessageManager:
        return "MessageManager";
    case ModuleType::MiniMap:
        return "MiniMap";
    case ModuleType::MulNXUISystem:
        return "MulNXUISystem";
    case ModuleType::VirtualUser:
        return "VirtualUser";
    case ModuleType::HandleSystem:
        return "HandleSystem";
    default:
        return "UnknownModule";
    }
}