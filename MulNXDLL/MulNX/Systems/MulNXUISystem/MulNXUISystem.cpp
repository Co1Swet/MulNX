#include "MulNXUISystem.hpp"

#include "../MessageManager/IMessageManager.hpp"
#include "../IPCer/IPCer.hpp"
#include "../HandleSystem/HandleSystem.hpp"
#include "../KeyTracker/KeyTracker.hpp"
#include "../MulNXiGlobalVars/MulNXiGlobalVars.hpp"

#include "../../Core/Core.hpp"

bool MulNX::UISystem::Init() {
	this->MainMsgChannel = this->ICreateAndGetMessageChannel();
	(*this->MainMsgChannel)
		.Subscribe(MulNX::Messaging::MsgType::UISystem_Start)
		.Subscribe(MulNX::Messaging::MsgType::UISystem_ModulePush);
	return true;
}

void MulNX::UISystem::ProcessMsg(MulNX::Messaging::Message* Msg) {
	switch (Msg->Type) {
	case MulNX::Messaging::MsgType::UISystem_Start: {
		MulNX::Base::any_unique_ptr pEntryStr = this->Core->IHandleSystem().ReleaseHandle(Msg->Handle);
		std::string* pStr = pEntryStr.get<std::string>();
		this->UIContext.EntryDraw = std::move(*pStr);
		break;
	}
	case MulNX::Messaging::MsgType::UISystem_ModulePush: {
		MulNX::Base::any_unique_ptr pCtx = this->Core->IHandleSystem().ReleaseHandle(Msg->Handle);
		this->UIContext.AddSingleContext(Msg->Handle, std::move(pCtx));
		break;
	}
	}
}

std::string PathToGBKForwardSlash(const std::filesystem::path& path) {
	// 方法1：通过wstring中转
	std::wstring wpath = path.wstring();  // 系统原生宽字符（Windows上是UTF-16）

	// 替换分隔符
	std::replace(wpath.begin(), wpath.end(), L'\\', L'/');

	// UTF-16 -> GBK
	int len = WideCharToMultiByte(CP_ACP, 0, wpath.c_str(), (int)wpath.length(),
		nullptr, 0, nullptr, nullptr);
	std::string gbkStr(len, 0);
	WideCharToMultiByte(CP_ACP, 0, wpath.c_str(), (int)wpath.length(),
		&gbkStr[0], len, nullptr, nullptr);
	return gbkStr;
}

int MulNX::UISystem::Render() {
	std::unique_lock lock(this->UIMtx);
	if (!this->Core->GlobalVars().SystemReady) {
		return 0;
	}
	this->EntryProcessMsg();
	
	this->FrameBefore();

	if (this->Core->KT().CheckComboClick(VK_INSERT, 1)) {
		this->UIContext.Active = !this->UIContext.Active;
	}
	if(this->UIContext.Active) {
		this->Core->VirtualMain();
		this->UIContext.Draw();
	}
	this->FrameBehind();

	return 0;
}

void MulNX::UISystem::SetFrameBefore(std::function<void(void)>Before) {
	this->FrameBefore = Before;
	return;
}
void MulNX::UISystem::SetFrameBehind(std::function<void(void)>Behind) {
	this->FrameBehind = Behind;
	return;
}