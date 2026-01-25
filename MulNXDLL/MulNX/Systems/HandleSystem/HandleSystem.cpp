#include "HandleSystem.hpp"

#include "../../Core/Core.hpp"

bool MulNX::HandleSystem::Init(){
	//无需初始化操作
	return true; 
}

MulNXHandle MulNX::HandleSystem::RegisteHandle(MulNX::Base::any_unique_ptr Resource) {
	std::unique_lock lock(this->MapMutex);
	MulNXHandle handle = MulNXHandle::CreateHandle();
	this->Resources[handle] = std::move(Resource);
	return handle;
}
MulNX::Base::any_unique_ptr MulNX::HandleSystem::ReleaseHandle(MulNXHandle Handle) {
	std::unique_lock lock(this->MapMutex);
	auto it = this->Resources.find(Handle);
	if (it == this->Resources.end()) {
		return nullptr;
	}
	MulNX::Base::any_unique_ptr temp = std::move(it->second);
	this->Resources.erase(it);
	return temp;
}