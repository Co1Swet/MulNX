#pragma once

#include"../../../../MulNXB/MulNXB.hpp"

#include"../../../HandleSystem/HandleSystem.hpp"

#include"Buffer/Buffer.hpp"

class MulNXUIContext;

class MulNXSingleUIContext {
public:
    MulNXB::any_unique_ptr pBuffer = nullptr;
	std::string name{};
	std::function<void(MulNXSingleUIContext*)>MyFunc = nullptr;

	template<typename T>
	DataRead<T> GetRead() {
		auto* buf = this->pBuffer.get<TripleBuffer<T>>();
		return DataRead<T>(static_cast<TripleBufferBase*>(buf));
	}
	template<typename T>
	DataWrite<T> GetWrite() {
		auto* buf = this->pBuffer.get<TripleBuffer<T>>();
		return DataWrite<T>(static_cast<TripleBufferBase*>(buf));
	}

	//按照线程管理进行成员分类

	//初始化即可
	MulNXHandle HModule{};
	IMessageChannel* OwnerMsgChannel = nullptr;
	IMessageChannel* MyMsgChannel = nullptr;

	//跨线程数据

	std::atomic<bool>Active = true;
	std::atomic<bool>WaitingResponse = false;
	std::atomic<MulNXMessage*>pUpdateData = nullptr;


	MulNXUIContext* MainContext = nullptr;

	void Draw();
	bool SendToOwner(MulNXMessage&& Msg);
	MulNXMessage CreateMsg(uint32_t SubType);

	bool CallSingleUIContext(std::string&& Name);
	bool SetNextSingleUIContext(std::string&& Name);

	static MulNXB::any_unique_ptr Create(const ModuleBase* const MB);

	MulNXSingleUIContext() = default;
	//禁止拷贝
	MulNXSingleUIContext(const MulNXSingleUIContext& Other) = delete;
};