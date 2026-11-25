//! 候选的正文与译词：5.1.10 起框架才有独立的 comment 字段。
#pragma once
#include "compat/feature.h"
#include <fcitx/candidatelist.h>
#include <string>

namespace qingjian::compat {
/// 正文与译词分开保存的候选。
///
/// 5.1.10 起的框架把译词放在 comment 字段里，默认面板单独排一列；更早的框架没有该字段，
/// 只能按 CandidateWord::textWithComment() 的规则用一个空格接在正文后，显示与新版一致，
/// 但 text() 会含译词 —— 需要正文时用 body()。
class CommentedCandidate : public fcitx::CandidateWord {
public:
    CommentedCandidate(std::string body, std::string annotation);

    /// 不含译词的正文。
    const std::string &body() const { return body_; }

    /// 译词；旧版框架同时已把它接进 text()。
    const std::string &annotation() const { return annotation_; }

private:
    std::string body_;

    std::string annotation_;
};
}
