//! 候选正文与第一条译词；选词仍回 Server 调 Engine::commit。
#pragma once
#include "compat/candidate.h"
#include <functional>
#include <utility>
namespace qingjian {
class Word final : public compat::CommentedCandidate {
public:
    Word(std::string text, std::string comment, std::function<void(fcitx::InputContext *)> select)
        : CommentedCandidate(std::move(text), std::move(comment)), select_(std::move(select)) {
        setPlaceHolder(body().empty());
    }
    void select(fcitx::InputContext *context) const override { if (!isPlaceHolder()) select_(context); }
private:
    /// 含帧版本检查的选择操作。
    std::function<void(fcitx::InputContext *)> select_;
};
}
