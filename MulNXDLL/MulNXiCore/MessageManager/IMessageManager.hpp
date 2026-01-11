#pragma once

#include"MulNXMessage/MulNXMessage.hpp"
#include"../ModuleBase/ModuleBase.hpp"

#include<atomic>
#include<string>
#include<deque>
#include<unordered_set>
#include<shared_mutex>

class IMessageChannel {
public:
	virtual IMessageChannel& Subscribe(const MsgType MsgType) = 0;
	virtual IMessageChannel& Unsubscribe(const MsgType MsgType) = 0;
	virtual bool PushMessage(std::unique_ptr<MulNXMessage> InMsg) = 0;
	virtual bool PullMessage(std::unique_ptr<MulNXMessage>& OutMsg) = 0;
};

class IMessageManager :public ModuleBase {
	friend IMessageChannel;
	friend class MessageChannel;
public:
	IMessageManager(MulNXiCore* MulNXi) :ModuleBase(MulNXi) {
		this->Type = ModuleType::MessageManager;
	}

	//注册，获取消息指针
	virtual bool Registe(ModuleBase* const Module) = 0;
	//订阅，获得对应类型消息推送
	virtual bool Subscribe(const MsgType MsgType, ModuleBase* const Module) = 0;
	//创建私有消息队列（但是生命周期仍然委托给消息管理器）
	virtual bool CreateMessageChannel(ModuleBase* const Module) = 0;
	//获取消息管道
	static IMessageChannel* GetMessageChannel(const MulNXHandle& Handle);
	//取消订阅
	virtual bool Unsubscribe(const MsgType MsgType, ModuleBase* const Module) = 0;
	//发布，在堆空间创建消息后传递，所有权转移至总线
	virtual bool Publish(std::unique_ptr<MulNXMessage> const Msg) = 0;
	//释放，处理消息后必须调用，减少引用计数，直到为0后由总线释放空间
	virtual bool Release() = 0;
};