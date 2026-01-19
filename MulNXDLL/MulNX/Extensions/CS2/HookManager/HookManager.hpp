#pragma once

#include "../../../Core/ModuleBase/ModuleBase.hpp"
#include <d3d11.h>
#include <functional>

class HookManager final :public MulNX::ModuleBase {
    friend MulNX::Core;
    friend class MulNXiCoreImpl;
public:
    HookManager(MulNX::Core* MulNXi) :ModuleBase(MulNXi) {}
private:
    std::atomic<bool>GuardPleaseAction = false;
    

    //Hook Present
    using Present_t = HRESULT __stdcall(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
    MulNXB::HookUtility::HookUtility<Present_t>& hkPresent = MulNXB::HookUtility::HookUtility<Present_t>::GetInstance();
    HRESULT __stdcall MyPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
    //Hook Release
    using Release_t = ULONG __stdcall(IUnknown* pThis);
    MulNXB::HookUtility::HookUtility<Release_t>& hkRelease = MulNXB::HookUtility::HookUtility<Release_t>::GetInstance();
    ULONG __stdcall MyRelease(IUnknown* pThis);
    //窗口处理函数处理，这里不适用Hook
    WNDPROC OriginWndProc = nullptr;
    static LRESULT __stdcall EntryMyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT __stdcall MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    std::atomic<bool>ReHook = false;
    std::atomic<bool>NeedReHook = false;

    
    bool Init()override;
    void ThreadMain()override;
    void ProcessMsg(MulNX::Messaging::Message* Msg)override;

   

    DWORD CreateHook();//创建Hook

    //保护对 ImGui API 调用（NewFrame/Render/WndProcHandler 等）的互斥量，避免跨线程并发访问导致的数据竞争
    //使用递归互斥以允许在同一线程通过 Win32 消息回调重入时再次上锁（避免 resource_deadlock 异常）
    

    //持久化保存 imgui.ini 路径字符串，保证 io.IniFilename 指向有效内存
    
};