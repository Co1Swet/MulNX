#pragma once

#include"../../MulNXUIElements/MulNXUIElements.hpp"
#include"../../../../MulNXB/MulNXB.hpp"

#include <memory>
#include <vector>

class IMessageChannel;
class ModuleBase;
enum class MsgType :uint32_t;

class MulNXSingleUIContext {
public:
	MulNXHandle HModule{};
	IMessageChannel* MsgChannel = nullptr;
	std::atomic<bool>Active = true;
	std::vector<std::unique_ptr<MulNXUIFunction>> Elements;

	void Draw();
	bool SendTothis(std::unique_ptr<MulNXMessage> Msg);
	bool SendTothis(MsgType Type);
	void AddElement(std::unique_ptr<MulNXUIFunction>&& Element);
	static std::unique_ptr<MulNXSingleUIContext> Create(const ModuleBase* const MB);

	//默认构造函数
	MulNXSingleUIContext() = default;

	//拷贝构造函数实现
	MulNXSingleUIContext(const MulNXSingleUIContext& Other) {
		this->HModule = Other.HModule;
		this->MsgChannel = Other.MsgChannel;
		this->Active = Other.Active.load();
		for (const auto& It : Other.Elements) {
			this->Elements.push_back(std::make_unique<MulNXUIFunction>(*It));
		}
	}
};