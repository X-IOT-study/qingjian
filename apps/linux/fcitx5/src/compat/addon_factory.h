//! addon 工厂注册：5.1.12 起按 addon 名导出工厂符号。
#pragma once
#include "compat/feature.h"
#include <fcitx/addoninstance.h>

#if QINGJIAN_FCITX_HAS_ADDON_FACTORY_V2
/// 5.1.12 起的加载器只认 fcitx_addon_factory_instance_<name>，更早的只认无后缀的那个。
#define QINGJIAN_ADDON_FACTORY(name, ClassName) FCITX_ADDON_FACTORY_V2(name, ClassName)
#else
#define QINGJIAN_ADDON_FACTORY(name, ClassName) FCITX_ADDON_FACTORY(ClassName)
#endif
