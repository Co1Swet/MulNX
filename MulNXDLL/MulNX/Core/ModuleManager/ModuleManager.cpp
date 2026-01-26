#include "ModuleManager.hpp"
#include "../Core.hpp"
#include "../../Systems/HandleSystem/IHandleSystem.hpp"

#include <deque>

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

bool ModuleManager::RegisteModule(std::unique_ptr<MulNX::ModuleBase>&& Module, std::string&& Name, int Priority) {
	std::unique_lock lock(this->MyThreadMutex);
	if (!Module->HModule.IsValid()) {
		Module->HModule = MulNXHandle::CreateHandle();
	}
	MulNXHandle hModule = Module->HModule;
	this->Modules[hModule] = std::move(Module);
	this->NameToHandleMap[Name] = hModule;
	this->HandleToNameMap[hModule] = std::move(Name);
	this->InitPriority[Priority] = hModule;
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
	// 通过有序的初始化任务列表进行初始化，尽管Modules是无序的
	for (auto& [Priority, HModule] : this->InitPriority) {
		if (!this->Modules[HModule]->EntryInit(this->Core)) {
			return false;
		}
	}
	// 完成初始化
	return true;
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