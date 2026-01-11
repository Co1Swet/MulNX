#pragma once

#include"IDebugger.hpp"

#include<algorithm>
#include<deque>
#include<string>

class Debugger final :public IDebugger {
private:
	std::deque<std::string> DebugMsg{};
	bool IfShowStream = true;
	int MaxMsgCount = 1000;


	const std::string Info = "[提示]";
	const std::string Succ = "[成功]";
	const std::string Warning = "[警告]";
	const std::string Error = "[错误]";
public:
	Debugger(MulNXiCore* MulNXi) :IDebugger(MulNXi) {
		this->Type = ModuleType::Debugger;
	};

	bool NeedAutoScroll = false;
	
	//模块基类接口实现

	bool Init()override;

	void Menu()override;
	void Windows()override;

	//调试器接口实现

	void AddInfo(const std::string& NewMsg)override;
	void AddSucc(const std::string& NewMsg)override;
	void AddWarning(const std::string& NewMsg)override;
	void AddError(const std::string& NewMsg)override;

	void ResetMaxMsgCount(const int Max)override;

	//其它函数
	
	void PushBack(const std::string& NewMsg, const std::string& prefix);

	void MenuMonitor();
	void MenuStream();

	void ShowStream();
	void HideStream();
};