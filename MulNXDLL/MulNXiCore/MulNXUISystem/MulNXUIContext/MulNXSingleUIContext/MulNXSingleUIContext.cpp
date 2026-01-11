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
bool MulNXSingleUIContext::SendTothis(std::unique_ptr<MulNXMessage> Msg) {
	this->MsgChannel->PushMessage(std::move(Msg));
	return true;
}
bool MulNXSingleUIContext::SendTothis(MsgType Type) {
	auto Msg = std::make_unique<MulNXMessage>(Type);
	return this->SendTothis(std::move(Msg));
}
void MulNXSingleUIContext::AddElement(std::unique_ptr<MulNXUIFunction>&& Element) {
	this->Elements.push_back(std::move(Element));
}

std::unique_ptr<MulNXSingleUIContext> MulNXSingleUIContext::Create(const ModuleBase* const MB) {
	auto SContext = std::make_unique<MulNXSingleUIContext>();
	SContext->HModule = MB->HModule;
	IMessageChannel* pChannel = MB->IGetMessageChannel();
	if (pChannel) {
		SContext->MsgChannel = pChannel;
	}
	else {
		return nullptr;
	}
	return std::move(SContext);
}