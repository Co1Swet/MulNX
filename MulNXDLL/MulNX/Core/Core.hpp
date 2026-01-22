#pragma once
// 整个系统的核心，负责所有子模块的生命周期管理，初始化，主循环调用等
// 同时提供各子模块的接口访问
#include "../Base/Base.hpp"

// 前向声明实现类
class CoreImpl;

namespace MulNX {
    namespace Core {
        class Core {
        private:
            // 数据存储：

            // Impl的指针
            std::unique_ptr<CoreImpl> pImpl;
        public:
            // 获取核心实例
            static Core& GetInstance();
        private:
            // 构造函数
            Core();
        public:
            // 析构函数
            ~Core();
            // 删除拷贝构造
            Core(const Core&) = delete;
            // 删除拷贝赋值
            Core& operator=(const Core&) = delete;

            // 初始化
            void Init();
            // 主循环
            void VirtualMain();
            // 窗口逻辑
            void MulNXiMainWindow();

            // 获取子模块的接口
            MulNX::Messaging::IMessageManager& IMessageManager();
            MulNX::IUISystem& IUISystem();
            MulNX::GlobalVars& GlobalVars();
            MulNX::IPCer& IPCer();
            MulNX::KeyTracker& KT();
            MulNX::IHandleSystem& IHandleSystem();
            MulNX::IDebugger& IDebugger();

            CSController& CS();
            ConsoleManager& ConsoleManager();
            GameSettingsManager& GameSettingsManager();
            ICameraSystem& ICameraSystem();
            VirtualUser& VirtualUser();
            HookManager& HookManager();
            IAbstractLayer3D& IAbstractLayer3D();
            MiniMap& MiniMap();
            GameCfgManager& GameCfgManager();
        };
    }
}