#include"MulNXUIFunction.hpp"

#include"../../MessageManager/IMessageManager.hpp"

std::unique_ptr<MulNXMessage> MulNXUIFunction::Draw(MulNXSingleUIContext* SContext) {
	this->MyFunc(SContext);

	return nullptr;
}