#pragma once

#include"../../Core/ModuleBase/ModuleBase.hpp"

#include<atomic>

class VirtualUser final :public MulNX::ModuleBase {
private:
	ICameraSystem* CameraSystem = nullptr;
public:
	VirtualUser() : ModuleBase() {
		this->Type = MulNX::ModuleType::VirtualUser;
	}

	bool Init()override final;

	void VirtualMain()override final;
	void ProcessMsg(MulNX::Messaging::Message* Msg)override final;

	void Menu()override final;
};