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

	//跨线程数据

	std::atomic<bool>Active = true;
	std::atomic<bool>WaitResponse = false;
	std::atomic<MulNXMessage*>pUpdateData = nullptr;


	MulNXUIContext* MainContext = nullptr;

	void Draw();
	bool SendToOwner(MulNXMessage&& Msg);
	bool SendToOwner(MsgType Type, uint32_t SubType);

	bool SetNextSingleUIContext(std::string&& Name);

	static MulNXB::any_unique_ptr Create(const ModuleBase* const MB);

	//默认构造函数
	MulNXSingleUIContext() = default;
    ~MulNXSingleUIContext() {
        if (this->pBuffer) {
            //delete this->pBuffer;
            this->pBuffer = nullptr;
        }
    }

	//拷贝构造函数实现
	MulNXSingleUIContext(const MulNXSingleUIContext& Other) = delete;
};