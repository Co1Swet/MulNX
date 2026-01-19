#include "HookManager.hpp"

#include "../../../Core/Core.hpp"
#include "../../../Systems/IPCer/IPCer.hpp"
#include "../../../Systems/MessageManager/IMessageManager.hpp"
#include "../../../Systems/Debugger/IDebugger.hpp"
#include "../../../Systems/KeyTracker/KeyTracker.hpp"
#include "../../../Systems/MulNXiGlobalVars/MulNXiGlobalVars.hpp"
#include "../../../Systems/MulNXUISystem/IMulNXUISystem.hpp"

#include "../../../../ThirdParty/imgui_d11/imgui.h"

#include <chrono>

static bool AllowReHook = false;//允许重hook
bool HookManager::Init() {
	MH_Initialize();
	this->MainMsgChannel = this->ICreateAndGetMessageChannel();
	return true;
}

void HookManager::ThreadMain() {
	this->EntryProcessMsg();

	if (this->GuardPleaseAction) {
		AllowReHook = true;
		this->GuardPleaseAction = false;
		this->IDebugger->AddInfo("检测到D3D11波动，等待用户手动ReHook");
	}
	//检查是否超时：正在等待CheckBack且超过2秒未收到回复
	if (AllowReHook) {
		if (this->MulNXi->KT().CheckComboClick(VK_INSERT, 2)) {
			ReHook = true;
			AllowReHook = false;
		}
	}
	if (ReHook) {
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		//执行重新Hook逻辑
		//先清理Hook
		this->hkPresent.Clear();
		this->hkRelease.Clear();
		

		//重置状态
		this->MulNXi->IUISystem().d3dInited = false;
		this->NeedReHook = true;

		//延迟重新创建Hook
		std::this_thread::sleep_for(std::chrono::seconds(1));
		this->CreateHook();

		//发送重新Hook消息
		MulNX::Messaging::Message Msg(MulNX::Messaging::MsgType::Core_ReHook);
		this->IPublish(std::move(Msg));

		ReHook = false;
		this->MulNXi->GlobalVars().CoreReady = true;
	}
}

void HookManager::ProcessMsg(MulNX::Messaging::Message* Msg) {
	switch (Msg->Type) {

	}
	return;
}

DWORD HookManager::CreateHook() {
	this->MulNXi->IUISystem().pSwapChain = nullptr;
	this->MulNXi->IUISystem().pd3dDevice = nullptr;
	while (this->NeedReHook) {
		const unsigned level_count = 2;
		D3D_FEATURE_LEVEL levels[level_count] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
		DXGI_SWAP_CHAIN_DESC sd{};
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = GetForegroundWindow();
		sd.SampleDesc.Count = 1;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		HRESULT hRusult = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			levels,
			level_count,
			D3D11_SDK_VERSION,
			&sd,
			&this->MulNXi->IUISystem().pSwapChain,
			&this->MulNXi->IUISystem().pd3dDevice,
			nullptr,
			nullptr);

		if (this->MulNXi->IUISystem().pSwapChain) {
			auto pVtable = (void***)(this->MulNXi->IUISystem().pSwapChain);
			auto Vtable = *pVtable;

			this->hkRelease.SetTarget(Vtable[2]);
			this->hkRelease.SetMyFunction([this](auto&&... args) {
				return this->MyRelease(std::forward<decltype(args)>(args)...); });
			this->hkRelease.CreateAndEnable();

			this->hkPresent.SetTarget(Vtable[8]);
			this->hkPresent.SetMyFunction([this](auto&&... args) {
				return this->MyPresent(std::forward<decltype(args)>(args)...); });
			this->hkPresent.CreateAndEnable();

			this->MulNXi->IUISystem().pd3dDevice->Release();
			this->MulNXi->IUISystem().pSwapChain->Release();

			this->GuardPleaseAction = false;
			this->NeedReHook = false;
		}
	}
	return 0;
}

LRESULT __stdcall HookManager::EntryMyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (MulNX::Core::GetInstance().HookManager().MyWndProc(hwnd, uMsg, wParam, lParam)) {
		return true;
	}	
	return CallWindowProcW(MulNX::Core::GetInstance().HookManager().OriginWndProc, hwnd, uMsg, wParam, lParam);
}



//ImGui窗口处理函数导入
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall HookManager::MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::unique_lock lock(this->MulNXi->IUISystem().UIMtx);
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
		return true;
	}

	ImGuiIO& io = ImGui::GetIO();
	//鼠标：当ImGui想要捕获时总是拦截
	if (io.WantCaptureMouse && MulNX::Base::WIN32Msg::IsMouseMessage(uMsg)) {
		return true;
	}
	//键盘：只在WantTextInput为true时拦截（表示输入框激活）
	else if (io.WantTextInput && MulNX::Base::WIN32Msg::IsKeyboardMessage(uMsg)) {
		return true;
	}

	return false;
}
HRESULT __stdcall HookManager::MyPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	if(!this->OriginWndProc)
		this->OriginWndProc = (WNDPROC)SetWindowLongPtrW(this->MulNXi->IUISystem().CS2hWnd, GWLP_WNDPROC, (LONG_PTR)MulNX::Core::GetInstance().HookManager().EntryMyWndProc);
	if (this->MulNXi->GlobalVars().SystemReady) {
		this->MulNXi->IUISystem().Render(swapChain, syncInterval, flags);
	}
	return 0;
}
ULONG __stdcall HookManager::MyRelease(IUnknown* pThis) {
	//MessageBoxW(NULL, L"交换链被释放", L"D3D11SwapChain", NULL);
	this->NeedReHook = true;
	this->GuardPleaseAction = true;

	return 0;
}