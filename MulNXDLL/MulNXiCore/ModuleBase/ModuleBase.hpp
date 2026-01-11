#pragma once

#include"../../MulNXB/MulNXB.hpp"

//模块基类
class ModuleBase {
	friend MulNXiCore;
	friend IMessageManager;
	friend class MessageManager;
protected:
	//核心管理器指针
	MulNXiCore* MulNXi;
	//模块类型
	ModuleType Type = ModuleType::ModuleBase;
	//全局变量指针
	MulNXiGlobalVars* MulNXiGlobalVars = nullptr;
	//3D抽象层指针
	IAbstractLayer3D* AL3D = nullptr;
private:
	//当前消息指针
	std::atomic<MulNXMessage*> CurrentMsg = nullptr;
public:
	//组件句柄
	MulNXHandle HModule{};
private:
	//消息管理器指针
	IMessageManager* IMsgManager = nullptr;
protected:
	//调试器指针
	IDebugger* IDebugger = nullptr;
	//窗口显示标志
	bool ShowWindow = false;
	//运行标志
	std::atomic<bool>Running = false;
	//线程对象成员
	std::thread MyThread;
	//线程运行状态
	std::atomic<bool>MyThreadRunning = false;
	//线程执行间隔，默认以100Hz基准执行
	std::atomic<int> MyThreadDelta = 10;
	//线程锁
	std::shared_mutex MyThreadMutex;
public:
	std::shared_mutex& GetMutex() { return this->MyThreadMutex; }
public:
	//删除不需要的构造函数
	ModuleBase() = delete;
	ModuleBase(const ModuleBase&) = delete;
	ModuleBase(ModuleBase&&) = delete;
	ModuleBase& operator=(const ModuleBase&) = delete;
	ModuleBase& operator=(ModuleBase&&) = delete;
	//提供输入核心管理器指针的构造函数，进行绑定
	ModuleBase(MulNXiCore* MulNXi);
	//虚析构函数确保正确调用析构函数
	virtual ~ModuleBase();
protected:
	//线程析构辅助函数
	void CloseMyThread();
public:
	//通用函数：

	//获取模块类型
	ModuleType GetType()const { return this->Type; }
	std::string GetName()const { return Module_EnumToName(this->GetType()); }

	//模块时间控制接口
	void SetMyThreadDelta(int Delta);

	//虚函数要求：
private:
	//初始化，拉取各种依赖
	virtual bool Init() = 0;

	//虚拟主循环，执行组件逻辑
	virtual void VirtualMain();
	//线程主循环，执行组件线程逻辑
	virtual void ThreadMain();
	//消息处理函数，只需处理即可，消息会由入口点释放
	virtual void ProcessMsg(MulNXMessage* Msg);

	//菜单绘制
	virtual void Menu();
	//窗口绘制
	virtual void Windows();


	//基本函数
protected:
	//基础初始化
	bool BaseInit();
	//基础主循环
	void BaseVirtualMain();
	//基础消息处理
	void BaseProcessMsg();
	//基础菜单
	void BaseMenu();
	//基础窗口
	void BaseWindows();


	friend MulNXiCore;
	//入口点
private:
	//初始化入口
	bool EntryInit();
	//主循环入口
	void EntryVirtualMain();
protected:
	//线程入口
	bool EntryCreateThread();
	//消息处理入口
	void EntryProcessMsg();
private:
	//菜单入口
	void EntryMenu();
	//窗口入口
	void EntryWindows();
public:
	//窗口控制
	void OpenWindow();
	void CloseWindow();
	bool IsWindowOpen()const;


	//工具函数

	//自动注册
	void IRegiste();
	//自动订阅消息类型
	void ISubscribe(MsgType MsgType);
	//自动发送消息
	void IPublish(std::unique_ptr<MulNXMessage> Msg);
	//根据类型自动构建消息并发送
	void IPublish(MsgType Msg);
	//自动创建私有消息管道
	bool ICreateMessageChannel();
public:
	//自动得到私有消息管道
	IMessageChannel* IGetMessageChannel()const;
};

//自动子窗口管理类，一般在Menu函数中使用
class AutoChild {
private:
	ModuleBase* Module = nullptr;
public:
	//构造时开始子窗口
	AutoChild(ModuleBase* Module, const float HeightRatio = 1.0f, const float WidthRatio = 1.0f);
	//析构时结束子窗口
	~AutoChild();
	//禁止拷贝
	AutoChild(const AutoChild&) = delete;
	AutoChild& operator=(const AutoChild&) = delete;
};