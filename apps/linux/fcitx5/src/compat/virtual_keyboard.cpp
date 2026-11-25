//! 虚拟键盘可见性在两种框架上的取法。
#include "compat/virtual_keyboard.h"
#include <fcitx/userinterfacemanager.h>
#include <utility>

namespace qingjian::compat {
bool virtualKeyboardVisible(fcitx::Instance &instance) {
#if QINGJIAN_FCITX_HAS_VIRTUAL_KEYBOARD
    return instance.userInterfaceManager().isVirtualKeyboardVisible();
#else
    (void)instance;
    return false;
#endif
}

std::unique_ptr<fcitx::HandlerTableEntry<fcitx::EventHandler>>
watchVirtualKeyboard(fcitx::Instance &instance, fcitx::EventHandler callback) {
#if QINGJIAN_FCITX_HAS_VIRTUAL_KEYBOARD
    return instance.watchEvent(fcitx::EventType::VirtualKeyboardVisibilityChanged,
        fcitx::EventWatcherPhase::PostInputMethod, std::move(callback));
#else
    (void)instance;
    (void)callback;
    return nullptr;
#endif
}
}
