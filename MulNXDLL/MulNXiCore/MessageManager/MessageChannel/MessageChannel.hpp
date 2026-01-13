#pragma once

#include"IMessageChannel.hpp"

#include<deque>

class MessageChannel final :public IMessageChannel {
	friend class IMessageManager;
	friend class MessageManager;

	MessageManager* MsgManager = nullptr;

	std::mutex ChannelMutex{};
	std::deque<MulNXMessage>Messages{};
public:
	MessageChannel(MessageManager* MsgManager);
private:
	IMessageChannel& Subscribe(const MsgType MsgType)override;
	IMessageChannel& Unsubscribe(const MsgType MsgType)override;
	bool PushMessage(MulNXMessage&& Msg)override;
	bool PullMessage(MulNXMessage& OutMsg)override;
};