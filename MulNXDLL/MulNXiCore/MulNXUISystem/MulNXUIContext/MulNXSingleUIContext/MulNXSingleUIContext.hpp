#pragma once

#include <atomic>
#include <array>
#include <cstdint>

#include"../../../../MulNXB/MulNXB.hpp"

#include"../../MulNXUIFunction/MulNXUIFunction.hpp"

#include"../../../HandleSystem/HandleSystem.hpp"

class TripleBufferBase {
public:
    // 将三个2-bit索引打包到32位中：bits [1:0]=Write, [3:2]=Mid, [5:4]=Read
    std::atomic<uint32_t> IndexState{ (0u) | (1u << 2) | (2u << 4) };

    virtual ~TripleBufferBase() = default;

    virtual void Clear() = 0;
    virtual void* GetWriteBuffer() = 0;
    virtual void* GetReadBuffer() = 0;
    virtual bool SwapWriteBuffer() = 0;  // 返回是否成功
    virtual bool SwapReadBuffer() = 0;   // 返回是否有新数据
    virtual bool HasNewData() const = 0;  // 检查是否有新数据

protected:
    static constexpr uint32_t Pack(int w, int m, int r) noexcept {
        return (static_cast<uint32_t>(w) & 0x3u)
            | ((static_cast<uint32_t>(m) & 0x3u) << 2)
            | ((static_cast<uint32_t>(r) & 0x3u) << 4);
    }

    static constexpr int UnpackWrite(uint32_t s) noexcept {
        return static_cast<int>(s & 0x3u);
    }

    static constexpr int UnpackMid(uint32_t s) noexcept {
        return static_cast<int>((s >> 2) & 0x3u);
    }

    static constexpr int UnpackRead(uint32_t s) noexcept {
        return static_cast<int>((s >> 4) & 0x3u);
    }
};

template<typename T>
class TripleBuffer final : public TripleBufferBase {
public:
    std::array<T, 3> Information{};

    void Clear() override {
        for (auto& It : Information) {
            It = T{};
        }
        IndexState = Pack(0, 1, 2);
    }

    void* GetWriteBuffer() override {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int w = UnpackWrite(s);
        return &Information[w];
    }

    void* GetReadBuffer() override {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int r = UnpackRead(s);
        return &Information[r];
    }

    // 写入线程：交换 Write 和 Mid
    // 返回true表示成功交换
    bool SwapWriteBuffer() override {
        uint32_t oldState = IndexState.load(std::memory_order_acquire);
        uint32_t newState;

        do {
            int w = UnpackWrite(oldState);
            int m = UnpackMid(oldState);
            int r = UnpackRead(oldState);

            // 交换 Write 和 Mid
            // 逻辑：写入完成，将Write缓冲区标记为就绪
            newState = Pack(m, w, r);  // Write <-> Mid

        } while (!IndexState.compare_exchange_weak(
            oldState, newState,
            std::memory_order_acq_rel,
            std::memory_order_acquire
        ));

        return true;
    }

    // 读取线程：交换 Read 和 Mid（如果有新数据）
    // 返回true表示有新数据并成功交换
    bool SwapReadBuffer() override {
        uint32_t oldState = IndexState.load(std::memory_order_acquire);
        uint32_t newState;

        do {
            int w = UnpackWrite(oldState);
            int m = UnpackMid(oldState);
            int r = UnpackRead(oldState);

            // 检查是否有新数据（Mid != Read）
            if (m == r) {
                return false;  // 没有新数据
            }

            // 交换 Read 和 Mid
            // 逻辑：读取完成，将Mid标记为新的读取缓冲区
            newState = Pack(w, r, m);  // Read <-> Mid

        } while (!IndexState.compare_exchange_weak(
            oldState, newState,
            std::memory_order_acq_rel,
            std::memory_order_acquire
        ));

        return true;
    }

    // 检查是否有新数据
    bool HasNewData() const override {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int m = UnpackMid(s);
        int r = UnpackRead(s);
        return m != r;  // Mid != Read 表示有新数据
    }

    // 类型安全的辅助方法
    T& GetWriteData() {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int w = UnpackWrite(s);
        return Information[w];
    }

    const T& GetWriteData() const {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int w = UnpackWrite(s);
        return Information[w];
    }

    T& GetReadData() {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int r = UnpackRead(s);
        return Information[r];
    }

    const T& GetReadData() const {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int r = UnpackRead(s);
        return Information[r];
    }

    T& GetMidData() {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int m = UnpackMid(s);
        return Information[m];
    }

    const T& GetMidData() const {
        uint32_t s = IndexState.load(std::memory_order_acquire);
        int m = UnpackMid(s);
        return Information[m];
    }
};

//RAII帮助，让上下文使用get获取指针时更方便，离开作用域自动提交

// RAII写入包装器
template<typename T>
class DataWrite {
public:
    TripleBuffer<T>* pBuffer = nullptr;
    T* pData = nullptr;

    DataWrite(TripleBufferBase* pBuffer)
        : pBuffer(static_cast<TripleBuffer<T>*>(pBuffer)) {
        if (this->pBuffer) {
            this->pData = static_cast<T*>(this->pBuffer->GetWriteBuffer());
        }
    }

    ~DataWrite() {
        if (this->pBuffer && this->pData) {
            this->pBuffer->SwapWriteBuffer();
        }
    }

    T* get() { return this->pData; }

    T* operator->() { return this->pData; }
    T& operator*() { return *this->pData; }

    explicit operator bool() const { return pData != nullptr; }
};

// RAII读取包装器
template<typename T>
class DataRead {
public:
    TripleBuffer<T>* pBuffer = nullptr;
    T* pData = nullptr;
    bool hasNewData = false;

    DataRead(TripleBufferBase* pBuffer)
        : pBuffer(static_cast<TripleBuffer<T>*>(pBuffer)) {
        if (this->pBuffer) {
            this->pData = static_cast<T*>(this->pBuffer->GetReadBuffer());
            this->hasNewData = this->pBuffer->SwapReadBuffer();
        }
    }

    ~DataRead() = default;  // 读取不需要特殊清理

    T* get() { return this->pData; }

    T* operator->() { return this->pData; }
    T& operator*() { return *this->pData; }

    bool hasNew() const { return hasNewData; }

    explicit operator bool() const { return pData != nullptr; }
};


class MulNXSingleUIContext {
public:
	TripleBufferBase* pBuffer = nullptr;
    //MulNXB::any_unique_ptr pBuffer = nullptr;

	template<typename T>
	DataRead<T> GetRead() {
		return DataRead<T>(this->pBuffer);
	}
	template<typename T>
	DataWrite<T> GetWrite() {
		return DataWrite<T>(this->pBuffer);
	}

	//按照线程管理进行成员分类

	//初始化即可
	MulNXHandle HModule{};
	IMessageChannel* OwnerMsgChannel = nullptr;

	//UI线程私有数据

	std::vector<std::unique_ptr<MulNXUIFunction>> Elements;

	//跨线程数据

	std::atomic<bool>Active = true;
	std::atomic<bool>WaitResponse = false;
	std::atomic<MulNXMessage*>pUpdateData = nullptr;


	void Draw();
	bool SendToOwner(std::unique_ptr<MulNXMessage> Msg);
	bool SendToOwner(MsgType Type, int ParamInt);
	void AddElement(std::unique_ptr<MulNXUIFunction>&& Element);
	static MulNXB::any_unique_ptr Create(const ModuleBase* const MB);

	//默认构造函数
	MulNXSingleUIContext() = default;
    ~MulNXSingleUIContext() {
        if (this->pBuffer) {
            //delete this->pBuffer;
            this->pBuffer = nullptr;
            this->Elements.clear();
        }
    }

	//拷贝构造函数实现
	MulNXSingleUIContext(const MulNXSingleUIContext& Other) {
		this->HModule = Other.HModule;
		this->OwnerMsgChannel = Other.OwnerMsgChannel;
		this->Active = Other.Active.load();
		//this->pBuffer = Other.pBuffer.clone(); //注意：这里是浅拷贝
		for (const auto& It : Other.Elements) {
			this->Elements.push_back(std::make_unique<MulNXUIFunction>(*It));
		}
	}
};