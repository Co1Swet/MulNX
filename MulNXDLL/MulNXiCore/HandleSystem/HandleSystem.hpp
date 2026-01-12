#pragma once

#include"../ModuleBase/ModuleBase.hpp"

using HContext = MulNXHandle;
using HMessageChannel = MulNXHandle;
using HModule = MulNXHandle;

template<typename T>
concept Handles = std::derived_from<T, MulNXHandle>;

//句柄系统仅用于全局提供句柄标识，不真正管理资源生命周期
class HandleSystem final :public ModuleBase {
private:
	std::shared_mutex MapMutex{};
	std::unordered_map<MulNXHandle, MulNXB::any_unique_ptr>Resources{};
	inline static std::atomic<uint64_t> CurrentHandleValue = 16;
public:
	HandleSystem(MulNXiCore* MulNXi) :ModuleBase(MulNXi) {
		this->Type = ModuleType::HandleSystem;
	}
	//static HandleSystem& GetInstance();

	bool Init()override { return true; }

	template<Handles T>
	T CreateHandle() {
		T handle{};
		handle.Value = this->CurrentHandleValue.fetch_add(1);
		return handle;
	}

	template<Handles T>
	T RegisteHandle(MulNXB::any_unique_ptr Resource) {
		std::unique_lock lock(this->MapMutex);
		T handle{};
		handle.Value = this->CurrentHandleValue.fetch_add(1);
		this->Resources[handle] = std::move(Resource);
		return handle;
	}

	template<Handles T>
	MulNXB::any_unique_ptr ReleaseHandle(T Handle) {
		std::unique_lock lock(this->MapMutex);
		auto it = this->Resources.find(Handle);
		if (it == this->Resources.end()) {
			return nullptr;
		}
		MulNXB::any_unique_ptr temp = std::move(it->second);
		this->Resources.erase(it);
		return std::move(temp);
	}
};