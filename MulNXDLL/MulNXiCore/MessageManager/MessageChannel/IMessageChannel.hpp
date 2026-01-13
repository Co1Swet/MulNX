#pragma once

#include"../MulNXMessage/MulNXMessage.hpp"

class IMessageChannel {
public:
	virtual IMessageChannel& Subscribe(const MsgType MsgType) = 0;
	virtual IMessageChannel& Unsubscribe(const MsgType MsgType) = 0;
	virtual bool PushMessage(MulNXMessage&& InMsg) = 0;
	virtual bool PullMessage(MulNXMessage& OutMsg) = 0;
};