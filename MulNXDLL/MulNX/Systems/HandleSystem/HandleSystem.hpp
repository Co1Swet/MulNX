#pragma once

#include"IHandleSystem.hpp"

namespace MulNX {
	//句柄系统用于全局提供句柄标识，中转any_unique_ptr资源
	class HandleSystem final :public IHandleSystem {
	private:
		std::shared_mutex MapMutex{};
		std::unordered_map<MulNXHandle, MulNX::Base::any_unique_ptr>Resources{};
		inline static std::atomic<uint64_t> CurrentHandleValue = 16;
	public:
		HandleSystem(MulNX::Core* MulNXi) :IHandleSystem(MulNXi) {
			this->Type = ModuleType::HandleSystem;
		}

		bool Init()override;

		MulNXHandle CreateHandle();
		MulNXHandle RegisteHandle(MulNX::Base::any_unique_ptr Resource);
		MulNX::Base::any_unique_ptr ReleaseHandle(MulNXHandle Handle);
	};
}