#include"MulNXUISystem.hpp"

#include"../MulNXiCore.hpp"
#include"../MessageManager/IMessageManager.hpp"
#include"../IPCer/IPCer.hpp"

bool MulNXUISystem::Init() {
	this->ICreateMessageChannel();
	IMessageChannel* Channel = this->IGetMessageChannel();
	(*Channel)
		.Subscribe(MsgType::UISystem_ModulePush);
	return true;
}

void MulNXUISystem::ProcessMsg(MulNXMessage* Msg) {
	switch (Msg->Type) {
	case MsgType::UISystem_ModulePush: {
		auto UIMsg = static_cast<MulNXMessage*>(Msg);
		MulNXB::any_unique_ptr pCtx = std::move(UIMsg->ParamData);
		std::unique_ptr<MulNXSingleUIContext> pSContext = std::move(pCtx.to_unique<MulNXSingleUIContext>());
		this->UIContext.AddSingleContext(std::move(pSContext));
		break;
	}
	}
}

bool MulNXUISystem::InitUIStyle() {
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
void MulNXUISystem::d3dInit(IDXGISwapChain* _this) {
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

			/*this->imguiIniPath = this->MulNXi->IPCer().PathGet_Core() / "MulNXUIConfig.ini";
			static std::string str = this->imguiIniPath.generic_u16string()
			io.IniFilename = str.c_str();*/


			// 转换为GBK（供ImGui使用）
			this->imguiIniPath = this->MulNXi->IPCer().PathGet_Core() / "MulNXUIConfig.ini";

			//static std::string str2 = "C:/Users/lenovo/Desktop/测试/ImGui.ini";
			//static std::string str2 = PathToGBKForwardSlash(this->imguiIniPath);
			static std::string str1 = this->imguiIniPath.string();
			std::replace(str1.begin(), str1.end(), L'\\', L'/');
			io.IniFilename = str1.c_str();
			io.IniFilename = "C:/Users/lenovo/Desktop/测试/ImGui.ini";
			this->InitUIStyle();

			this->imguiIniPath = this->MulNXi->IPCer().PathGet_Core() / "MulNXUIConfig.ini";
			std::u8string u8path = this->imguiIniPath.u8string();
			static std::string utf8Path(u8path.begin(), u8path.end());
			std::replace(utf8Path.begin(), utf8Path.end(), '\\', '/');
			io.IniFilename = utf8Path.c_str();

			this->ImGuiInited = true;
		}


		this->d3dInited = true;
	}
}

HRESULT __stdcall MulNXUISystem::RenderBefore(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	this->d3dInit(swapChain);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return 0;
}
HRESULT __stdcall MulNXUISystem::RenderBehind(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	ImGui::EndFrame();
	ImGui::Render();
	this->pd3dContext->OMSetRenderTargets(1, &this->view, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return 0;
}
HRESULT __stdcall MulNXUISystem::Render(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	std::unique_lock lock(this->UIMtx);
	this->RenderBefore(swapChain, syncInterval, flags);

	this->MulNXi->VirtualMain();
	this->EntryProcessMsg();

	static bool OpThis = true;
	if (OpThis) {
		ImGui::Begin("异步UI", &OpThis);
		this->UIContext.Draw();
		ImGui::End();
	}
	
	this->RenderBehind(swapChain, syncInterval, flags);
	return 0;
}