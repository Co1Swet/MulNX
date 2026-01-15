#include"../ModuleBase/ModuleBase.hpp"

#include"../HandleSystem/HandleSystem.hpp"

class MulNXSingleUIContext;
class TripleBufferBase;

class DemoHelper final :public ModuleBase {
private:
	std::vector<float>Marks{};
	std::atomic<MulNXB::any_unique_ptr*>* ppUpdateData = nullptr;
	MulNXHandle hContext{};
public:
	DemoHelper(MulNXiCore* MulNXi) :ModuleBase(MulNXi) {};

	bool Init()override;

	void VirtualMain()override;
	void ProcessMsg(MulNXMessage* Msg)override;

	void HandleUICommand(MulNXMessage* Msg);
	//void HandleUICommand(MulNXMessage* Msg);

	bool MarkTime();
	bool JumpMark(size_t Index);
};