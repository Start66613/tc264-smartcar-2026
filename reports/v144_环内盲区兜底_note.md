# v144 —— 环内盲区兜底（HEX D4335CE6 / 2026-10-11 17:30 烧录）

**起底**：v143 整树（BE6E1A07） ｜ **改动范围**：仅 big_ring.c/h（+11 行纯新增）

## 改动内容
**LOOP 相位 + 连续 ≥2 帧无有效线 → 注入温和右转合成线（mid=118, valid=1, degraded=1），
取代 track_core 的"冻结浅舵"；线回来（下一帧 valid）即停止注入，自然恢复。**
- 单帧闪断**不触发**（≥2 帧）
- TAIL（出环口）**不注入**（保守：出环口需直行）
- 依据："环内回正帧+盲区=失败配方"双证据（run11：f100-103 回正 → f104 盲区 → 冻结 -4.8° → 转不过来 → 停车；
  run12：同位置回正+速度冲 2232，f105 恰好单帧闪断才逃过）

## 影子验证（全绿）
- LOOP 盲区 ≥2 帧 → 注入 mid=118 ✓（原版不注入=失败配方）
- 单帧闪断不触发 ✓；TAIL 不注入 ✓
- v141 左转协调回归 ✓；TURN 注入回归 ✓（mid=124 不变）
- 一键复验：`python scripts/run_blind_ring_check.py`

## 验证回执（四门禁）
- host 测试 **12/12** + v144 专项全绿
- 编译 **0 错 0 警**；HEX **D4335CE6**（123254B）
- **FlashVerified=true**；回读 flash=runtime=D4335CE6 一致、fault=0、cam_adv=True
- 实车复测：**待你跑**

## 回退（三级）
- 回 v143：`cp big_ring.c.bak_v144pre big_ring.c; cp big_ring.h.bak_v144pre big_ring.h` → 重编 = **BE6E1A07**
- 回 v142：`cp track_core.c.bak_v143pre ...; cp track_core.h.bak_v143pre ...` → 重编 = **47CD2352**（再叠加 big_ring 回退）
- 或直接烧对应 zip 内 HEX

## 试车关注点
1. 环内（进环后~5-6s）若遇盲区：舵应改"温和右转"继续绕环（不再冻结浅值）
2. 正常段与 v143 完全一致（≤1 帧闪断不触发）
3. 出环口（TAIL）遇盲区行为不变（不注入）
