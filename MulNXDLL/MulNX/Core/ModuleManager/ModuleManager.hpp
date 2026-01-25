#pragma once

#include "../ModuleBase/ModuleBase.hpp"

namespace MulNX {
	namespace Core {
		// 模块管理器类，负责加载、卸载和管理各个模块
		class ModuleManager final :public MulNX::ModuleBase {
		private:
			// 存储已加载模块的映射
			std::unordered_map<MulNXHandle, std::unique_ptr<MulNX::ModuleBase>> Modules;
			// 存储从字符串到模块句柄的映射，便于按名称查找
			std::unordered_map<std::string, MulNXHandle> NameToHandleMap;
			// 存储从模块句柄到字符串的映射，便于按句柄查找名称
			std::unordered_map<MulNXHandle, std::string> HandleToNameMap;
		public:
			bool Init()override;
			void VirtualMain()override;
			void Windows()override;

			// 注册模块
			bool RegisteModule(std::unique_ptr<MulNX::ModuleBase>&& Module, std::string&& Name);
			// 根据名称获取模块指针
			MulNX::ModuleBase* FindModule(const std::string& Name);

			// 初始化最后部分使用
			bool PackedInit();
			// 主循环调用所有模块的VirtualMain
			void PackedVirtualMain();
			// 对窗口入口的包装
			void PackedWindows();
		};
	}

}