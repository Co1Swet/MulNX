#pragma once

#include"../ModuleBase/ModuleBase.hpp"

#include<atomic>

class VirtualUser final :public ModuleBase {
private:
	ICameraSystem* CameraSystem = nullptr;
public:
	VirtualUser(MulNXiCore* MulNXi) : ModuleBase(MulNXi) {
		this->Type = ModuleType::VirtualUser;
	}

	bool Init()override final;

	void VirtualMain()override final;
	void ProcessMsg(MulNXMessage* Msg)override final;

	void Menu()override final;
};