#include "RMClient.hpp"

#include "../../Core/Core.hpp"
#include "../../Systems/Systems.hpp"
#include "../../Extensions/MQTT/MQTTClient/MQTTClient.hpp"
#include "../../../ThirdParty/protobuf/messages_down.pb.h"
#include "../../../ThirdParty/protobuf/messages_up.pb.h"
#include "../../../ThirdParty/All_ImGui.hpp"

#include <chrono>

std::atomic<bool> EnableRemoteControl = false;

static void MyDraw(MulNXSingleUIContext* This) {
	ImGui::Text("远程控制客户端模块");
	bool prev = EnableRemoteControl.load();
	if (ImGui::Checkbox("启用远程控制", &prev)) {
		EnableRemoteControl.store(prev);
	}
}

bool RMClient::Init() {
	this->MainMsgChannel = this->ICreateAndGetMessageChannel();
	(*this->MainMsgChannel)
		.Subscribe(MulNX::MsgType::UISystem_UICommand);

	auto SC = MulNXSingleUIContext::Create(this);
	auto* pSC = SC.get<MulNXSingleUIContext>();
	pSC->name = "RMClient";
	pSC->MyFunc = MyDraw;
	MulNX::Message Msg(MulNX::MsgType::UISystem_ModulePush);
	Msg.Handle = this->Core->IHandleSystem().RegisteUnique(std::move(SC));
	this->IPublish(std::move(Msg));

	return true;
}

void RMClient::VirtualMain() {
	this->EntryProcessMsg();
	this->SendControlMsg();
}

void RMClient::ProcessMsg(MulNX::Message* Msg) {
	if (Msg->Type == MulNX::MsgType::UISystem_UICommand) {
		this->HandleUICommand(Msg);
		Msg->pMsgChannel->PushMessage(MulNX::Message(MulNX::MsgType::UISystem_ModuleResponse));
	}
	return;
}

void RMClient::HandleUICommand(MulNX::Message* Msg) {
	// 目前不处理任何UI命令
	return;
}

RMClient& RMClient::CheckKey(RMKeyMask Mask) {
	if(this->KT->IsKeyPressed(RMKeyBoard::ToVKCode(Mask)))
		this->KeyBoard.AddKey(Mask);
	return *this;
}

void RMClient::SendControlMsg() {
	if (EnableRemoteControl.load() == false) {
		return;
	}
	// 这个任务需要以75Hz的频率发送控制消息
	// 通过静态时间点变量实现
	static auto LastTime = std::chrono::high_resolution_clock::now();
	auto Now = std::chrono::high_resolution_clock::now();
	auto Duration = std::chrono::duration_cast<std::chrono::milliseconds>(Now - LastTime);
	if (Duration.count() < 13) {
		return;
	}
	LastTime = Now;

	// 先构建MQTT消息体
	rm_client_up::RemoteControl controlMsg;
	controlMsg.set_mouse_x(100);
	controlMsg.set_mouse_y(-100);
	controlMsg.set_mouse_z(0);

	// 鼠标按钮状态
	// 左键
	if (this->KT->IsKeyPressed(VK_LBUTTON))controlMsg.set_left_button_down(true);
	else controlMsg.set_left_button_down(false);
	// 右键
	if (this->KT->IsKeyPressed(VK_RBUTTON))controlMsg.set_right_button_down(true);
	else controlMsg.set_right_button_down(false);
	// 中键
	if (this->KT->IsKeyPressed(VK_MBUTTON))controlMsg.set_mid_button_down(true);
	else controlMsg.set_mid_button_down(false);

	// 键盘按键值
	// 初始化键盘位掩码
	controlMsg.set_keyboard_value(0);
	this->KeyBoard.Clear();
	(*this)
		.CheckKey(RMKeyMask::W)
		.CheckKey(RMKeyMask::A)
		.CheckKey(RMKeyMask::S)
		.CheckKey(RMKeyMask::D)
		.CheckKey(RMKeyMask::Shift)
		.CheckKey(RMKeyMask::Ctrl)
		.CheckKey(RMKeyMask::Q)
		.CheckKey(RMKeyMask::E)
		.CheckKey(RMKeyMask::R)
		.CheckKey(RMKeyMask::F)
		.CheckKey(RMKeyMask::G)
		.CheckKey(RMKeyMask::Z)
		.CheckKey(RMKeyMask::X)
		.CheckKey(RMKeyMask::C)
		.CheckKey(RMKeyMask::V)
		.CheckKey(RMKeyMask::B);
	controlMsg.set_keyboard_value(this->KeyBoard.GetMaskValue());


	// 序列化MQTT消息体
	std::string serializedMsg;
	controlMsg.SerializeToString(&serializedMsg);

	// 构造MQTT消息
	auto pMsg = MulNX::Base::make_any_unique<MQTT::Message>();
	auto serializedMsgObj = pMsg.get<MQTT::Message>();
	// 设置MQTT消息的主题和内容
	serializedMsgObj->Topic = "RemoteControl";
	serializedMsgObj->Body = std::move(serializedMsg);
	
	// 注册MQTT消息到句柄系统
	MulNXHandle hMsg = this->Core->IHandleSystem().RegisteUnique(std::move(pMsg));

	// 构造MulNX消息
	MulNX::Message Msg(MulNX::MsgType::MQTT_ModulePublish);
	// 设置消息的句柄
	Msg.Handle = hMsg;
	// 发送消息
	this->IPublish(std::move(Msg));
}