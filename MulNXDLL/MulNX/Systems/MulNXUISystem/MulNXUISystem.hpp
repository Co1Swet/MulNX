#pragma once

#include"IMulNXUISystem.hpp"

namespace MulNX {
	class UISystem final :public IUISystem {
	private:
		bool UIInited = false;
		std::shared_mutex UIMutex{};
		MulNXUIContext UIContext{};
		std::function<void(void)>FrameBefore = nullptr;
		std::function<void(void)>FrameBehind = nullptr;
	public:
		UISystem(MulNX::Core* MulNXi) :IUISystem(MulNXi) {
			this->Type = ModuleType::MulNXUISystem;
		}
		bool Init()override;

		void ProcessMsg(MulNX::Messaging::Message* Msg)override;

		bool InitUIStyle()override;
		bool UIStyleInited()const override { return this->UIInited; }

		std::shared_mutex& GetMutex()override { return this->UIMutex; }


		bool ImGuiInited = false;
		std::filesystem::path imguiIniPath;
		void d3dInit(IDXGISwapChain* _this);
		HRESULT __stdcall RenderBefore(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
		HRESULT __stdcall RenderBehind(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
		HRESULT __stdcall Render(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)override;

		MulNXSingleUIContext* GetSingleContext(const MulNXHandle& hContext)override {
			return this->UIContext.GetSingleContext(hContext);
		}
	};
}