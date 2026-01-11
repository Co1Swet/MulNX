#include"MulNXiGlobalVars.hpp"

#include"../MulNXiCore.hpp"
#include"../MessageManager/IMessageManager.hpp"

#include<chrono>

bool MulNXiGlobalVars::Init() {
	return true;
}

void MulNXiGlobalVars::VirtualMain() {
	static std::chrono::steady_clock::time_point StartTime = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point CurrentTime = std::chrono::steady_clock::now();
    static long long LastDelta = 0;
    long long Delta = std::chrono::duration_cast<std::chrono::seconds>(CurrentTime - StartTime).count();
    if (Delta > LastDelta) {
        if (Delta - LastDelta >= 60) {
            this->PublishTickAll();
            this->CoreTick += Delta - LastDelta;
            LastDelta = Delta;
            return;
        }
        for (long long i = 0; i < Delta - LastDelta; ++i) {
            ++this->CoreTick;
            this->Tick();
        }
        LastDelta = Delta;
    }
    return;
}

void MulNXiGlobalVars::PublishTickAll() {
    this->IPublish(MsgType::Core_Tick1);
    this->IPublish(MsgType::Core_Tick5);
    this->IPublish(MsgType::Core_Tick10);
    this->IPublish(MsgType::Core_Tick15);
    this->IPublish(MsgType::Core_Tick20);
    this->IPublish(MsgType::Core_Tick30);
    this->IPublish(MsgType::Core_Tick45);
    this->IPublish(MsgType::Core_Tick60);
}
void MulNXiGlobalVars::Tick() {
    this->IPublish(MsgType::Core_Tick1);
    if (this->CoreTick % 5 == 0) this->IPublish(MsgType::Core_Tick5);
    if (this->CoreTick % 10 == 0) this->IPublish(MsgType::Core_Tick10);
    if (this->CoreTick % 15 == 0) this->IPublish(MsgType::Core_Tick15);
    if (this->CoreTick % 20 == 0) this->IPublish(MsgType::Core_Tick20);
    if (this->CoreTick % 30 == 0) this->IPublish(MsgType::Core_Tick30);
    if (this->CoreTick % 45 == 0) this->IPublish(MsgType::Core_Tick45);
    if (this->CoreTick % 60 == 0) this->IPublish(MsgType::Core_Tick60);
    if (this->CoreTick % 1800 == 0) this->IPublish(MsgType::Core_Tick30min);
    return;
}