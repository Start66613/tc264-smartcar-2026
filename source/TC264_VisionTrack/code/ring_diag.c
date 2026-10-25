#include "ring_diag.h"
#if !defined(RING_DIAG_HOST)
#pragma section all "cpu0_dsram"
#endif
volatile RingDiagReport ring_diag_report;
volatile RingDiagSample ring_diag_samples[RING_DIAG_MAX];
volatile RingDiagEvent ring_diag_events[RING_DIAG_EVENT_MAX];
static unsigned diag_seq, prev_flags, prev_candidate, post_frames, triggered;

static void begin_write(void)
{
    ++diag_seq;
    ring_diag_report.sequence = diag_seq;
    ring_diag_report.sequence_end = diag_seq;
}
static void end_write(void)
{
    ++diag_seq;
    ring_diag_report.sequence_end = diag_seq;
    ring_diag_report.sequence = diag_seq;
}
static void event(const RingDiagSample *s, unsigned type)
{
    unsigned n = ring_diag_report.event_count;
    if (n < RING_DIAG_EVENT_MAX) {
        ring_diag_events[n].frame = s->ccd_frame;
        ring_diag_events[n].odo = s->odo;
        ring_diag_events[n].yaw10 = s->yaw10;
        ring_diag_events[n].type = type;
        ring_diag_events[n].flags = s->flags;
        ring_diag_events[n].mouth_count = s->mouth_count;
        ring_diag_report.event_count = n + 1u;
    } else ++ring_diag_report.events_dropped;
}
void RingDiagReset(void)
{
    unsigned resets = ring_diag_report.reset_count + 1u;
    begin_write();
    ring_diag_report.signature = RING_DIAG_SIGNATURE;
    ring_diag_report.version = 2u;
    ring_diag_report.size = sizeof(RingDiagReport);
    ring_diag_report.count = ring_diag_report.write_index = 0u;
    ring_diag_report.frozen = 0u; ring_diag_report.recording = 1u;
    ring_diag_report.total_frames = ring_diag_report.start_frame = 0u;
    ring_diag_report.trigger_frame = ring_diag_report.trigger_odo = 0u;
    ring_diag_report.mouth1_odo = ring_diag_report.mouth2_odo = 0u;
    ring_diag_report.candidate_entries = ring_diag_report.action_enabled = 0u;
    ring_diag_report.event_count = ring_diag_report.events_dropped = 0u;
    ring_diag_report.gyro_min = ring_diag_report.gyro_max = 0u;
    ring_diag_report.last_frame = ring_diag_report.stop_reason = 0u;
    ring_diag_report.reset_count = resets;
    prev_flags = prev_candidate = post_frames = triggered = 0u;
    end_write();
}
void RingDiagFreeze(unsigned reason)
{
    if (!ring_diag_report.recording) return;
    begin_write();
    ring_diag_report.recording = 0u;
    ring_diag_report.frozen = 1u;
    ring_diag_report.stop_reason = reason;
    end_write();
}
void RingDiagCapture(const RingDiagSample *s, const unsigned char raw[128])
{
    unsigned slot;
    int g;
    (void)raw;   /* v129: raw rows no longer stored */
    if (!ring_diag_report.recording || ring_diag_report.frozen) return;
    begin_write();
    slot = ring_diag_report.write_index;
    ring_diag_samples[slot] = *s;
    ring_diag_report.write_index = (slot + 1u) % RING_DIAG_MAX;
    if (ring_diag_report.count < RING_DIAG_MAX) ++ring_diag_report.count;
    ++ring_diag_report.total_frames;
    if (ring_diag_report.total_frames == 1u) ring_diag_report.start_frame = s->ccd_frame;
    ring_diag_report.last_frame = s->ccd_frame;
    ring_diag_report.action_enabled = (s->flags & RD_ACTION_ENABLED) ? 1u : 0u;
    g = (int)s->gyro_f;
    if (g < (int)ring_diag_report.gyro_min) ring_diag_report.gyro_min = s->gyro_f;
    if (g > (int)ring_diag_report.gyro_max) ring_diag_report.gyro_max = s->gyro_f;
    if ((s->flags & RD_ZONE) && ((s->flags ^ prev_flags) & RD_MOUTH)) {
        event(s, (s->flags & RD_MOUTH) ? 1u : 2u);
        if ((s->flags & RD_MOUTH) && s->mouth_count == 1u && !ring_diag_report.mouth1_odo)
            ring_diag_report.mouth1_odo = s->odo;
        if ((s->flags & RD_MOUTH) && s->mouth_count >= 2u && !ring_diag_report.mouth2_odo)
            ring_diag_report.mouth2_odo = s->odo;
    }
    if (s->candidate && !prev_candidate) {
        event(s, 3u); ++ring_diag_report.candidate_entries;
        if (!triggered) {
            triggered = 1u; post_frames = 0u;
            ring_diag_report.trigger_frame = s->ccd_frame;
            ring_diag_report.trigger_odo = s->entry_odo;
        }
    } else if (!s->candidate && prev_candidate) event(s, 4u);
    prev_flags = s->flags; prev_candidate = s->candidate;
    if (triggered && ++post_frames > RING_DIAG_POST_FRAMES) {
        ring_diag_report.frozen = 1u; ring_diag_report.recording = 0u;
        ring_diag_report.stop_reason = 4u;
    }
    end_write();
}
#if !defined(RING_DIAG_HOST)
#pragma section all restore
#endif
