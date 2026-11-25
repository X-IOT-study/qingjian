//! 切中 / 英时框架的气泡提示：5.1.11 起才能显示自定义文本。
#pragma once
#include "compat/feature.h"
#include <fcitx/instance.h>
#include <string>

namespace qingjian::compat {
/// 在 context 上显示一条输入法信息；旧版框架没有这项能力，调用后什么都不发生。
/// 全局设置里关掉「切换输入法时显示输入法信息」就不显示，与框架自己的切输入法气泡一致。
void showInputMethodInfo(fcitx::Instance &instance, fcitx::InputContext *context, const std::string &text);
}
