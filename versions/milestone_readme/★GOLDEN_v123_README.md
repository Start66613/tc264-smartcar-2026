# ★★★ GOLDEN v123 —— 完赛特别版（39 秒，除大圆环外）★★★

**封印日期**: 2026-10-11 00:25（北京时间）
**HEX SHA256**: 504F2ECEDF57C560D188DE402ECD3F72994A823FAC5C7CE1CE05B0B9EC5202C1（122678 字节）
**实车成绩**: 2026-10-10 深夜 **完赛 39 秒**（除大圆环外全赛道）
**当前车上**: 就是本版本（2026-10-11 00:25 烧录，FlashVerified=true，回读 cam_adv/fault0/imu1 正常）

## 本版关键行为（全部实车验证过）
- 速度/舵机节奏：slew 150、speed scale 100%（v123 恢复原速，用户要求）
- v121 种子海选：长链走廊胜过底部碎片（J1 劫持修复）
- v120 不可信中线守卫、v117 弯道降速
- v122 陀螺阻尼（符号经实车记录验证 ✓）、big_ring 出环角可配
- 大圆环模块：**默认关闭（BIG OFF）**——本版成绩为除大圆环外完赛

## 已回退、不在本版（留档在 outputs/）
- v124 白面顶住初版、v125 异常帧保持方向、v127 路宽防污染+白面顶住 —— 均经用户判定回退
- 相关源码快照: track_core.c.v127_rejected / .bak_v127 / .bak_v125 / .rejected_v124v125

## ★ 铁律（本版 = 基线，动它之前必须满足）★
1. **先数据后改动**：任何修改必须先给出帧/记录/回放证据 + 理论方案，**报告用户批准后才动手**；
2. **不批准不改**；一次只动一处；
3. 改完必须：全套 host 测试 + 0错0警 + HEX 校验 + 实车复测，四者齐备才算数；
4. **本 zip 是回退保险**：任何新版出问题，解出源码/HEX 一键烧回本版。

## 回退/烧录方法
解压本 zip → 用 firmware/TC264_VisionTrack/Debug/TC264_VisionTrack.hex
或还原源码后:
  python scripts/build_tc264_servo_check.py TC264_VisionTrack TC264_VisionTrack --no-wipe --no-indexer --fresh-workspace
  python scripts/flash_tc264.py TC264_VisionTrack vision-track
