#include"DemoHelper.hpp"

#include"../MulNXiCore.hpp"
#include"../MessageManager/IMessageManager.hpp"
#include"../MulNXUISystem/IMulNXUISystem.hpp"
#include"../Debugger/IDebugger.hpp"
#include"../AbstractLayer3D/IAbstractLayer3D.hpp"

//真正的组件化，连具体数据类型都隐藏在CPP文件中，Fuck编译依赖！
struct DemoHelperPrivateData {
	std::vector<float> TimeMarks{};
};

static std::atomic<int> ClickCount = 0;

static void MyDraw(MulNXSingleUIContext* This) {
	auto ReadData = This->GetRead<DemoHelperPrivateData>();
	auto ThisData = ReadData.get();
	ImGui::Begin("Demo助手");
	ImGui::Text("第一个异步模块");
	if (ImGui::Button("标记当前时间")) {
		This->SendToOwner(MsgType::UISystem_UICommand, 0x101);
	}
	ImGui::Text("目前，时间列表容器是");
	ImGui::SameLine();
	if(ThisData->TimeMarks.empty()){
		ImGui::Text("空的");
	}
	else{
		ImGui::Text("有 %d 个时间点的", (int)ThisData->TimeMarks.size());
		for(auto time : ThisData->TimeMarks){
			ImGui::Text("时间点： %.3f 秒", time);
			ImGui::SameLine();
			std::string str = "跳转##" + std::to_string(time);
			if (ImGui::Button(str.c_str())) {
				auto Msg = std::make_unique<MulNXMessage>(MsgType::UISystem_UICommand);
				Msg->ParamInt = 0x102;
				Msg->ParamFloat = time;
				This->SendToOwner(std::move(Msg));
			}
		}
	}
	ImGui::Text("按钮已被点击 %d 次", ClickCount.load());
	ImGui::End();

	return;
}

bool DemoHelper::Init() {
	this->ICreateMessageChannel();
	IMessageChannel* MsgChannel = this->IGetMessageChannel();
	(*MsgChannel)
		.Subscribe(MsgType::UISystem_UICommand);

	auto Msg = std::make_unique<MulNXMessage>(MsgType::UISystem_ModulePush);

	MulNXB::any_unique_ptr SingleContext = MulNXSingleUIContext::Create(this);
	MulNXSingleUIContext* SContextPtr = SingleContext.get<MulNXSingleUIContext>();
	//this->hContext = this->MulNXi->HandleSystem().CreateHandle<HContext>();
	

	auto ElemFunc = std::make_unique<MulNXUIFunction>("任何你想要的ID，多语言支持");
	ElemFunc->MyFunc = MyDraw;
	SContextPtr->AddElement(std::move(ElemFunc));

	//SContextPtr->pBuffer = MulNXB::make_any_unique<TripleBuffer<DemoHelperPrivateData>>();
	SContextPtr->pBuffer = new TripleBuffer<DemoHelperPrivateData>();

	this->hContext = this->MulNXi->HandleSystem().RegisteHandle<HContext>(std::move(SingleContext));
	Msg->Handle = this->hContext;
	//Msg->ParamData = std::move(SingleContext);
	this->IPublish(std::move(Msg));

	return true;
}

void DemoHelper::ProcessMsg(MulNXMessage* Msg) {
	switch (Msg->Type) {
	case MsgType::UISystem_UICommand: {
		this->IDebugger->AddSucc("测试成功");
		this->HandleUICommand(Msg);
		break;
	}
	}
}

void DemoHelper::HandleUICommand(MulNXMessage* Msg) {
	switch (Msg->ParamInt) {
	case 0x101: {
		this->MarkTime();
		ClickCount++;
		break;
	}
	case 0x102: {
		float data = Msg->ParamFloat;
		std::string str = "跳转到" + std::to_string(data);
		this->IDebugger->AddInfo(str);
	}
		
	}
}

void DemoHelper::VirtualMain() {
	this->EntryProcessMsg();
	auto ctx = this->MulNXi->IUISystem().GetSingleContext(this->hContext);
	if (!ctx) return; // 或跳过本帧处理
	auto data = ctx->GetWrite<DemoHelperPrivateData>();
	data->TimeMarks = this->Marks;
}

bool DemoHelper::MarkTime() {
	this->Marks.push_back(this->AL3D->GetTime());


	return true;
}