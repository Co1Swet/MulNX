#pragma once

#include"../ModuleBase/ModuleBase.hpp"

#include"MulNXUIContext/MulNXUIContext.hpp"
#include"MulNXUIElements/MulNXUIElements.hpp"

class IMulNXUISystem :public ModuleBase {
public:
	std::recursive_mutex UIMtx;

	//D3D11指针组

	ID3D11Device* pd3dDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pd3dContext = nullptr;
	ID3D11RenderTargetView* view = nullptr;
	bool d3dInited = false;
	HWND CS2hWnd = nullptr;//CS2窗口句柄

	IMulNXUISystem(MulNXiCore* MulNXi) :ModuleBase(MulNXi) {
		this->Type = ModuleType::MulNXUISystem;
	}
	
	virtual bool InitUIStyle() = 0;
	virtual bool UIStyleInited()const = 0;

	virtual std::shared_mutex& GetMutex() = 0;

	virtual HRESULT __stdcall Render(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = 0;
};