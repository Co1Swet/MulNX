#include "Base.hpp"

std::string Module_EnumToName(MulNX::ModuleType Type) {
    switch (Type) {
    case MulNX::ModuleType::ModuleBase:
        return "ModuleBase";
    case MulNX::ModuleType::AbstractLayer3D:
        return "AbstractLayer3D";
    case MulNX::ModuleType::CameraSystem:
        return "CameraSystem";
    case MulNX::ModuleType::ConsoleManager:
        return "ConsoleManager";
    case MulNX::ModuleType::CSController:
        return "CSController";
    case MulNX::ModuleType::Debugger:
        return "Debugger";
    case MulNX::ModuleType::DemoHelper:
        return "DemoHelper";
    case MulNX::ModuleType::GameCfgManager:
        return "GameCfgManager";
    case MulNX::ModuleType::GameSettingsManager:
        return "GameSettingsManager";
    case MulNX::ModuleType::MulNXiGlobalVars:
        return "MulNXiGlobalVars";
    case MulNX::ModuleType::HookManager:
        return "HookManager";
    case MulNX::ModuleType::IPCer:
        return "IPCer";
    case MulNX::ModuleType::KeyTracker:
        return "KeyTracker";
    case MulNX::ModuleType::MessageManager:
        return "MessageManager";
    case MulNX::ModuleType::MiniMap:
        return "MiniMap";
    case MulNX::ModuleType::MulNXUISystem:
        return "MulNXUISystem";
    case MulNX::ModuleType::VirtualUser:
        return "VirtualUser";
    case MulNX::ModuleType::HandleSystem:
        return "HandleSystem";
    default:
        return "UnknownModule";
    }
}