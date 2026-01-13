#include"MessageChannel.hpp"
#include"../MessageManager.hpp"

MessageChannel::MessageChannel(MessageManager* MsgManager) {
	this->MsgManager = MsgManager;
}
IMessageChannel& MessageChannel::Subscribe(const MsgType MsgType) {
	std::unique_lock<std::shared_mutex>lock(this->MsgManager->GetMutex());
	this->MsgManager->ChannelSubscribeMap[MsgType].push_back(this);
	return *this;
}
IMessageChannel& MessageChannel::Unsubscribe(const MsgType MsgType) {
	std::unique_lock lock(this->MsgManager->GetMutex());//独占锁
	auto MapIt = this->MsgManager->ChannelSubscribeMap.find(MsgType);//找订阅列表
	if (MapIt != this->MsgManager->ChannelSubscribeMap.end()) {
		auto& SubscriberVector = MapIt->second;//得到订阅列表容器
		auto VecIt = std::find(SubscriberVector.begin(), SubscriberVector.end(), this);//找到自己的位置
		if (VecIt != SubscriberVector.end()) {
			SubscriberVector.erase(VecIt);//找到则删除
			if (SubscriberVector.empty())this->MsgManager->ChannelSubscribeMap.erase(MapIt);//删除后如果为空，则删除这个条目
		}
	}
	return *this;
}
bool MessageChannel::PullMessage(MulNXMessage& OutMsg) {
	std::unique_lock lock(this->ChannelMutex);
	if (!this->Messages.empty()) {
		OutMsg = std::move(this->Messages.front());
		this->Messages.pop_front();
		return true;
	}
	return false;
}
bool MessageChannel::PushMessage(MulNXMessage&& Msg) {
	std::unique_lock lock(this->ChannelMutex);
	this->Messages.push_back(std::move(Msg));
	return true;
}