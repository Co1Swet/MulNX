#include"MulNXUIContext.hpp"

bool MulNXUIContext::CallSingleContext(const std::string& Name) {
	auto ItEntry = this->CallMap.find(Name);
	if (ItEntry == this->CallMap.end())return false;
	HContext& hContext = ItEntry->second;
	auto ItSContext = this->ContextMap.find(hContext);
	if (ItSContext == this->ContextMap.end())return false;
	MulNXB::any_unique_ptr& SContext = ItSContext->second;
	MulNXSingleUIContext* pSContext = SContext.get<MulNXSingleUIContext>();
	this->next = std::string{};
	pSContext->Draw();
	return true;
}

void MulNXUIContext::Draw() {
	this->next = this->EntryDraw;
	bool CallResult = true;
	while (CallResult) {
		CallResult = this->CallSingleContext(this->next);
	}

	/*for (const auto& It : this->ContextOrder) {
		auto& SContext = this->ContextMap[It];
		MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
		SContextPtr->Draw();
	}*/
}
void MulNXUIContext::AddSingleContext(HContext hContext, MulNXB::any_unique_ptr SContext) {
	MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
	this->CallMap[SContextPtr->name] = hContext;
	this->ContextOrder.push_back(hContext);
	SContextPtr->MainContext = this;
	this->ContextMap[hContext] = std::move(SContext);
}
MulNXSingleUIContext* MulNXUIContext::GetSingleContext(const HContext& hContext) {
	auto It = this->ContextMap.find(hContext);
	if (It != this->ContextMap.end()) {
		return It->second.get<MulNXSingleUIContext>();
	}
	return nullptr;
}