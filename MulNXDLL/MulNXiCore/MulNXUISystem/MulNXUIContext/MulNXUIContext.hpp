#pragma once

#include"MulNXSingleUIContext/MulNXSingleUIContext.hpp"

#include<vector>

class MulNXUIContext {
public:
	
	std::vector<HContext>ContextOrder{};
	std::unordered_map<HContext, MulNXB::any_unique_ptr>ContextMap{};

	void Draw() {
		for (const auto& It : this->ContextOrder) {
			auto& SContext = this->ContextMap[It];
			MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
			SContextPtr->Draw();
		}
	}
	void AddSingleContext(HContext hContext, MulNXB::any_unique_ptr SContext) {
		MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
		this->ContextOrder.push_back(hContext);
		this->ContextMap[hContext] = std::move(SContext);
	}

	MulNXSingleUIContext* GetSingleContext(const HContext& hContext) {
		auto It = this->ContextMap.find(hContext);
		if (It != this->ContextMap.end()) {
			return It->second.get<MulNXSingleUIContext>();
		}
		return nullptr;
	}
};