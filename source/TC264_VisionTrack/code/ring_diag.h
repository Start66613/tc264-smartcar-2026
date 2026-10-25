#ifndef RING_DIAG_H
#define RING_DIAG_H

#define RING_DIAG_SIGNATURE 0x52444731u
/* v142 内存腾挪：128→64（-5376B）、事件 32→16（-384B）。
 * 依据：本工程（TC264_VisionTrack）无任何读取工具（旧读取脚本 read_ring_diag.py 只读
 * TC264_Senior6_RingDiag 工程）；v129 已先删 raw 16KB（无消费者）。64 样本 @31Hz ≈ 2.1s
 * 窗口 + 32 post ≈ 1.0s 覆盖环口事件足够。main 记录系统（trace_*）不受影响。 */
#define RING_DIAG_MAX 64u
#define RING_DIAG_EVENT_MAX 16u
#define RING_DIAG_POST_FRAMES 32u
#define RD_BIGWHITE 1u
#define RD_ALLBRIGHT 2u
#define RD_MOUTH 4u
#define RD_ZONE 8u
#define RD_NEAR_W2 16u
#define RD_ACTION_ENABLED 32u
#define RD_ARMED 64u

/* All fields are little-endian 32-bit words. Signed values use two's complement. */
typedef struct {
    unsigned ccd_frame, odo, yaw10, flags, white, wide_width, wide_center, center;
    unsigned deviation_milli, steering_milli, servo_duty, left_pwm, right_pwm;
    unsigned enc5, enc6, gyro_f, mouth_count, candidate, entry_odo, enc_sequence;
    unsigned sample_dt_ms;
} RingDiagSample;
typedef struct {
    unsigned frame, odo, yaw10, type, flags, mouth_count;
} RingDiagEvent;
typedef struct {
    unsigned signature, version, size, sequence;
    unsigned count, write_index, frozen, recording;
    unsigned total_frames, start_frame, trigger_frame, trigger_odo;
    unsigned mouth1_odo, mouth2_odo, candidate_entries, action_enabled;
    unsigned event_count, events_dropped, gyro_min, gyro_max;
    unsigned last_frame, stop_reason, reset_count, sequence_end;
} RingDiagReport;

/* Stop reasons: 1=key, 2=lost line, 3=finish, 4=32 frames after candidate entry. */
void RingDiagReset(void);
void RingDiagCapture(const RingDiagSample *sample, const unsigned char raw[128]);
void RingDiagFreeze(unsigned reason);
void RingDiagReadController(unsigned *flags, unsigned *count,
                            unsigned *candidate, unsigned *entry_odo);
extern volatile RingDiagReport ring_diag_report;
extern volatile RingDiagSample ring_diag_samples[RING_DIAG_MAX];
extern volatile RingDiagEvent ring_diag_events[RING_DIAG_EVENT_MAX];
#endif
