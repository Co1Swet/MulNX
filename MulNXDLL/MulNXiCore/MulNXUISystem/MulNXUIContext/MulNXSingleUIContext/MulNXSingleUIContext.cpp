#include"MulNXSingleUIContext.hpp"

#include"../../../MessageManager/IMessageManager.hpp"

void MulNXSingleUIContext::Draw() {
	for (const auto& It : this->Elements) {
		std::unique_ptr<MulNXMessage> Msg = It->Draw(this);
		if (Msg) {
			IMessageChannel* MsgChannel = IMessageManager::GetMessageChannel(this->HModule);
			if (MsgChannel) {
				MsgChannel->PushMessage(std::move(Msg));
			}
		}
	}
}
bool MulNXSingleUIContext::SendToOwner(std::unique_ptr<MulNXMessage> Msg) {
	this->OwnerMsgChannel->PushMessage(std::move(Msg));
	return true;
}
bool MulNXSingleUIContext::SendToOwner(MsgType Type, int ParamInt) {
	auto Msg = std::make_unique<MulNXMessage>(Type);
	Msg->ParamInt = ParamInt;
	return this->SendToOwner(std::move(Msg));
}
void MulNXSingleUIContext::AddElement(std::unique_ptr<MulNXUIFunction>&& Element) {
	this->Elements.push_back(std::move(Element));
}

MulNXB::any_unique_ptr MulNXSingleUIContext::Create(const ModuleBase* const MB) {
	auto SContext = MulNXB::make_any_unique<MulNXSingleUIContext>();
	MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
	SContextPtr->HModule = MB->HModule;
	SContextPtr->OwnerMsgChannel = MB->IGetMessageChannel();
	return SContext;
}