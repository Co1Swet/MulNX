#pragma once

#include "../../../Core/ModuleBase/ModuleBase.hpp"

#include "../../../Systems/HandleSystem/HandleSystem.hpp"

class MulNXSingleUIContext;
class TripleBufferBase;

class DemoHelper final :public MulNX::ModuleBase {
private:
	std::vector<float>Marks{};
	std::atomic<MulNX::Base::any_unique_ptr*>* ppUpdateData = nullptr;
	MulNXHandle hContext{};
public:
	DemoHelper() : ModuleBase() {};

	bool Init()override;

	void VirtualMain()override;
	void ProcessMsg(MulNX::Messaging::Message* Msg)override;

	void HandleUICommand(MulNX::Messaging::Message* Msg);
	//void HandleUICommand(MulNXMessage* Msg);

	bool MarkTime();
	bool JumpMark(size_t Index);
};