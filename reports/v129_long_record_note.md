# v129 长录制版（2000 帧 ≈ 118 秒）——2026-10-11 00:43 已烧

## 用户需求
"把录制记录功能做的更长时间一点，我给你录一段完赛的"（原 200 帧≈12s，不够跑 39s 一圈）。

## 改动（控制逻辑零改动；仅录制系统 + 内存腾挪）
1. **TRACE_MAX 200 → 2000**（每帧 59ms 采样不变 → 约 118 秒）。
2. 内存腾挪（CPU0 dsram0 原本只剩 1.9KB）：
   - 移除 ring_diag_raw（16,384B，D4 时代环岛原始图像行缓冲，当前流程无消费者；
     ring_diag 标量采样 samples/events 保留）；
   - line_trace 25,600B → [CAM_H][CAM_W]=22,560B（弃用已无消费者的"每帧 128B 行副本"；
     存帧功能保留、地址符号不变）。
   - 结果：dsram0 free = 1,607B（新 map）。
3. 读取脚本同步：read_track_trace.py / read_ring_fp.py（NMAX 2000）/ capture_trace_run2.py。

## 验证
- 构建 0 错 0 警；SOURCES_MATCH；HEX C70B91A9 校验通过；
- 00:43:05 烧录 FlashVerified=true；
- 板上回读：cam_adv=true / fault=0 / imu=1；
- DAS 实测新数组：trace_center 2000B ✓、trace_gyro 4000B ✓、line_trace 22560B ✓ 全部可读。
- host 测试 11/11 通过（视觉链未受影响）。

## 用法（用户）
- P20.7 短按开始 → 跑一整圈（39s）→ 再短按停止。
- 可录最长 ~118 秒，跑完整个比赛 + 前后余量都够。
- 读取耗时约 30-60 秒（18 次 DAS 通道读取）。

## 关系
- 本版 = GOLDEN v123（除大圆环外完赛39s）+ 长录制；跑车行为与 GOLDEN 一致（控制逻辑未动）。
- GOLDEN v123 封印包仍为比赛基线：firmware/_versions/★GOLDEN_v123_除大圆环外完赛_20261011.zip
