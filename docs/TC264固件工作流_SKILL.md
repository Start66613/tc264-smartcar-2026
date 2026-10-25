---
name: tc264-firmware-pipeline
description: 在逐飞 TC264（英飞凌 AURIX）智能车工程上新建固件、编译、校验 HEX、烧录到板子。当用户说"写个代码烧进去""烧录""编译烧到板子上""加个新工程""把图片/摄像头显示到屏幕""循迹车固件改一下""视觉/中线/二值化/蓝白赛道""蓝区误检""十字路口看不到中线/中线闪烁/全白"等涉及 TC264 / 逐飞库 / D:\智能车 的项目时使用。包含无头 ADS 构建、AURIXFlasher 烧录、版本存档、蓝白赛道视觉诊断（两级递归大津法 + 左右边线分离 + 真实帧 A/B）、ASCII 二值图渲染 + 帧级逐行 dump 定位法、十字/环岛入口"拉线补全"（远端锚点 + 两点直线，对齐国赛 connect_line）、以及"烧不进去"的排错顺序。
agent_created: true
---

# 逐飞 TC264 固件：新建 → 编译 → 校验 → 烧录

## 环境事实（先读，能省半小时）

| 项 | 值 |
|---|---|
| 工程根 | `D:\智能车\firmware\<工程名>` |
| 脚本 | `D:\智能车\scripts\` |
| 日志 | `D:\智能车\logs\`；版本存档 `D:\智能车\firmware\_versions\` |
| ADS | `D:\Infineon\AURIX-Studio-1.10.10`（无头构建，不用开 IDE） |
| 烧录器 | `D:\Infineon\AURIX-Studio-1.10.10\tools\AurixFlasherSoftwareTool_v3.0.4\AURIXFlasher.exe` |
| ADS 暂存 | `D:\Infineon\projects\<工程名>`（脚本自动复制过去，路径要纯英文） |
| 用的 python | `C:\Users\ASUS\.workbuddy\binaries\python\versions\3.13.12\python.exe`（脚本只用标准库） |
| 图像处理 | `C:\Users\ASUS\.workbuddy\binaries\python\envs\default\Scripts\python.exe`（唯一装了 Pillow 的解释器） |

**用户这块板是和别的会话共用的**：烧录会互相覆盖。烧之前先确认"现在是谁在用这块板"，
并提醒用户上一次烧的是哪个工程（看 `logs/*-flash-result.json` 里最新的那条）。

## 标准四步

```bash
cd /d/智能车

# 1) 编译
python scripts/build_tc264_servo_check.py <工程名> <日志名> --no-indexer --no-wipe
#    ⚠️ 必须带 --no-wipe：脚本默认会先清空 D:\Infineon\projects\<工程名>\Debug（几百个文件），
#       会被批量删除保护拦住，只吐一行 [safe-delete][SAFE_DELETE_BULK_CONFIRM_REQUIRED] 就退出。
#       --no-wipe 保留暂存树、覆盖式拷贝，效果一样且不触发保护。
#    失败先看是否 "Workspace already in use!" -> 再考虑加 --fresh-workspace
#    残留进程：Get-Process AURIX-studioc | Stop-Process -Force（Git Bash 下 taskkill //PID 会失效）
#    主机侧编译（host 测试）另有一条坑：cc1.exe 要 libgmp-10.dll，
#       必须把 E:\tool\Dev-Cpp\MinGW64\bin 加进 PATH，否则 gcc 静默 rc=1 且零输出。

# 2) HEX 校验（只允许 PFlash、逐行校验和、无重叠）
python scripts/check_expression_hex.py --project <工程名>

# 3) 烧录（-erase on -prog on -ver on，自动读回校验）
python scripts/flash_tc264.py <工程名> <日志名>

# 4) 存档（可选但用户明确要求过：每个版本都要能回退）
#    参考 scripts/snapshot_line_adapt.py 的做法，或直接 zip 整个工程（排除 Debug）
```

**成功标志**：`flash-*.json` 里 `"FlashVerified": true`。
`scripts/build_flash_snapshot.py` 是"构建+校验+烧录+存档"一条龙，但它**写死了循迹车工程**，
换工程要照着改。

**⚠️ 改完代码必做"补丁真进了编译吗"这一步**（2026-10-08 有过一次惨案：补丁写进了
`#if RING_ENABLE 0` 的死代码区，构建显示成功、HEX 却逐字节和上一版相同，排查了半小时）：
- 比对 `logs/<日志名>-build-result.json` 的 `artifactSha256` 与上一版是否**不同**；
  HEX 没变就是补丁没生效，别急着烧。
- 反过来别把补丁打进条件编译的关闭分支里，改前先确认那段代码真的参与编译。
- `build_tc264_servo_check.py` 现在同时哈希 `user/` 和 `code/`（2026-10-09 前只哈希 `user/`，
  重构后算法层搬到了 `code/`，等于没有源码↔HEX 绑定）。
- `package_<工程>.py` 会断言暂存区源码与工程源码逐字节一致，跑一次可确认构建用的就是改过的源码。

**配套 host 测试**：见 `tc264-host-unit-test` skill（PC 上先验证纯逻辑，再上车）。
VisionTrack 的视觉链路有统一 runner（**2026-10-09 起，别再手动一个个编**）：

```bash
cd /d/智能车 && python scripts/run_vision_host_tests.py
```

它编译并运行 **12 个用例**（9 套件 + 3 个真实帧回归；真实 `cam_image.c` + `track_core.c`/`big_ring.c`，`-std=c99 -Wall -Wextra`）：
`test_bigring_rev_detect`（33 变体+8 负样本）/ `test_bigring_rev_fsm` / `test_vision_bluewhite` / `test_replay_crossing`（真实帧回放）/
`test_vision_track_cross` / `_logic` / `_lost_recover` / `_start` / `test_vision_clipline` + 3 个真实帧回归（路口 e0/e3、J1）。
host 编译用 mock 头 `scripts/mock_vision/zf_common_headfile.h`（与 PID 用的 `tests/host/mock` 分开，
不要混）。编译器 `E:/tool/Dev-Cpp/MinGW64/bin/gcc.exe`，**必须先 `export PATH`**（见标准四步里的坑）。

**"未烧录版"怎么收尾归档**（下载器不在 / 用户只要改代码时）：
`package_vision_track.py` 有 `assert fl['FlashVerified']`，没烧录就套不上。
照着 `scripts/finalize_vision_track_v105.py` 改：源码一致性 + 构建门禁(0/0) + HEX 校验
→ 写 `bin/validation.json`（`flash.flashed_this_version=false`，把上一版成功烧录记录放进
`flash.last_known_flash` 保留可追溯，**不要冒用 `FlashVerified: true`**）+ 打
`firmware/<工程>.zip` + 拷到 `firmware/_versions/<工程>_<标签>_<时间戳>.zip`。

## 版本封存与回退（烧回旧版本）——2026-10-11 v136→v135 沉淀

**用户说"把这个版本标记下 / 烧回那个版本"时的标准动作**（模板脚本 `scripts/mark_v136_seal.py`）：

1. **先封存当前版本（务必在动任何源码之前！）**：
   - `cp code/<改过的文件> code/<文件>.vXXX_marked`（一键恢复用；本工程改动通常只在一两个文件）；
   - 封存包 `firmware/_versions/vXXX_<标签>_<HEX前8>_封存_<日期>.zip` 内容：**HEX 文件**、当前版本源码、
     上一版源码（bak）、README（是什么/为何封存/如何恢复/如何回退）+ note/对比图；outputs 放副本 + 一页标记 md。
   - 版本状态写三处：`logs/flash-history-v116.md`、`_versions/INDEX.md`、MEMORY.md。
2. **回退源码**：`cp code/<文件>.bak_vXXX code/<文件>`。先核对改动面——用封存 zip 里的副本逐字节比对没动过的文件
   （如 "big_ring.c 与 v135 副本一致: True"），确认要还原的只有已知文件。
3. **重编 → 用"历史记录里的 HEX sha256"做字节级闸门（关键！）**：
   重建 sha 必须 == 该版本当初烧录的 sha256——从 `outputs/vXXX_..._flashed.zip` 里的
   `logs/vision-track-flash-result.json` / `runtime-report.json` 取期望值。
   **一致 = 证明源码树与该版本逐字节相同 + 工具链可复现**（实例：v135 重建 sha=`D3074C9C…`，与 04:56 原版逐位一致）；
   不一致 = 有文件被污染或工具链漂移，**先查再烧**。
4. **烧录 + 回读**：`flash_tc264.py` → `read_vision_track_report.py`。
   ⚠️ 烧录后**立刻**跑回读可能报 `Incoherent report`（启动窗口 report 未就绪/读到更新中）——
   等 ~10 秒重跑，或先裸读 `track_report` 看 signature/uptime 是否在涨（涨=板子正常）。
5. **恢复封存版**：`cp code/<文件>.vXXX_marked code/<文件>` → 重编（重建 sha 应=封存 HEX 的 sha）→ 烧录。

## 改控制算法前：先查这两个参考库（别闭门造车）

| 资产 | 路径 | 看什么 |
|---|---|---|
| 智能车开源参考库（16 项目，README 逐项点评） | `D:\智能车\references\Smartcar_OpenSource_20261007\` | **HammerTa**（同 TC264+逐飞库）：`CODE/IMG_DEAL.c` 的左/右边界独立跟踪 `left_jump`/`right_jump`、单边丢线用另一边 + `TrackWild[row]/2` 补线；**SJTU AuTop**：`Project/CODE/imgproc.c` 边界搜索、`USER/src/main.c:538` 双边跟踪 + 局部角度 + 几何前瞻 |
| 国赛学长 6.0 对照报告 | `D:\智能车\reports\MINE-20261008\07_学长代码_对照报告.md` | 652 行极简内核 vs 我们；丢线=**满舵+降速**；差速"内轮刹死咬弯"（1:10） |

## 丢线/停车类问题的排查顺序（v101 血泪）

1. **先看相机帧龄，别先怀疑算法**。相机不出帧会让 `vt_step` 的 `now-v->stamp>150u` 判
   `VT_CAMERA` 直接停车，症状和"弯道丢线停车"**一模一样**。
   `python scripts/read_vision_track_report.py` 看 `camera_advancing` / `frame_sequence` / `imu_ready`；
   `main_age` 很小但 `camera_age==uptime` 就是相机一帧没出（查排线/供电）。
2. 再看 `fault` 码区分 `VT_LOST`（丢线）/ `VT_CAMERA`（相机）/ `VT_STALL`（堵转）。
3. 最后才动控制参数。

## 诊断视觉类问题：读板上真实帧 → 离线回放（**别只靠推理**）

用户说"车就在赛道上，你可以读数据啊"时，要的是**用实测数据定位根因**，不是猜。
这套闭环在 2026-10-09 治"路口看不到中线"时一次命中，值得固化成标准动作：

```bash
cd /d/智能车
# ① 从板上 dump 真实灰度帧（CPU1 DSRAM，分块读，见下）
python scripts/read_camera_frame.py cam_gray      # 存 tmp/frame_gray.bin + PNG
# ② 离线复刻固件的大津法 + 逐行白段结构，看清"哪几行是整行全白、哪几行是单段"
python scripts/analyze_frame.py
# ③ 把这帧真图喂给固件同款 vt_analyze，验证改动是否真的治好了
gcc -std=c99 -I firmware/TC264_VisionTrack/code scripts/test_replay_crossing.c \
    firmware/TC264_VisionTrack/code/track_core.c -o tmp/vt/replay.exe && tmp/vt/replay.exe
# ④ 烧完再回读，做闭环确认
python scripts/read_vision_track_report.py       # 看 fault / confidence / camera_advancing
```

**为什么必须读 CPU1**：摄像头 `cam_gray`/`cam_bin` 在 **CPU1 DSRAM（0x60000000 段）**，
而常用的 `das_read_memory.exe` **只允许 CPU0 段（0x70000000）**，读 CPU1 一律 `rc=2`。
必须换 **`tmp/das_read_camera_memory.exe`**（支持 0x60000000 段），且**单次 ≤1024 字节**，
所以 `read_camera_frame.py` 是分块拼的（22560 字节 = 188×120 → 23 块）。

| 坑 | 现象 | 处理 |
|---|---|---|
| 读 CPU1 `rc=2` | "无法读取" | 换 `das_read_camera_memory.exe`，别用 `das_read_memory.exe` |
| `cam_bin` 尾部越界 | 最后一块 `rc=2`（`cam_bin`@0x6000da1c + 22560 越过 0x60012000 上限） | 诊断改用 `cam_gray`（整段在范围内） |
| `symbol not found` | objdump 符号行解析错列 | 找类型列用 `parts[2]`（形如 `6000da1c g O .bss.cpu1_dsram 00005820 cam_gray`），**不是 `parts[3]`** |
| 模型看不了图 | Read PNG 报 "does not support images" | 用 `analyze_frame.py` 做数值化行结构分析替代肉眼看图 |

**记录表单位换算（v135 校准，分析事故必用）**：`read_track_trace.py` 的 `center` 列 = `v->mid × 128/188`
（**64 = 直行；<64 = 向左打舵；>64 = 向右**）；`steer` 列 = 度×10（±100 = ±10° 满舵）；`gyro` 正=左转。
定位「跑反/掉头」类事故：先找 steer 持续饱和段 + gyro 积分方向反转的时刻，再看该时刻 mid 的偏移。

**存帧读取（P20.7 长按存单帧 → 电脑回读，v136 用过）**：用户在车上按 P20.7 存一帧后：
- 板上标志：`cam_saved_valid`、`cam_saved_sequence`（CPU0 段 .data），灰度在 `line_trace`（CPU0 .bss, 22560B），中线快照在 `cam_saved_mid`（CPU0 .bss, 120×int16）；
- 读图：`python scripts/read_camera_frame.py line_trace` → `tmp/frame_saved.bin/png`（自动检查 valid/seq 防串帧）；
- 读当时中线：`das_read_memory.exe 0x700005c4 240 out.bin` → 120×`<h` 解包（**这是"屏幕当时画的那条线"的像素级证据**）；
- 地址用 objdump 自动解析，**别硬编码**（每次编译会变）。

**影子验证：不改工程源码先证明修复有效（v136 沉淀，强烈推荐）**：
- **影子验证标准做法（v143 实战范式）**：①复制 track_core.c/.h → tmp/shadow_vXXX/（副本）；②副本内打补丁（结构体加字段+功能块，注意本工程缩进是单空格制、行尾 LF）；③写合成序列测试（直接构造 VTFrame 喂 vt_step——正常巡航段从 t=2500 起步**避开 2s 起步 ramp**，encoder 固定值模拟低反馈/打滑；全白帧=max(valid=0,degraded=1,all_white=1,mid=-1)）；④gcc 编译原版与影子两份（-I 顺序让影子头文件优先），跑同序列对比；⑤原版复现问题+影子显示修复=全绿，产出"待批准"报告。**一键复验脚本范式：scripts/run_blind_shadow_check.py**（编译两版+跑+断言输出）。
1. 复制工程 `track_core.c` 到 `tmp/shX_core.c`，用 Python 脚本做**断言式替换**（`assert src.count(old)==1` 防止改错位置）；
2. 测试程序用 `#ifndef TCORE / #define TCORE "track_core.c" / #include TCORE`，编译时 `-DTCORE='"tmp/shX_core.c"'` 一行切换原版/变体；
3. 对比输出（`mid/measured/model/degraded` 逐帧 + 中线数组），确认"目标帧改善 + 历史帧零回归"后再动工程源码；
4. 工程改完后**再跑一遍同一对比**证明两版行为一致（我改动=已验证的影子版）。
诊断模板：`scripts/diag_frame_0605.c`（逐行类别/白段/中线/种子 dump + br2 检测器值）；
影子对比模板：`tmp/shadow_cmp.c` + `tmp/run_shadow_tests.py`。

**1px 缝断链（v136 根因，2026-10-11）**：赛道白路与赛道外白色地板之间可能只被一条
**1px 浅缝**（灰度 224~247，仍高于 Otsu 阈值→判黑）切开。逐行追踪的"双边界连续性"检查
在缝行全部失败，**连续 3 行失败即 break** → 上方几十行无测量 → 第 3 遍"竖直外推"把中线
插到赛道外 → 表现为"中线整条竖直指向远处白色地板"。修复=逐行白段提取时**合并 1px 缝**
（并回上一段；合并后宽度必须 ≤VT_SEG_MAX 才生效）。诊断特征：`model_rows` 远大于
`measured` 且中线在断点上下折成直角/竖直。

**大圆环模块（big_ring.c / br2）速查**——详细设计文档：`D:/智能车/docs/大圆环状态机交接文档_v135.md`（含全部代码/参数/移植指南）。
- 三锚点状态机：点3武装 → 点2(1st)右转进环 → 环内(左转协调) → 点2(2nd)直行 → 点1退出；全部通过改写 VTFrame 的 mid 注入（右转=124，直行=94；v141 起环内=左转协调：mid>=80 放行，mid<80 → 右转 118）。
- **br2 诊断字节**（记录表 b2 列，v134+）：`bit0..2=arm/mouth/exit 命中，bit3..7=相位`（0=OFF…6=DONE）。**DONE=52**；模块完成后永久静默（本轮不再武装）——若某段行为异常但 b2 恒 52，先排除环岛模块。
- 状态机测试 `scripts/test_bigring_rev_fsm.c`、检测器回归 `scripts/test_bigring_rev_detect.c`（33变体+8负样本）；锚点帧在 `scripts/fixtures_bigring_rev/`。
- **环岛模块 × 丢线/舵角行为交互**（v137 核实）：
  - 模块在 TURN/STRAIGHT 阶段**强制 valid=1**（注入 mid=124/94、special_route=1）→ 丢线路径**不可能触发**；
  - LOOP 阶段只夹左转 + special_route=1 → 丢线路径**会触发**（模块不管这块），但 v137 的"不可信→保持舵角"规则被 special_route 门控**不介入**；
  - TAIL 阶段只夹左转、**未标记 special_route** → v137 规则理论上可触发（要彻底屏蔽需给 big_ring.c TAIL 补 special_route=1，一行）。
- **"丢线保持舵角"沿革（v120→v137）**：v120 = `measured<12 → e=0`（强制回正，J1 满左锁修复）；v137（用户方案）= `measured<VT_MID_TRUST_MIN(24) && !special_route → 保持舵角逐位冻结`。症状区分：车在"假中线（竖直外推≈直行）"段**舵角被慢慢回正→直冲出道** = 回正行为；盲区中**舵角乱变** = 另有问题。专项测试 `scripts/test_vision_track_hold.c`（真帧 tmp/fixtures/floor_0605_gray.bin，断言：保持生效 + special_route 让位）。
- **v138 8s 强制关闭保险**：开跑后（首次 running 0→1 计时）8s 未 DONE → 强制 DONE（静默/零注入/不再武装）。实现 `BR2_GUARD_MS=8000u`（big_ring.c，一处）。正常一跑≈7.8~8.0s 自然 DONE → 保险静默；记录里 **b2 相位列在 ~8s 直接跳 6=保险触发**（正常路径会先经 5/TAIL）。副作用：30s(ARMED)/25s(LOOP) 老超时在持续跑动下不可达（保留兜底）。慢跑误伤 → BR2_GUARD_MS 调 9000~10000 一行。测试 `test_bigring_rev_fsm.c` sc2（9000ms 不触发 / 9020ms 触发边界）。
- **v139 环岛模式停用十字路口算法**（用户方案）：`VTTracker.suppress_cross` 开关——track_vision.c 每帧写 `= big_ring_enabled && phase!=BR2_DONE`；track_core.c 4 处门控（FULL顶住判据 / v107远端拉线 / v108跨白带续行 / 合并路口重建），**ok_rows==0 全盲兜底保留**（近全白帧仍顶住=环岛白区安全网）。实测差：junction_1824 measured 20→55（改用真实链）；0605/cross_1634/j1 零变化。测试 `test_vision_cross_off.c`（四场景）。**注意**：0605 帧的直行中线源自"模型外推"（非路口分支），此开关不改它——若该点仍冲出，叠加 v136 缝合并。
- **v140 环岛舵量增强**（用户方案）：`VTFrame.steer_boost_pct` 字段 + vt_step 一行放大 `if(special_route && boost>0) turn*=1+boost/100`；big_ring 在 TURN/LOOP/STRAIGHT 写 `BR2_STEER_BOOST_PCT=50`（big_ring.h 一行可调）。依据：v139 修好了"直行"问题但环内"打舵不够→脱道"。**隔离**：无 special_route 时 80 拍逐位零影响（`test_vision_steer_boost.c`）。**环岛隔离界限表**：docs/环岛逻辑隔离界限_v140.md（六条环岛行为门控清单 + "BIG OFF⇒全不执行 / DONE⇒全失效"回归保证）。
- **★★★ v141 里程碑版（2026-10-11 09:05 封存；全天候现役主力 + 回退保险基线）★★★**：封印包 `firmware/_versions/★★★里程碑_v141_全程含环岛一次跑通_20261011.zip`（65文件；全源码+HEX/ELF+日志+913帧记录+回退备份；另存outputs/同名），README=`★里程碑_v141_README.md`。**实车里程碑：全程+大圆环首次一次跑通**（车发动→停49.3s、VT_FINISH自停、环内满左0帧、左转协调生效、8s保险=车发动后整8.0s）。四门禁齐备（测试12/12+0错0警+HEX校验+实车复测）；源码40文件与构建记录逐字节一致。回退：烧包内HEX / 还原源码重编 / 退v134。（构成：从 **v134 整树重新起底**（v135~v140 的禁左/保险变体/停用路口/舵量增强**均已移出现役源码树，仅存档**），只加两处、全部在 big_ring.c/h 纯新增：①**左转协调**（用户原话"大左转时让他右转"）——LOOP/TAIL：mid>=80 轻微左转**放行**；mid<80 大左转 → **mid=118**（94+24）；旋钮 `BR2_LEFT_SOFT_MID`/`BR2_LEFT_FIX_BIAS`。②**8s 保险**（**起点=车发动/首个 running 帧**，用户明令：非武装帧）→ 到点未 DONE 强制 DONE；`BR2_GUARD_MS=8000`。环外逐字节零改动（track_core/track_vision/adapter/cpu0 与 v134 核验一致）；回退=`cp big_ring.c.bak_v141pre big_ring.h.bak_v141pre` → 重编(=`D2ED57B1` v134)。测试 12/12（sc5 左转协调 + sc2 车发动起算边界）。注意：全程跑若发车→环岛 >8s，模块会在到环岛前自动关闭（用户指定语义；要改起点只改一行条件）。**2026-10-11 全程+环岛成功轮实测（913帧，outputs/v141_full_run_65s_trace.txt）**：车发动f13→VT_FINISH终点自停f758（49.3s，全程一次跑通）；环岛段f42-136（ARM→TURN→LOOP→STRAIGHT，**保险在车发动后整8.0s=f136触发于STRAIGHT相位**——全程跑"发车→环岛走完"本身≈8s，保险≈每次都在出环口附近关模块，本轮无碍）；**环内(LOOP)满左0帧**（对照旧v134轮同段满左+10°约1s）；**左转协调实际生效**：f109 mid=56轻左放行(+2.3°)→f110-113连续4帧mid钉死在118(fw)=大左改右转（判据=118仅自然孤立出现、连续4帧必为重写）；驾驶段跳变仅3处；特征点近停(<300)5处全在P6段=既有行为（同v130完赛轮8处同类，非环岛改动引入）。分析报告=outputs/v141_全程环岛成功轮_数据分析_20261011.md。 **第2轮实测（同日稍晚，836帧，用户计时器45.382s）**：再跑通一轮——环岛f43-130比首轮多走到TAIL相位（首轮止于STRAIGHT）；环内满左0帧；左转协调f106-108连续3帧；驾驶段无效线17帧全孤立、最大跳变仅1处（两轮画像高度一致=环岛逻辑稳定）。分析报告=outputs/v141_第2轮_45.382s_数据分析_20261011.md。 **★失败模式（同日第3轮，282帧）：环内长失线→三缺口连锁★** 环内 f89-128 失线2.5s（含1.8s全白）期间：①全白帧转向被冻结（track_core L774 reliable_steering，设计给路口，环内=不转弯飞出）②速度不降（"丢线找回"降速路径明确排除 all_white 帧；仅 degraded-320 上限≈巡航65-75%）③VT_WHITE_MAX_FRAMES=100拍(2s)停车差9拍未触发（f89-116=1.82s后被2帧闪现有效线重置）。对照：成功轮环内失线≤4单帧 vs 失败轮38帧；进环前段逐帧几乎一致 → **病根=失线兜底缺失被偶发长失线引爆**。修复候选（环内专用，待批准）：A 盲区≥2帧注入温和右转 mid≈118 + measured=20/degraded（目标压220，约巡航1/3）；B 注入全量右转124；C 环内全白0.8s → special_fault 停车保险。（后段另有独立现象：f223-229 车轮堵转~400ms→VT_STALL 停→自动重启，f211-228 中线36-73但 e=0（v120不可信规则），均属普通行车段既有行为。）分析=outputs/v141_第3轮失败_282帧_分析_20261011.md。 **第4轮（同日，576帧，VT_LOST自停）确认该模式非环岛专属**：环岛六相位全通+环内零长盲区（历史最干净），但出环后21秒在"岔路+洞"特征点冲进全白盲区2.2s+2s→自停；成功轮同点（run1 f478-488/run2 f471-480）是减速~450-600爬行通过、仅1-2帧失线。三缺口原样复现（全白冻结/速度反升到degraded上限/2s停车被闪烁绕过）。**修复范围结论：必须覆盖"任何长全白盲区"，不能只改环岛模块**。另：连续记录只有8路数字信号无图；单帧=P20.7长按存 line_trace（120×188 灰度+中线快照，读法 `python scripts/read_camera_frame.py line_trace`→tmp/frame_saved.bin/png，先查 cam_saved_valid/sequence 防串帧）。分析=outputs/v141_第4轮失败_576帧_分析_20261011.md。 **★v143 盲区降速（2026-10-11 16:48 烧 BE6E1A07）★**：track_core.c/h +15行——连续≥2拍无有效线（含AW）→ 强制压爬行档（每拍-160→450）跳过PI、线回恢复；单帧闪断不触发。根因=盲区时PI积分反向加油（本工程 track_core 丢线降速路径排除 all_white 帧）。影子验证范式见 scripts/run_blind_shadow_check.py。回退=cp track_core.c/h.bak_v143pre→重编(=47CD2352)。
- **v143 首次实车验证（run11，198帧）**：降速确认生效（环内盲区 1440→444，6拍压住；对比 run7 同场景 648→1980 攀升）。**但环内盲区仍有新缺口：舵冻结在最后可信值，若那值是"回正帧"（如-4.8°）则不够绕环（需-6~-10°）→ 爬行后丢线停车**。**v144 候选=环内(LOOP)盲区时把冻结舵替换为"环向最小转向"**（mid=118合成线注入 或 上一段绕环平均右转值下限）。
- **★v143 完整成功轮（run12，862帧，17:15）**：全程跑通+VT_FINISH自停；**降速实车双验证**：f368-371（1956→672）与 f738-753（1764→444稳住12帧→恢复→终点）。对照 v142"越盲越加速"= 根治。**"盲区降速"从影子验证到实车成功的完整闭环**（该轮环内无长盲区，转向缺口未踩到）。
- **v144 环内盲区兜底（影子验证全绿 2026-10-11 17:30）**：问题="环内回正帧+盲区=失败配方"（冻结浅舵转不过来）。方案=LOOP+连续≥2帧无有效线→注入 mid=118 温和右转合成线（big_ring.c/h +11行，复用 TURN 注入同型机制）。影子=tmp/shadow_v144/ + scripts/test_bigring_blind_ring.c + scripts/run_blind_ring_check.py（一键复验）。待批准烧录。
- **★v144 已烧（2026-10-11 17:30，HEX D4335CE6）**：环内盲区兜底落地。四门禁全过+专项测试全绿。回退=cp big_ring.c/h.bak_v144pre→重编(=BE6E1A07)。
- **v144 已回退；v145 已烧（2026-10-11 17:42，HEX 91E8FA16）**：左转协调阈值 80→60（big_ring.h 一行）。**"锁舵机"机制（run13）**：全白帧/非全白帧交替→两个停车保护计数器互相清零都攒不满→无限僵持（444爬行+舵冻漂着）；修复候选=统一盲区计时（A）/超时主动慢转（B）**待用户决策**。阈值 60 依据：误伤段（原始70-79放行）+ring_backwards_0428（28-60捕捉）。
- **⏪ 当前基线（2026-10-11 17:47）：v143 = BE6E1A07 = run12 完整成功版。v144（环内盲区注入）/v145（阈值60）已作废回退。注意"7.6s 超长盲区僵持"（全白/非全白计数器互相清零）问题仍存在于 v143 中，修复待用户新思路。
- **★v146 已烧（2026-10-11 18:03，HEX C4E5B2DC）= v143 + 弯道速度放松**（用户："弯道速度提高一点"）：track_core.c 两行——turncap 360→400（硬弯260→300）、弯度限速 ×10/240→×8/200（深弯220→260）。影子范式=scripts/run_corner_speed_check.py（orig=tmp/pre_v146 快照 vs shadow=tmp/shadow_v146）；注意：改弯道上限会牵动 test_vision_track_start.c 的深弯守卫断言（220→260 已更新）。回退=cp track_core.c/h.bak_v146pre→重编(=BE6E1A07)。
- **★v147 已烧（2026-10-11 18:20，HEX 8DDA1A37）**：左转阈值 80→60 + 弯道二次提速（turncap 440、弯度 ×6/160）。依据 run14 出环反打实录（P5 注入 118 反打→关闭后才可左转）。**读板规程实战**：v146/v147 间用 16 符号比对确认零漂移后才读 run14——此规程连续两次防坑成功。回退：big_ring.h.bak_v147pre + track_core.c.bak_v147pre → 重编（=C4E5B2DC v146）。
- **★v148 顺序竞态补救（2026-10-11 18:41 已烧，HEX 47743D54）**：修 run15"不进环"——根因=**确认顺序竞态**（口特征 f135-136 先凑齐确认，当时还在 OFF 被丢弃；武装 f138 才确认；此后口不再出现 → 卡 P1 永不进环。run12/14 顺序正 → 正常进环）。方案=OFF 期跟踪口预确认（2hit/165ms 同标准）+ 武装确认时 1.2s 内口刚确认过 → **直接进环**（big_ring.c/h：宏 `BR2_PREMOUTH_WIN=1200`、+3 字段 `pre_mhits/pre_mstamp0/pre_mstamp`、OFF 分支、enter() 清理）。影子=tmp/shadow_v148 + tmp/pre_v148（v147 快照）；一键复验 `scripts/run_premouth_check.py`（双模式 -DEXPECT_SHADOW）。**正式套件已加 sc6 竞态场景**（test_bigring_rev_fsm.c）。四门禁全过（12/12、0错0警、FlashVerified、回读一致）。回退：cp big_ring.c/h.bak_v148pre → 重编（=8DDA1A37 v147）。⚠ 烧后相机无信号（dma_int=0/lost=1；与 v147 除 big_ring 零差异 → 非本版引入；排查=断电重启+查排线；库里 lost_flag=1 且 dma_int=0 = 从启动起从未收到 DMA 中断）。
- **★当前去向（2026-10-11 21:54）**：用户 18:56 指定回退 v146（C4E5B2DC = 赛车固件）→ 比赛（40s 成绩）→ 21:54 换烧 **RC 遥控固件**（比赛结束，休息娱乐）。赛车固件恢复 = `flash_tc264.py TC264_VisionTrack vision-track`（树=v146，hex 已建好可直接烧）。
- **★RC 遥控固件（TC264_RCManual）重烧流程与事实（2026-10-11 沉淀）**：枪式遥控 SBUS 版。构建=`build_tc264_servo_check.py TC264_RCManual <stem>`；烧录=`flash_tc264.py TC264_RCManual rc-manual-740`。**烧前先拿 rc-manual-740-build-result.json 逐字节核验**（sourceSha256×9 + artifactSha256×5；本次全 OK）；staging 与源码树两份 hex 都要 sha 对照。硬件：接收机 SBUS→P10.6（UART2 100k 8E2）、舵机 P33.9 中位 740（750 会左偏）、电机 P02.4-7；**P10.5/P10.6 = boot 引脚**（逐飞告诫；接收机占其为官方示例配置）。行为：软启动（扳机回中才解锁）/失联 500ms 停车回中/CH4 三段档 30-50-80%/CH5、6 ∓500 微调上限（1200~9000）。回读=`uart_receiver@0x60000318`（**CPU1 DSRAM**，须用 das_read_camera_memory.exe；结构=channel[6]×u16 LE + state + finsh_flag；普通 das_read_memory 只允许 0x70000000-0x70012000，读 CPU1 会被拒）。**烧后 DAS hot attach error 0x4 = 调试链接不在**（2026-10-11 两次实证：车被拿走/下载器断开必现；重试无效，等待重新接线；不影响已完成的烧录）。
- **★文件行尾陷阱（2026-10-11 实中，重要）★**：本工程**混合行尾**（big_ring.c=纯LF、big_ring.h=CRLF、track_core.c=LF、test_bigring_rev_fsm.c=CRLF）。**改任何文件前先诊断行尾**（`b.count(b'\r\n')` 对比 `b.count(b'\n')`），写回按原行尾。踩坑链：假设 CRLF → assert 失败；shadow 被 heredoc 写成 CRLF → diff 全行红；anchor 因空格数（' *   改动' 3空格 vs ' * 改动'）不匹配 → index 报错。**稳妥姿势**：①Write 落脚本文件再执行（避 heredoc 转义）；②用 repr() 先 dump 精确 anchor；③多行改写用"读入 split + 修改 + join 写回"或"shadow 内容 normalize 后整体写入"；④改完用 `diff <(tr -d '\r' < a) b` 或逐字节 sha 验证。

**★⚠️ELF 符号地址漂移坑（2026-10-11 实中）★**：**任何重编后符号在 CPU0 DSRAM 的地址会变化**（曾只因 +15 行代码就漂移 4 字节）。若用"新 ELF + 板上旧固件"去读内存，会读出**垃圾数据**（曾在 line_trace_count 处误读出 2421 的假帧数、相位序列全是假的）。**规程：每次读板前先确认三件事**——①板子喷的/回读的 HEX sha（或 flash-result.json 的 SHA256）与目标版本一致；②ELF 是那个版本的构建产物；③存档每版 ELF（如 tmp/elf_v142/）供跨版本读旧数据时使用。**读到的数据先做合理性检查**（帧数≤2000、相位序列单调合法、fault 码可信）再采信。

**★v142 盲区自动存帧（2026-10-11 实装并实战验证）★****★v142 盲区自动存帧（2026-10-11 实装并实战验证）★**：连续3拍无有效线（≈195ms）自动把该帧灰度存入 line_trace 缓冲（cam_saved_auto=1，meta: flags/br2/ms/recf 五变量）；读法：`python scripts/read_camera_frame.py line_trace`（自动识别 BLIND-AUTO/MANUAL 并打印 meta+渲染PNG）。**踩坑：TC264 的 CPU0 DSRAM（0x70000000-0x70012000, 72KB）是连续分配**，余量 12.6KB 时加 28B 变量也曾触发 ltc E112（cannot locate N sections）——**加任何 CPU0 变量前先用 elf+objdump 算余量**。腾挪手法：ring_diag 缩容（无读取工具无消费者，128→64/32→16，-5.7KB）。**首图结论（run5 f232）**：盲区画面=53%过曝白+35%中间调斜带+黑像素0.5%——**线真的不在画面里（不是阈值误判）**，所以"盲区行为兜底（减速+温和保持转向）"是唯一正解，"局部阈值"救不了无对比度的帧。报告=outputs/v142_第5轮成功_812帧_含盲区存帧首图_20261011.md。 **第二次命中（run6 f406，小圆环区）同为过曝型**（58%过曝+32%中间调+暗1.5%）→两个不同地点两次独立触发、"过曝盲区"系统性确立（报告=outputs/v142_第6轮_小圆环区乱跑_954帧_20261011.md）。 **⚠️操作坑：读存帧命令绝不能接 `| head -N`** —— head 提前关闭管道令脚本 SIGPIPE 中断，frame_saved.bin/png 会残留**上一次的旧文件**（造成"两帧相同"假象，2026-10-11 实际中招）。正确做法：`python scripts/read_camera_frame.py line_trace > tmp/read.log 2>&1`，然后对读出的 bin/png 做 sha256 核验（连续两次读必须哈希不同）。） **第3/4弹（run7 f62、run8 f101，均大环内 LOOP）同为过曝型**（54.9%/56.9%）；run8 f101 首次拍到**场外结构（车头已偏出赛道——上半部墙面/横幅+下半部98%过曝白）**。**★环岛失败相关性终定论：失败轮 run3/run7/run8 全部伴随环内≥1s长盲区；成功轮 run1/2/5 环内零长盲区 → 环内长盲区=大环头号杀手。**报告=outputs/v142_第7轮_环内盲区停车_167帧_对比分析_20261011.md / v142_第8轮_大环内盲区_186帧_20261011.md。

**判据量必须在正确时机求值**（v102→v103 的教训）：
`track_core.c` 的 `vt_analyze()` 是 `for(y=110; y>=28; y--)` **近→远**扫描，
所以 `valid`（选中真实边界才 `++`）要扫到**远处**才非零。
拿 `valid` 当"要不要给近处补线"的前置条件会**死锁**——近处行轮到它时 `valid` 还是 0。
要判断"这帧到底有没有真看到路"，用**循环结束后**的 `real_rows`（补线行不计入）。

## 视觉症状定位：先渲染 ASCII 二值图 + 帧级逐行 dump（v106/v107 沉淀）

**别读数字、别只看 PNG——把整帧印成 ASCII。** `scripts/ascii_frame.c`：

```bash
gcc -std=c99 -I scripts/mock_vision -I firmware/TC264_VisionTrack/code \
    scripts/ascii_frame.c firmware/TC264_VisionTrack/code/cam_image.c -o tmp/ascii.exe
tmp/ascii.exe tmp/frame_gray.bin 2     # '#'=白 '.'=黑；x_step=2 时 94 列宽
```
> 注意：里面用了 `atoi`，**必须 `#include <stdlib.h>`**，否则 MinGW 只给 implicit-declaration 警告。

路口这种"整块白 + 局部有结构"的场面，ASCII 一眼就能看出远处哪几行是真路面。
**v107 就是靠它发现：路口帧 `y=28..46` 有一条干净横截面（x=66..120），而 `y=47..110` 整块白**
——底部 14 行全被横向白带盖住，所以"底部取种子"这条路必然失败。

**帧级逐行 dump**：`scripts/diag_cross_frames.c`（`#include "track_core.c"` **直接并入**以读它的
static 逐行缓冲；编译时**还要链 `cam_image.c`**）逐行打印
`vt_rowc/vt_sn/vt_sl/vt_sr/vt_clr/vt_cll/vt_rowv/vt_roww` + `t->mid`，并复算种子落点。

| 现象 | 含义 |
|---|---|
| 大量行 `VT_ROW_FULL` | 横向道路铺满（路口/环岛入口/整帧过曝）；仅剩的白段多半是横带边缘的**残缺白斑** |
| `measured` 极小、`model` 极大（如 1/82） | 整条中线被那 1 行劫持 → 屏幕必然"逐帧跳/闪" |
| `vt_wfit[y]==0` | 该行没学到路宽 → 单边贴边补线（要求路宽已知）也用不上 |

## 视觉通用教训（会反复踩，务必记住）

1. **凡"会决定整帧结果"的量（种子/锚点），必须与逐行路径用同一套资格闸门。**
   v105 的宽度合理性闸门（`VT_WID_LO 40%`~`VT_WID_HI 250%`）只加在 `vt_take_row()`，
   **漏在种子选择上**——一行只有一条白段时"宽度≥该行最宽段 3/4"恒真，等于零门槛，
   结果一个 16px 贴边白斑劫持整帧。
2. **判"路口"用"整行铺满(FULL)的行数"，不要用"白段太宽"。**
   急弯每行只有一条窄白带、其余是暗的，**永远凑不出 FULL 行**；只有横向道路才会让白铺满左右
   → 这条判据天然不误伤急弯。（本项目：`VT_FULL_MIN_ROWS 24` 且 `ok_rows<VT_OK_MIN 8`）
3. **`valid` 不只是"线可不可信"，它直接决定屏幕画不画线。**
   `track_adapter.c`：`line_out_center = frame.valid ? frame.mid*128/188 : -1;`
   → 任何"我想让用户看到我还在跟线"的状态（如路口顶住），`valid` 不能是 0，
   否则用户看到的是"**全白 = 根本没画线**"。
4. **`hold`/顶住类分支必须让 `v->valid=0` 之外的 `previous_mid` 不被污染**：
   `t->previous_mid` 只在 `v->valid` 分支更新，这样顶住帧不会把参考值越带越偏。

## 整块图像缺失（十字/环岛入口）：国赛统一招法是「拉线」，不是「造几何」

| 来源 | 做法 |
|---|---|
| NEFU `element.c:connect_line(x1,y1,x2,y2,way)` | 「从起始点往下**拉一条补线**」补全一边；旧版三叉口 `lian_xian_zhi(边线,行,目标列,...)` 拉直线，配 `HOW` 状态机（HOW=1 十字补线 / 3 右斜 / 4 左斜） |
| HammerTa | `k=(star_col-Col[0])/(star_row-Row[0])` —— **两点反解斜率再沿直线铺点**，专门用于"整块图像缺失"；并把补出来的点用**另一种颜色**画出来，与"真看到的"区分 |
| NEFU 缺口定位 | 在固定列上从上往下扫"白·白·黑·黑"模式，**找横向白带的上沿** |

本项目落地（v107，`track_core.c` 第 2b 遍，插在第 2 遍与第 3 遍之间）：
只在 `ok_rows<VT_OK_MIN && full_rows>=VT_FULL_MIN_ROWS`（确实有横向白带）时启用：
① 从上（远）往下找第一条可信路面横截面当**远端锚点**（复用种子资格闸门），**它本身算真实测量**；
② **拉线**：远端锚点 → 近端参考（上一帧中心）两点连直线铺满盲区 → 行分类 `VT_ROW_LINE`；
③ `VT_ROW_LINE` **计 `modeled` 不计 `measured`**（无真实边界），所以 `modeled>=24` 仍会
`degraded=1` → 路口照样限速通过。
> 关键点：**不要只"顶住上一帧中心"就完事**。远处往往真的看得见路面，用它当参考才是对的。
> 回归测试必须断言 `f.measured>=12` 之类，否则"退化成只会硬顶"测不出来。

## 本车赛道是"蓝白"：二值化才是根因（v105，血的教训）

**不要假设是灰底黑线赛道。** 本车赛道 = **蓝底 + 白色跑道 + 黑色胶带边界**，
灰度呈**三峰**：黑胶带≈40 / 蓝底 **205~220** / 白路 **248~255**（蓝白只差 30 灰阶）。

**单次全局大津法会选"黑胶带 | 其余"那个切法**（它的类间方差更大），
于是 **蓝底和白路一起被判成"白"**。真实帧实测一级 Otsu=**169**，落在蓝底之下 →
二值图里蓝底全是 255 → 每行"白段"横跨整幅 → 逐行取中点，**中线落到蓝区上**。

> 同一个根因曾以两个不同症状出现过：
> - v103「**路口的中线和前面的中线不对齐**」
> - v104「**蓝区也会识别出中线然后乱跑**」
>
> v103/v104 都在**中线环节绕**（贴边行不测量、可信测量+模型插值），没治二值化 →
> v104 的 `measured` 只剩 3 行、`model_rows=80`、中线被冻结在 61..62、`degraded=1`，
> 看起来"对齐了"，实际是把整幅中线的主动权交给了上一帧。

**修法（v105，已落地）**：
1. **两级递归大津法** `code/cam_image.c:cam_otsu_two_level()`
   —— 一级分暗/亮；**亮类占比 >62%**（说明一级只分出黑胶带）就对**亮类**重建直方图
   再跑一次 Otsu，切出"蓝底|白路"。两条兜底保证不比一级差：亮类跨度<12 回退；
   二级没比一级高>10 回退。
2. **左右边线各自分离 + 双边界连续性** `code/track_core.c:vt_analyze()`
   —— 从画面最下方 14 行取种子，**向上**逐行要求**左端与右端同时**落在上一行窗口内
   （`VT_EDGE_WIN=12`），中线=(L+R)/2。**跨蓝缝在结构上不可能**（要两端同时跳 >12px）。

**量化证据脚本**（改视觉时直接跑，别猜）：`scripts/diag_real_frame_ab.c`
—— 同一真实帧跑"旧单次全局 Otsu vs 新两级递归 Otsu"，统计"中线跑出白路的行数 /
其中落在幽灵白（蓝底被当白）的行数"。v105 实测 **30/83 → 4/83**、幽灵白 **26 → 0**。

**别照搬国赛系数**：国赛普遍是"大津法+偏置/形态学"（NEUQ `TS_OTSU_with_bias`、
CQU `Fast_otsuThreshold`+`image_filter`），那是**灰底黑线**场景推出来的数。
参考它们的**结构**（逐行求左右边线 `get_left`/`get_right`、中线=(L+R)/2），
阈值/跨度/占比**必须按本车实测量级自己推**。

**若上了两级 Otsu 蓝区还在误检**，先怀疑**相机曝光/增益**：蓝底 205~220 与白路 248~255
只差 30 灰阶，曝光一压低就两峰合一，任何二值化都救不回来。

**可观测位**（通电后在板上确认用）：相机预览页新增 `L`=大津法级别(蓝白赛道应=2)、
`wp`=白像素占比(正常 35~55)、`S`=`cam_bin_suspect`(应=0)；
`track_report` v105 起是 **28 字/112 字节**（v104 是 26 字/104 字节，追加了
`[26] otsu_level` / `[27] white_pct`），`read_vision_track_report.py` 已同步 `<28I`+`size==112`。

**反例一定要建**：治"该看见的路看不见"时，同时要测"没路了必须停"。
v103 的兜底是 `all_white` 帧数超 `VT_WHITE_MAX_FRAMES(100)≈2s` → `VT_LOST`。

## ⚠️ 归档前的日志命名陷阱（会让 validation 写错指纹）

`scripts/package_vision_track.py` 读的是**混用的固定名**，不是 `<日志名>-*`：
`logs/TC264_VisionTrack-build.stdout.log`、`logs/vision-track-flash-result.json`、
`logs/vision-track-runtime-report.json`、`logs/TC264_VisionTrack-hex-check.json`。
**用非默认日志名构建/烧录后，必须先把本次结果对齐到这些名字**，否则归档会写入上一次的指纹。
（`read_vision_track_report.py` 还会 assert HEX 与 flash-result 的 SHA256 一致，名字没对齐会直接报错。）

## ⚠️ 门禁话术必须实测产生，禁止硬编码"通过"（2026-10-09 自查发现的问题）
- `package_ring_diag.py` 曾把「40项主机检查通过」写死在 INDEX 注释和 `validation.json` 里，
  但实际**从未运行** host 测试。真相是：`tests/host/run_ring_diag.py` 的工装停留在 v092 前结构，
  对 v099 的 `PID.c` 直接报 `conflicting types for 'cring_entry_odo'`（PID.c:691 vs 667）**编不过**。
- 规矩：打包脚本里凡是"XX 检查通过"的措辞，都要由脚本**当场调用并解析结果**后生成；
  跑不了就如实写 "not_available + 原因"，不能沿用上一版的漂亮话。
- AURIX 工程侧**唯一有效门禁**是 ADS 构建 `0 errors, 0 warnings` + HEX 校验 + Flash 回读；
  主机测试只是加分项，工装与工程不同步时以 ADS 为准。

## ⚠️ 批量改多个文件时：同一文件必须串行编辑（并行 Edit 写竞争丢改动）
- 2026-10-11 v135 实测：对同一文件连发 4 个并行 Edit，回执全部"成功"，实际**只有最后 1 个写入生效**
  （读-改-写竞争），结果代码缺宏定义、测试缺函数，直到编译才暴露（undeclared / undefined reference）。
- 规矩：**同一文件的多个改动必须串行**（一次只发一个 Edit 并等回执），或写一个 python 脚本在单进程里
  做完全部替换（每处 assert old 唯一 + count 校验）。
- 改完先 grep 验证每一处改动真的在文件里（别信"成功"回执），再进编译。

## 新建一个工程

1. 从 `D:\智能车\firmware` 里挑一个**同类型、最干净**的工程复制：
   - 最简骨架 → `TC264_Expression`（user 下只有 cpu0_main / cpu1_main / isr / isr_config）
   - 图片显示 → `TC264_PhotoBlink`
   - 摄像头 → 无，参照下面的接口表自己写
2. **三处必须同时改成新名字，否则 `-cleanBuild <工程>/Debug` 找不到目标**：
   - 目录名
   - `.project` 里的 `<name>`
   - `.cproject` 里的 `artifactName`
   命令：`sed -i 's/旧名/新名/g' .project .cproject`
3. ⚠️ **删掉 `Debug/` 里复制过来的旧 `.hex/.elf/.map`**，否则极容易烧错文件。
4. **把新工程名加进 `scripts/check_expression_hex.py` 第 9 行的白名单**，否则第 2 步直接 assert 失败。
5. 删掉不用的 user 源文件（Eclipse 会自动扫描编译 `user/` 下所有 .c）。
6. 不要改 `user/isr.c` / `isr_config.h` —— 这套库的 ERU/DMA 中断表已经完整
   （`exti_ch3_ch7_isr` = VSY 优先级 43、`dma_ch5_isr` = DMA 优先级 60、
   `EXTI_CH2_CH6_INT_SERVICE = IfxSrc_Tos_dma` 供 PCLK），摄像头直接能用。

## 库接口速查（已实测核对，别凭记忆写）

- 总头文件 `zf_common_headfile.h`
- `clock_init()` —— 内部已含 关总中断→关看门狗→取频率→延时初始化→**开全局中断**，
  **不需要**再调 `enableInterrupts()` / `interrupt_global_enable()`
- 屏幕：`ips200_set_dir(IPS200_CROSSWISE)` **必须先于** `ips200_init(IPS200_TYPE_SPI)`，反了白屏。
  并口屏用 `IPS200_TYPE_PARALLEL8`。1.14 寸是另一套 `ips114_*` API。
- 灰度图：`ips200_show_gray_image(x,y,img,W,H,disW,disH,thr)`；
  宏 `ips200_displayimage03x(img,W,H)` = 它的 (0,0, 阈值0, 1:1) 版。**宽或高传 0 会卡死屏幕驱动。**
- 彩色图：`ips200_show_rgb565_image(x,y,img,W,H,disW,disH,color_mode)`，
  `color_mode = 0` = 标准 RGB565 大端（配 ST7789）；传 1 会红蓝互换。
- 摄像头：`mt9v03x_init()` 返回 0=成功；`mt9v03x_image[MT9V03X_H][MT9V03X_W]`；
  `MT9V03X_W`=188 / `MT9V03X_H`=120；`mt9v03x_finish_flag`。
  配置通道先试 SCCB（IIC，P02_2/P02_3），失败自动退 UART(9600)，同两个引脚。
- **`mt9v03x_finish_flag = 0` 必须放在显示之后**：不清→永远只显示第一帧；先清→读到半帧（撕裂）。
- 摄像头引脚：数据 `P00_0~P00_7`、VSY `P02_0`、PCLK `P02_1`。模块上的输出模式跳线要在**并口/DVP**侧。

## 大图显示（别把整屏塞 RAM）

整屏 320×240 RGB565 = 150KB，TC264 放不下。
做法：**`const uint16` 放 Flash**，刷屏时按条带搬 RAM：

```c
#pragma section all "cpu0_dsram"
static uint16 strip[320 * 8];              /* 5120 字节 */
...
ips200_show_rgb565_image(0, y, strip, 320, count, 320, count, 0);
```

生成素材用 `scripts/make_photoshow_assets.py <图片> <裁切上边> <工程名>`（4:3 裁切 + LANCZOS 缩放 + 预览图）。
它需要 Pillow → **必须用上面那个隔离环境解释器**。
生成的大 C 文件（几十万字节）**注释和编码保持 ASCII**，别混中文。

## 烧不进去？按这个顺序查

| 报错 | 原因 | 处理 |
|---|---|---|
| `Cannot initialize device connection` / `session set freq failed` | 下载器连不上**芯片**，与固件无关 | ① 板子还有没有电（电池/开关/电源灯）② 调试排线两头 ③ 下载器 USB 重插 ④ 核心板插稳 ⑤ 换 USB 线 |
| `No device found` | 板子没供电 | 下载器只通信不供电，板子必须独立上电 |
| 下载成功但屏幕全黑 | ADS 停在入口没继续 | 点 Resume / F8 |
| 烧完还是旧程序 | 没 Build 成功就去烧了 | 先 Build 成功再烧 |
| `Workspace already in use!` | 残留 ADS 进程 | 加 `--fresh-workspace`；`Get-Process AURIX-studioc \| Stop-Process -Force` |
| HEX 校验 assert 失败 | 新工程没加进白名单 | 改 `check_expression_hex.py` 第 9 行 |

**旁证技巧**：判断"是不是硬件问题"，除了看 USB 枚举，再补一次
`./tmp/das_read_memory.exe 0x80000000 64 tmp/probe.bin` ——
USB 在 ≠ 能连上芯片。这个能读成功才说明链路真的通。

救命备选：Infineon **MemTool** 直接烧 `Debug/` 下的 .hex，绕过 ADS 调试配置。
