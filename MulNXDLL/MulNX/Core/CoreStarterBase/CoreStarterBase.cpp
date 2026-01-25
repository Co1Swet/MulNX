#include "CoreStarterBase.hpp"

#include "../CoreImpl.hpp"

using namespace MulNX::Core;

bool CoreStarterBase::SystemInit(CoreImpl* pImpl, MulNX::Core::Core* pCore) {
	// 无依赖核心基础模块初始化
	pImpl->ModuleManager.EntryInit(pCore);
	pImpl->MessageManager.EntryInit(pCore);
	pImpl->MessageManager.EntryCreateThread();// 包含线程创建
	pImpl->MessageManager.SetMyThreadDelta(10);// 注意，此模块内部动态调整频率
	pImpl->KT.EntryCreateThread();// 包含线程创建
	pImpl->KT.SetMyThreadDelta(3);
	pImpl->IPCer.EntryInit(pCore);
	pImpl->Debugger.EntryInit(pCore);
	pImpl->GlobalVars.EntryInit(pCore);

	// UI模块初始化
	pImpl->UISystem.EntryInit(pCore);

	return true;
}