#include "zf_common_headfile.h"
#include "track_adapter.h"
#include "cam_image.h"
#include "cam_line.h"
#include "big_ring.h"
extern volatile int line_yaw10,line_imu_ok,line_gyro_ready;
#pragma section all "cpu1_dsram"
CamRowT cam_row[CAM_H];
int16 cam_mid[CAM_H],cam_mid_near=-1,cam_mid_far=-1,cam_dev_near,cam_dev_far;
uint8 cam_mid_rows,cam_ring_hole,cam_ring_wide;
int16 cam_ring_hole_y=-1,cam_ring_wide_y=-1;
volatile short cam_ring_cl,cam_ring_cr;
static VTTracker tracker;
static BR2State big_route;
static unsigned initialized,sequence,last_epoch,invalid_frames;
#pragma section all restore
void cam_line_scan(void){}
void cam_line_track(void){}
void cam_line_ref(void){}
void cam_line_ring(void){}
void cam_line_all(void)
{
 VTFrame result;unsigned y;
 extern volatile unsigned cam_taken_stamp;
 if(!initialized||last_epoch!=track_epoch){vt_tracker_reset(&tracker);br2_reset(&big_route);initialized=1;last_epoch=track_epoch;}
 tracker.edge_mode=TrackEdgeMode();
 vt_analyze(&tracker,&cam_bin[0][0],(unsigned)(cam_gmax-cam_gmin),cam_taken_stamp,++sequence,&result);
 br2_process(&big_route,&tracker,&cam_bin[0][0],&result,big_ring_enabled,TrackRunning(),
            (unsigned)(line_imu_ok&&line_gyro_ready),line_yaw10);
 big_ring_phase=big_route.phase;big_ring_hits=big_route.dbg;
 if(!result.valid){if(++invalid_frames>=3u){vt_tracker_reset(&tracker);invalid_frames=0;}}else invalid_frames=0;
 for(y=0;y<CAM_H;y++){cam_mid[y]=tracker.mid[y];cam_row[y].n=tracker.mid[y]>=0?1u:0u;}
 cam_mid_near=(int16)result.mid;cam_mid_far=(int16)result.far_mid;cam_mid_rows=(uint8)result.rows;
 cam_dev_near=result.valid?(int16)((result.mid-94)*1000/94):0;cam_dev_far=result.valid?(int16)((result.far_mid-94)*1000/94):0;
 cam_ring_hole=(uint8)result.hole;cam_ring_wide=(uint8)result.fork;
 cam_ring_cl=(short)result.open_left;cam_ring_cr=(short)result.open_right;
 TrackVisionPublish(&result);
}
