#pragma once

#include "../../Core/ModuleBase/ModuleBase.hpp"

namespace MulNX {
	class IHandleSystem :public ModuleBase {
	public:
		IHandleSystem() : ModuleBase() {
			//this->Type = ModuleType::HandleSystem;
		}
		
		virtual MulNXHandle RegisteHandle(MulNX::Base::any_unique_ptr Resource) = 0;
		virtual MulNX::Base::any_unique_ptr ReleaseHandle(MulNXHandle Handle) = 0;
	};
}