#include"ConsoleManager.hpp"

#include"../GameSettingsManager/GameSettingsManager.hpp"
#include"../GameCfgManager/GameCfgManager.hpp"

#include"../../../Core/Core.hpp"

#include"../../../Systems/Debugger/IDebugger.hpp"
#include"../../../Systems/IPCer/IPCer.hpp"
#include"../../../Systems/KeyTracker/KeyTracker.hpp"
#include"../../../Systems/MulNXiGlobalVars/MulNXiGlobalVars.hpp"

#include"../../../Extensions/MiniMap/MiniMap.hpp"

#include"../../../../ThirdParty/All_ImGui.hpp"

bool ConsoleManager::Init() {
	return true;
}

void ConsoleManager::Menu() {
	MulNX::AutoChild Child(this, 0.5f);//占据半个窗口高度，另一半给VirtualUser

	if (ImGui::CollapsingHeader("调试器控制")) {
		//调试功能设置
		//调试模式下提供更多功能，但可能影响性能和稳定性
		static bool debugMode = this->GlobalVars->DebugMode;
		if (ImGui::Checkbox("调试模式（Debug Mode），提供更多功能，但可能影响性能和稳定性", &debugMode)) {
			this->GlobalVars->DebugMode = debugMode;
		}
		if (ImGui::Button("解限所有CS2控制台变量")) {
			int Count = 0;
			this->MulNXi->CS().GetCvarSystem().UnlockHiddenCVars(Count);
			this->MulNXi->IDebugger().AddSucc("成功解限" + std::to_string(Count) + "个控制台命令！");
		}
		if (ImGui::Button("限住所有CS2控制台变量")) {
			int Count = 0;
			this->MulNXi->CS().GetCvarSystem().LockAllCvars(Count);
			this->MulNXi->IDebugger().AddSucc("成功限住" + std::to_string(Count) + "个控制台命令！");
		}
		if(ImGui::Button("列出所有CS2控制台变量")) {
			this->IDebugger->AddInfo("---------------------------------------------------------------------------------");
			uint64_t idx = 0;
			this->MulNXi->CS().GetCvarSystem().GetFirstCvarIterator(idx);
			while (idx!=0xFFFFFFFF) {
				C_ConVar* var = this->MulNXi->CS().GetCvarSystem().GetCVarByIndex(idx);
				if (var) {
					std::string Name = var->szName ? var->szName : "未知";
					this->IDebugger->AddInfo("控制台命令：" + Name);
				}
				this->MulNXi->CS().GetCvarSystem().GetNextCvarIterator(idx);
			}
			this->IDebugger->AddInfo("---------------------------------------------------------------------------------");
		}

		if (ImGui::Button("打开调试器")) {
			this->IDebugger->OpenWindow();
		}
		ImGui::SameLine();
		if (ImGui::Button("关闭调试器")) {
			this->IDebugger->CloseWindow();
		}
		ImGui::Checkbox("当有错误信息时弹出调试器", &this->IDebugger->ShowWhenError);
		ImGui::Checkbox("自动滚动到最新消息", &this->IDebugger->AutoScroll);
		static int MaxDebugMsgs = 1000;
		ImGui::Text("设置最大消息数量（至少100）:");
		ImGui::SameLine();
		ImGui::InputInt("##最大消息数量", &MaxDebugMsgs);
		ImGui::SameLine();
		if (ImGui::Button("应用")) {
			if (MaxDebugMsgs < 100)MaxDebugMsgs = 100;
			this->IDebugger->ResetMaxMsgCount(MaxDebugMsgs);
		}
	}

	if (ImGui::CollapsingHeader("初始化控制")) {
		if (ImGui::Button("初始化IPCer")) {
			this->IDebugger->AddInfo("正在尝试初始化IPCer");
			this->MulNXi->IPCer().Init();
		}
		ImGui::SameLine();
		if (ImGui::Button("查看IPCer结果")) {
			this->ShowFilePath();
		}
	}

	if (ImGui::CollapsingHeader("游戏配置管理器控制")) {
		if (ImGui::Button("打开游戏配置管理器")) {
			this->MulNXi->GameCfgManager().OpenWindow();
		}
		ImGui::SameLine();
		if (ImGui::Button("关闭游戏配置管理器")) {
			this->MulNXi->GameCfgManager().CloseWindow();
		}
	}

	if (ImGui::CollapsingHeader("小地图控制")) {
		if (ImGui::Button("打开小地图")) {
			this->MulNXi->MiniMap().OpenWindow();
		}
		ImGui::SameLine();
		if (ImGui::Button("关闭小地图")) {
			this->MulNXi->MiniMap().CloseWindow();
		}
	}


	//static bool aimbot = false;
	//ImGui::Checkbox("自瞄", &this->MulNXi->CS().AimbotEnable);

	/*ImGui::InputInt("要瞄准的实体索引", &this->MulNXi->CS().AimTargetIndex);
	constexpr KeyCheckPack pack1 = { VK_F1,true,false,false,2 };
	constexpr KeyCheckPack pack2 = { VK_F2,true,false,false,2 };
	constexpr KeyCheckPack pack3 = { VK_F3,true,false,false,2 };
	constexpr KeyCheckPack pack4 = { VK_F4,true,false,false,2 };
	constexpr KeyCheckPack pack5 = { VK_F5,true,false,false,2 };
	constexpr KeyCheckPack pack6 = { VK_F6,true,false,false,2 };
	constexpr KeyCheckPack pack7 = { VK_F7,true,false,false,2 };
	constexpr KeyCheckPack pack8 = { VK_F8,true,false,false,2 };
	constexpr KeyCheckPack pack9 = { VK_F9,true,false,false,2 };
	constexpr KeyCheckPack pack10 = { VK_F10,true,false,false,2 };

	if (this->MulNXi->KT().IsKeyPressed(VK_RBUTTON)) {
		this->MulNXi->CS().AimbotMain();
		if (this->MulNXi->KT().CheckWithPack(pack1)) {
			this->MulNXi->CS().AimTargetIndex = 1;
		}

		if (this->MulNXi->KT().CheckWithPack(pack2)) {
			this->MulNXi->CS().AimTargetIndex = 2;
		}

		if (this->MulNXi->KT().CheckWithPack(pack3)) {
			this->MulNXi->CS().AimTargetIndex = 3;
		}

		if (this->MulNXi->KT().CheckWithPack(pack4)) {
			this->MulNXi->CS().AimTargetIndex = 4;
		}

		if (this->MulNXi->KT().CheckWithPack(pack5)) {
			this->MulNXi->CS().AimTargetIndex = 5;
		}

		if (this->MulNXi->KT().CheckWithPack(pack6)) {
			this->MulNXi->CS().AimTargetIndex = 6;
		}

		if (this->MulNXi->KT().CheckWithPack(pack7)) {
			this->MulNXi->CS().AimTargetIndex = 7;
		}

		if (this->MulNXi->KT().CheckWithPack(pack8)) {
			this->MulNXi->CS().AimTargetIndex = 8;
		}

		if (this->MulNXi->KT().CheckWithPack(pack9)) {
			this->MulNXi->CS().AimTargetIndex = 9;
		}

		if (this->MulNXi->KT().CheckWithPack(pack10)) {
			this->MulNXi->CS().AimTargetIndex = 10;
		}
	}*/

	return;
}

void ConsoleManager::VirtualMain() {

}

void ConsoleManager::ShowFilePath() {
	if (this->MulNXi->IPCer().Inited) {
		this->IDebugger->AddInfo("---------------------------------------------------------------------------------");
		this->IDebugger->AddInfo(this->MulNXi->IPCer().GetAllPathMsg());
		this->IDebugger->AddInfo("---------------------------------------------------------------------------------");
	}
	else {
		this->IDebugger->AddError("IPCer尚未初始化成功！");
	}
}