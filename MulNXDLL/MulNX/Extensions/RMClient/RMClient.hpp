#pragma once

#include "../../Core/ModuleBase/ModuleBase.hpp"

// 定义RM键盘掩码 - 基于RoboMaster 2026通信协议V1.1.0
enum class RMKeyMask : uint32_t {
    W = 1 << 0,      // bit0: W键
    S = 1 << 1,      // bit1: S键
    A = 1 << 2,      // bit2: A键
    D = 1 << 3,      // bit3: D键
    Shift = 1 << 4,  // bit4: Shift键
    Ctrl = 1 << 5,   // bit5: Ctrl键
    Q = 1 << 6,      // bit6: Q键
    E = 1 << 7,      // bit7: E键
    R = 1 << 8,      // bit8: R键
    F = 1 << 9,      // bit9: F键
    G = 1 << 10,     // bit10: G键
    Z = 1 << 11,     // bit11: Z键
    X = 1 << 12,     // bit12: X键
    C = 1 << 13,     // bit13: C键
    V = 1 << 14,     // bit14: V键
    B = 1 << 15      // bit15: B键
};
class RMKeyBoard {
private:
	RMKeyMask KeyMask = static_cast<RMKeyMask>(0);
public:
	RMKeyBoard() = default;
    void Clear() {
        KeyMask = static_cast<RMKeyMask>(0);
    }
    void AddKey(RMKeyMask key) {
        KeyMask = static_cast<RMKeyMask>(static_cast<uint32_t>(KeyMask) | static_cast<uint32_t>(key));
	}
    uint32_t GetMaskValue() const {
        return static_cast<uint32_t>(KeyMask);
	}
	constexpr static unsigned char ToVKCode(RMKeyMask Mask) {
        switch (Mask) {
        case RMKeyMask::W: return 'W';
        case RMKeyMask::S: return 'S';
        case RMKeyMask::A: return 'A';
        case RMKeyMask::D: return 'D';
        case RMKeyMask::Shift: return VK_SHIFT;
        case RMKeyMask::Ctrl: return VK_CONTROL;
        case RMKeyMask::Q: return 'Q';
        case RMKeyMask::E: return 'E';
        case RMKeyMask::R: return 'R';
        case RMKeyMask::F: return 'F';
        case RMKeyMask::G: return 'G';
        case RMKeyMask::Z: return 'Z';
        case RMKeyMask::X: return 'X';
        case RMKeyMask::C: return 'C';
        case RMKeyMask::V: return 'V';
        case RMKeyMask::B: return 'B';
        default: return 0;
        }
	}
};

class RMClient final :public MulNX::ModuleBase {
private:
	RMKeyBoard KeyBoard;
    RMClient& CheckKey(RMKeyMask Mask);
public:
	bool Init()override;
	void VirtualMain()override;
	void ProcessMsg(MulNX::Message* Msg)override;
	void HandleUICommand(MulNX::Message* Msg);

	void SendControlMsg();
};