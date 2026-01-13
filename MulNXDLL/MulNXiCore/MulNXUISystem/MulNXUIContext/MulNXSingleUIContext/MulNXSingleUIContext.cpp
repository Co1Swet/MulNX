#include"MulNXSingleUIContext.hpp"

#include"../../../MessageManager/IMessageManager.hpp"

#include"../MulNXUIContext.hpp"

void MulNXSingleUIContext::Draw() {
	this->MyFunc(this);
	//for (const auto& It : this->Elements) {
	//	//std::unique_ptr<MulNXMessage> Msg = 
	//		It->Draw(this);
	//	/*if (Msg) {
	//		IMessageChannel* MsgChannel = IMessageManager::GetMessageChannel(this->HModule);
	//		if (MsgChannel) {
	//			MsgChannel->PushMessage(std::move(Msg));
	//		}
	//	}*/
	//}
}
bool MulNXSingleUIContext::SetNextSingleUIContext(std::string&& Name) {
	this->MainContext->next = std::move(Name);
	return true;
}

bool MulNXSingleUIContext::SendToOwner(MulNXMessage&& Msg) {
	this->OwnerMsgChannel->PushMessage(std::move(Msg));
	return true;
}
bool MulNXSingleUIContext::SendToOwner(MsgType Type, uint32_t SubType) {
	MulNXMessage Msg(Type);
	Msg.SubType = SubType;
	return this->SendToOwner(std::move(Msg));
}


MulNXB::any_unique_ptr MulNXSingleUIContext::Create(const ModuleBase* const MB) {
	auto SContext = MulNXB::make_any_unique<MulNXSingleUIContext>();
	MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
	SContextPtr->HModule = MB->HModule;
	SContextPtr->OwnerMsgChannel = MB->IGetMessageChannel();
	return SContext;
}