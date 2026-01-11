#pragma once

#include"../MulNXUIContext/MulNXUIContext.hpp"

#include"../MulNXUIElements/MulNXUIElements.hpp"

#define MulNXBuilder MulNXUIBuilder
#define ICreate(Target) \
/*通过整数值构造*/ \
MulNXBuilder& Target(uint64_t val) { \
	auto Target = std::make_unique<MulNXUI##Target>(val); \
	this->pCurrentElement = Target.get(); \
	this->SContext->AddElement(std::move(Target)); \
	return *this; \
} \
/*通过字符串右值构造*/ \
MulNXBuilder& Target(std::string&& val) { \
	auto Target = std::make_unique<MulNXUI##Target>(std::move(val)); \
	this->pCurrentElement = Target.get(); \
	this->SContext->AddElement(std::move(Target)); \
	return *this; \
} \
/*通过字符串指针右值构造，指针必须分配在堆上！*/ \
MulNXBuilder& Target(std::string*&& val) { \
	auto Target = std::make_unique<MulNXUI##Target>(std::move(val)); \
	this->pCurrentElement = Target.get(); \
	this->SContext->AddElement(std::move(Target)); \
	return *this; \
}

class MulNXUIBuilder {
private:
	std::unique_ptr<MulNXSingleUIContext>SContext = nullptr;
public:
	static MulNXUIBuilder Begin() {
		MulNXUIBuilder MulNXUIBuilder;
		MulNXUIBuilder.SContext = std::move(std::make_unique<MulNXSingleUIContext>());
		return MulNXUIBuilder;
	}
	std::unique_ptr<MulNXSingleUIContext> End() {
		return std::move(this->SContext);
	}

	MulNXUIBuilder& SetModuleHandle(MulNXHandle HModule) {
		this->SContext->HModule = HModule;
		return *this;
	}

	MulNXUIBuilder& SetMessageChannel(IMessageChannel* Channel) {
		this->SContext->MsgChannel = Channel;
		return *this;
	}
};