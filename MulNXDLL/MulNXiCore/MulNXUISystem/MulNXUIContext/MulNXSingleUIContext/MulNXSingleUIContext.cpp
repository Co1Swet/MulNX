#include"MulNXSingleUIContext.hpp"

#include"../MulNXUIContext.hpp"

#include"../../../../MulNXiCore/MulNXiCore.hpp"
#include"../../../MessageManager/IMessageManager.hpp"

void MulNXSingleUIContext::Draw() {
	if (this->MyMsgChannel->HasMessage()) {
		MulNXMessage Msg(MsgType::Null);
		while(this->MyMsgChannel->PullMessage(Msg)){
			if(Msg.Type == MsgType::UISystem_ModuleRespose){
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

bool MulNXSingleUIContext::SendToOwner(MulNXMessage&& Msg) {
	if (this->WaitingResponse) {
		this->MainContext->EnableErrorHandle = true;
		return false;
	}
	this->OwnerMsgChannel->PushMessage(std::move(Msg));
	this->WaitingResponse = true;
	return true;
}
MulNXMessage MulNXSingleUIContext::CreateMsg(uint32_t SubType) {
	MulNXMessage Msg(MsgType::UISystem_UICommand);
	Msg.SubType = SubType;
	Msg.pMsgChannel = this->MyMsgChannel;
	return Msg;
}

MulNXB::any_unique_ptr MulNXSingleUIContext::Create(const ModuleBase* const MB) {
	auto SContext = MulNXB::make_any_unique<MulNXSingleUIContext>();
	MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
	SContextPtr->HModule = MB->HModule;
	SContextPtr->OwnerMsgChannel = MB->MainMsgChannel;
	SContextPtr->MyMsgChannel = MulNXiCore::GetInstance().IMessageManager()
		.GetMessageChannel(
			MulNXiCore::GetInstance()
			.IMessageManager()
			.CreateMessageChannel()
		);
	return SContext;
}