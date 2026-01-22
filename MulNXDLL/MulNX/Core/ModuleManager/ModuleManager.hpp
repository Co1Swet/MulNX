#pragma once

#include "../../Base/Base.hpp"

namespace MulNX {
	namespace Core {
		// 模块管理器类，负责加载、卸载和管理各个模块
		class ModuleManager {
		private:
			// 存储已加载模块的映射
			std::unordered_map<std::string, std::unique_ptr<MulNX::ModuleBase>> loadedModules;
		public:
			// 加载模块
			bool LoadModule(const std::string& moduleName);
			// 卸载模块
			bool UnloadModule(const std::string& moduleName);
			// 获取模块指针
			MulNX::ModuleBase* GetModule(const std::string& moduleName);

			// 主循环调用所有模块的VirtualMain
			void PackedVirtualMain();
		};
	}

}