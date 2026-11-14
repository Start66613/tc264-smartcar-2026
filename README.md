# TC264 智能车 · 2026 赛季全记录

> 从零基础到完赛 **40 秒** —— 7 天、198 个版本、25 次可验证烧录的完整工程档案。

![status](https://img.shields.io/badge/比赛成绩-40s-brightgreen)
![versions](https://img.shields.io/badge/版本包-198-blue)
![flashes](https://img.shields.io/badge/可验证烧录-25-orange)
![days](https://img.shields.io/badge/从零到完赛-7天-red)
![files](https://img.shields.io/badge/归档文件-5064-lightgrey)

---

## 这是什么

一台基于 **逐飞 TC264D** 的摄像头循迹智能车，从 2026-10-05 写下第一行代码，到 2026-10-11 上场比赛。

这不是"成功学复盘"，而是一份**带证据链的工程档案**：

- 每个版本都能回退（源码 + HEX 指纹 + 逐字节复现验证）
- 每个改动都有数据支撑（15 轮实车记录、9 张盲区存帧、862 帧成功轮）
- 每个失败都留着（被否决的方案、回退记录、误判与修正）

## 成绩

| 项 | 结果 |
|---|---|
| **比赛成绩** | **40 秒**（2026-10-11） |
| 实测最好单圈 | 39 秒（v123，大环模块关闭） |
| 含大环完整跑通 | 46.9 秒（v143 run12，终点自动停车） |
| 比赛用固件 | v146 / `C4E5B2DC` |

---

# 📖 快速导航

| 我想… | 去这里 |
|---|---|
| 看整体复盘 | [`docs/01_比赛总结.md`](docs/01_比赛总结.md) |
| 学踩过的坑（最值钱） | [`docs/02_经验与教训.md`](docs/02_经验与教训.md) |
| 看系统怎么设计的 | [`docs/03_技术架构.md`](docs/03_技术架构.md) |
| 查某个版本改了什么 | [`docs/04_版本全史.md`](docs/04_版本全史.md) / [`versions/manifest.csv`](versions/manifest.csv) |
| 看某轮跑车的数据 | [`data/`](data/) + [`docs/05_数据资产.md`](docs/05_数据资产.md) |
| 学工程流程 | [`docs/06_工程方法论.md`](docs/06_工程方法论.md) |
| 下载完整工程 | [Release v1.0-archive](https://github.com/Start66613/tc264-smartcar-2026/releases/tag/v1.0-archive) |
| 直接读比赛固件源码 | [`source/TC264_VisionTrack/`](source/TC264_VisionTrack/) |
| 复现一次构建 | [`scripts/`](scripts/) + [`docs/06`](docs/06_工程方法论.md) 第二节 |

---

# 🗂 完整目录索引

```
├── README.md                      本文件（总索引）
├── docs/                          文档：总结 + 当时的专题（27 个 .md）
│   ├── 01_比赛总结.md             ★ 主报告
│   ├── 02_经验与教训.md           ★ 30 条踩坑实录
│   ├── 03_技术架构.md             视觉/控制/环岛/记录
│   ├── 04_版本全史.md             198 版演进 + HEX 索引
│   ├── 05_数据资产.md             15 轮记录 + 9 张存帧
│   ├── 06_工程方法论.md           影子验证/四门禁/归档
│   ├── 07_下载与校验.md           Release 分卷合并与 SHA256
│   ├── TC264固件工作流_SKILL.md   可复用操作手册（46 KB）
│   └── original/                  当时的 19 篇专题（未删改，见下表）
├── reports/                       61 份数据分析报告（见下表）
├── data/                          145 个原始数据文件（见下表）
├── versions/                      198 个版本的抽取源码
│   ├── manifest.csv               全量清单（含 SHA256）
│   ├── INDEX_原始.md              当时的版本索引
│   ├── README.md                  版本目录用法
│   ├── VisionTrack/               54 版（比赛固件）
│   ├── Senior6_RingDiag/          49 版（旧架构主线）
│   ├── Senior6_PinAdapt/          61 版（引脚适配/早期）
│   ├── RCManual/                  6 版（遥控固件）
│   ├── CamLine/                   6 版（摄像头接缝）
│   └── Milestones/                22 版（里程碑封印包）
├── source/                        当前源码（50 个文件）
│   ├── TC264_VisionTrack/         比赛固件 v146
│   └── TC264_RCManual/            遥控固件
├── scripts/                       工具链（354 个文件）
├── logs/                          烧录/构建/回读记录（81 个）
├── hardware/                      引脚表 + 硬件设计说明
└── .gitignore
```

---

## 一、核心文档（docs/，7 篇）

| 文件 | 内容 | 适合谁看 |
|---|---|---|
| [`01_比赛总结.md`](docs/01_比赛总结.md) | 结果解读、比赛配置、七天五阶段、三场关键战役、**差距分析**、遗留问题、如果重来 | 想快速了解全局 |
| [`02_经验与教训.md`](docs/02_经验与教训.md) | **30 条**踩坑实录（方法论 6 / 视觉 8 / 控制 7 / 工程 9），每条带版本号或帧号证据 | 要动手做车的人 |
| [`03_技术架构.md`](docs/03_技术架构.md) | 两级 Otsu、双边界连续性、模型插值、PID/限速/盲区、环岛六相位 FSM、内存布局 | 想看实现细节 |
| [`04_版本全史.md`](docs/04_版本全史.md) | 198 个包 / 5 个阶段 / 28 个 HEX 指纹 / 被否决版本清单 / 回退方法 | 要追溯某个改动 |
| [`05_数据资产.md`](docs/05_数据资产.md) | 15 轮跑车记录表、9 张盲区存帧直方图、指标换算、复读命令 | 要分析数据 |
| [`06_工程方法论.md`](docs/06_工程方法论.md) | 影子验证、四门禁、烧录两铁律、改动边界、归档五件套 | 想复用流程 |
| [`07_下载与校验.md`](docs/07_下载与校验.md) | Release 资产清单、分卷合并命令、全量 SHA256 | 要下载完整工程 |

### 当时的专题文档（docs/original/，19 篇）

| 文件 | 主题 |
|---|---|
| `IMU_编码器_功能总表.md` | IMU + 编码器接口与参数总表 |
| `v100_丢线爬行找回_20261009.md` | 丢线后满舵爬行找回 |
| `v101_弯道降级与满舵找回_20261009.md` | 急弯降级（不停车） |
| `v102_交叉口全白不判丢线_20261009.md` | 整行全白不计入丢线 |
| `v103_路口近处全白惯性补线_20261009.md` | 路口惯性补线（判据移出循环） |
| `v104_视觉升级_模型引导中线提取_20261009.md` | **测量行 + 模型插值** |
| `v105_视觉升级_蓝白赛道_20261009.md` | **两级递归 Otsu + 双边界连续性** |
| `v106_十字路口中线闪烁修复_20261009.md` | 种子资格闸门 + 路口顶住 |
| `v107_十字路口远端锚点拉线_20261009.md` | 远端锚点拉线 |
| `全量控制代码复核_d4_20261007.md` | 控制链全量代码复核 |
| `大圆环状态机交接文档_v135.md` | 环岛 FSM 完整交接（含开源参考包） |
| `大环实测数据_20261007.md` | 大环里程/宽度实测 |
| `大环接手复核_20261007.md` | 大环方案复核 |
| `学长代码对照_中线十字环岛陀螺_20261010.md` | 与国赛代码对照 |
| `摄像头接缝排查_20261007.md` | 1px 缝导致中线断裂 |
| `环岛逻辑隔离界限_v140.md` | 环岛模块与巡线的隔离契约 |
| `给Codex的图片分析提示词.md` | 图像分析提示词模板 |
| `速度参数快照_20261010.md` | 速度参数快照 |
| `v097_review_20261008/` | 一次完整代码评审（含快照） |

---

## 二、数据分析报告（reports/，61 份）

### A. 里程碑与实车分析（12 份）

| 报告 | 关键结论 |
|---|---|
| `v141_全程环岛成功轮_数据分析_20261011.md` | ★ 首次全程含环岛跑通（913 帧 / 49.3s） |
| `v141_第2轮_45.382s_数据分析_20261011.md` | 复现验证（836 帧，环岛走到 TAIL） |
| `v141_第3轮失败_282帧_分析_20261011.md` | 环内失线 2.5s → 盲飞（三缺口定位） |
| `v141_第4轮失败_576帧_分析_20261011.md` | 失败点不在环内，在环外"岔路+洞" |
| `v142_第5轮成功_812帧_含盲区存帧首图_20261011.md` | **盲区存帧第 1 张** |
| `v142_第6轮_小圆环区乱跑_954帧_20261011.md` | 小环区连续丢线 → 反向回起点自停 |
| `v142_第7轮_环内盲区停车_167帧_对比分析_20261011.md` | 进 LOOP 首帧全盲 → 盲飞 2.4s |
| `v142_第8轮_大环内盲区_186帧_20261011.md` | 首次拍到车头偏出赛道 |
| `v142_第10轮_9.2秒超长盲区_662帧_20261011.md` | 史最长盲区（9.2s） |
| `v143_首次实车_run11_198帧_20261011.md` | 降速生效，但舵冻结在回正帧 |
| **`v143_完整成功轮_run12_862帧_20261011.md`** | ★★ **完整成功轮 + 盲区降速双验证** |
| `v144_run13分析_锁舵机定位_20261011.md` | 7.6s 僵持根因（两计数器互相清零） |

### B. 版本改动说明（v1xx_note 系列，24 份）

| 报告 | 版本 |
|---|---|
| `v124_C_ring_before_after.md` | v124 白面顶住前后对比 |
| `v125_hold_direction_fix.md` | v125 统一"保持方向" |
| `v126_revert_note.md` | v126 回退到 v123 |
| `v127_analysis_note.md` | v127 路宽防污染（被否决） |
| `v129_long_record_note.md` | v129 记录扩到 2000 样本 |
| `v130_record_ab_note.md` / `v130_run_0132_analysis.md` | v130 记录 A/B + 首轮 721 帧分析 |
| `v131_brake_floor_speedup_note.md` / `v131_cring_diagnosis.md` | v131 刹车下限 + C 环诊断 |
| `v132_blind_slow_note.md` | v132 长盲区降速（首版） |
| `v133_bigring_rev_note.md` | v133 环岛状态机重写 |
| `v134_bigring_robust_note.md` | v134 检测器稳健化（30 变体） |
| `v135_bigring_noleft_note.md` | v135 环内禁止左转 |
| `v136_1px缝合并修复_note.md` / `v136_封存标记.md` | v136 1px 缝合并 + 封存 |
| `v137_丢线保持舵角_note.md` | v137 丢线保持舵角 |
| `v138_8s保险_note.md` | v138 8s 强制关闭 |
| `v139_环岛停用十字路口_note.md` | v139 环内停用路口算法 |
| `v140_环岛舵量增强_note.md` | v140 环内舵量 +50% |
| `v141_环岛左转协调_8s保险_note.md` | v141 左转协调 |
| `v143_盲区降速_note.md` / `v143_盲区降速_方案与影子验证_待批准_20261011.md` | v143 盲区降速（方案 + 影子验证） |
| `v144_环内盲区兜底_note.md` | v144 环内盲区注入（后被回退） |
| `v145_左转协调阈值收紧_note.md` | v145 阈值 80→60 |
| `v146_弯道速度放松_note.md` / `v146_run14分析_出环反打实录_20261011.md` | **v146 比赛用版** + 出环反打证据 |
| `v147_阈值收紧_弯道再提速_note.md` | v147 阈值 60 + 弯道二次提速 |
| `v148_不进环原因与影子验证_note.md` / `v148_顺序竞态补救_note.md` | v148 顺序竞态（影子 + 实施） |

### C. 环岛专项（4 份）

| 报告 | 内容 |
|---|---|
| `bigring_reverse_plan_v1.md` | 反向环岛方案 v1（点3/点2/点1 三锚点） |
| `bigring_reverse_entry_frame_0230_note.md` | 进环锚点帧标注 |
| `大圆环状态机交接文档_v135.md` | 完整 FSM 交接文档（可移植） |
| `环岛逻辑隔离界限_v140.md` | 隔离契约（BIG OFF ⇒ 全不执行） |

### D. 国赛/开源对比（5 份）

| 报告 | 内容 |
|---|---|
| `国赛对比_环岛与视觉_20261011.md` | 总览对比 |
| `国赛深挖2_环岛视觉_20261011.md` | CQU 10 阶段 + connect_line；22届 Width_Max |
| `国赛深挖3_环岛视觉_20261011.md` | NEUQ 硬保险；HammerTa 几何特征 |
| `国赛深挖4_环岛视觉_20261011.md` | CQU 角点两套实现；SJTU 局部自适应阈值 |
| `国赛深挖5_环岛视觉_20261011.md` | caipingjiang 灰度不二值化；**element() 里已有通用 FSM** |

### E. 固件审计（5 份）

| 报告 | 内容 |
|---|---|
| `固件审计报告_v142_20261011.md` | 总报告 |
| `固件审计_隐患扫描_20261011.md` | **B-1 必现缺陷**（丢线计数被 ISR 布尔化） |
| `固件审计_死代码_20261011.md` | 14 死函数 + 13 死全局（5.2% 源码，链接器已剔除） |
| `固件审计_历史残渣_20261011.md` | ~2.4GB 可回收清单 |
| `固件审计_编码注释_20261011.md` | 推翻"7000 乱码"，真问题是 GBK 声明 vs UTF-8 存储 |

### F. 其他（6 份）

| 报告 | 内容 |
|---|---|
| `醒来先看_工作总览_20261011.md` | 当时的工作总览 |
| `定时任务停止_全天汇总_20261011.md` | 8 份报告的全天汇总 |
| `下一步优化清单_v144影子验证_20261011.md` | 优化优先级清单 |
| `finish_reverse_0050_analysis.md` / `finish_reverse_0050_optimization.md` | 反向完赛轮分析 + 优化 |
| `RC遥控固件_操作卡_20261011.md` | 遥控固件操作速查 |

---

## 三、原始数据（data/，145 个文件）

> 全部来自实车记录，未做任何加工。命名规则：`<固件>_run<N>_<内容>.<扩展名>`

| 扩展名 | 数量 | 内容 |
|---|---:|---|
| `.bin` | 54 | 原始二进制（记录数组 / 存帧灰度 / 21KB 每帧） |
| `.json` | 32 | 解析后的结构化数据（每帧字段） |
| `.txt` | 30 | 可读的记录表（含每帧 mid/steer/speed/phase） |
| `.png` | 26 | 存帧渲染图 / 对比图 / 赛道图 |
| `.jpg` | 3 | 现场照片 |

### 覆盖的轮次（14 轮）

| 轮次 | 对应文件 | 结果 |
|---|---|---|
| run5 / run6 / run7 / run8 / run10 | `*run5*` … `*run10*` | v142 阶段（含盲区存帧） |
| run11 / run12 | `v143_run11*` / `v143_run12*` | 首次实车 / **完整成功轮** |
| run13 | `v144_run13*` | 锁舵机实录 |
| run14 | `v146_run14*` | 出环反打实录 |
| run15 | `v147_run15*` | 不进环（顺序竞态） |
| v141 run2/3/4 | `v141_run2*` … | 里程碑轮 + 两次失败 |

### 关键文件

| 文件 | 是什么 |
|---|---|
| `v143_run12_rows.json` | **成功轮 862 帧逐帧数据**（最完整的成功样本） |
| `v143_run12_raw.bin` | 成功轮原始二进制 |
| `run5_blind_frame_auto.png` | **盲区存帧第 1 张**（过曝 53%） |
| `run14_blind_frame_auto.png` | 盲区存帧第 9 张 |
| `bigring_frames_compare_0230_vs_0243.png` | 两个锚点帧对比 |
| `v144_run13_rows.json` | 锁舵机 535 帧（僵持段在 f418-534） |
| `bigring_reverse_entry_frame_0230.bin` | 点3（武装）标定帧 |
| `point1_capture.zip` / `point2_capture.zip` / `point3_capture.zip` | 三锚点原始采集包 |

---

## 四、版本源码（versions/，198 版）

### 分组

| 目录 | 版本数 | 时间 | 说明 |
|---|---:|---|---|
| `VisionTrack/` | **54** | 10-09 → 10-11 | **比赛固件**（v001 → v148） |
| `Senior6_PinAdapt/` | 61 | 10-05 → 10-08 | 引脚适配与早期实验 |
| `Senior6_RingDiag/` | 49 | 10-07 → 10-09 | 旧架构主线（v063 → v099） |
| `Milestones/` | 22 | — | 里程碑封印包 + 交接包 |
| `CamLine/` | 6 | 10-07 | 摄像头接缝专项 |
| `RCManual/` | 6 | 10-08 → 10-09 | 遥控娱乐固件 |

### 关键版本（VisionTrack）

| 版本 | HEX | 意义 |
|---|---|---|
| `v021` | `31AA87EC` | 大环首次通过（跟最左白段） |
| `v079` | — | **摄像头首次正常巡线**（撕裂修复） |
| `v105` | `31459250` | **两级 Otsu 蓝白分离**（measured 7→46） |
| `v106` | `7BDF9725` | 种子资格闸门 + 路口顶住 |
| `v123` | `504F2ECE` | **GOLDEN · 39 秒完赛（除大环）** |
| `v134` | `D2ED57B1` | 环岛检测器稳健化 |
| `v141` | `9620E1C0` | **首次全程含环岛跑通** |
| `v143` | `BE6E1A07` | **盲区降速 · 完整成功轮 46.9s** |
| **`v146`** | **`C4E5B2DC`** | **比赛用固件** |
| `v148` | `47743D54` | 顺序竞态补救（赛后回退） |

> 完整 HEX 索引见 [`docs/04_版本全史.md`](docs/04_版本全史.md)；每版一句话说明见 [`versions/INDEX_原始.md`](versions/INDEX_原始.md)。

---

## 五、当前源码（source/）

| 目录 | 文件数 | 说明 |
|---|---:|---|
| `TC264_VisionTrack/` | 41 | 比赛固件 v146（`user/` + `code/` + 工程配置） |
| `TC264_RCManual/` | 9 | 遥控固件（740 中位版） |

主要文件：

| 文件 | 行数 | 作用 |
|---|---:|---|
| `user/cpu0_main.c` | 3979 | 主循环、按键、屏幕、记录、诊断 |
| `code/track_core.c` | 847 | 视觉 + 控制核心 |
| `user/line_ui_assets.c` | 676 | 屏幕 UI 资源 |
| `code/cam_image.c` | 448 | 两级 Otsu、二值化 |
| `user/isr.c` | 261 | 中断配置 |
| `code/big_ring.c` | 183 | 大圆环状态机 |
| `code/track_adapter.c` | 145 | 适配层、报告回读 |
| `code/track_core.h` | 137 | VTFrame / VTTracker / VTControl |

---

## 六、工具链（scripts/，354 个文件）

| 类别 | 代表脚本 | 用途 |
|---|---|---|
| **构建** | `build_tc264_servo_check.py` | headless AURIX 构建 |
| **校验** | `check_expression_hex.py` | HEX 逐行 checksum / 仅 PFlash |
| **烧录** | `flash_tc264.py` | 烧录 + FlashVerified |
| **回读** | `read_vision_track_report.py` / `read_track_trace.py` / `read_camera_frame.py` | DAS 只读回读 |
| **host 测试** | `run_vision_host_tests.py` | 12 套件（PC 上跑真代码） |
| **影子验证** | `run_blind_shadow_check.py` / `run_corner_speed_check.py` / `run_premouth_check.py` | 一键复验 |
| **归档** | `build_github_archive.py` / `stage_repo_content.py` | 本仓库的生成脚本 |
| 诊断 | `diag_cross_frames.c` / `analyze_frame.py` / `ascii_frame.c` | 逐行 dump / ASCII 渲染 |

---

## 七、运行日志（logs/，81 个）

| 类型 | 文件 | 作用 |
|---|---|---|
| 烧录结果 | `*-flash-result.json` | HEX SHA256 + `FlashVerified` |
| 构建结果 | `*-build-result.json` | 源文件哈希 + 产物哈希 |
| HEX 校验 | `*-hex-check.json` | checksum 校验结果 |
| 运行回读 | `*-runtime-report.json` | `fault` / `cam_adv` / 样本 |
| 烧录史 | `flash-history-v116.md` | 41 条按时间的烧录记录 |
| 构建日志 | `*-build.stdout.log` | 完整编译输出（0 错 0 警证据） |

---

## 八、硬件（hardware/）

| 文件 | 内容 |
|---|---|
| `PINMAP.csv` | 引脚分配（含 confirmed/provisional 标注） |
| `DESIGN_SPEC.md` | 硬件设计说明 |

---

# 📦 完整归档下载（Release）

仓库里是**可浏览的源码与文档**；每个版本的**完整工程**（含逐飞库、可直接编译）在 Release：

**➡️ [Release v1.0-archive](https://github.com/Start66613/tc264-smartcar-2026/releases/tag/v1.0-archive)**

| 文件 | 大小 | 内容 | SHA256 前 12 |
|---|---:|---|---|
| `01_docs_reports.zip` | 13.2 MB | 全部文档 + 61 份报告 + 清单 | `D7DA6506640E` |
| `02_source_and_toolchain.zip` | 1.0 MB | 当前源码 + 354 个脚本 + 81 个日志 | `F66C04FD70F4` |
| `03_versions_all.zip.01~06` | 454 MB | **198 个原始版本包**（完整工程） | `838E4297F5DE` |
| `04_versions_source_extracted.zip` | 10.1 MB | 每版本抽取的纯源码（4243 个文件） | `1A00FC7B05D6` |

合并分卷：

```bash
cat 03_versions_all.zip.0* > 03_versions_all.zip
sha256sum 03_versions_all.zip
# 期望 838e4297f5dede34e6d63971ac76689eb8fb86fb99fad6b4f5730bea3e6b6b98
```

详见 [`docs/07_下载与校验.md`](docs/07_下载与校验.md)。

---

# 🔧 怎么用

### 查两个版本差在哪

```bash
cd versions/VisionTrack
diff -ru v145_leftcoop_tighten_91E8FA16_flashed v146_corner_speed_C4E5B2DC_flashed
```

### 找某个参数是什么时候改的

```bash
grep -rn "turncap" --include=*.c versions/ | sort
```

### 找某个 bug 的修复版本

```bash
grep -rln "pre_mstamp" versions/     # v148 顺序竞态
grep -rln "blind_frames" versions/   # v143 盲区降速
```

### 回退并重新烧录

```bash
cd firmware/TC264_VisionTrack/code
cp track_core.c.bak_v146pre track_core.c          # 回到 v143
python ../../scripts/build_tc264_servo_check.py TC264_VisionTrack TC264_VisionTrack \
    --no-wipe --no-indexer --fresh-workspace
python ../../scripts/check_expression_hex.py --project TC264_VisionTrack
# 期望 SHA = BE6E1A07...
python ../../scripts/flash_tc264.py TC264_VisionTrack vision-track
```

---

# 三句话总结

1. **赢在"先数据后改动"**：每次调参都对着记录里的帧号和数值，不靠手感。v143 的盲区降速是 6 次失败记录逼出来的唯一正解。
2. **输在"盲区"**：环岛所有失败都伴随 ≥1 秒长盲区；9 张存帧证明那段画面里真的没有线。
3. **真正带走的是流程**：影子验证 → 四门禁 → 烧录 → 回读 → 归档。198 个版本里没有一个"说不清是什么"的固件。

---

# 诚实声明

- 比赛当轮**没有板上记录**（未开记录通道），40s 为用户计时报告。
- 环岛识别只做了**右环**；四环中的左环/其余环未实现（`element()` 里已有通用 FSM，未接）。
- 遗留缺陷 3 项（锁舵机僵持、B-1 丢线计数、相机偶发无信号），详见 [`docs/01`](docs/01_比赛总结.md) 第五节。

---

*档案整理：2026-10-26 ｜ 全部数据来自本机 `D:/智能车`*
