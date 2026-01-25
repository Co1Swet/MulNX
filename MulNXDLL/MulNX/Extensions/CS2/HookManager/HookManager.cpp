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
#include "../../../../ThirdParty/All_ImGui.hpp"

static bool AllowReHook = false;//允许重hook
bool HookManager::Init() {
	this->pInstance = this;
	return true;
}
void HookManager::StartAll() {
	this->MainMsgChannel = this->ICreateAndGetMessageChannel();
	this->ReHook = true;
	MH_Initialize();
	this->EntryCreateThread();
	this->SetMyThreadDelta(250);
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
		if (this->KT->CheckComboClick(VK_INSERT, 2)) {
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
		this->d3dInited = false;
		this->NeedReHook = true;

		//延迟重新创建Hook
		std::this_thread::sleep_for(std::chrono::seconds(1));
		this->CreateHook();

		//发送重新Hook消息
		MulNX::Messaging::Message Msg(MulNX::Messaging::MsgType::Core_ReHook);
		this->IPublish(std::move(Msg));

		ReHook = false;

		// UI系统主界面初始化
		auto StartString = MulNX::Base::make_any_unique<std::string>("MainDraw");
		MulNXHandle hStr = this->Core->IHandleSystem().RegisteHandle(std::move(StartString));
		MulNX::Messaging::Message StartMsg(MulNX::Messaging::MsgType::UISystem_Start);
		StartMsg.Handle = hStr;
		this->IPublish(std::move(StartMsg));
	}
}

void HookManager::ProcessMsg(MulNX::Messaging::Message* Msg) {
	switch (Msg->Type) {

	}
	return;
}

DWORD HookManager::CreateHook() {
	this->pSwapChain = nullptr;
	this->pd3dDevice = nullptr;
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
			&this->pSwapChain,
			&this->pd3dDevice,
			nullptr,
			nullptr);

		if (this->pSwapChain) {
			auto pVtable = (void***)(this->pSwapChain);
			auto Vtable = *pVtable;

			this->hkRelease.SetTarget(Vtable[2]);
			this->hkRelease.SetMyFunction([this](auto&&... args) {
				return this->MyRelease(std::forward<decltype(args)>(args)...); });
			this->hkRelease.CreateAndEnable();

			this->hkPresent.SetTarget(Vtable[8]);
			this->hkPresent.SetMyFunction([this](auto&&... args) {
				return this->MyPresent(std::forward<decltype(args)>(args)...); });
			this->hkPresent.CreateAndEnable();

			this->pd3dDevice->Release();
			this->pSwapChain->Release();

			this->Core->IUISystem().SetFrameBefore([this]()->void {

				this->d3dInit(this->pSwapChain);
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();

				return;
				});
			this->Core->IUISystem().SetFrameBehind([this]()->void {

				ImGui::EndFrame();
				ImGui::Render();
				this->pd3dContext->OMSetRenderTargets(1, &this->view, nullptr);
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				return;
				});

			this->GuardPleaseAction = false;
			this->NeedReHook = false;
		}
	}
	return 0;
}
bool HookManager::InitUIStyle() {
	ImGuiStyle& style = ImGui::GetStyle();

	return true;
}
void HookManager::d3dInit(IDXGISwapChain* _this) {
	if (!this->d3dInited) {
		_this->GetDevice(__uuidof(ID3D11Device), (void**)&this->pd3dDevice);
		this->pd3dDevice->GetImmediateContext(&this->pd3dContext);

		DXGI_SWAP_CHAIN_DESC sd;
		_this->GetDesc(&sd);
		this->CS2hWnd = sd.OutputWindow;

		ID3D11Texture2D* buf = nullptr;
		_this->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buf);
		if (buf == nullptr)return;
		this->pd3dDevice->CreateRenderTargetView(buf, nullptr, &this->view);
		buf->Release();



		if (!this->ImGuiInited) {


			ImGui::CreateContext();
			//设置ini文件路径
			ImGuiIO& io = ImGui::GetIO();


			ImGui_ImplWin32_Init(this->CS2hWnd);
			ImGui_ImplDX11_Init(this->pd3dDevice, this->pd3dContext);

			ImFont* font = io.Fonts->AddFontFromFileTTF(
				"C:/Windows/Fonts/msyh.ttc",				// 微软雅黑字体路径
				16.0f,										// 字体大小
				nullptr,									// 使用默认配置
				io.Fonts->GetGlyphRangesChineseFull()		// 加载所有中文字符
			);
			ImGui_ImplDX11_CreateDeviceObjects();

			// 转换为GBK（供ImGui使用）

			this->imguiIniPath = this->Core->IPCer().PathGet_Core() / "MulNXUIConfig.ini";
			this->imguiIniPathString = MulNX::Base::CharUtility::FilePathToString(this->imguiIniPath);
			io.IniFilename = this->imguiIniPathString.c_str();

			this->InitUIStyle();
			this->ImGuiInited = true;
		}


		this->d3dInited = true;
	}
}

LRESULT __stdcall HookManager::EntryMyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (HookManager::pInstance->MyWndProc(hwnd, uMsg, wParam, lParam)) {
		return true;
	}	
	return CallWindowProcW(HookManager::pInstance->OriginWndProc, hwnd, uMsg, wParam, lParam);
}



//ImGui窗口处理函数导入
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall HookManager::MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::unique_lock lock(this->Core->IUISystem().UIMtx);
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
		this->OriginWndProc = (WNDPROC)SetWindowLongPtrW(this->CS2hWnd, GWLP_WNDPROC, (LONG_PTR)HookManager::pInstance->EntryMyWndProc);
	if (this->GlobalVars->SystemReady) {
		this->pSwapChain = swapChain;
		this->Core->IUISystem().Render();
	}
	return 0;
}
ULONG __stdcall HookManager::MyRelease(IUnknown* pThis) {
	//MessageBoxW(NULL, L"交换链被释放", L"D3D11SwapChain", NULL);
	this->NeedReHook = true;
	this->GuardPleaseAction = true;

	return 0;
}