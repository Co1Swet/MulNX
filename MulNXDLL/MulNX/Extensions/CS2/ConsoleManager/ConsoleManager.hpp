#pragma once

#include"../../../Core/ModuleBase/ModuleBase.hpp"

class ConsoleManager final :public MulNX::ModuleBase {
private:
	int TargetTick = 0;
public:
	ConsoleManager(MulNX::Core* MulNXi) :MulNX::ModuleBase(MulNXi) {
		this->Type = MulNX::ModuleType::ConsoleManager;
	}

	bool Init()override;
	void VirtualMain()override;

	void Menu()override;
	
	void ShowFilePath();
};