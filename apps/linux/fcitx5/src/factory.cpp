//! 青简 addon 工厂。
#include "qingjian.h"
#include "compat/addon_factory.h"
namespace fcitx {
class QingjianFactory final : public AddonFactory {
public:
    AddonInstance *create(AddonManager *manager) override { return new QingjianEngine(manager); }
};
}
QINGJIAN_ADDON_FACTORY(qingjian, fcitx::QingjianFactory)
