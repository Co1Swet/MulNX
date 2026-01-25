#include "ModuleManager.hpp"
#include "../Core.hpp"
#include "../../Systems/HandleSystem/IHandleSystem.hpp"

using namespace MulNX::Core;

bool ModuleManager::Init() {
	
	return true;
}

void ModuleManager::VirtualMain() {
	this->PackedVirtualMain();
}
void ModuleManager::Windows() {
	this->PackedWindows();
}

bool ModuleManager::RegisteModule(std::unique_ptr<MulNX::ModuleBase>&& Module, std::string&& Name) {
	std::unique_lock lock(this->MyThreadMutex);
	if (!Module->HModule.IsValid()) {
		Module->HModule = MulNXHandle::CreateHandle();
	}
	MulNXHandle HModule = Module->HModule;
	this->Modules[HModule] = std::move(Module);
	this->NameToHandleMap[Name] = HModule;
	this->HandleToNameMap[HModule] = std::move(Name);
	return true;
}
MulNX::ModuleBase* ModuleManager::FindModule(const std::string& Name) {
	std::shared_lock lock(this->MyThreadMutex);
	auto it = this->NameToHandleMap.find(Name);
	if (it == this->NameToHandleMap.end()) {
		return nullptr;
	}
	MulNXHandle HModule = it->second;
	auto mit = this->Modules.find(HModule);
	if (mit == this->Modules.end()) {
		return nullptr;
	}
	return mit->second.get();
}

bool ModuleManager::PackedInit() {
	std::shared_lock lock(this->MyThreadMutex);
	for (auto& Module : this->Modules) {
		if (!Module.second->EntryInit(this->Core)) {
			return false;
		}
	}
}

void ModuleManager::PackedVirtualMain() {
	std::shared_lock lock(this->MyThreadMutex);
	for (auto& Module : this->Modules) {
		Module.second->EntryVirtualMain();
	}
}
void ModuleManager::PackedWindows() {
	std::shared_lock lock(this->MyThreadMutex);
	for (auto& Module : this->Modules) {
		Module.second->EntryWindows();
	}
}