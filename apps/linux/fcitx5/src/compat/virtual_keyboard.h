//! 虚拟键盘可见性：5.1.0 起框架才有这个概念。
#pragma once
#include "compat/feature.h"
#include <fcitx/instance.h>
#include <memory>

namespace qingjian::compat {
/// 虚拟键盘当前是否可见；旧版框架恒为 false。
bool virtualKeyboardVisible(fcitx::Instance &instance);

/// 监听虚拟键盘可见性变化；旧版框架没有该事件，返回空句柄。
std::unique_ptr<fcitx::HandlerTableEntry<fcitx::EventHandler>>
watchVirtualKeyboard(fcitx::Instance &instance, fcitx::EventHandler callback);
}
