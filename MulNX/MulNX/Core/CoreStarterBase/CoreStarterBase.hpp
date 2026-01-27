#pragma once

#include "../ModuleBase/ModuleBase.hpp"

class CoreImpl;
class MulNXSingleUIContext;

namespace MulNX {
	namespace Core {
		// 核心启动器基类，定义核心启动的基本接口
		class CoreStarterBase :public MulNX::ModuleBase {
		public:
			bool Init() override {
				return true;
			}

			bool SystemInit(CoreImpl* pImpl, MulNX::Core::Core* pCore);

			// UI系统启动辅助函数
			void StartUIWith(std::string&& EntryName);
			// 注册主绘制函数
			void RegisteMainDrawWith(std::function<void(MulNXSingleUIContext*)>&& MainDrawFunc);

			std::function<void()>InitEndCall = nullptr;

			// 所有系统全部启动启动启动！！！
			virtual void StartAll(){}
		};
	}
}