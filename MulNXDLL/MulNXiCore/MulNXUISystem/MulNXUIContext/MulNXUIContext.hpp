#pragma once

#include"MulNXSingleUIContext/MulNXSingleUIContext.hpp"

#include<vector>

class MulNXUIContext {
public:
	
	std::vector<std::unique_ptr<MulNXSingleUIContext>>Singles{};

	void Draw() {
		for (auto& It : this->Singles) {
			It->Draw();
		}
	}
	void AddSingleContext(std::unique_ptr<MulNXSingleUIContext>&& SContext) {
		this->Singles.push_back(std::move(SContext));
	}
};