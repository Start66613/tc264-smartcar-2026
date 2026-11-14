# 版本索引（每次快照自动追加）

| 版本 | 时间 | 说明 | HEX SHA256 | 关键参数 |
|---|---|---|---|---|
| v001 | 20261005-1757 | 圆环状态机+环心兜底方向+圆环上报（环1/2成功，环3待修）；舵机限幅200us | `A0281D57DDA9…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 / SERVO_TRAVEL_US 200 |
| v002 | 20261005-1758 | 舵机不对称限幅各160us+缓变防撞死（治大转弯卡底座） | `079B44348CE1…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v003 | 20261005-1817 | 圆环改宽度三段式状态机(入口宽区≥55→环带<50→出口宽区)+环内低速1400 | `A45ADF4B135D…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v004 | 20261005-1836 | 修圆环误触发(仅RUN判环+正常基线+方向不定不进环)+待机舵机保持中位 [待烧录] | `9CFEA7C5E0E8…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v005 | 20261005-1843 | 关闭圆环状态机(RING_ENABLE=0)：弯道白宽变宽被误判成进环→强制左转卡死；恢复基础循迹 | `F48019FD585F…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v006 | 20261005-2037 | 中线连续性优先：上一帧中线所在白段优先，绝不跨暗区跳段（治横跨蓝区抄近道）+新增白段数/暗区宽诊断；圆环仍关闭 | `DB37A1FD6FD8…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v007 | 20261005-2042 | （空跑：补丁未生效，源码与 v006 相同，仅作留痕） | `DB37A1FD6FD8…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v008 | 20261005-2043 | 记录时长 3.1s→11.8s：按CCD帧降采样(每4帧存1帧,与主循环解耦)【v007为空跑，本版为实际变更】 | `385CBA9D3284…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v009 | 20261005-2051 |（波形页首发，中线标记坐标有bug已由v010修复）界面新增「CCD 波形页」(20.6循环切页:主界面→波形→记录):灰度曲线+阈值线+中线标记+二值条(绿=白/红=黑),让相机所见直接可见 | `E178B6FBC2E6…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v010 | 20261005-2052 | 修正波形页中线标记坐标(应比较CCD像素而非屏幕列);v009为未重编译的空版本 | `EAE7C46FBEFD…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v011 | 20261005-2102 | 波形页增加「宽」实时读数+上报white字段（两次录制实测：蓝心从未出现在视野，gap≤1px；宽度才是可用信号） | `543509297B7A…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v012 | 20261005-2106 | 修「宽」一直显示0：白宽统计移出CalculateCenterLine(只在RUN/ARM调用)，改到主循环CCD()后无条件更新 | `7AAD3F99F992…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v013 | 20261005-2118 | ★关键修复★ 给库函数adc_convert加超时保护：它在CCD采集中断里被调用，原实现等不到ADC标志会永远死等→整个系统卡死（疑似大环转弯崩溃根因：大电流干扰ADC） | `2F7B96BB7B17…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v014 | 20261005-2135 | ★★崩溃根因修复：屏幕越界断言死机。波形页「宽」读数破100变3位数(x=262..322>320)触发屏幕库zf_assert(line332)→while(1)死机→CPU冻结时PWM保持导致乱跑。修复：布局左移+fill_rect/paint_label/draw_digits三处全局越界保护+数值限幅 | `F98E199C150F…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v015 | 20261005-2149 | v015 大环出环状态机：宽度≥95进环(RING)，绕满2.5s后再次≥95判定回到岔口→EXIT期偏差×0.2近似直行送出岔口，宽度回落<70恢复。实测数据支撑：普通路≤75/小环≤85/大环岔口95-109 | `AF08C3C6F28B…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v016 | 20261005-2157 | v016 出环升级(阈值90/绕圈2.2s/出环限速1300/冷静期60帧)+状态追踪数组(trace_state/white/center,回读可看每帧状态机决策) | `DDE3CCF633E9…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v017 | 20261005-2158 | v017 修复：trace数组移入cpu0_dsram段(DAS可读)；状态追踪+出环升级正式生效 | `818C1815414C…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v018 | 20261005-2203 | v018 出环判据改自适应「宽→窄→宽」三段式(去掉150帧时间窗,第一圈回到岔口即可出环)+诊断peak/cross上报 | `A49BE83EB170…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v019 | 20261005-2211 | v019 修EXIT误触发：阈值88→96(真岔口100-109/环内误报仅89-93)+进环后至少绕180ccd帧(2.6s)才允许出环(误报在1.3s/真一圈4.9s) | `D4BE23176127…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v020 | 20261005-2228 | v020 修出环失败两铁证：①EXIT直行最少80帧(1.2s,原来0.4秒就被“宽<70”误判中断,车没走完就恢复追踪被领回环)②冷静期60→300帧(4.4s>一整圈,原来绕圈回来冷却已过就重触发RING死循环) | `83FEBA12AD9C…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| **v021** ★ | 20261005-2239 | **★里程碑：大环通过！★** 出环改「跟最左白段」：出环瞬间视野分两段白(岔口蓝缝)，左侧=出口白路，改选最左段跟随即可弯出；不再直行(直行会冲上蓝区)。恢复条件改“单段+普通宽度连续8次” | `31AA87ECAFD5…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |

---

## ★ 里程碑版本（重点保留）★

| 版本 | 达成 | 说明 |
|---|---|---|
| **v006** | 基础循迹稳定 | 中线连续性优先（不跨暗区）、丢线自动重起步、白宽诊断 |
| **v021** ★ | **大环通过（完整跑通）** | 出环跟最左白段 + 冷静期 + 绕圈最少时长；出环瞬间顺白路弯出，不再冲蓝区 |
| 备份命名 | `★MILESTONE_v021_大环通过_20261005.zip` | v021 的专用副本，便于快速查找 |
| **v079** ★ | **摄像头正常巡线（MT9V03X 换装后首次）** | 撕裂修复（DMA 完成中断内快照）+ 方向链翻转（CTRL_MID_MIRROR）；实车确认正常循迹。遗留：速度偏慢、未进大圆环 |
| 备份命名 | `★MILESTONE_v079_摄像头正常巡线_20261007.zip` | v079 的专用副本（= v063-d4_20261007-234828），便于快速查找 |
| **v123** ★★★ | **GOLDEN 完赛（除大圆环外）39 秒** | **2026-10-10 深夜实车 39 秒完赛**（除大圆环外全赛道）；大圆环模块默认关闭。**基线锁定版本：改动须先有数据+理论+用户批准，未批准不得修改。** | `504F2ECE…` |
| 备份命名 | `★GOLDEN_v123_除大圆环外完赛_20261011.zip` | v123 封印副本（源码+HEX+ELF+证据+回退说明），位于 firmware/_versions/ |

### 硬件发现（v021 调试中获得）
- **摄像头安装方向与图像坐标相反**：代码中"图像最左白段"实际对应**车身右侧**的白路。
  （用户实测确认：指示"跟左边白线"实际生效于物理右侧 → 摄像头可能为镜像安装/翻转。）
  **对后续调参的影响**：凡涉及"左/右"的经验值，都要注意**图像坐标与物理方向相反**。
- 大环完整逻辑链（已跑通）：宽度≥96+绕圈≥180ccd 触发出环 → 跟最左白段顺白路弯出 →
  「单段白+正常宽度」连续8次恢复普通循迹。
| v022 | 20261005-2257 | v022 修S道误触发残留：利用环周长固定(336ccd)——进环80-180ccd内再见宽波=物理不可能绕完一圈→立即复位；+RING寿命上限480ccd强制复位。解决全赛道时S道污染状态导致大环直穿/半途穿出 | `17D294DD9222…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v023 | 20261005-2341 | v023 修v022误伤：删时间窗trap+改双自愈(长窄路130ccd/超时500ccd) [待烧录，板子未连接] | `7212E2D313E5…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v024 | 20261006-0028 | v024 速度闭环：编码器反馈接入控制(固定PWM→PI闭环)，换电池不再影响车速；目标随弯道/环内缩放 ✅已烧录 | `35B76715D56A…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v025 | 20261006-0041 | v025 速度闭环消抖：Kp 14→7、反馈4拍滑动平均、PWM变化率限幅±45/20ms（治车身前后震动）✅已烧录 | `A722D4FCC9BA…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v026 | 20261006-0051 | v026 提速：速度环折中调参(Kp10/Ki0.8/slew70)治'消抖过头变肉'；目标 40→46、环30/出27 | `3F7B4D0E3A5D…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v027 | 20261006-0053 | v027 IMU启用(软件IIC模式,避开SPI0与屏幕冲突)：陀螺读取+失败自动降级+imu_ok上报【需断电重启一次接通】 | `0E82AB173E01…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| **v028** ★ | 20261006-0107 | **★IMU 成功接入！★** 根因=初始化顺序：屏幕先→IMU后（与已验证 IMUCheck 一致）；SPI 模式。imu_ok=1、gyro_z 可读、无花屏。之前"IMU先→屏幕后"导致失联+历史花屏疑云 | `CEA8A0186050…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| **v029** ★ | 20261006-0122 | **★IMU 完整功能+自愈（v029+v030 合并内容）★** IMU完整功能+自愈：陀螺零偏标定+滤波+阻尼进转向+加速度上报+编码器里程/堵转；★临界区保护IMU SPI读取(修gyro恒0失步)+失步自愈重初始化 | `12B56AB1D84C…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v030 | 20261006-0138 | v031 修复陀螺阻尼限幅bug(±1.0→±50)：上版把舵机行程压到10%导致'舵机不动+电机慢'；顺手修堵转检测仅在RUN时判定 | `FA58239D4572…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v031 | 20261006-0146 | v032 关闭编码器速度闭环(SPEED_LOOP_ENABLE=0)：闭环目标未按实车标定导致PWM被压到下限800(8%)，车慢成蜗牛；回到v021验证过的固定PWM 2100 | `31F5AEEB63DE…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v032 | 20261006-2316 | v033 固定PWM(速度闭环关闭)+新增enc_peak峰值上报：用于标定速度闭环目标值(编码器计数/20ms) | `85AE897DCCF9…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v033 | 20261006-2329 | v035 修复大环残留状态：RingReset漏清bigring全部状态(导致'单独跑大环能跑全程不行')+中线滤波器状态复位+大环入口加直行防误触发(|dev|小于0.40)；另含enc_peak中值滤波+enc_avg标定探针 | `3968289E702A…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v034 | 20261006-2337 | v036 速度x2(基础PWM 2100→4200)+大环兜底修复(上带判定兜底/出口判据96→88/自愈130→500/超时500→900)+进环瞬间自动开始曲线记录 | `63A250B4EA22…` | left_speed = 0 / left_speed = 4200 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v035 | 20261006-2340 | v037 速度降为1.2倍(2100→2520)：v036的2倍太快车失控；弯道减速/丢线/环内限速同步按1.2倍缩放 | `3B2D2D06876D…` | left_speed = 0 / left_speed = 2520 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v036 | 20261007-0008 | v038 治直线抖：控制严格每CCD帧一次(修低通/限幅/环计数器被压缩~2倍的失真)+舵机缓变18→36等效补偿+关闭陀螺阻尼(排查IMU) | `9A9F71117339…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v037 | 20261007-0014 | v039 转向加强：舵机行程160→200us(实测早就满舵，上限全由行程决定，1500±160只用了全行程1/3→转弯略出道)+缓变36→60(满舵约1.3帧到位) | `4F51A0B03FEC…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v038 | 20261007-0030 | v040 照学长国赛原版恢复转弯能力：①转向Kp 15→30(学长原值，之前被砍半)②恢复差速辅助转向(内轮-servo_duty*20/外轮+*3，测到>10才启动)③弯道整体减速降为辅助(1200→400) | `F61D45146D4A…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v039 | 20261007-0040 | v041 ①舵机行程200→160(200卡底座) ②接入堵转保护(堵转→PWM压到800以下保护电机) 【尚未烧录，等舵机极限测完一起烧回】 | `51CB0152D363…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v040 | 20261007-0042 | v042 舵机行程按实测定精确值(ServoCheck测:右转极限1320/左转极限1640→L=170us/R=130us各留10us)：旧值160会让左转打到1660>1640一直顶底座；含堵转保护 | `74984F99D8BF…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v041 | 20261007-0051 | v043 大环按用户思路重做：图像右半区(车身左侧)大片白出现第2次→左转进环；进环后陀螺积分yaw转够360°→出环；停用旧宽度触发；新增cring_cnt/cring_w/cring_in/yaw10上报+环口自动曲线记录 | `D41D2BE92AA4…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v042 | 20261007-0057 | v044 紧急修复:移除大环所有舵机动作(上一版'直行'条件永久成立+普通白路误触发→完全不跟道)，改为只测量(cring_w=右半区最宽白段, cring_wmax=历史峰值)；循迹行为回到v042 | `7AB40AEB2EDE…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v043 | 20261007-0114 | v046 车头左侧大片白期间保持直行(用户实测:这个口子该直行不该左转)；判据按实车原始图定(最宽白段>=70px且中心>=72)；hold带1.5s超时兜底绝不永久生效；检测移至主循环可待机验证 | `1FFEEFBA7A87…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v044 | 20261007-0126 | v048 大环完整逻辑(两口径均已用实车原图验证):第1口(侧向大片白)直行/第2口(整幅全亮)左转进环+陀螺360度出环+15s兜底;CCD改分位数对比度(修整幅白面被误判丢线);治飘(DIFF_INNER20→14,弯道减速400→700) | `9FB7ED7B2849…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v045 | 20261007-0203 | v048b 大环改用侧向大片白【计数】区分两口(第1次=出环口直行/第2次=进环口左转进环)+陀螺360出环+15s兜底(上一版错用'整幅全亮'做进环条件:全亮只在已站上环时出现,开车路上不满足,且第1口的直行hold在第2口也触发→被带直行开过) | `AE62BC583F38…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v046 | 20261007-0208 | v049 大环视觉判据全部停用(普通弯道白路被误判成环口→第2次误触发→强行左转卡死)；改走用户建议的【编码器里程触发】方案，里程值待实测(把车从起点推到环口读odometer)；视觉判据仅保留上报 | `6D38AC20AD46…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v047 | 20261007-0213 | v050 大环改为【编码器里程触发】(实测起点->进环口514328计数,取510000提前0.2s)：里程到→左转进环+陀螺360出环+15s兜底；视觉判据彻底退出控制只保留上报 | `96B84734BF12…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v048 | 20261007-0216 | v051 修正D1:514328处应为【直行】不是左转(用户俯视图指正:该口=大环右侧与竖直白条交界)；改为里程到D1后直行窗口50000计数；D2(该左转进环口)待实测 | `4686AF855018…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v049 | 20261007-0221 | v052 大环里程标定完成: D1=514328(直行,窗口9300) / D2=525671(左转进环)+陀螺360出环+15s兜底；两个里程均由用户实车实测(口1停读514328,手推口1->口2读11343) | `016E6676BF40…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v050 | 20261007-0228 | v053 大环改【里程窗口+视觉判断】混合方案(用户要求:距离只当辅助提醒别写死)：口1窗口[484328~518328]内看到大片白→直行(提前1.3s开始,修v052入弯太晚);口2窗口[521671~550671]内看到大片白→左转进环;窗口外视觉判据不生效(治普通弯道误触发) | `939C4F2851E2…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v051 | 20261007-1435 | v054 修两处:①W2进环条件加allbright(第2口整幅全白,原bigwhite的中心>=72判据不成立→直接开过去)②环内舵量0.85→0.22(原满舵→转弯半径远小于大环→原地打转;改后约66%行程) | `34BFE4D5F4FE…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v052 | 20261007-1444 | v055 ★速度闭环终于接上★：用编码器把两轮平均速度闭环到520计数/20ms(≈昨晚标定车速)，电池电压漂移不再影响车速；差速辅助作为叠加量加在基准PWM上(不打架)；弯道靠降目标减速；环内260/丢线220；PWM限幅1200-3400 | `5356D26DDD8B…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v053 | 20261007-1451 | v056 降速:平均速度目标520→460(用户要求'调到1800',按PWM口径换算=原2100的86%) | `035BBD149EB0…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v054 | 20261007-1505 | v057 治'舵机慢+进环转弯出道':①舵机变化率60→150(学国赛无变化率限幅,满舵1帧到位)②用里程在进环前提前减速(窗口464328~545671,目标×65%,治直线加速后转弯惯性出道) | `F5C5192EDFAC…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v055 | 20261007-1513 | v058 【host测试建成】+ 修复它抓到的漏洞:①出环后加3万计数冷却(防出环瞬间画面仍全亮→立刻重进环死循环)②舵机变化率60→150 ③进环前里程提前减速。host测试8项全通过(tests/host/run.py) | `8DD2E4074910…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v056 | 20261007-1523 | v059 进环判据改为【里程闸门+白段沿变计数】(学国赛Jason环岛状态机:第1次大片白→直行/消失后再现→第2次进环)；里程窗口放宽成环区460000-620000只防误触发；上报bw_cnt口计数；host测试9/9通过 | `7604DC922E8F…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v057 | 20261007-1533 | v060 ①堵进环误触发:allbright收窄到口2邻近窗(505671-585671),赛道别处只有方向性强的bigwhite才算②陀螺阻尼接上国赛系数0.00685(我们原来0.0006小11倍)③新增环口里程锁存(上报cring_w)→下次跑完就知道两个口真实在哪 | `86ABEEAB2AEF…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 8000 / left_speed = 0 |
| v058 | 20261007-1537 | v061 【编码器+IMU大版本】①逐轮速度闭环(差速改为'目标速度百分比',内轮多刹25%,学Jason speed_dif1)②侧翻保护(总倾角>53度连续10帧→停车)③陀螺阻尼(0.00685,上一版)④进环误触发已堵(整幅全亮只在口2邻近窗生效)。host测试11/11 | `F1CCBF4DE9AE…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 0 / left_speed = 8000 |
| v059 | 20261007-1546 | v062 紧急:关闭陀螺阻尼(国赛系数0.00685未换算量程,我们SteerErr±50 vs国赛±250强5倍+符号未验证→正反馈→一开始乱跑)；保留:逐轮闭环/差速目标百分比/侧翻保护/进环误触发修复 | `DE66ACBF45FF…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 0 / left_speed = 8000 |
| v060 | 20261007-1553 | v063 陀螺阻尼按量程自己推导(0.0014=0.00685*50/250)重新接入+符号改+1.0(由v061事故推断)+阻尼项限幅±5保险(即使符号反也不失控) | `80C70B733C05…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 0 / left_speed = 8000 |

独立大环诊断 v063-d1（20261007-162657）：`TC264_Senior6_RingDiag_v063-d1_20261007-162657.zip`；HEX `5eac0dfbb38e`；26项主机检查通过、目标0错误0警告、HEX校验通过。未烧录，原v063工程与板上程序保留。

独立大环诊断 v063-d1（20261007-162950）：`TC264_Senior6_RingDiag_v063-d1_20261007-162950.zip`；HEX `5eac0dfbb38e`；26项主机检查通过、目标0错误0警告、HEX校验通过。已烧录并通过Flash回读校验，待实车采集。

独立大环诊断 v063-d2（20261007-165933）：`TC264_Senior6_RingDiag_v063-d2_20261007-165933.zip`；HEX `9cf8bd3cd392`；27项主机检查通过、目标0错误0警告、HEX校验通过；修复逐轮PI起跑复位遗漏。已烧录并通过Flash回读校验，待实车采集。

独立大环诊断 v063-d3（20261007-185620）：`TC264_Senior6_RingDiag_v063-d3_20261007-185620.zip`；HEX `fb4fc6a2149b`；32项主机检查通过、目标0错误0警告、HEX校验通过；柔和起步、解除1200PWM下限、直行释放滤波衔接、记录真实采样时间。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-191529）：`TC264_Senior6_RingDiag_v063-d4_20261007-191529.zip`；HEX `c3907bdf6935`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

TC264_Senior6_RingDiag_v063-d3m_20261007-192732.zip：d3仅修正左右电机输出引脚，21+6测试通过，已烧录校验，待实车。

独立大环 v063-d4（20261007-193755）：`TC264_Senior6_RingDiag_v063-d4_20261007-193755.zip`；HEX `26527123e717`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-194827）：`TC264_Senior6_RingDiag_v063-d4_20261007-194827.zip`；HEX `74e1aee8d96b`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-194934）：`TC264_Senior6_RingDiag_v063-d4_20261007-194934.zip`；HEX `53d1f6be0e7d`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-200010）：`TC264_Senior6_RingDiag_v063-d4_20261007-200010.zip`；HEX `53d1f6be0e7d`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-203044）：`TC264_Senior6_RingDiag_v063-d4_20261007-203044.zip`；HEX `f08e74c62fc8`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-210436）：`TC264_Senior6_RingDiag_v063-d4_20261007-210436.zip`；HEX `9822bd93c9b8`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-212744）：`TC264_Senior6_RingDiag_v063-d4_20261007-212744.zip`；HEX `d055c494d937`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-213149）：`TC264_Senior6_RingDiag_v063-d4_20261007-213149.zip`；HEX `ba3613e90bae`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-213555）：`TC264_Senior6_RingDiag_v063-d4_20261007-213555.zip`；HEX `80854a2e3b36`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-215019）：`TC264_Senior6_RingDiag_v063-d4_20261007-215019.zip`；HEX `64b8b15d6250`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-215307）：`TC264_Senior6_RingDiag_v063-d4_20261007-215307.zip`；HEX `5d28924cda0a`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-215458）：`TC264_Senior6_RingDiag_v063-d4_20261007-215458.zip`；HEX `c47108553a5b`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-220511）：`TC264_Senior6_RingDiag_v063-d4_20261007-220511.zip`；HEX `caa641b6f9dc`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-220738）：`TC264_Senior6_RingDiag_v063-d4_20261007-220738.zip`；HEX `34323fce99be`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-225636）：`TC264_Senior6_RingDiag_v063-d4_20261007-225636.zip`；HEX `739f9c6cf86c`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-230944）：`TC264_Senior6_RingDiag_v063-d4_20261007-230944.zip`；HEX `394fa63ab9e3`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261007-234828）：`TC264_Senior6_RingDiag_v063-d4_20261007-234828.zip`；HEX `c79e8bf28b5a`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

## ★ 实车验证记录（20261007 深夜）★
- v063-d4_20261007-234828（=★MILESTONE_v079）：**摄像头换装后首次【正常巡线】成功**（方向链+撕裂修复均生效）。
- 遗留（下一步）：①速度偏慢（调 SPEED_AVG_TARGET/弯道降速/起步限速）；②未进大圆环（环口判据沿变失效 + 里程窗需按当前起点重标定）。
| v061 | 20261008-1448 | v080：遥控检测页（uart_receiver_init + ui_page4 六通道显示，P20.6 切页） | `80C70B733C05…` | left_speed = 0 / left_speed = 2100 / left_speed = 0 / left_speed = 0 / left_speed = 0 / left_speed = 8000 |

独立大环 v063-d4（20261008-145209）：`TC264_Senior6_RingDiag_v063-d4_20261008-145209.zip`；HEX `7f398b11bfc1`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。未烧录，原v063工程与板上程序保留。

独立大环 v063-d4（20261008-150220）：`TC264_Senior6_RingDiag_v063-d4_20261008-150220.zip`；HEX `7f398b11bfc1`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-152346）：`TC264_Senior6_RingDiag_v063-d4_20261008-152346.zip`；HEX `112cb964dcca`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。未烧录，原v063工程与板上程序保留。

独立大环 v063-d4（20261008-152527）：`TC264_Senior6_RingDiag_v063-d4_20261008-152527.zip`；HEX `112cb964dcca`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-153543）：`TC264_Senior6_RingDiag_v063-d4_20261008-153543.zip`；HEX `6da538f3d081`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

## TC264_RCManual

| 版本 | 时间 | 说明 | HEX SHA256 | 关键参数 |
|---|---|---|---|---|
| v001 | 20261008-1559 | v001：独立遥控车（CH4速度档/软启动/失联保护，主循环无阻塞） | `DDCB8018587B…` | #define RC_MID          992 / #define RC_THR_DEAD     50 / #define RC_STEER_DEAD   40 / #define RC_THR_FULL     800 / #define RC_STEER_FULL   700 / #define RC_LOST_MS      500u |

独立大环 v063-d4（20261008-160922）：`TC264_Senior6_RingDiag_v063-d4_20261008-160922.zip`；HEX `c5be945c1244`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。未烧录，原v063工程与板上程序保留。
| v002 | 20261008-1610 | v002：CH5 减速/CH6 加速（速度上限微调 1200~9000） | `7C9B7D12C962…` | #define RC_MID          992 / #define RC_THR_DEAD     50 / #define RC_STEER_DEAD   40 / #define RC_THR_FULL     800 / #define RC_STEER_FULL   700 / #define RC_LOST_MS      500u |
| v003 | 20261008-1637 | v003：转弯差速（内轮减速）+ 刹车两段（轻拉滑行/重拉倒车） | `7C9B7D12C962…` | #define RC_MID          992 / #define RC_THR_DEAD     50 / #define RC_STEER_DEAD   40 / #define RC_THR_FULL     800 / #define RC_STEER_FULL   700 / #define RC_LOST_MS      500u |
| — | 20261008-1637 | **更正**：v003 为误操作包（补丁断言失败未生效、内容同 v002），已改名 _ERR_ 标注；真版本见 v004 | 7C9B7D12C962…（同 v002） | — |
| v004 | 20261008-1639 | v004：转弯差速+刹车两段（v003 误存后的重建版；hex 08f50bc9） | `08F50BC9F28A…` | #define RC_MID          992 / #define RC_THR_DEAD     50 / #define RC_STEER_DEAD   40 / #define RC_THR_FULL     800 / #define RC_STEER_FULL   700 / #define RC_LOST_MS      500u |

独立大环 v063-d4（20261008-164218）：`TC264_Senior6_RingDiag_v063-d4_20261008-164218.zip`；HEX `69ccd7757c6a`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。未烧录，原v063工程与板上程序保留。
| v005 | 20261008-1644 | v005：v004 源码的烧录验证包（差速+刹车，hex 08f50bc9 已烧录） | `08F50BC9F28A…` | #define RC_MID          992 / #define RC_THR_DEAD     50 / #define RC_STEER_DEAD   40 / #define RC_THR_FULL     800 / #define RC_STEER_FULL   700 / #define RC_LOST_MS      500u |

独立大环 v063-d4（20261008-165253）：`TC264_Senior6_RingDiag_v063-d4_20261008-165253.zip`；HEX `69ccd7757c6a`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。未烧录，原v063工程与板上程序保留。

独立大环 v063-d4（20261008-165410）：`TC264_Senior6_RingDiag_v063-d4_20261008-165410.zip`；HEX `69ccd7757c6a`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-170841）：`TC264_Senior6_RingDiag_v063-d4_20261008-170841.zip`；HEX `b87aec39c87a`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-194249）：`TC264_Senior6_RingDiag_v063-d4_20261008-194249.zip`；HEX `3502a9ec31d9`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-200911）：`TC264_Senior6_RingDiag_v063-d4_20261008-200911.zip`；HEX `8b6e2ef7acd9`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-202226）：`TC264_Senior6_RingDiag_v063-d4_20261008-202226.zip`；HEX `09a6cf3ad627`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-203944）：`TC264_Senior6_RingDiag_v063-d4_20261008-203944.zip`；HEX `c2a577786762`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-204325）：`TC264_Senior6_RingDiag_v063-d4_20261008-204325.zip`；HEX `09a6cf3ad627`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-204729）：`TC264_Senior6_RingDiag_v063-d4_20261008-204729.zip`；HEX `3a2fe61260ed`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-205552）：`TC264_Senior6_RingDiag_v063-d4_20261008-205552.zip`；HEX `a12e7a3603b4`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-210337）：`TC264_Senior6_RingDiag_v063-d4_20261008-210337.zip`；HEX `270c4d3eb6ca`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-211429）：`TC264_Senior6_RingDiag_v063-d4_20261008-211429.zip`；HEX `651a2dd87e5a`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-212258）：`TC264_Senior6_RingDiag_v063-d4_20261008-212258.zip`；HEX `d6b57e0f3195`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-214522）：`TC264_Senior6_RingDiag_v063-d4_20261008-214522.zip`；HEX `3c739402eaaa`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-215507）：`TC264_Senior6_RingDiag_v063-d4_20261008-215507.zip`；HEX `751fd4bc7140`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-220533）：`TC264_Senior6_RingDiag_v063-d4_20261008-220533.zip`；HEX `751fd4bc7140`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-222228）：`TC264_Senior6_RingDiag_v063-d4_20261008-222228.zip`；HEX `ebf53bedb81d`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

独立大环 v063-d4（20261008-225756）：`TC264_Senior6_RingDiag_v063-d4_20261008-225756.zip`；HEX `8fad2e15a0e4`；40项主机检查通过、目标0错误0警告、HEX校验通过；25ms测速口径、恢复弯道目标、出口选白段、超圈停车、停止才冻结记录。已烧录并通过Flash回读校验，待实车采集。

## TC264_VisionTrack

| 版本 | 时间 | 说明 | HEX SHA256 | 关键参数 |
|---|---|---|---|---|
| v001 | 20261009-1647 | 丢线爬行找回：单帧丢线不再停车，保持舵量+降到爬行速度继续找线，连续10帧(200ms)才判VT_LOST | `25354C15B0D3…` | #define SERVO_KEY       (P11_2 / #define SERVO_CENTER_RAW   740 / #define SERVO_TRAVEL_L_US  170 / #define SERVO_TRAVEL_R_US  130 / #define SERVO_SLEW_RAW     150 |
| — | 20261009-1654 | **回滚前存档**：v001（丢线爬行找回）实车弯道仍不行，用户决定放弃 VisionTrack 重构、回滚旧架构 | `25354C15B0D3…` | — |

回滚加固（20261009-165811）：`TC264_Senior6_RingDiag_v063-rollback-servo740_20261009-165811.zip`；HEX `95e9810bffb0`；恢复至 v099 旧架构最新版（VisionTrack 重构版被放弃），唯一功能改动：舵机中位 750→740 raw（1480us 才是本车真直）；门禁：ADS 构建 0错误0警告 + HEX 校验通过（PFlash 独占逐行校验和）；主机测试不可用——tests/host 工装停留在 v092 前结构，对 v099 PID.c 报 conflicting types for 'cring_entry_odo'，无法编译，故本版**无主机测试背书**。已烧录并通过 Flash 回读校验，待实车验证。

> 更正：本行此前误写「40项主机检查通过」，系脚本硬编码话术，实际未运行主机测试。已更正，脚本已改为如实记录。

## v101 弯道降级 + 满舵找回（VisionTrack，20261009-1716）

| 版本 | 时间 | 说明 | HEX SHA256 | 关键参数 |
|---|---|---|---|---|
| v101 | 20261009-1716 | 治"弯道易停"：①急弯远端无路但近处有线时**降级控制**（不停车，限速320）②丢线时**朝上次方向满舵**+爬行找回（对齐国赛学长做法）③**内轮差速加强**（满舵时内轮降到45%，对齐学长"内轮刹死咬弯"） | `6869DFB1F55E…` | #define VT_NEAR_ROWS 6 / #define VT_DEGRADE_SPEED 320 / #define VT_LOST_FRAMES 10 / #define VT_LOST_CRAWL 150 / inner=target*(100-|steer|*55/100)/100 |

- 归档：`TC264_VisionTrack_v101_bendfix_20261009-1716.zip`
- 门禁：主机测试 3/3 通过（logic + lost_recover + start）；ADS 构建 0错误0警告；HEX 校验 PFlash 独占逐行校验和；烧录 FlashVerified: true
- 参考：HammerTa（同 TC264）单边丢线用另一边+半路宽补线；国赛学长 6.0 丢线"朝上次方向满舵 + 降速"
- ⚠️ 归档时板上运行时报告：`standby_zero=true` 但 `camera_advancing=false`（**相机 57 秒无帧、IMU 未就绪**，疑似排线/供电，非本次代码改动引入）
- 实车验证：physical_track_confirmed = False（待跑）

## v102 交叉口不判丢线（VisionTrack，20261009-1730）+ 遥控固件 740

| 版本 | 时间 | 说明 | HEX SHA256 | 关键参数 |
|---|---|---|---|---|
| v102 | 20261009-1730 | 治"一到交叉/环岛入口就停"：整行全白（w>=186）不再被跳过→不再累计成丢线。补线条件 wide && previous>0 && **valid>0** && wide_fill<50，用 `valid>0` 把"跑出赛道整帧全白"挡在外面 | `3D5A19AF4E43…` | #define VT_WIDE_FILL_MAX 50 / VT_NEAR_ROWS 6 / VT_DEGRADE_SPEED 320 / VT_LOST_FRAMES 10 |
| RCManual v740 | 20261009-1732 | 遥控固件舵机中位 750→740（与 VisionTrack 对齐，1480us 真直） | `054E5AAC7898…` | #define SERVO_CENTER 740（pwm_init 引用它，随之生效） |

- 归档：`TC264_VisionTrack_v102_crossfix_20261009-1732.zip`、`TC264_RCManual_v740_20261009-1732.zip`
- v102 门禁：主机测试 **4/4**（新增 test_vision_track_cross.c 正/反向断言）；ADS 0错0警告；HEX 校验通过；**已烧录** FlashVerified: true
- RCManual v740 门禁：ADS 0错0警告；HEX 校验通过；**未烧录**（板上在跑 v102）
- 参考：国赛学长 CalculateCenterLine 的"全白→沿用上次值"；坑：第一版直接用"全白→沿用"导致"整帧全白也直冲"，被 logic 测试当场抓住

## v103 路口"近处整片全白"惯性补线（VisionTrack，20261009-1755）

| 版本 | 时间 | 说明 | HEX SHA256 | 关键参数 |
|---|---|---|---|---|
| v103 | 20261009-1755 | 治"车放在十字路口看不到中线"：**删掉循环内的 `valid>0` 闸门**（v102 的死锁：近处行在扫描时 `valid` 还是 0，补线永不触发），判据放宽为 `wide \|\| max_w>road_w*3/2`；防误判闸门移到循环后用 `real_rows`；新增整帧全白 `all_white` 分支 + `white_frames` 2s 超时兜底 | `2A3078BB6E45…` | #define VT_WIDE_FILL_MAX 50 / VT_WHITE_MAX_FRAMES 100 / VT_NEAR_ROWS 6 / VT_DEGRADE_SPEED 320 / VT_LOST_FRAMES 10 |

- 归档：`TC264_VisionTrack_v103_roadwhite_20261009-1755.zip`
- 根因（**数据驱动，真实帧**）：`read_camera_frame.py cam_gray` 从 CPU1 DSRAM dump 188×120 帧 → `analyze_frame.py` 复刻大津法（thr=199）→ 近处 y=48..110 共 **63 行整行全白**，远处 y=28..45 共 19 行单段中心 92~93 → `valid=19<20`、`near=0<6`、`wsum=0` → `v->valid=0` → `VT_LOST`
- 门禁：主机测试 **4/4**（logic + cross + lost_recover + start，含新增"车停在路口 mid≠-1"用例）；**真实帧离线回放** `test_replay_crossing.c` → `valid=1 rows=70 near_rows=21 confidence=84 mid=94 degraded=1`；ADS 0错0警告；HEX 校验通过；**已烧录** `FlashVerified: true`
- 板上 live 回读：`hex_sha256=2A3078BB…`、`fault=0`（v102 是 3）、`confidence=84`、`camera_advancing=true`、`imu_ready=1`、`standby_zero=true`
- 待实车：路口起步看得到中线不停车；过路口限速是否合适；**反例复测**：冲出赛道/对白墙应在 2s 后 VT_LOST
- 文档：`docs/v103_路口近处全白惯性补线_20261009.md`
- 参考：国赛学长 CalculateCenterLine 的"全白→沿用上次值"；教训：判据量必须**在正确时机**求值——`valid` 只能循环结束后用

## v104 视觉升级：模型引导的中线提取（VisionTrack，20261009-1830）

| 版本 | 时间 | 说明 | HEX SHA256 | 关键参数 |
|---|---|---|---|---|
| v104 | 20261009-1830 | 治"**路口中线与前面的中线不对齐**"（屏幕黄点线底部 x≈101、顶部 x≈160，整段落在黑区）。把"逐行追中点"改成**逐行分类 + 可信测量 + 模型插值**：① 只用"左右边界都在画面内"的行当测量值（贴边白段的中点必然偏向被裁侧，是漂移根源）② 其余行用同帧最近可信行插值/外推 → 近处与远处出自同一条模型，**天然对齐** ③ 修掉 `max_w` 跨行累积（注释说"本行"实际是"本帧扫过的"）④ 行宽只在可信行学习 ⑤ 中线质量可观测 | `448979E1FDBC…` | #define VT_SEG_MIN 12 / VT_SEG_MAX 150 / VT_FULL_W (VT_W-2) / VT_CONT_MAX 35 / VT_OK_MIN 8 / VT_MODEL_DEGRADE 24 / VT_ROW_TOP 28 / VT_ROW_BOT 110 |

- 归档：`TC264_VisionTrack_v104_modelcenter_20261009-1830.zip`
- 根因（**数据驱动**，`read_camera_frame.py cam_gray` 从 CPU1 DSRAM 取真实帧 → `dump_rows.c` + `diag_center.py` + `ascii_view.py` 逐行分类）：y=80..47 共 **36 行白段右端 r=186 顶到画面边缘**，旧算法拿这种"被裁白段"的中点当中线，中心从 109 一路漂到 159；y=46 真路跳到左边又被 `abs(center-previous)*3>150` 拒掉，补线把 160 冻住 → 屏幕上中部中线整段在黑区
- **同帧量化**：\|中线−94\| 平均 **35.2→7.8 px**，最大偏移 **66→18 px**，远端 y=28 的中线 x **160→91**
- 门禁：主机测试 **4/4**（cross 新增用例 5：贴边白段不许把中上部中线拖到 120+，旧算法会挂）；真实帧离线回放 `valid=1 rows=83 near=21 conf=100 mid=102 far_mid=91`；`-Wall -Wextra` 零告警；ADS 0错0警告；HEX 校验通过；**已烧录** `FlashVerified: true`
- 板上 live 回读：`hex=8D340683…`、`fault=0`、`confidence=100`、`camera_advancing=true`、`imu_ready=1`、`standby_zero=true`
  > 该次回读对应 18:31 烧的中间版（与最终版仅差一处"贴边行存哪一侧边界"、且此值当前无读取方）。18:34 烧最终版 `448979E1` 并 `FlashVerified: true` 后，回读时调试链路已断（车被移到桌上、下载器/供电断开），故最终版的 live 回读**待下次通电补做**。
- **`track_report` 从 24 字扩到 26 字（104 字节）**：末尾追加 `measured`（可信测量行数）/ `model_rows`（模型补出行数），`read_vision_track_report.py` 同步改为读 26 字、用 `v[3]==v[23]` 校验
- 反例保护不变：整帧白茫茫仍走 `all_white`（不设 valid）+ `VT_WHITE_MAX_FRAMES=100`（≈2s）超时停车；`ok_rows==0` 一律不产生 valid
- 已废弃（保留定义仅为兼容文档）：`VT_NEAR_ROWS`、`VT_WIDE_FILL_MAX`
- 文档：`docs/v104_视觉升级_模型引导中线提取_20261009.md`
- 待实车：黄点中线在路口是否竖直对齐；过路口速度；直道/弯道手感不应变化（可信行仍是实测值）；反例（冲出赛道 2s 后 VT_LOST）

## v105 视觉系统彻底升级：蓝白赛道（VisionTrack，20261009-1901）

| 版本 | 时间 | 说明 | HEX SHA256 | 关键参数 |
|---|---|---|---|---|
| v105 | 20261009-1944 | 治"**蓝区也会被识别成中线然后乱跑**"（v104 上板后现场反馈）。**根因与 v103"路口中线不对齐"同源但这次真正修掉**：① 二值化改**两级递归大津法**——一级分暗/亮，亮类占比 >62%（说明一级只分出黑胶带）就对亮类再跑一次 Otsu，得到"蓝底\|白路"分界；② 中线改**左右边线各自分离 + 双边界连续性**（对齐国赛 get_left/get_right 思路）——从最下方 14 行取种子，向上**并向下**逐行要求左端与右端**同时**落在上一行窗口内；③ 新增**单边贴边补线**（对齐国赛"单边丢线用另一边 + 学习路宽补线"）——急弯白路跑出画面边缘时，用可见边 ± 学习路宽/2 定中心，不再冻住上一帧 | `314592507926…` | #define VT_SEG_K 4 / VT_EDGE_WIN 12 / VT_WID_LO 40 / VT_WID_HI 250 / VT_SEED_ROWS 14 / VT_MODEL_DEGRADE 24 ；OTS_L1_GUARD_PCT 62 / OTS_L1_MIN_SEP 10 / OTS_L2_MIN_RANGE 12 |

- 归档：`TC264_VisionTrack_v105_bluewhite_20261009-1946.zip`（**含单边补线 + 已烧录 + live 回读样本 = 现役**）；
  另有 `..._20261009-1901.zip` = 中间版（只有"两级 Otsu + 左右边线分离"，**没有**单边贴边补线），保留作 A/B 回退
- **数据驱动根因（真实帧 tmp/frame_gray.bin，scripts/diag_real_frame_ab.c）**：赛道是"黑胶带≈40 / 蓝底205~220 / 白路248~255"三峰。单次全局大津法在"黑 vs 其余"类间方差更大时，**会把蓝底和白路一起判成白**（真实帧一级 Otsu=169，落在蓝底之下 → y=90 行白段 (11,186,176) 横跨整幅，大半是蓝底）；中线因此落到蓝区上。同帧 A/B：
  | 指标 | v104 单次全局 | v105 两级递归 |
  |---|---|---|
  | 阈值 | 169（蓝底之下） | 224（蓝底之上） |
  | 中线跑出白路的行 | **30/83** | **4/83** |
  | 其中落在"幽灵白"（蓝底当白） | **26** | **0** |
  | measured / model_rows | 3 / 80 | **68 / 15** |
  | degraded | 1 | 0 |
  | 中线 x 范围 | **61..62（冻死）** | 26..70（跟路走） |
- 门禁：主机测试 **7/7**（新增 `test_vision_bluewhite.c`：13 个蓝白场景，断言"蓝底被二值化成白的数量==0"且"中线绝不落在蓝底行"；新增 `test_vision_clipline.c`：5 个场景专测单边贴边补线）；`-Wall -Wextra` 0 告警；ADS 无头构建 **0 错误 0 警告**；HEX 校验 `checksumsValid=true` / `onlyPFlash=true` / 115767 字节
- **已烧录** `FlashVerified: true`（2026-10-09 19:45:50，Load→Erase→Program→Verify 全 Pass，1807ms）
- **板上 live 回读**（`read_vision_track_report.py`）：`hex=3145925079269E48`、`size=116`（29 字新布局生效）、
  **`otsu_level=2`（两级大津法在板上真的跑起来了）**、**`white_pct=36`**（落在预期 35~55；若蓝底被判白这里会到 80+）、
  `clip_rows=0`（静止直道，符合预期）、**`measured=46` / `model_rows=37`**（v104 是 7/76，可信测量翻了 6 倍）、
  `fault=0`、`confidence=100`、`camera_advancing=true`、`imu_ready=1`、`standby_zero=true`
- `track_report` **26 字/104 字节 → 29 字/116 字节**：追加 `[26] otsu_level`（1/2）、`[27] white_pct`（%）、`[28] clip_rows`（单边补线行数）；`read_vision_track_report.py` 同步改 `<29I` 与 size==116
- 屏幕预览页新增 3 个诊断标签：`L`=大津法级别（蓝白赛道应为 2）、`wp`=白像素占比（正常 35~55）、`S`=`cam_bin_suspect`（应为 0）
- 单边贴边补线实测收益（`scripts/test_vision_clipline.c`）：右急弯可见段 `150..187` → 中点 **181**（真值 181，旧做法 168）；左急弯可见段 `0..50` → 中点 **19**（真值 19，旧做法 25）；合成急弯跟随误差 **最大 6px → 最大 0px**
- 新增脚本：`test_vision_bluewhite.c`、`test_vision_clipline.c`、`run_vision_host_tests.py`（统一入口，7 套件）、`diag_bluewhite.c`、`diag_real_frame_ab.c`、`finalize_vision_track_v105.py`（未烧录版也能收尾归档）
- 文档：`docs/v105_视觉升级_蓝白赛道_20261009.md`
- 已废弃（保留定义仅为兼容文档）：`VT_CONT_MAX`、`VT_WIDE_FILL_MAX`（单点连续性闸门被"双边界连续性"取代）。`VT_OK_MIN` 在 v106 已复活，见下
- 待实车：蓝区不再出中线；`L`=2 / `wp`=35~55 / `S`=0；过路口中线竖直对齐；直道弯道手感不变；反例（冲出赛道 2s 后 VT_LOST 且 S=1）

---

## v106 十字路口中线闪烁修复：路口不许拿"贴边小白斑"当路面（VisionTrack，20261009-2009）

- 现场反馈：「十字路口还是**看不到中线**，但是之前是路口没有黄线，现在是路口的黄线**一直在屏幕左边和中线两个地方闪**」
- 版本：`v106` | 时间：20261009-2009 | 一句话：治"**路口黄线在 x≈7（屏幕最左）和 x≈94（中线）之间逐帧跳**"。根因是**种子的资格判据完全不看"这白段像不像一条路"**——真实路口帧 83 行里 82 行是 `VT_ROW_FULL`（整行铺满），只剩 y=106 一个 `0..15` 的 **16px 贴左边缘小白斑**，它被当成种子 → 中心 **x=7** → `measured=1`、其余 82 行全从 x=7 插值 → 整条中线画到屏幕最左；下一帧白斑位置一变就是 x≈94 → 闪烁 | `7BDF9725…` | #define VT_FULL_MIN_ROWS 24 / VT_OK_MIN 8（复活）
- 归档：`TC264_VisionTrack_v106_crosshold_20261009-2009.zip`（**已烧录 = 现役**）
- **两处改动（都在 `code/track_core.c`）**：
  - ① **种子资格闸门 `vt_seed_ok()`**：种子中心决定整条中线，所以它必须是可信的路面横截面——学习路宽已知时用与 `vt_take_row` **完全相同**的宽度合理性闸门（路口那个 16px 白斑 vs 学到 90px 路宽 = **18% < VT_WID_LO 40%** → 淘汰；急弯贴边白段 38/62 = **61%** 能过）；路宽未知（刚复位/整帧过曝）时只接受**两端都可见**的白段（贴边段一侧被裁且无路宽可补，中点纯属瞎猜）。
    > v105 的漏洞：逐行跟踪 `vt_take_row()` **本来就有**这条宽度闸门，**漏在了种子上**——一行只有一条白段时，"宽度 ≥ 该行最宽段 3/4"恒真，等于没有门槛。
  - ② **十字路口判据 + 顶住（`crossing`/`hold`）**：一帧里 `FULL 行 >= 24`（分析带 83 行，≈29%）且 `可信测量行 < 8` → 判定"正在穿路口"，用**上一帧中心**铺一条竖直线顶过去，`valid=0 + degraded=1 + all_white=1`（复用 v101/v102 已验证的降级通道，限速 `VT_DEGRADE_SPEED=320`、不停车，2s 全白超时兜底仍生效）。
    > 判据为什么用 **FULL 行数**而不是"白段太宽"：**急弯每行只有一条窄白带、其余是暗的，永远凑不出 FULL 行**，只有横向道路才会让白铺满左右 → 天然不误伤急弯。
    > `previous_mid` 不会被污染：`hold` 帧 `v->valid=0`，而 `t->previous_mid` 只在 `v->valid` 分支更新 → 路口期间一直锁在"进路口前最后一次真实路面中心"，出路口立刻接回。
- **真实路口帧 A/B（同一帧 tmp/frame_gray_v102_crossing.bin，77% 白）**：

  | 指标 | v105（旧） | v106 |
  |---|---|---|
  | `measured` 可信测量行 | **1** | **0**（小白斑不再算测量） |
  | `model` 插值行 | 82 | 0 |
  | `mid` | **7（屏幕最左）** | **94（上一帧中心）** |
  | `mid[110]` / `mid[40]` | 7 / 7 | **94 / 94**（全帧竖直线） |
  | `valid/degraded/all_white` | 1 / 1 / 0 | 0 / **1** / **1** |
  | 逐帧稳定性 | **7↔94 逐帧跳（=闪烁）** | **8 帧恒为 94，零跳动** |

  同一路口的相邻帧 `tmp/frame_gray_prev.bin`（也是 77% 白）同样从"被白斑带偏"变成"稳定顶在 94"。
  蓝白赛道帧 `tmp/frame_gray.bin`（41% 白）**完全未受影响**：`valid=1 mid=48 measured=68 model=15 degraded=0`，与 v105 一致。
- 门禁：主机测试 **7/7**（`test_vision_track_cross.c` 新增第 6 节：整帧铺满 + 只留 y=106 一个 `0..15` 贴边小白斑，6a 路宽未知 / 6b 路宽已知，均要求 `measured=0` 且 `mid∈(80,108)`、`mid[110]=mid[40]=94`）；`-Wall -Wextra` 0 告警；ADS 无头构建 **0 errors 0 warnings**（7.862s）；HEX `checksumsValid=true` / `onlyPFlash=true` / **115975 字节**
- **已烧录** `FlashVerified: true`（2026-10-09 20:09:24，三路 SHA256 全为 `7BDF97256FBD54A1…` 才放行）
- **板上 live 回读**：`hex_sha256` 与产物一致（跑的确是 v106）、`fault=0`、`confidence=100`、`otsu_level=2`、`white_pct=44`、`camera_advancing=true`、`standby_zero=true`
- 新增诊断脚本：`diag_cross_frames.c`（把 track_core.c 直接并入以观察 static 逐行缓冲；打印逐行分类/白段/贴边/中线/种子落点，可复用于任何实拍帧）、`finalize_vision_track_v106.py`
- 文档：`docs/v106_十字路口中线闪烁修复_20261009.md`
- 回滚：`TC264_VisionTrack_v105_bluewhite_20261009-1946.zip`（烧回即复现"路口闪烁"，那是本版专治的）
- 待实车：路口中线变**竖直**且不闪；不再出现 x≈7 的贴左值；穿路口不停车（走 320 限速）；出路口立刻接回跟线；蓝白直道/弯道手感不变、急弯 `clip_rows>0`；真冲出赛道 2s 后仍 `VT_LOST`

---

## v107 十字路口"往远处看"：远端锚点 + 拉线补全（VisionTrack，20261009-2023 已烧录 = 现役）

- 现场反馈：「在十字路口，**还是全白**。但是我看你能不能不止看前面一点，能不能稍微往远处看点看**远处的中线**，有个参考」
- 版本：`v107` | 时间：20261009-2023（已烧录） | 一句话：v106 堵住了"贴边小白斑当种子"，但路口**仍然看不到线**——因为 v106 只会"顶住上一帧中心"，而**远处那条真实可见的路面被完全忽略**；v107 把它用起来 | `77E2FB4F…` | #define 无新增；新增行分类 `VT_ROW_LINE`
- 归档：`TC264_VisionTrack_v107_farline_20261009-2023.zip`（**已烧录 = 现役**）；20:18 那份只有「未烧录」记录、代码完全相同，已删
- **先把路口帧整幅画出来才看清结构**（新增 `scripts/ascii_frame.c`，渲染 ASCII 二值图）：
  - **y=28..46**：一条干净路面横截面 x=66..120（中心 **93**），左右皆黑 ← **这就是"远处的中线"**
  - y=47..110：整块白（横向道路铺满，宽 188）← 底部 14 行全被盖住，**底部种子无从取**
  - → v106 命中"底部没种子" → 直接顶住上一帧中心 → 远处 y=28..46 被完全忽略
- **还有个更隐蔽的坑**：`track_adapter.c` 的 `line_out_center = frame.valid ? ... : -1`。v106 路口走 `hold` 分支 `valid=0` → **`line_out_center=-1` → 屏幕上根本不画黄线**。用户说的"还是全白"= **没画线**，不是白得看不见。
  > 教训：`valid` 不只表示"线可不可信"，还直接决定屏幕画不画。
- **国赛依据（本轮唯一参考）**：两家都"**拉线不造几何**"——NEFU `element.c:connect_line(x1,y1,x2,y2,way)`「从起始点往下拉一条补线」+ 旧版三叉口 `lian_xian_zhi` 配 `HOW` 状态机（HOW=1 十字补线/3 右斜/4 左斜）；HammerTa 用 `k=(star_col-Col[0])/(star_row-Row[0])` **两点反解斜率再沿直线铺点**，并把补出来的点用**另一种颜色**画出来以示区分。NEFU 报告原话：**"用'换线/拉线'而不是'造几何'"**。
- **改动（`code/track_core.c` 第 2b 遍，插在第 2 遍与第 3 遍之间）**：只在 `ok_rows<VT_OK_MIN(8)` **且** `full_rows>=VT_FULL_MIN_ROWS(24)`（确实存在横向铺满的白带）时启用：
  ① **从上（远）往下找第一条可信路面横截面**当"远端锚点"（复用 v106 的 `vt_seed_ok` 资格闸门），它本身是**真实测量**，从它朝车头方向延伸；
  ② **拉线**：远端锚点 → 近端参考（上一帧中心）两点连直线，把中间没有可信测量的行铺满 → 新增行分类 **`VT_ROW_LINE`**；
  ③ `vt_trust()` 上界放宽到 `<=VT_ROW_LINE`；第 3 遍把 `VT_ROW_LINE` **计 `modeled` 不计 `measured`**（无真实边界），所以 `modeled>=24` 仍会 `degraded=1`，路口照样限速 320。
  > 只在"有横向白带"时启用 → 急弯/岔口不会把另一条分支误当远处路面（急弯永远凑不出 FULL 行）。
- **真实路口帧 A/B（tmp/frame_gray_v102_crossing.bin，77% 白）**：

  | 指标 | v106 | **v107** |
  |---|---|---|
  | `valid` | 0（**屏幕不画线**） | **1**（屏幕画线） |
  | `measured` | 0 | **19**（远处那 19 行是真实测量） |
  | `model` / `all_white` | 0 / 1 | 64 / **0** |
  | `mid` / `far_mid` | 94 / 94 | 94 / **93** |
  | 逐行 | 全 94 竖线且不显示 | y32..44≈**93**（跟远处真路面）、y50..110≈94（拉线） |

  相邻帧 `frame_gray_prev.bin`（同 77% 白）结果一致；蓝白帧 `frame_gray.bin` **完全未受影响**（mid=48 / measured=68 / model=15 / degraded=0）。
- 门禁：主机测试 **7/7**（`test_vision_track_cross.c` 新增第 7 节：路口**远处路面故意偏移到 x=60**，要求斜拉线追过去 —— `measured=19 model=64 mid=75 | mid[40]=60(远处真值) mid[80]=82(斜拉线) mid[110]=94(近端) degraded=1`，并 `assert(f.measured>=12)` **所以哪天退化成"只会硬顶"会立刻红**）；`-Wall -Wextra` 0 告警；ADS 无头构建 **0 errors 0 warnings**（7.103s）；HEX `checksumsValid=true` / `onlyPFlash=true` / **116671 字节**
- **已烧录** `FlashVerified: true`（2026-10-09 20:23:11，三路 SHA256 全 `77E2FB4F…` 才放行）；板上 live 回读 `hex_sha256` 与产物一致、`fault=0`、`confidence=100`、`otsu_level=2`、`white_pct=46`、**`measured=68` / `model_rows=15`**、`camera_advancing=true`、`standby_zero=true`
- 注：20:13 先按用户要求烧了**遥控固件**（`TC264_RCManual` `054E5AAC…`）让他先玩，20:23 换回 v107
- 新增脚本：`ascii_frame.c`（**看路口全局结构最快的一招**）、`finalize_vision_track_v107.py`
- 文档：`docs/v107_十字路口远端锚点拉线_20261009.md`
- 回滚：`TC264_VisionTrack_v106_crosshold_20261009-2009.zip`（路口"顶住"、不画线）、`..._v105_bluewhite_..._1946.zip`
- 待实车：路口**画得出线**且跟着远处路面；`valid=1`；直道/弯道手感不变、急弯 `clip_rows>0`；路口 `degraded=1` 限速 320；远处也全白（宽路口/对白墙）→ 找不到远端锚点 → 退回 v106 顶住 + 2s 超时 `VT_LOST`；蓝区依旧不出中线



## ★★★ GOLDEN v123 · 完赛特别版（2026-10-11 00:25 封印）★★★
- **成绩**：2026-10-10 深夜实车 **完赛 39 秒**（除大圆环外）。用户判定为当前最优，直接锁为基线。
- **HEX**：`504F2ECEDF57C560D188DE402ECD3F72994A823FAC5C7CE1CE05B0B9EC5202C1`（122678 字节，与 v123 原版逐位一致重建）。
- **状态**：2026-10-11 00:25 烧回车上（FlashVerified=true；回读 cam_adv=true/fault=0/imu=1）。
- **本版内容**：slew150/scale100（速度原值）+ v121 种子海选 + v120 守卫 + v122 陀螺阻尼（符号验证✓）+ big_ring（默认OFF）。
- **同日实验均回退**：v124（白面顶住初版）/v125（异常帧保持方向）/v127（路宽防污染+白面顶住）——
  源码快照保留：`track_core.c.v127_rejected`、`.bak_v127`、`.bak_v125`、`.rejected_v124v125`；
  归档 zip 在 outputs/v124_*、v125_*、v127_*。
- **封印副本**：`★GOLDEN_v123_除大圆环外完赛_20261011.zip`（含 README 铁律/回退方法）。
- **后续优化规矩（用户要求）**：不得拍脑门就改；先有数据+理论，**报告并获批后再动手**；一次只动一处；改完四门禁齐备（tests/0警/HEX校验/实车复测）才算数。

## v136「1px缝合并」· 封存标记（2026-10-11 06:50，用户要求"标记下"）
- **是什么**：修复存帧0605（大圆环左上连接转弯）"中线整条竖直指向赛道外白色地板"——1px浅缝断链被竖直外推；
  修复=逐行白段提取增加"1px缝合并"。效果 measured 20→50、mid 93→115。**已验证未实车**。
- **HEX**：`CA8577E03EE5FE826416E81413902FC6308FED3CFB1D1717007B519DC2376506`（封存包内 `hex/`）。
- **当前车上 = v135**（`D3074C9C…`；本日用户判定"环岛跑得最好"——0504/0510 双跑完赛；烧回时间 06:50，重建字节级一致）。
- **恢复 v136**：`cp code/track_core.c.v136_marked code/track_core.c` → 重编（重建 sha 应=CA8577E0…）→ 烧录。
- **封存包**：`v136_1px缝合并_CA8577E0_封存_20261011.zip`（README/HEX/v136+135 源码/修复说明/同帧对比图）。
- v136 与 v135 唯一差异 = `track_core.c` 单处（1px 缝合并）。


## v138「8s 强制关闭保险」（2026-10-11 07:28，已烧 HEX 9FF057B9）
- 内容：开跑后 8s 内环岛模块必须 DONE；到点未关 → 强制 DONE（静默/零注入，不影响后续）；8s 内自然关闭则不动作。
- 与 v137 关系：v137 + 本保险（track_core.c 逐字节同 v137；只改 big_ring.c/h）。
- 实车待验：正常一跑（≈8s 内自然关闭）保险应保持静默；记录里 b2 相位若在 ~8s 直接跳 6=保险触发。
- 回退：cp code/big_ring.c.bak_v138pre + big_ring.h.bak_v138pre → 重编 sha=5A35E5B1… → 烧。
- 归档 outputs/v138_guard8s_9FF057B9_flashed.zip。


## v139「环岛模式停用十字路口算法」（2026-10-11 07:46 **已烧** HEX FF0C8F1A）
- = v135 底 + suppress_cross（BIG ON 且模块未 DONE 时跳过 4 处路口处理）+ v138 8s 保险。
- 实测：路口帧改用真实链（measured 20→55）；非路口帧零影响；全白顶住保留。
- 状态：构建/测试/HEX 校验完成；**07:46 已烧（FlashVerified）**，回读三方一致。
- 归档 outputs/v139_crossoff_ringmode_FF0C8F1A_flashed.zip。


## v140「环岛舵量增强 + 隔离界限」（2026-10-11 07:56 已烧 HEX 8F487455）
- 用户反馈：v139 不再直行但环内打舵不够 → 脱道。修复：模块驱动期间转向需求 +50%（BR2_STEER_BOOST_PCT）。
- 界限：环岛全部行为门控化（见 docs/环岛逻辑隔离界限_v140.md）；专项测试断言"普通行车逐位零影响"。
- 验证：14/14 + 0错0警 + FlashVerified + 回读；备份 *.bak_v140pre（=v139）。
- 调参：big_ring.h 的 BR2_STEER_BOOST_PCT 一行（60/80 更足，30/40 更稳）。
- 归档 outputs/v140_ring_steer_boost_8F487455_flashed.zip。


## v134 回退烧录（2026-10-11 08:05，用户要求对比测试）
- 板上现为 v134（HEX D2ED57B1…，字节级重建==原版）。源码树同步=v134（big_ring/track_vision 从归档还原；track_core=bak_v137pre）。
- v134 无：左转禁令(v135)/8s保险(v138)/停用十字路口(v139)/舵量增强(v140)。普通行车=v123 GOLDEN。
- 返回 v140：`cp *.bak_v140` → 重编(= 8F487455…) → 烧。五个备份：track_core.c/h + big_ring.c/h + track_vision.c。

## v141 烧录（2026-10-11 08:47）——环岛左转协调 + 8s 保险（从 v134 起底）
- 板上 HEX 9620E1C0…（FlashVerified；回读一致、fault=0、cam_adv=True）。改动仅 big_ring.c/h（纯新增）。
- 环外零改动：track_core.c/h、track_vision.c、track_adapter.c/h、cpu0_main.c 与 v134 逐字节一致。
- 回退：cp big_ring.{c,h}.bak_v141pre -> 重编（= D2ED57B1 v134）。测试 12/12。

## v141 全程+环岛成功轮（2026-10-11 上午，板上 9620E1C0）
- **首轮全程+大圆环一次跑通**：913帧记录（59.3s）；车发动f13(0.85s)→停f758(49.3s)，VT_FINISH终点斑马线自动停车（fault=10）。
- 环岛段 f42-f136：ARM→TURN→LOOP→STRAIGHT→DONE；**环内满左0帧**（旧版同段满左1s）；**左转协调f110-113实际生效**（大左→右转118）；8s保险=车发动后整8.0s于f136触发（STRAIGHT相位）。
- 特征点近停5处（P6段，既有行为，同v130轮8处同类）；驾驶段跳变仅3处。
- 数据：outputs/v141_full_run_65s_trace.txt / _rows.json / v141_fullrun_913_raw.bin / v141_全程环岛成功轮_数据分析_20261011.md

# ★★★ 里程碑 v141（2026-10-11 09:05 封存）——全程+大圆环 首次一次跑通 ★★★
- **封印包**：`★里程碑_v141_全程含环岛一次跑通_20261011.zip`（65文件/2.1MB；全源码树+HEX/ELF+脚本+日志+913帧记录+回退备份）
  另存 outputs/ 同名文件；README：`★里程碑_v141_README.md`
- **板上 = 本版**（HEX 9620E1C0，FlashVerified；回读一致、fault=0、cam_adv=True）
- **实车成绩**：车发动→停 49.3s 全程含大圆环一次跑通；VT_FINISH 终点自停；环内满左0帧；左转协调f110-113生效；8s保险=车发动后整8.0s
- **源码封印核验**：40 个源文件与构建记录 sourceSha256 逐字节一致（重建即得同款 HEX）
- **身份**：积分赛现役主力版 + 回退保险基线（v123 GOLDEN 保留为"除环岛外"备选）
- 回退：直接烧包内 HEX；或还原源码重编；退 v134=`cp big_ring.*.bak_v141pre` 重编（=D2ED57B1）

## v141 第2轮全程+环岛（2026-10-11，板上 9620E1C0，计时器 45.382s）
- 又一全程跑通；环岛f43-130比上轮多走到TAIL相位；环内满左0；左转协调f106-108生效；终点VT_FINISH自停。
- 数据：outputs/v141_run2_45s_trace.txt / _rows.json / _raw.bin / _summary.json / v141_第2轮_45.382s_数据分析_20261011.md

## v141 第3轮 失败记录（2026-10-11，板上 9620E1C0）★重要教训
- 环内 f89-128 长失线2.5s → 转向冻结+速度不降 → 横穿环岛飞出 → 全程失败（282帧止于满左空转）。
- 三缺口（全白转向冻结/全白不降速/2s停车差9拍未触发）已定位；修复候选A/B/C（环内盲区注右转+降速）待批准。
- 数据：outputs/v141_run3_fail_*；分析：v141_第3轮失败_282帧_分析_20261011.md

## v141 第4轮 失败记录（2026-10-11，板上 9620E1C0，VT_LOST自停）★
- 环岛最干净一轮（全相位/零长盲区/左转协调生效）；失败在出环后21秒特征点：全白盲2.2s+2s→自停。
- 与第3轮同病根（全白盲区=转向冻结+速度不降）；"成功轮该处减速爬行通过"对照成立。
- 数据：outputs/v141_run4_fail_*；分析：v141_第4轮失败_576帧_分析_20261011.md

## v143 烧录（2026-10-11 16:48）——盲区降速
- HEX BE6E1A07…（FlashVerified；回读一致、fault=0、cam_adv=True）。改动仅 track_core.c/h（+15行纯新增）：
  连续≥2拍无有效线（含AW）→ 强制压爬行档（每拍-160→450）、跳过PI、线回恢复；单帧闪断不触发。
- 依据：7次盲飞实录（PWM 648→1980/2820 的"越挡越加油"）+ 影子验证全绿。
- 回退：cp track_core.c/h.bak_v143pre → 重编（=47CD2352 v142）。测试12/12。

## v144 烧录（2026-10-11 17:30）——环内盲区兜底
- HEX D4335CE6…（FlashVerified；回读一致、fault=0、cam_adv=True）。改动仅 big_ring.c/h（+11行）：
  LOOP+连续≥2帧无有效线→注入 mid=118 温和右转合成线；单帧不触发、TAIL不注入。
- 依据：run11失败/run12险过的"环内回正帧+盲区=失败配方"+影子验证全绿。测试12/12。
- 回退：cp big_ring.c/h.bak_v144pre → 重编（=BE6E1A07 v143）。

## v145 烧录（2026-10-11 17:42）——左转协调阈值收紧 80→60（含 v144 回退）
- HEX 91E8FA16…（FlashVerified；回读一致、fault=0、cam_adv=True）。big_ring.h 一行（BR2_LEFT_SOFT_MID 60）。
- 依据：run13 误伤实录 + ring_backwards_0428 复古数据。测试 12/12。
- 回退：cp big_ring.h.bak_v145pre → 重编（=BE6E1A07）。
- 遗留：超长盲区僵持（"锁舵机"）修复方案待决策。

## ⏪ 回退烧录（2026-10-11 17:46）——v143 恢复（BE6E1A07）
- 用户指定：v144/v145 全部作废（换思路）。当前板上/源码树 = **v143 = run12 完整成功版**。
- 重编字节级复现 BE6E1A07；FlashVerified、回读一致、fault=0。测试 12/12。
- 回退路径（如再需）：v144=bak 组合×2 + zip；v145 同。当前树即 v143。
- ⚠️ v144（环内盲区注入 D4335CE6）、v145（阈值60 91E8FA16）**已作废**，不再采用。

## v146 烧录（2026-10-11 18:03）——弯道速度放松
- HEX C4E5B2DC…（FlashVerified）。track_core.c 两行：turncap 400-|s|、弯度限速 ×8/200。影子 +13~17%。
- 起点= v143（BE6E1A07）。回退：cp track_core.c/h.bak_v146pre → 重编（=BE6E1A07）。
- 测试守卫更新（深弯 220→260）；测试 12/12。

## v147 烧录（2026-10-11 18:20）——左转阈值 80→60 + 弯道二次提速
- HEX 8DDA1A37…（FlashVerified；回读一致、fault=0、cam_adv=True）。
- big_ring.h：BR2_LEFT_SOFT_MID 80→60；track_core.c：turncap 440-|s|、弯度 ×6/160。
- 影子：硬弯 250→334、中弯 298→371（相对 v143）。测试 12/12。
- 回退：cp big_ring.h.bak_v147pre + track_core.c.bak_v147pre → 重编（=C4E5B2DC v146）。

## v148 烧录（2026-10-11 18:41）——顺序竞态补救（修 run15"不进环"）
- HEX 47743D54…（FlashVerified；回读一致、fault=0）。big_ring.c/h：OFF 期口预确认 + 1.2s 迟到武装直接进环。
- 影子对照全绿；测试 12/12（新增 sc6）。回退：cp big_ring.c/h.bak_v148pre -> 重编（=v147 8DDA1A37）。
- ⚠ 烧录后相机无信号（非本版引入，待硬件检查）。
