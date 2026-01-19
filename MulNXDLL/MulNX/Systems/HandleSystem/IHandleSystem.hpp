#pragma once

#include "../../Core/ModuleBase/ModuleBase.hpp"

namespace MulNX {
	class IHandleSystem :public ModuleBase {
	public:
		IHandleSystem(MulNX::Core* Core) :ModuleBase(Core) {
			this->Type = ModuleType::HandleSystem;
		}

		virtual MulNXHandle CreateHandle() = 0;
		virtual MulNXHandle RegisteHandle(MulNXB::any_unique_ptr Resource) = 0;
		virtual MulNXB::any_unique_ptr ReleaseHandle(MulNXHandle Handle) = 0;
	};
}