//! 输入法信息气泡在两种框架上的取法。
#include "compat/input_method_info.h"
#include <fcitx/globalconfig.h>
#include <fcitx/inputcontext.h>

namespace qingjian::compat {
void showInputMethodInfo(fcitx::Instance &instance, fcitx::InputContext *context, const std::string &text) {
#if QINGJIAN_FCITX_HAS_CUSTOM_IM_INFO
    // showCustomInputMethodInformation 不看全局设置，这里自己补上。
    if (instance.globalConfig().showInputMethodInformation())
        instance.showCustomInputMethodInformation(context, text);
#else
    (void)instance;
    (void)context;
    (void)text;
#endif
}
}
