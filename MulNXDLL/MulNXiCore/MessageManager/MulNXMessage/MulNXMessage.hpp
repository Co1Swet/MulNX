#pragma once

#include<cstdint>
#include<memory>

#include"../../../MulNXB/MulNXB.hpp"

enum class MsgType :uint32_t {
	Core_Begin,
	Core_Shutdown,
	Core_Tick1,
	Core_Tick5,
	Core_Tick10,
	Core_Tick15,
	Core_Tick20,
	Core_Tick30,
	Core_Tick45,
	Core_Tick60,
	Core_Tick30min,

	//Core_Check,
	//Core_CheckBack,
	Core_ReHook,

	UISystem_UIPull,//UI通知组件拉取信息
	UISystem_ModulePush,//组件响应信息，更新相关上下文
	UISystem_UICommand,//UI通知组件处理UI命令
	UISystem_ModuleRespon,//组件通知UI命令已处理完毕，可继续进行

	Game_NewRound,
	Game_RoundStart,
	
	Game_BombPlanted,
	Game_BombDefused,
	Game_Boomed,

	Game_RoundEnd,

	CameraSystem_CallSolution,
	CameraSystem_PlayingShutdown,

	Command_SpecPlayer//指定观战某个玩家，Param1存玩家索引
};

//MulNX消息基类
class MulNXMessage {
public:
	//消息类型
	MsgType Type;
	//Int型参数
	int ParamInt{};
	//类型擦除参数
	MulNXB::any_unique_ptr ParamData{};

	MulNXMessage() = delete;
	MulNXMessage(MsgType Type) :Type(Type) {}
	MulNXMessage(const MulNXMessage& Other)
		: Type(Other.Type),
		ParamInt(Other.ParamInt) {
		//调用真实类的拷贝构造函数，实现多态拷贝
		//至于是深拷贝还是浅拷贝由具体类型决定
		//提示，最好都实现为深拷贝
		this->ParamData = Other.ParamData.clone();
	}
	std::unique_ptr<MulNXMessage> Clone()const{
		return std::make_unique<MulNXMessage>(*this);
	}
};
