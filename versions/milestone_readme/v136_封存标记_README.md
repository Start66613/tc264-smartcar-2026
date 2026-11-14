# v136「1px缝合并」封存包（2026-10-11）

**封存时间**：2026-10-11 06:44（用户指定："这个版本标记下"）
**HEX(sha256)**：`CA8577E03EE5FE826416E81413902FC6308FED3CFB1D1717007B519DC2376506`（295545 字节）
**状态**：已烧录验证（FlashVerified，06:37）；主机测试 12/12；构建 0错0警；
**实车未测**。
**封存原因**：用户判定 v135（`D3074C9C…`）**环岛跑得最好**（0504/0510 双跑均成功完赛环岛），
本轮先烧回 v135；v136 留档待测。

## v136 是什么
修复存帧 0605（大圆环左上"连接转弯"）的"中线整条竖直指向赛道外白色地板"：
1px 浅缝（灰度 224~247，仍高于 Otsu 241 → 被判黑）把赛道白块切成两段 →
逐行双边界连续性连续 3 行失败 → 追踪链在 y≈85 断裂 → 上方 58 行无测量全部
竖直外推 → mid 恒 93（车以为直行）。
**修复**=逐行白段提取增加"1px 缝合并"（并回上一段；合并后宽 ≤VT_SEG_MAX 才生效）。
**效果**：measured 20→50、model 63→33、mid 93→115（跟随赛道右弯）、clip 0→10。

## v136 与 v135 的唯一差异
`track_core.c` 单处（逐行白段提取的 1px 缝合并；`big_ring.c` 等其余文件逐字节一致）。

## 恢复 v136（重编字节级可复现）
```
cd /d/智能车
cp firmware/TC264_VisionTrack/code/track_core.c.v136_marked firmware/TC264_VisionTrack/code/track_core.c
python scripts/build_tc264_servo_check.py TC264_VisionTrack TC264_VisionTrack --no-wipe --no-indexer --fresh-workspace
python scripts/check_expression_hex.py --project TC264_VisionTrack
# 期望 sha256 = CA8577E03EE5FE826416E81413902FC6308FED3CFB1D1717007B519DC2376506
python scripts/flash_tc264.py TC264_VisionTrack vision-track
```
重建 sha 若与上表不符 → 源码被其他改动污染，先查再烧。

## 回退 v135（本次已执行，板上现为 v135）
```
cd /d/智能车
cp firmware/TC264_VisionTrack/code/track_core.c.bak_v135 firmware/TC264_VisionTrack/code/track_core.c
# 重编 → 校验 sha256=D3074C9C91BE2B24E6225C26B2AF9324DB3DFACB6D05D3CE8071AEAEE80DBF9E → 烧录
```

## 包内文件
- `hex/TC264_VisionTrack_CA8577E0.hex` —— v136 烧录固件（与封存 sha 一致）
- `code/track_core.c` —— v136 源码（恢复用）
- `code/track_core.c.bak_v135` —— v135 源码（当前车上版本，备用）
- `v136_1px缝合并修复_note.md` —— 完整修复记录（诊断/验证/效果）
- `v136_修复对比_0605帧.png` —— 同帧对比图（红=修复前/绿=修复后）
