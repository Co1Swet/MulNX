#include "MulNXUISystem.hpp"

#include "../MessageManager/IMessageManager.hpp"
#include "../IPCer/IPCer.hpp"
#include "../HandleSystem/HandleSystem.hpp"
#include "../KeyTracker/KeyTracker.hpp"

#include "../../Core/Core.hpp"

#include "../../../ThirdParty/All_ImGui.hpp"

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
		MulNX::Base::any_unique_ptr pEntryStr = this->MulNXi->IHandleSystem().ReleaseHandle(Msg->Handle);
		std::string* pStr = pEntryStr.get<std::string>();
		this->UIContext.EntryDraw = std::move(*pStr);
		break;
	}
	case MulNX::Messaging::MsgType::UISystem_ModulePush: {
		MulNX::Base::any_unique_ptr pCtx = this->MulNXi->IHandleSystem().ReleaseHandle(Msg->Handle);
		this->UIContext.AddSingleContext(Msg->Handle, std::move(pCtx));
		break;
	}
	}
}

bool MulNX::UISystem::InitUIStyle() {
	ImGuiStyle& style = ImGui::GetStyle();

	
	return true;
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
void MulNX::UISystem::d3dInit(IDXGISwapChain* _this) {
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
				"C:/Windows/Fonts/msyh.ttc",  // 微软雅黑字体路径
				16.0f,                        // 字体大小
				nullptr,                      // 使用默认配置
				io.Fonts->GetGlyphRangesChineseFull() // 加载所有中文字符
			);
			ImGui_ImplDX11_CreateDeviceObjects();

			// 转换为GBK（供ImGui使用）

			this->imguiIniPath = this->MulNXi->IPCer().PathGet_Core() / "MulNXUIConfig.ini";
			std::u8string u8path = this->imguiIniPath.u8string();
			static std::string utf8Path(u8path.begin(), u8path.end());
			std::replace(utf8Path.begin(), utf8Path.end(), '\\', '/');
			io.IniFilename = utf8Path.c_str();

			this->InitUIStyle();
			this->ImGuiInited = true;
		}


		this->d3dInited = true;
	}
}

HRESULT __stdcall MulNX::UISystem::RenderBefore(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	this->d3dInit(swapChain);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return 0;
}
HRESULT __stdcall MulNX::UISystem::RenderBehind(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	ImGui::EndFrame();
	ImGui::Render();
	this->pd3dContext->OMSetRenderTargets(1, &this->view, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return 0;
}
HRESULT __stdcall MulNX::UISystem::Render(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	std::unique_lock lock(this->UIMtx);
	this->RenderBefore(swapChain, syncInterval, flags);

	this->EntryProcessMsg();
	
	if (this->MulNXi->KT().CheckComboClick(VK_INSERT, 1)) {
		this->UIContext.Active = !this->UIContext.Active;
	}
	if(this->UIContext.Active) {
		this->MulNXi->VirtualMain();
		this->UIContext.Draw();
	}
	
	this->RenderBehind(swapChain, syncInterval, flags);
	return 0;
}