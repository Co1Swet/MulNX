#include"../ModuleBase/ModuleBase.hpp"

class MulNXUIMessage;

class DemoHelper final :public ModuleBase {
private:
	std::vector<float>Marks{};
public:
	DemoHelper(MulNXiCore* MulNXi) :ModuleBase(MulNXi) {};

	bool Init()override;

	void VirtualMain()override;
	void ProcessMsg(MulNXMessage* Msg)override;

	//void HandleUICommand(MulNXMessage* Msg);

	bool MarkTime();
	bool JumpMark(size_t Index);
};