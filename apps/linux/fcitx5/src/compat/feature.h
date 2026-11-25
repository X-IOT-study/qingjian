//! 兼容层的版本开关：CMake 按系统 Fcitx5 的版本生成这些宏，源码里只有 compat/ 内按它分叉。
//! 门槛、降级行为与替代路线见 docs/notes/linux-fcitx5-compat.md。
#pragma once

// 5.0.20  CapabilityFlag::Disable
// 5.1.0   UserInterfaceManager 的虚拟键盘可见性
// 5.1.10  CandidateWord::setComment
// 5.1.11  Instance::showCustomInputMethodInformation
// 5.1.12  FCITX_ADDON_FACTORY_V2
// 5.1.22  InputPanel::overlayMessage（只有测试用）
#if !defined(QINGJIAN_FCITX_HAS_DISABLE) || !defined(QINGJIAN_FCITX_HAS_VIRTUAL_KEYBOARD) \
    || !defined(QINGJIAN_FCITX_HAS_CANDIDATE_COMMENT) || !defined(QINGJIAN_FCITX_HAS_CUSTOM_IM_INFO) \
    || !defined(QINGJIAN_FCITX_HAS_ADDON_FACTORY_V2) || !defined(QINGJIAN_FCITX_HAS_OVERLAY_MESSAGE)
#error "缺少 QINGJIAN_FCITX_HAS_* 宏：请用 CMake 构建（apps/linux/fcitx5/CMakeLists.txt）"
#endif
