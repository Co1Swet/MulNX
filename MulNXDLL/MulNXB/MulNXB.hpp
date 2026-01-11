#pragma once

#include"Math/Math.hpp"
#include"Memory/Memory.hpp"
#include"WIN32Msg/WIN32Msg.hpp"
#include"HookUtility/HookUtility.hpp"
#include"CameraSystemIO/CameraSystemIO.hpp"
#include"any_unique_ptr/any_unique_ptr.hpp"

#include"../ThirdParty/All_cs2_dumper.hpp"
#include"../ThirdParty/All_ImGui.hpp"
#include"../ThirdParty/All_MinHook.hpp"
#include"../ThirdParty/All_pugixml.hpp"

#include<Windows.h>
#include<algorithm>
#include<cmath>
#include<chrono>
#include<cstdint>
#include<sstream>
#include<d3d11.h>
#include<string>
#include<shared_mutex>
#include<atomic>
#include<thread>
#include<filesystem>
#include<functional>
#include<memory>
#include<variant>

#undef min
#undef max

typedef float GameTime_t;

//句柄内容

//MulNX资源句柄
class MulNXHandle {
public:
	constexpr inline static uint32_t Invalid = 0xFFFFFFFF;
	uint32_t Value;

	MulNXHandle() {
		this->Value = MulNXHandle::Invalid;
	}
	bool IsValid()const {
		return this->Value != MulNXHandle::Invalid;
	}
	bool operator == (const MulNXHandle & Other)const {
		return this->Value == Other.Value;
	}
};
namespace std {
	template<>
	struct hash<MulNXHandle> {
		size_t operator()(const MulNXHandle& Handle)const noexcept {
			return std::hash<uint32_t>{}(Handle.Value);
		}
	};
}

//摄像机系统输入输出前置声明
class CameraSystemIO;

//核心类前置声明

//核心管理器
class MulNXiCore;
//MulNX消息
class MulNXMessage;
//MulNX消息类型
enum class MsgType :uint32_t;
//消息管理器
class IMessageManager;
//消息管道
class IMessageChannel;
//调试器接口
class IDebugger;
//全局变量
class MulNXiGlobalVars;
//3D抽象层
class IAbstractLayer3D;
//MulNX UI系统
class IMulNXUISystem;
//Hook管理器
class HookManager;
//CS游戏控制器
class CSController;
//MulNX总控台
class ConsoleManager;
//CS游戏Cfg文件管理器
class GameCfgManager;
//CS游戏设置管理器
class GameSettingsManager;
//MulNX内部全局变量
class MulNXiGlobalVars;
//3D抽象层
class IAbstractLayer3D;
//摄像机系统接口
class ICameraSystem;
//调试器接口
class IDebugger;
//跨进程信息接口
class IPCer;
//按键追踪器
class KeyTracker;
//小地图
class MiniMap;
//虚拟用户，用于执行自动化功能
class VirtualUser;


//模块类型枚举
enum class ModuleType :unsigned int {
	ModuleBase,
	AbstractLayer3D,
	CameraSystem,
	ConsoleManager,
	MulNXUISystem,
	CSController,
	Debugger,
	DemoHelper,
	GameSettingsManager,
	MulNXiGlobalVars,
	HookManager,
	IPCer,
	KeyTracker,
	MessageManager,
	MiniMap,
	VirtualUser,
	GameCfgManager,
	HandleSystem
};

std::string Module_EnumToName(ModuleType Type);