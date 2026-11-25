//! 额外翻页键与 `-` / `=` 翻不动时落回原字符。
use super::support::{compose, key, router};
use crate::dispatch::Composed;
use qingjian_core::{Candidate, CandidateKind, CandidateLayout, Query};
use qingjian_platform::PageKeys;
use qingjian_platform::protocol::{KeyModifiers, KeyOutcome};

fn with_extra(router: &mut crate::dispatch::Router) {
    router.config.extra_page_keys = PageKeys::default().with("-=", true);
}

#[test]
fn extra_page_keys_turn_pages_in_chinese_and_english() {
    let normal = KeyModifiers::default();
    let mut chinese = router(5);
    with_extra(&mut chinese);
    compose(&mut chinese, "qq", normal);
    let next = key(&mut chinese, b'=' as u32, Some('='), normal);
    assert_eq!(next.0, KeyOutcome::Consumed);
    assert_eq!(next.2.page, 1);
    let previous = key(&mut chinese, b'-' as u32, Some('-'), normal);
    assert_eq!(previous.0, KeyOutcome::Consumed);
    assert_eq!(previous.2.page, 0);

    let english = KeyModifiers {
        english_mode: true,
        ..normal
    };
    let mut english_candidates = router(1);
    with_extra(&mut english_candidates);
    compose(&mut english_candidates, "hel", english);
    assert_eq!(
        key(&mut english_candidates, b'=' as u32, Some('='), english)
            .2
            .page,
        1
    );
    assert_eq!(
        key(&mut english_candidates, b'-' as u32, Some('-'), english)
            .2
            .page,
        0
    );
}

#[test]
fn extra_page_keys_fall_back_to_characters_without_a_page() {
    let normal = KeyModifiers::default();
    let mut previous = router(5);
    with_extra(&mut previous);
    compose(&mut previous, "qq", normal);
    // 第一页没有上一页：`-` 仍是英文直输段里的连字符。
    assert_eq!(
        key(&mut previous, b'-' as u32, Some('-'), normal).0,
        KeyOutcome::Consumed
    );
    assert_eq!(
        key(&mut previous, 13, None, normal).1.as_deref(),
        Some("qq-")
    );

    let mut next = router(5);
    with_extra(&mut next);
    compose(&mut next, "qq", normal);
    key(&mut next, 0x22, None, normal);
    // 末页没有下一页：`=` 落回原字符。
    assert_eq!(
        key(&mut next, b'=' as u32, Some('='), normal).0,
        KeyOutcome::Consumed
    );
    assert_eq!(key(&mut next, 13, None, normal).1.as_deref(), Some("qq="));
}

#[test]
fn main_page_keys_fall_back_only_for_hyphen_and_equals() {
    let normal = KeyModifiers::default();
    let mut previous = router(5);
    previous.config.page_keys = ('-', '=');
    compose(&mut previous, "qq", normal);
    assert_eq!(
        key(&mut previous, b'-' as u32, Some('-'), normal).0,
        KeyOutcome::Consumed
    );
    assert_eq!(
        key(&mut previous, 13, None, normal).1.as_deref(),
        Some("qq-")
    );

    let mut next = router(5);
    next.config.page_keys = ('-', '=');
    compose(&mut next, "qq", normal);
    assert_eq!(key(&mut next, b'=' as u32, Some('='), normal).2.page, 1);
    assert_eq!(
        key(&mut next, b'=' as u32, Some('='), normal).0,
        KeyOutcome::Consumed
    );
    assert_eq!(key(&mut next, 13, None, normal).1.as_deref(), Some("qq="));

    // `[` `]` 翻不动照旧吃键，避免把括号插进文档。
    let mut brackets = router(5);
    brackets.config.page_keys = ('[', ']');
    compose(&mut brackets, "qq", normal);
    key(&mut brackets, b']' as u32, Some(']'), normal);
    let boundary = key(&mut brackets, b']' as u32, Some(']'), normal);
    assert_eq!(boundary.0, KeyOutcome::Consumed);
    assert_eq!(boundary.2.page, 1);
    assert!(boundary.1.is_none());
}

#[test]
fn sparse_fixed_candidate_pages_do_not_swallow_hyphen() {
    let normal = KeyModifiers::default();
    let mut router = router(5);
    compose(&mut router, "qq", normal);
    let fixed = Candidate {
        text: "固定短语".into(),
        kind: CandidateKind::Custom(6),
        syllables: Vec::new(),
        reading: None,
        translation: None,
        aux_code: None,
    };
    router.composed = Some(Composed::Candidates {
        preedit: Vec::new(),
        cursor: 0,
        layout: CandidateLayout::new(vec![fixed], 5, 0),
        spelling: Box::new(Query::default()),
        manual_sequence: None,
    });
    router.highlight = 5;
    with_extra(&mut router);

    assert!(!router.can_page(-1));
    assert_eq!(
        key(&mut router, b'-' as u32, Some('-'), normal).0,
        KeyOutcome::Consumed
    );
    assert!(router.engine.raw_mode());
}
