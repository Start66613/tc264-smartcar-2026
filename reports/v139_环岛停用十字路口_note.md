# v139：环岛模式下"十字路口算法"停用（用户方案；v135 底 + 8s 保险）
## 状态：已构建+全测通过+HEX 校验+**已烧录（07:46，FlashVerified）**——HEX `FF0C8F1A`

## 一、用户需求与实现
需求：连接转弯处画面"看着像十字路口"，怀疑十字路口算法把中线按直行处理导致冲出赛道；
不改算法本身，让"十字路口算法"在环岛模式下停用；以 v135 为底；保留 8s 保险。

实现（3 个文件，一处概念）：
1. `track_core.h`：VTTracker 增加开关字段 `suppress_cross`；
2. `track_vision.c`：每帧写开关 —— `suppress_cross = (big_ring_enabled && big_route.phase != BR2_DONE)`
   （BIG ON 且模块**尚未完成**期间 = 环岛模式；模块 DONE 后自动恢复）；
3. `track_core.c`：开关为 1 时跳过全部十字路口处理（共 4 处）：
   - FULL行"顶住"判据（crossing）；
   - v107 远端锚点拉线补全；
   - v108 跨白带续行（prefer straight ahead）；
   - 远端合并路口重建。
   **保留** ok_rows==0 全盲兜底（近全白帧仍顶住——环岛白区安全网不拆）。

## 二、底座说明
- track_core.c/h = **v135 原样**（v137"丢线保持舵角"已按用户要求回退；其专项测试同步撤下
  → scripts/test_vision_track_hold.c.v137_rejected）；
- big_ring.c/h = v138（8s 强制关闭保险，保留）。

## 三、实测行为差（关键数据，帧级对比 suppress 0/1）
| 帧 | suppress=0（旧） | suppress=1（新） |
|---|---|---|
| junction_1824（真路口帧） | measured=20 model=63 mid=117 | **measured=55 model=28 mid=122**（改用真实追踪链） |
| 存帧0605（连接转弯） | mid=93 | mid=93（**不变**——该帧不走路口分支，见备注） |
| cross_1634 / j1_static / 0230 / 0243 | — | 逐字段一致（不受影响） |
| ringc_sheet（近全白） | all_white=1（顶住） | all_white=1（顶住，安全网保留） |
> **备注（重要）**：用户截图帧 0605 的"直行中线"来自模型外推（不是路口分支），本版**不改变**它的输出。
> 若实车复测仍在该点冲出，数据支持的下一个候选 = v136 的"1px 缝合并"（封存包已就绪），
> 可与本版叠加成一版（v135+缝合并+停用+保险）。

## 四、验证（四门禁，烧录项待办）
| 项 | 结果 |
|---|---|
| host 全量（新增 test_vision_cross_off 四场景） | **13/13 通过** |
| 构建 | 0 错 0 警 |
| HEX | `FF0C8F1A2535A50F97387EDA5FECF31177D34F9AC24E2CAD7A1A9E853775383A` 校验通过（122698 字节） |
| 烧录 | ✅ **已烧（07:46，FlashVerified）**；回读 fault=0 / cam_adv=true / 三方指纹一致 |

## 五、回退
- 回 v138（v135+保险，无停用）：`cp track_core.c.bak_v137pre track_core.c` + `track_core.h.bak_v137pre track_core.h`
  + `cp track_vision.c.bak_v139pre track_vision.c` → 重编（sha 应=9FF057B9…）→ 烧。
- 回 v135（无保险）：再 `cp big_ring.c.bak_v138pre big_ring.c` + `big_ring.h.bak_v138pre big_ring.h` → 重编。
- 上 v136（缝合并）：封存包 `_versions/v136_1px缝合并_CA8577E0_封存_20261011.zip` 内 README。

## 六、实车验证要点
1. 长按 P20.6 → BIG ON；跑到连接转弯（大圆环左上）；
2. 观察：路口型"顶住/拉线"在环岛段（模块 DONE 前）不再生效，帧改用真实追踪链——
   若该点画面确实"像路口"，中线应比之前更贴近实际赛道形状；
3. 若仍冲出：立即回报（P20.7 录一段最好），下一步上"缝合并+停用"叠加版。
