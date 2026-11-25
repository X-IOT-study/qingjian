# Fcitx5 5.0 兼容层

`apps/linux/fcitx5/src/compat/` 让插件在 Fcitx5 5.0.14（Ubuntu 22.04 LTS 自带、Jammy 源里唯一版本）上编译、
加载并工作。5.1.x 上走原生接口，行为与不带这层时相同。

## 为什么分开

青简 0.3.4 的插件源码实际用到 5.1.12 的接口（CMake 里写的 5.1.8 与源码不符），而 Ubuntu 22.04 的
Fcitx5 停在 5.0.14-1，Jammy 的 `-updates` / `-backports` / `-security` 都没有更高版本；Launchpad 上
也没有提供 Jammy + Fcitx5 ≥ 5.1.11 的可信 PPA（2026-11 逐个查过 62 个相关 PPA，只有
`hrzhu/fcitx5-backport` 的 5.1.0，仍缺本插件要的接口，且是个人源、无安全维护）。预编译插件的
deb 依赖 `libc6 (>= 2.38)`、`libstdc++6 (>= 14)` 与三个 `5.1.12` 的 fcitx 库，在 Jammy 上连改元数据都装不上。
要让 22.04 用户用上 Fcitx5 前端，只能让源码在 5.0.14 上编。

## 结构

版本差异只在 `src/compat/` 里分叉，业务代码不写 `#if`：

| 文件 | 补的是什么 |
|---|---|
| `feature.h` | 要求 CMake 给出全部 `QINGJIAN_FCITX_HAS_*` 宏，缺一个就编不过 |
| `capability.h` | `CapabilityFlag::Disable`（5.0.20 才有枚举成员） |
| `candidate.h/.cpp` | `CommentedCandidate`：正文与译词分开存，旧版把译词接进正文 |
| `virtual_keyboard.h/.cpp` | 虚拟键盘可见性与 watcher（5.1.0 起才有） |
| `addon_factory.h` | `QINGJIAN_ADDON_FACTORY`：5.1.12 起按 addon 名导出工厂符号 |
| `input_method_info.h/.cpp` | 中 / 英切换气泡（5.1.11 起才有） |

`CMakeLists.txt` 里一张表给出各接口的首次版本，`qingjian_require_fcitx5(feature version)` 据此生成
0/1 宏并同时交给插件与每个测试目标：

| 宏 | 门槛 | 5.0.14 上 |
|---|---|---|
| `QINGJIAN_FCITX_HAS_DISABLE` | 5.0.20 | 0 |
| `QINGJIAN_FCITX_HAS_VIRTUAL_KEYBOARD` | 5.1.0 | 0 |
| `QINGJIAN_FCITX_HAS_CANDIDATE_COMMENT` | 5.1.10 | 0 |
| `QINGJIAN_FCITX_HAS_CUSTOM_IM_INFO` | 5.1.11 | 0 |
| `QINGJIAN_FCITX_HAS_ADDON_FACTORY_V2` | 5.1.12 | 0 |
| `QINGJIAN_FCITX_HAS_OVERLAY_MESSAGE` | 5.1.22 | 0（仅测试用） |

加一层新的兼容：先在这里加一行门槛，再在 `compat/` 加一个头（必要时加 `.cpp` 到 `QINGJIAN_SOURCES`），
业务代码只调用它，不出现 `#if`。

## 旧版上的行为差异

| 新接口 | 5.0.14 上的做法 | 用户可感知的差异 |
|---|---|---|
| `CandidateWord::setComment` | 译词按上游 `textWithComment()` 的规则（一个空格）接在正文后 | 默认面板的显示与 5.1 一致；`CandidateWord::text()` 含译词，其他面板（如 kimpanel）与无障碍读到的正文会带译词 |
| `CapabilityFlag::Disable` | 按上游同一个位（`1ull << 40`）检测 | 无：5.0.14 的 core 原样保存客户端发来的 64 位能力；Jammy 的 fcitx5-qt 5.0.10 与 fcitx5-gtk 5.0.12 都不置这一位 |
| 虚拟键盘可见性 | 恒为不可见，不注册 watcher | 无：5.0.14 没有虚拟键盘这个概念（也没有对应事件） |
| `FCITX_ADDON_FACTORY_V2` | 用旧的 `FCITX_ADDON_FACTORY`，导出无后缀符号 | 无：5.0.14 的加载器只认这个 |
| `showCustomInputMethodInformation` | 空操作 | 切中 / 英时不弹「中 / 英」气泡；模式切换本身正常 |

密码框、私密输入与 Disable 的保护逻辑没有删也没有绕过：`syncPrivacy`、`process`、选区读取三处判断
在两种版本上走同一条路径。

## 替代实现

用户提到「通过其他方式达到类似的目的」，三处差异可以这样补，现在都没做：

- **候选译词单独成列**：5.0.14 的默认面板没有 comment 字段，最快的补法是插件自己把译词画进
  `auxDown`（只显示高亮候选的译词），但会改变版式；根治是规划里的自绘候选窗（Server 渲染、各平台贴图，
  见 [linux-fcitx5.md](linux-fcitx5.md) 与 roadmap Phase 5），译词与正文本来就在我们自己的渲染器里。
- **中 / 英切换气泡**：可以自己用 `auxDown` 显示一帧、用 `notify-send`，或移植上游
  `InputWindow` 里画 input method information 的那段；前两者都与框架自己的气泡风格不一致，第三样要跟着
  上游 UI 代码走，收益不如等自绘面板。
- **虚拟键盘**：5.0.14 没有这个功能，没有可移植的对象。

## 构建与验证

```sh
cmake -S apps/linux/fcitx5 -B target/fcitx5-jammy -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON \
  -DQINGJIAN_SERVER_EXECUTABLE=/usr/bin/qingjian-linux-server
cmake --build target/fcitx5-jammy --parallel
env -u DBUS_SESSION_BUS_ADDRESS ctest --test-dir target/fcitx5-jammy --output-on-failure < /dev/null
```

2026-11-25 在 Ubuntu 22.04.5（Fcitx5 5.0.14、GCC 11.4、glibc 2.35）上实测：全部测试通过
（5.0.14 上没有虚拟键盘事件，`shared-vkeyboard-*` 两个场景不注册）；`qingjian.so` 只导出
`fcitx_addon_factory_instance`，符号版本最高 `GLIBC_2.34` / `GLIBCXX_3.4.29`，`ldd -r` 无未定义符号；
用 5.0.14 的 `fcitx::Instance` 加载临时前缀里的配置输出 `Loaded addon qingjian`。

测试里按版本取候选正文与译词（`tests/support/context.h` 的 `candidateText` / `candidateComment`），
`Flag::Disable` 一律用 `qingjian::compat::Disable`，虚拟键盘场景用 `QINGJIAN_FCITX_HAS_VIRTUAL_KEYBOARD` 包住。

## 向上游提

这层适合作为**独立分支**提交，不并进 main 的默认构建路径：main 的 CI 与发行包都对着 5.1.19 / 5.1.12，
兼容层在那边只会多一份没人跑的代码。

- 分支名建议 `linux/fcitx5-5.0-compat`，从 `main` 切出，只含 `apps/linux/fcitx5/` 与本页。
- 提交拆四条，便于单独回退：
  1. `refactor(linux): 候选译词与能力位收进 compat 层`（现有接口的等价搬移，5.1 行为不变）
  2. `feat(linux): Fcitx5 插件兼容 5.0.14`（版本表、旧版实现、测试适配）
  3. `docs(linux): 记 5.0.14 兼容层与降级行为`（本页 + linux-fcitx5.md 的构建门槛）
  4. 可选的 CI：在 22.04 容器里跑一次上面的构建与测试，防止兼容分支烂掉
- PR 说明要写清楚：为什么留分支而不是改 main 的下限（main 的预编译包与 AppImage 都按新版本构建，
  降门槛不减少任何产物）；降级只有「切中英无气泡」与「`text()` 带译词」；隐私逻辑未改；
  验证环境是 Ubuntu 22.04.5 + Fcitx5 5.0.14，5.1 上的回归由 main 的 CI 保证。
- 维护成本：上游新增接口时不自动跟进——兼容分支只在有人报 22.04 的问题时按需 rebase；
  `compat/` 的宏表是唯一需要改的地方。22.04 的标准支持到 2027-04，之后这个分支可以删。

另一条路是**改 main 的下限**并把 5.0 兼容并进去。代价是 main 上多一份没有 CI 覆盖的代码路径，
且预编译包仍然按 5.1.12 构建、不会因此多支持一个发行版；除非上游愿意为 22.04 单独出包，否则不划算。