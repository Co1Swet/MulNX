#pragma once

#include "../ModuleBase/ModuleBase.hpp"

class CoreImpl;

namespace MulNX {
	namespace Core {
		// 核心启动器基类，定义核心启动的基本接口
		class CoreStarterBase :public MulNX::ModuleBase {
		public:
			bool Init() override {
				return true;
			}

			bool SystemInit(CoreImpl* pImpl, MulNX::Core::Core* pCore);

			std::function<void()>InitEndCall = nullptr;

			// 所有系统全部启动启动启动！！！
			virtual void StartAll(){}

			// 必须提供一个入口点，供渲染线程调用UI渲染
			virtual void EntryRender() {}
			// 必须提供一个入口点，供核心初始化完成后循环调用
			virtual void EntryLogic() {}
		};
	}
}