#pragma once

#include"Debugger.hpp"

#include"../MulNXiCore.hpp"
#include"../CSController/CSController.hpp"
#include"../CameraSystem/ICameraSystem.hpp"
#include"../KeyTracker/KeyTracker.hpp"
#include"../MulNXiGlobalVars/MulNXiGlobalVars.hpp"
#include"../AbstractLayer3D/IAbstractLayer3D.hpp"
#include <bitset>

bool Debugger::Init() {
    return true;
}



//// 检查所有点是否都在屏幕内
//bool allPointsVisible = true;
//    for (int i = 0; i < 5; i++) {
//    if (points[i].x < 0 || points[i].y < 0) {
//        allPointsVisible = false;
//        break;
//    }
//}
//
//if (!allPointsVisible) {
//    // 如果有任何点在屏幕外，可以选择不绘制或只绘制可见部分
//    return;
//}


void Debugger::Menu() {
	AutoChild Child(this);

    if (!this->MulNXiGlobalVars->DebugMode) {
		ImGui::Text("调试模式未启用，无法使用该功能，请前往控制台打开");
        return;
    }
    // 显示当前位置和旋转
    ImGui::Text("本地位置: X: %.6f, Y: %.6f, Z: %.6f",
        this->MulNXi->CS().GetLocalPlayer().GetPosition().x,
        this->MulNXi->CS().GetLocalPlayer().GetPosition().y,
        this->MulNXi->CS().GetLocalPlayer().GetPosition().z);
    ImGui::Text("本地欧拉角: Pitch: %.6f, Yaw: %.6f, Roll: %.6f",
        this->MulNXi->CS().GetLocalPlayer().GetRotationEuler().x,
        this->MulNXi->CS().GetLocalPlayer().GetRotationEuler().y,
        this->MulNXi->CS().GetLocalPlayer().GetRotationEuler().z);

    int FOV = this->MulNXi->CS().LocalPlayer.Entity.Pawn.CameraServices.iFOV;
    ImGui::Text("FOV:%d", FOV);
    int FOVStart = this->MulNXi->CS().LocalPlayer.Entity.Pawn.CameraServices.FOVStart;
    ImGui::Text("FOVStart:%d", FOVStart);
    float FOVRate = this->MulNXi->CS().LocalPlayer.Entity.Pawn.CameraServices.FOVRate;
    ImGui::Text("FOVRate:%.4f", FOVRate);
    float FOVTime = this->MulNXi->CS().LocalPlayer.Entity.Pawn.CameraServices.FOVTime;
    ImGui::Text("FOVTime:%.4f", FOVTime);
    static bool F1 = false, F2 = false;
    ImGui::Checkbox("F1", &F1);
    ImGui::Checkbox("F2", &F2);

    ImGui::Separator();
    ImGui::Separator();

    
    if (this->MulNXi->CS().LocalPlayer.pGlobalFOV) {
        int GlobalFOV = *this->MulNXi->CS().LocalPlayer.pGlobalFOV;
        static bool GlobalFOVControl = false;
        ImGui::Checkbox("GlobalFOVControl", &GlobalFOVControl);

        static float inputGFOV = 0;
        ImGui::SliderFloat("全局FOV", &inputGFOV, 0, 360);

        if (GlobalFOVControl) {
            *this->MulNXi->CS().LocalPlayer.pGlobalFOV = inputGFOV;
        }
    }
    
    ImGui::Separator();
    ImGui::Separator();

    static DirectX::XMFLOAT3 TestPosition;
    static DirectX::XMFLOAT3 TestRotationEuler;
    static DirectX::XMFLOAT4 TestRotation4;

    // 检测K键按下
    if (ImGui::IsKeyPressed(ImGuiKey_K)) {
        TestPosition = this->MulNXi->CS().GetLocalPlayer().GetPosition();
        TestRotationEuler = this->MulNXi->CS().GetLocalPlayer().GetRotationEuler();
        MulNXB::Math::CSEulerToQuat(TestRotationEuler, TestRotation4);
    }

    // 显示存储的测试值
    ImGui::Separator();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "按下K存储数值:");
    ImGui::Text("新位置: X: %.6f, Y: %.6f, Z: %.6f",
        TestPosition.x, TestPosition.y, TestPosition.z);
    ImGui::Text("新欧拉角: X: %.6f, Y: %.6f, Z: %.6f",
        TestRotationEuler.x, TestRotationEuler.y, TestRotationEuler.z);
    ImGui::Text("新四元数: X: %.6f, Y: %.6f, Z: %.6f, W: %.6f",
        TestRotation4.x, TestRotation4.y, TestRotation4.z, TestRotation4.w);

    DirectX::XMFLOAT3 back{};
	MulNXB::Math::CSQuatToEuler(TestRotation4, back);
    ImGui::Text("回转欧拉角：X: %.6f, Y: %.6f, Z: %.6f",
        back.x, back.y, back.z);


    
    ImGui::Separator();
    ImGui::Separator();
    ImGui::Separator();

    static float CameraHigh = 20.0;
    static float CameraX = 30.0;
    static float CameraY = 15.0;
    static float AxisLenth = 10.0;

    ImGui::SliderFloat("摄像机高度", &CameraHigh, 1.0, 500.0);
    ImGui::SliderFloat("摄像机长度", &CameraX, 1.0, 500.0);
    ImGui::SliderFloat("摄像机宽度", &CameraY, 1.0, 500.0);
    ImGui::SliderFloat("摄像机坐标轴长度", &AxisLenth, 1.0, 100.0);

    ImGui::SliderFloat3("位置", &TestPosition.x, -2000.0, 2000, 0);
    ImGui::SliderFloat("俯仰角", &TestRotationEuler.x, -89.0, 89.0);
    ImGui::SliderFloat("偏航角", &TestRotationEuler.y, -179.0, 179.0);
    ImGui::SliderFloat("滚转角", &TestRotationEuler.z, -179.0, 179.0);

    static ImVec4 cameraColourVec = ImVec4(1.0f, 0.0f, 1.0f, 1.0f); // RGBA，每个分量0-1
    ImGui::ColorEdit4("相机颜色", (float*)&cameraColourVec);

    

    // 将ImVec4转换为ImU32
    ImU32 cameraColour = IM_COL32(
        (int)(cameraColourVec.x * 255),
        (int)(cameraColourVec.y * 255),
        (int)(cameraColourVec.z * 255),
        (int)(cameraColourVec.w * 255)
    );

    //this->MulNXi->CameraSystem().CameraDrawer.Init(CameraHigh, CameraX, CameraY, AxisLenth, cameraColour);
    //this->MulNXi->CameraSystem().CameraDrawer.Update(this->MulNXi->CS().CSGetMatrix(), this->MulNXi->CS().CSGetScreenWide(), this->MulNXi->CS().CSGetScreenHigh());
    //this->MulNXi->CameraSystem().CameraDrawer.DrawCamera(TestPosition, TestRotationEuler, "旧测试");
    this->MulNXi->ICameraSystem().ResetCameraModule(CameraHigh, CameraX, CameraY, AxisLenth, cameraColour);
    this->MulNXi->ICameraSystem().DrawCameraByPAR(TestPosition, TestRotationEuler, "测试摄像机");

    ImGui::Separator();
    ImGui::Separator();
    ImGui::Separator();
    
    
    ImGui::Text(("OB模式：" + std::to_string(this->MulNXi->CS().LocalPlayer.Entity.Pawn.m_pObserverServices.m_iObserverMode)).c_str());
    ImGui::Text(("m_hObserverTarget：" + std::to_string(this->MulNXi->CS().LocalPlayer.Entity.Pawn.m_pObserverServices.m_hObserverTarget)).c_str());
    ImGui::Text(("m_iObserverLastMode：" + std::to_string(this->MulNXi->CS().LocalPlayer.Entity.Pawn.m_pObserverServices.m_iObserverLastMode)).c_str());
    ImGui::Text(("m_bForcedObserverMode：" + std::to_string(this->MulNXi->CS().LocalPlayer.Entity.Pawn.m_pObserverServices.m_bForcedObserverMode)).c_str());
    ImGui::Text(("m_flObserverChaseDistance：" + std::to_string(this->MulNXi->CS().LocalPlayer.Entity.Pawn.m_pObserverServices.m_flObserverChaseDistance)).c_str());
    ImGui::Text(("m_flObserverChaseDistanceCalcTime：" + std::to_string(this->MulNXi->CS().LocalPlayer.Entity.Pawn.m_pObserverServices.m_flObserverChaseDistanceCalcTime)).c_str());
    
    return;
}



void Debugger::ResetMaxMsgCount(const int Max) {
    if (Max < 1) {
		this->AddError("最大信息条数不能小于一1!");
        return;
    }
    if (DebugMsg.size() > Max) {
        DebugMsg.erase(DebugMsg.begin(), DebugMsg.end() - Max);
    }
	this->MaxMsgCount = Max;
	this->AddInfo("已重置最大信息条数为 " + std::to_string(Max) + " 条");

    return;
}

void Debugger::PushBack(const std::string& NewMsg, const std::string& prefix) {
	//这里不需要锁，因为调用此函数的上层函数已经加锁
    //检查字符串是否包含换行符
    if (NewMsg.find('\n') == std::string::npos && NewMsg.find('\r') == std::string::npos) {
        //没有换行符，直接添加（注意：这里需要加上前缀）
        if (this->DebugMsg.size() == this->MaxMsgCount) {
            this->DebugMsg.pop_front();
        }
        this->DebugMsg.push_back(prefix + NewMsg);
    }
    else {
        //有换行符，分割字符串并为每一行添加前缀
        std::istringstream iss(NewMsg);
        std::string line;
        bool firstLine = true;
        int lineCount = 0;

        while (std::getline(iss, line)) {
            //清理回车符
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            //跳过空行
            if (line.empty()) continue;

            std::string formattedLine;
            if (firstLine) {
                //第一行直接添加前缀
                formattedLine = prefix + line;
                firstLine = false;
            }
            else {
                //后续行添加前缀//和缩进
                formattedLine = prefix + line;
            }

            //添加到消息队列
            if (this->DebugMsg.size() == this->MaxMsgCount) {
                this->DebugMsg.pop_front();
            }
            this->DebugMsg.push_back(formattedLine);
            lineCount++;
        }
    }

    if (AutoScroll) {
        this->NeedAutoScroll = true;
    }
    return;
}

void Debugger::AddInfo(const std::string& NewMsg) {
    std::unique_lock<std::shared_mutex>lock(this->MyThreadMutex);
    this->PushBack(NewMsg, this->Info);
}

void Debugger::AddSucc(const std::string& NewMsg) {
    std::unique_lock<std::shared_mutex>lock(this->MyThreadMutex);
    this->PushBack(NewMsg, this->Succ);
}

void Debugger::AddWarning(const std::string& NewMsg) {
    std::unique_lock<std::shared_mutex>lock(this->MyThreadMutex);
    this->PushBack(NewMsg, this->Warning);
}

void Debugger::AddError(const std::string& NewMsg) {
    std::unique_lock<std::shared_mutex>lock(this->MyThreadMutex);
    this->PushBack(NewMsg, this->Error);
    if(this->ShowWhenError){
        this->ShowWindow = true;
        this->IfShowStream = true;
    }
}


void Debugger::Windows() {
    if (!this->ShowWindow)return;
    // 设置窗口属性
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    // 创建独立窗口
    ImGui::Begin("调试器", &this->ShowWindow);

    if (ImGui::BeginTabBar("Debuger")) {
        
        if (ImGui::BeginTabItem("调试信息")) {
            this->MenuStream();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("监控")) {
            this->MenuMonitor();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
 
    ImGui::End();  
}

void Debugger::MenuStream() {
    std::shared_lock lock(this->MyThreadMutex);
    // 在标签页内创建一个子窗口
    ImVec2 childSize = ImGui::GetContentRegionAvail();
    childSize.y -= ImGui::GetStyle().ItemSpacing.y; // 留出一点空间

	
    // 开始子窗口，占据标签页的剩余空间
    ImGui::BeginChild("信息", childSize, true, ImGuiWindowFlags_HorizontalScrollbar);

    // 使用虚拟列表优化性能
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(DebugMsg.size()));
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            const auto& msg = DebugMsg[i];

            // 根据消息类型着色
            if (msg.find(Info) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(50, 50, 255, 255));
            }
            else if (msg.find(Succ) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 100, 0, 255));
            }
            else if (msg.find(Warning) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 0, 255));
            }
            else if (msg.find(Error) == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 50, 50, 255));
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
            }

            ImGui::TextUnformatted(msg.c_str());

            //弹出
            ImGui::PopStyleColor();
        }
    }

    // 自动滚动到最新消息
    if (this->NeedAutoScroll) {
        ImGui::SetScrollHereY(1.0f);
		this->NeedAutoScroll = false;
    }

    // 结束子窗口
    ImGui::EndChild();
}

void Debugger::ShowStream() {
    this->IfShowStream = true;
}

void Debugger::HideStream() {
    this->IfShowStream = false;
}

void Debugger::MenuMonitor() {
    std::shared_lock lock(this->MyThreadMutex);
	ImGui::Text("调试监控");
    static int Result = 0;
    Result = this->MulNXi->CS().GetMsgResult;
    ImGui::Text("CSController信息获取返回值: %d", Result);
    if (Result < 0) {
        this->AddError("成功捕获一次内核错误！");
    }
    if (!this->MulNXiGlobalVars->DebugMode) {
        ImGui::Text("调试模式未启用，无法使用该功能，请前往控制台打开");
        return;
    }

    ImGui::Separator();

    ImGui::Text("本地测试");
    std::ostringstream Msg = this->MulNXi->CS().GetLocalPlayer().GetMsg();
    ImGui::Text(Msg.str().c_str());

    ImGui::Separator();
    ImGui::Separator();

    ImGui::Text("当前时间： %f", this->MulNXi->CS().CSGetCurrentTime());

    ImGui::Separator();

    float C4BlowTime = this->MulNXi->CS().PlantedC4.m_flC4Blow;
	ImGui::Text("炸弹爆炸时间： %f", C4BlowTime);
	ImGui::Text("倒计时: %f", this->MulNXi->IAbstractLayer3D().GetPhaseRemainingTime());

    ImGui::Separator();
    ImGui::Separator();
    // 基本游戏状态
	C_CSGameRules CSGameRules = this->MulNXi->CS().GetCSGameRules();

    ImGui::Text("冻结期： %s", CSGameRules.m_bFreezePeriod ? "是" : "否");
    ImGui::Text("热身期： %s", CSGameRules.m_bWarmupPeriod ? "是" : "否");
    ImGui::Separator();
    ImGui::Text("热身开始时间： %f", CSGameRules.m_fWarmupPeriodStart);
    ImGui::Text("热身结束时间： %f", CSGameRules.m_fWarmupPeriodEnd);
    static int TargetSkipTickForRenderTick = 0;
	ImGui::InputInt("TargetSkipTickForRenderTick", &TargetSkipTickForRenderTick);
    if(ImGui::Button("应用TargetSkipTickForRenderTick")){
        static int TickSchemaed = 0;
        TickSchemaed = TargetSkipTickForRenderTick - CSGameRules.m_fWarmupPeriodEnd * 64;
		this->MulNXi->CS().Execute(("demo_gototick " + std::to_string(TickSchemaed)).c_str());
	}
    ImGui::Separator();

    ImGui::Text("T方暂停： %s", CSGameRules.m_bTerroristTimeOutActive ? "是" : "否");
    ImGui::Text("CT方暂停： %s", CSGameRules.m_bCTTimeOutActive ? "是" : "否");
    ImGui::Text("T方暂停剩余： %f", CSGameRules.m_flTerroristTimeOutRemaining);
    ImGui::Text("CT方暂停剩余： %f", CSGameRules.m_flCTTimeOutRemaining);
    ImGui::Text("T方暂停次数： %d", CSGameRules.m_nTerroristTimeOuts);
    ImGui::Text("CT方暂停次数： %d", CSGameRules.m_nCTTimeOuts);
    ImGui::Text("技术暂停： %s", CSGameRules.m_bTechnicalTimeOut ? "是" : "否");
    ImGui::Text("比赛等待恢复： %s", CSGameRules.m_bMatchWaitingForResume ? "是" : "否");
    ImGui::Separator();


    // 回合时间信息
    ImGui::Text("回合在未安装炸弹的情况下最长储蓄秒数： %d", CSGameRules.m_iRoundTime);
    ImGui::Text("比赛开始时间： %f", CSGameRules.m_fMatchStartTime);
    ImGui::Text("回合开始时间： %f", CSGameRules.m_fRoundStartTime);
    ImGui::Text("重启回合时间： %f", CSGameRules.m_flRestartRoundTime);
    ImGui::Separator();


    ImGui::Text("游戏重启： %s", CSGameRules.m_bGameRestart ? "是" : "否");
    ImGui::Text("游戏开始时间： %f", CSGameRules.m_flGameStartTime);
    ImGui::Separator();


    ImGui::Text("下一阶段开始时间： %f", CSGameRules.m_timeUntilNextPhaseStarts);
    ImGui::Text("游戏阶段： %d", CSGameRules.m_gamePhase);
    ImGui::Separator();


    // 回合计数信息
    ImGui::Text("总已完成回合数： %d", CSGameRules.m_totalRoundsPlayed);
    ImGui::Text("当前阶段已完成回合数： %d", CSGameRules.m_nRoundsPlayedThisPhase);
    ImGui::Text("加时回合： %d", CSGameRules.m_nOvertimePlaying);
    ImGui::Separator();


    ImGui::Text("剩余人质： %d", CSGameRules.m_iHostagesRemaining);
    ImGui::Text("有人质到达： %s", CSGameRules.m_bAnyHostageReached ? "是" : "否");
    ImGui::Separator();


    // 地图信息
    ImGui::Text("有爆破区： %s", CSGameRules.m_bMapHasBombTarget ? "是" : "否");
    ImGui::Text("有救援区： %s", CSGameRules.m_bMapHasRescueZone ? "是" : "否");
    ImGui::Text("有购买区： %s", CSGameRules.m_bMapHasBuyZone ? "是" : "否");
    ImGui::Separator();


    // 比赛设置
    ImGui::Text("m_bIsQueuedMatchmaking： %s", CSGameRules.m_bIsQueuedMatchmaking ? "是" : "否");
    ImGui::Text("m_nQueuedMatchmakingMode： %d", CSGameRules.m_nQueuedMatchmakingMode);
    ImGui::Text("Valve服务器： %s", CSGameRules.m_bIsValveDS ? "是" : "否");
    ImGui::Text("Logo地图： %s", CSGameRules.m_bLogoMap ? "是" : "否");
    ImGui::Text("m_bPlayAllStepSoundsOnServer： %s", CSGameRules.m_bPlayAllStepSoundsOnServer ? "是" : "否");
    ImGui::Text("m_iSpectatorSlotCount： %d", CSGameRules.m_iSpectatorSlotCount);
    ImGui::Text("m_MatchDevice： %d", CSGameRules.m_MatchDevice);
    ImGui::Text("比赛已开始： %s", CSGameRules.m_bHasMatchStarted ? "是" : "否");
    ImGui::Text("m_nNextMapInMapgroup： %d", CSGameRules.m_nNextMapInMapgroup);
    ImGui::Separator();


    // 炸弹状态
    ImGui::Text("炸弹掉落： %s", CSGameRules.m_bBombDropped ? "是" : "否");
    ImGui::Text("炸弹已安置： %s", CSGameRules.m_bBombPlanted ? "是" : "否");
    ImGui::Separator();


    // 回合结束信息
    ImGui::Text("上回合总时间： %d", CSGameRules.m_iRoundEndTimerTime);
    ImGui::Text("上回合结束玩家数： %d", CSGameRules.m_iRoundEndPlayerCount);
    ImGui::Text("上回合结束无音乐： %s", CSGameRules.m_bRoundEndNoMusic ? "是" : "否");
    ImGui::Separator();


    
    ImGui::Text("回合开始编号： %d", CSGameRules.m_iRoundStartRoundNumber);
    ImGui::Text("回合开始计数： %d", CSGameRules.m_nRoundStartCount);
    ImGui::Text("回合结束计数： %d", CSGameRules.m_nRoundEndCount);
    ImGui::Separator();


    // 其他信息
    ImGui::Text("最后性能采样时间： %f", CSGameRules.m_flLastPerfSampleTime);
    
    ImGui::Separator();
    ImGui::Separator();

    //return;

    
    for (size_t i = 0; i < 64; ++i) {
        try {
            C_Entity Entity = this->MulNXi->CS().GetEntityList().GetEntity(i);
            ImGui::Text("编号(位于游戏EntityList)：%d", Entity.IndexInEntityList);
            ImGui::SameLine();
            ImGui::Text(Entity.GetMsg().str().c_str());
        }
        catch (...) {
            continue;
        }

    }
    
}