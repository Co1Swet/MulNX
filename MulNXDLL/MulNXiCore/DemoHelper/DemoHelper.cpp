#include"DemoHelper.hpp"

#include"../MessageManager/IMessageManager.hpp"
#include"../MulNXUISystem/IMulNXUISystem.hpp"
#include"../Debugger/IDebugger.hpp"

enum DemoHelperID :uint64_t {
	你好,
	我好,
	大家好
};

bool DemoHelper::Init() {
	this->ICreateMessageChannel();
	IMessageChannel* MsgChannel = this->IGetMessageChannel();
	(*MsgChannel)
		.Subscribe(MsgType::UISystem_UICommand);

	auto Msg = std::make_unique<MulNXMessage>(MsgType::UISystem_ModulePush);
	//auto SingleContext = MulNXSingleUIContext::Create(this);
	auto SContext = MulNXB::make_any_unique<MulNXSingleUIContext>();
	MulNXSingleUIContext* SContextPtr = SContext.get<MulNXSingleUIContext>();
	SContextPtr->HModule = this->HModule;
	SContextPtr->MsgChannel = this->IGetMessageChannel();

	auto ElemFunc = std::make_unique<MulNXUIFunction>("任何你想要的ID，多语言支持");
	ElemFunc->MyFunc = [](MulNXSingleUIContext* This)->void {
		This->Active = true;

		ImGui::Begin("这是一个测试");
		ImGui::Text("异步执行！");
		if (ImGui::Button("异步传递")) {
			This->SendTothis(MsgType::UISystem_UICommand);
		}
		ImGui::End();

		return;
		};
	SContextPtr->AddElement(std::move(ElemFunc));

	Msg->ParamData = std::move(SContext);
	this->IPublish(std::move(Msg));

	return true;
}

void DemoHelper::ProcessMsg(MulNXMessage* Msg) {
	switch (Msg->Type) {
	case MsgType::UISystem_UICommand: {
		this->IDebugger->AddSucc("测试成功");
		break;
	}
	}
}

void DemoHelper::VirtualMain() {
	this->EntryProcessMsg();
}

//void DemoHelper::HandleUICommand(MulNXMessage* Msg) {
//	// 如果 CurrentData 是 uint64_t（之前用 uint64_t 存储整数）
//	if (auto pUint = std::get_if<uint64_t>(&Msg->Index.CurrentData)) {
//		uint64_t id = *pUint;
//		switch (id) {
//		case DemoHelperID::你好: {
//			this->IDebugger->AddInfo("你好");
//			break;
//		}
//		case DemoHelperID::我好: {
//			this->IDebugger->AddInfo("我好");
//			break;
//		}
//		case DemoHelperID::大家好: {
//			this->IDebugger->AddInfo("大家好");
//			break;
//		}
//		}
//
//		return;
//	}
//
//	// 如果 CurrentData 是 std::string
//	if (auto pStr = std::get_if<std::string>(&Msg->Index.CurrentData)) {
//		if (*pStr == "你好") {
//			this->IDebugger->AddSucc("接收到来自UI系统的消息，你好！");
//		}
//		return;
//	}
//
//	// 若有其它类型，或作防御性日志
//	this->IDebugger->AddInfo("收到未知类型的 UI 索引数据");
//}

