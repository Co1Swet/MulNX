#pragma once

#include"../ModuleBase/ModuleBase.hpp"
#include"../MulNXiCore.hpp"

class HandleSystem final :public ModuleBase {
private:
	HandleSystem(MulNXiCore* MulNXi) :ModuleBase(MulNXi) {
		this->Type = ModuleType::HandleSystem;
	}
public:
	static HandleSystem& GetInstance() {
		static HandleSystem HandleSystem(&MulNXiCore::GetInstance());
		return HandleSystem;
	}

	bool Init()override{}
};