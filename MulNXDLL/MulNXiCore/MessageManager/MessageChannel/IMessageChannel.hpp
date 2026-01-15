#pragma once

#include"../MulNXMessage/MulNXMessage.hpp"

class IMessageChannel {
public:
	virtual IMessageChannel& Subscribe(const MsgType MsgType) = 0;
	virtual IMessageChannel& Unsubscribe(const MsgType MsgType) = 0;
	virtual bool PushMessage(MulNXMessage&& InMsg) = 0;
	virtual bool PullMessage(MulNXMessage& OutMsg) = 0;

	virtual bool HasMessage()const = 0;

	virtual MulNXHandle GetHandle()const = 0;

	virtual ~IMessageChannel() = default;
};