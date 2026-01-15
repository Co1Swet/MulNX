#pragma once

#include"IMessageChannel.hpp"

#include<deque>

class MessageChannel final :public IMessageChannel {
	friend class IMessageManager;
	friend class MessageManager;

	std::mutex ChannelMutex{};
	MessageManager* MsgManager = nullptr;
	//自身句柄
	MulNXHandle hChannel{};
	std::deque<MulNXMessage>Messages{};
	std::atomic<bool>bHasMessage = false;
public:
	MessageChannel(MessageManager* MsgManager);
private:
	IMessageChannel& Subscribe(const MsgType MsgType)override;
	IMessageChannel& Unsubscribe(const MsgType MsgType)override;
	bool PushMessage(MulNXMessage&& Msg)override;
	bool PullMessage(MulNXMessage& OutMsg)override;
	bool HasMessage()const override;
	MulNXHandle GetHandle()const override;
};