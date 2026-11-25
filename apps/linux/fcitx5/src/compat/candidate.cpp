//! CommentedCandidate 的两种存法。
#include "compat/candidate.h"
#include <utility>

namespace qingjian::compat {
CommentedCandidate::CommentedCandidate(std::string body, std::string annotation)
    : body_(std::move(body)), annotation_(std::move(annotation)) {
#if QINGJIAN_FCITX_HAS_CANDIDATE_COMMENT
    setText(fcitx::Text(body_));
    setComment(fcitx::Text(annotation_));
#else
    fcitx::Text text(body_);
    if (!annotation_.empty()) {
        text.append(" ");
        text.append(annotation_);
    }
    setText(std::move(text));
#endif
}
}
