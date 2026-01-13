#pragma once

#include"MulNXSingleUIContext/MulNXSingleUIContext.hpp"

#include<queue>

class MulNXUIContext {
private:
	bool CallSingleContext(const std::string& Name);
public:
	bool Active = true;

	//入口点字符串
	std::string EntryDraw{};
	//由字符串映射到句柄
	std::unordered_map<std::string, HContext>CallMap{};
	//这里存储所有句柄
	std::vector<HContext>ContextOrder{};
	//然后从句柄得到具体的单上下文
	std::unordered_map<HContext, MulNXB::any_unique_ptr>ContextMap{};
	
	std::string next;

	void Draw();
	void AddSingleContext(HContext hContext, MulNXB::any_unique_ptr SContext);
	MulNXSingleUIContext* GetSingleContext(const HContext& hContext);
};