//所有组件的控制点，负责启停各个组件的各个功能，不涉及生命周期管理

#pragma once

#include"../ModuleBase/ModuleBase.hpp"

class ConsoleManager final :public ModuleBase {
private:
	int TargetTick = 0;
public:
	ConsoleManager(MulNXiCore* MulNXi) :ModuleBase(MulNXi) {
		this->Type = ModuleType::ConsoleManager;
	}

	bool Init()override;
	void VirtualMain()override;

	void Menu()override;
	
	void ShowFilePath();
};