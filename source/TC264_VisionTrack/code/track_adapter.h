#ifndef TRACK_ADAPTER_H
#define TRACK_ADAPTER_H
#include "track_core.h"
void TrackVisionPublish(const VTFrame *frame);
void TrackControlTick(void);
void TrackHeartbeat(void);
void TrackRemoteSeen(unsigned valid);
void TrackRemoteRequired(unsigned required);
uint32_t TrackNow(void);
uint32_t TrackEdgeMode(void);
uint32_t TrackRunning(void);
extern volatile unsigned big_ring_enabled,big_ring_phase,big_ring_hits;
extern volatile unsigned track_fault,track_frame_age,track_heartbeat_age,track_confidence;
extern volatile unsigned track_phase,track_control_updates,track_sequence,track_start_count;
extern volatile unsigned track_epoch;
#endif
