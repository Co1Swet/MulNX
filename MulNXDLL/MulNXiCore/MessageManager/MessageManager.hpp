//MessageManager.hpp
//跨线程安全的消息管理器
//管理整个系统消息的发送和接受
//消息由发布者创建，经过发布后，生命周期的管理即委托给消息管理器

#include"IMessageManager.hpp"

#include<shared_mutex>
#include<unordered_map>
#include<atomic>

class MessageChannel final :public IMessageChannel {
	friend class IMessageManager;
	friend class MessageManager;

	MessageManager* MsgManager = nullptr;

	std::mutex ChannelMutex{};
	std::deque<std::unique_ptr<MulNXMessage>>Messages{};
public:
	MessageChannel(MessageManager* MsgManager);
private:
	IMessageChannel& Subscribe(const MsgType MsgType)override;
	IMessageChannel& Unsubscribe(const MsgType MsgType)override;
	bool PushMessage(std::unique_ptr<MulNXMessage> Msg)override;
	bool PullMessage(std::unique_ptr<MulNXMessage>& OutMsg)override;
};

class MessageManager final :public IMessageManager {
	friend MessageChannel;
	friend IMessageManager;
private:
	//存储类
	std::deque<std::unique_ptr<MulNXMessage>> Messages;//类型擦除，准备实现多态（后续增加按type转换后销毁的能力）
	MulNXMessage* CurrentMessage = nullptr;
	std::unordered_map<MsgType, std::vector<ModuleBase*>>SubscribeMap{};
	std::unordered_map<MsgType, std::vector<MessageChannel*>>ChannelSubscribeMap{};
	//std::unordered_map<std::atomic<bool>*, RegistePack>RegisteMsg;
	std::atomic<uint32_t>RefCount = 0;

	std::unordered_map<MulNXHandle, std::unique_ptr<MessageChannel>>Channels;
public:
	MessageManager(MulNXiCore* MulNXi) :IMessageManager(MulNXi) {};
	~MessageManager();

	bool Init()override;
	void ThreadMain()override;
	//返回true表示正在处理消息，false表示没有消息可处理
	bool NextMsg();
	void VirtualMain()override;

	//接口实现：

	//注册，获取消息指针
	bool Registe(ModuleBase* const Module)override;
	//订阅，获得对应类型消息推送
	bool Subscribe(const MsgType MsgType, ModuleBase* const Module)override;
	//创建私有消息队列（但是生命周期仍然委托给消息管理器）
	bool CreateMessageChannel(ModuleBase* const Module)override;
	//获取消息管道
	//static IMessageChannel* GetMessageChannel(const MulNXHandle& Handle);
	//取消订阅
	bool Unsubscribe(const MsgType MsgType, ModuleBase* const Module)override;
	//需要在堆中构建消息，消息的创建由发送者负责，消息的销毁由消息总线负责
	bool Publish(std::unique_ptr<MulNXMessage>const Msg)override;
	//释放消息，降低引用计数
	bool Release()override;
};