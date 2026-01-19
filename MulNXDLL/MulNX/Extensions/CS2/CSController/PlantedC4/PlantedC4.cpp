#include "PlantedC4.hpp"

#include "../../../../../ThirdParty/All_cs2_dumper.hpp"

void C_PlantedC4::Update() {
	if (!this->Address)return;
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_bBombTicking, this->m_bBombTicking);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_nBombSite, this->m_nBombSite);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_flC4Blow, this->m_flC4Blow);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_bHasExploded, this->m_bHasExploded);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_flTimerLength, this->m_flTimerLength);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_bBeingDefused, this->m_bBeingDefused);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_flDefuseLength, this->m_flDefuseLength);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_flDefuseCountDown, this->m_flDefuseCountDown);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_bBombDefused, this->m_bBombDefused);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_hBombDefuser, this->m_hBombDefuser);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_PlantedC4::m_fLastDefuseTime, this->m_fLastDefuseTime);

	return;
}