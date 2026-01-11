#pragma once

#include"../../../../MulNXB/MulNXB.hpp"

class MulNXUIElementKey {
public:
    enum class KeyType : uint32_t {
        Int,
        String
    };

private:
    union Data {
        uint64_t Uint64;
        std::string* pString;//存储指针，正好八字节
        ~Data() {

        }
    }CurrentData;

    KeyType CurrentType;

public:
    //删除默认构造函数
    MulNXUIElementKey() = delete;
    //通过整数值构造
    MulNXUIElementKey(uint64_t val) : CurrentType(KeyType::Int) {
        this->CurrentData.Uint64 = val;
    }

    //字符类构造通过右值构造确定所有权，安全管理内存

    //通过字符串右值构造
    MulNXUIElementKey(std::string&& val) : CurrentType(KeyType::String) {
        std::string* pStr = new std::string(std::move(val));//移动构造右值
        this->CurrentData.pString = pStr;//设置指针
    }
    //通过字符串指针右值构造，指针必须分配在堆上！
    MulNXUIElementKey(std::string*&& val) : CurrentType(KeyType::String) {
        this->CurrentData.pString = val;
    }
    //析构函数，如果是字符串类型则进行析构
    ~MulNXUIElementKey() {
        if (this->CurrentType == KeyType::String) {
            delete this->CurrentData.pString;
        }
    }
    //删除拷贝构造
    MulNXUIElementKey(const MulNXUIElementKey&) = delete;
    MulNXUIElementKey& operator=(const MulNXUIElementKey&) = delete;
    //移动构造
    MulNXUIElementKey(MulNXUIElementKey&& Other)noexcept {
        this->CurrentType = Other.CurrentType;
        this->CurrentData = Other.CurrentData;
        //将Other置为无效状态，防止析构时释放内存
        if (Other.CurrentType == KeyType::String) {
            Other.CurrentData.pString = nullptr;
        }
	}
    void operator=(MulNXUIElementKey&& Other)noexcept {
        //先析构当前对象
        if (this->CurrentType == KeyType::String) {
            delete this->CurrentData.pString;
        }
        this->CurrentType = Other.CurrentType;
        this->CurrentData = Other.CurrentData;
        //将Other置为无效状态，防止析构时释放内存
        if (Other.CurrentType == KeyType::String) {
            Other.CurrentData.pString = nullptr;
        }
	}

    //获取一个基本无效右值Key，用于构造
    static MulNXUIElementKey&& BaseKey() {
        return MulNXUIElementKey(0xFFFFFFFF);
    }

    KeyType GetType()const {
        return this->CurrentType;
    }
    uint64_t GetUint64()const {
        return this->CurrentData.Uint64;
    }
    std::string GetString()const {
        std::string str = *this->CurrentData.pString;
        return str;
    }
};

class MulNXUIElementIndex {
public:
    std::variant<uint64_t, std::string>CurrentData;
    MulNXUIElementIndex() = delete;
    MulNXUIElementIndex(MulNXUIElementIndex&& Other)noexcept {
        this->CurrentData = std::move(Other.CurrentData);
    }
    void operator=(MulNXUIElementIndex&& Other)noexcept {
        this->CurrentData = std::move(Other.CurrentData);
    }
    MulNXUIElementIndex(const MulNXUIElementKey& Key) {
        if (Key.GetType() == MulNXUIElementKey::KeyType::String) {
            this->CurrentData = Key.GetString();
        }
        else {
            this->CurrentData = Key.GetUint64();
        }
    }
};

class MulNXSingleUIContext;

class MulNXUIFunction {
public:
    MulNXUIElementKey Key;
    std::string Text = "空";
    MulNXUIFunction() = delete;
    //通过整数值构造
    MulNXUIFunction(uint64_t val) : Key(val) {}
    //通过字符串右值构造
    MulNXUIFunction(std::string&& val) : Key(std::move(val)) {}
    //通过字符串指针右值构造，指针必须分配在堆上！
    MulNXUIFunction(std::string*&& val) : Key(std::move(val)) {}
	std::function<void(MulNXSingleUIContext*)>MyFunc = nullptr;

	std::unique_ptr<MulNXMessage> Draw(MulNXSingleUIContext*);

	//拷贝构造函数实现
    MulNXUIFunction(const MulNXUIFunction& Other) : Key(MulNXUIElementKey::BaseKey()) {
        //根据Other的Key类型进行构造
        if (Other.Key.GetType() == MulNXUIElementKey::KeyType::String) {
            std::string str = Other.Key.GetString();
            this->Key = MulNXUIElementKey(std::move(str));
        }
        else {
            this->Key = MulNXUIElementKey(Other.Key.GetUint64());
        }
        this->Text = Other.Text;
        this->MyFunc = Other.MyFunc;
	}
};