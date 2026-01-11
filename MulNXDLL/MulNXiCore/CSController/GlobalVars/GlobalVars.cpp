#include"GlobalVars.hpp"

void C_GlobalVars::Update() {
	MulNXB::Memory::Read(this->Address + 0x00, this->RealTime);
	MulNXB::Memory::Read(this->Address + 0x30, this->CurrentTime);
	MulNXB::Memory::Read(this->Address + 0x44, this->TickCount);

	return;
}
uintptr_t C_GlobalVars::GetCurrentTimePointer() {
	return this->Address + 0x30;
}