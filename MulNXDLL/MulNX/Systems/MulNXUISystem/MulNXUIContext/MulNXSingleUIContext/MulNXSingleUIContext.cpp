#include "MulNXSingleUIContext.hpp"

#include "../MulNXUIContext.hpp"

#include "../../../MessageManager/IMessageManager.hpp"

#include "../../../../Core/Core.hpp"

void MulNXSingleUIContext::Draw() {
	if (this->MyMsgChannel->HasMessage()) {
		MulNX::Messaging::Message Msg(MulNX::Messaging::MsgType::Null);
		while(this->MyMsgChannel->PullMessage(Msg)){
			if(Msg.Type == MulNX::Messaging::MsgType::UISystem_ModuleRespose){
				this->WaitingResponse = false;
			}
		}
	}
	this->MyFunc(this);
}
bool MulNXSingleUIContext::CallSingleUIContext(std::string&& Name) {
	return this->MainContext->CallSingleContext(Name);
}
bool MulNXSingleUIContext::SetNextSingleUIContext(std::string&& Name) {
	this->MainContext->next = std::move(Name);
	return true;
}

bool MulNXSingleUIContext::SendToOwner(MulNX::Messaging::Message&& Msg) {
	if (this->WaitingResponse) {
		this->MainContext->EnableErrorHandle = true;
		return false;
	}
	this->OwnerMsgChannel->PushMessage(std::move(Msg));
	this->WaitingResponse = true;
	return true;
}
MulNX::Messaging::Message MulNXSingleUIContext::CreateMsg(uint32_t SubType) {
	MulNX::Messaging::Message Msg(MulNX::Messaging::MsgType::UISystem_UICommand);
	Msg.SubType = SubType;
	Msg.pMsgChannel = this->MyMsgChannel;
	return Msg;
}

MulNX::Base::any_unique_ptr MulNXSingleUIContext::Create(const MulNX::ModuleBase* const MB) {
	auto SContext = MulNX::Base::make_any_unique<MulNXSingleUIContext>();
	MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
	SContextPtr->HModule = MB->HModule;
	SContextPtr->OwnerMsgChannel = MB->MainMsgChannel;
	SContextPtr->MyMsgChannel = MulNX::Core::Core::GetInstance().IMessageManager()
		.GetMessageChannel(
			MulNX::Core::Core::GetInstance()
			.IMessageManager()
			.CreateMessageChannel()
		);
	return SContext;
}