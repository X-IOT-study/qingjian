//! 客户端要求的「禁用输入」能力位：5.0.20 起才进 CapabilityFlag。
#pragma once
#include "compat/feature.h"
#include <fcitx-utils/capabilityflags.h>

namespace qingjian::compat {
#if QINGJIAN_FCITX_HAS_DISABLE
inline constexpr fcitx::CapabilityFlag Disable = fcitx::CapabilityFlag::Disable;
#else
/// 与上游 5.0.20 相同的位。5.0.14 的 core 不解释这个位，但会把客户端发来的 64 位能力原样存下，
/// 按位检测即可，语义与新版一致。
inline constexpr fcitx::CapabilityFlag Disable = static_cast<fcitx::CapabilityFlag>(1ull << 40);
#endif
}
