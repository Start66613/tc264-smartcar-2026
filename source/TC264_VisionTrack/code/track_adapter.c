#include "zf_common_headfile.h"
#include "track_adapter.h"
#include "ring_diag.h"
#include "IfxGpt12.h"
#include <string.h>
#include <stdlib.h>
#pragma section all "cpu0_dsram"
static VTControl controller;
static volatile VTFrame frame;
static volatile unsigned milliseconds,heartbeat;
static uint16 encoder_old[2];
static unsigned encoder_initialized,entry_odo;
static unsigned ticks_per_ms,last_tick,tick_remainder,last_control;
static unsigned remote_required,remote_stamp,remote_valid,remote_previous;
volatile unsigned track_report[29];
volatile unsigned track_epoch;
volatile unsigned big_ring_enabled=0u,big_ring_phase,big_ring_hits;
volatile unsigned track_fault,track_frame_age,track_heartbeat_age,track_confidence;
volatile unsigned track_phase,track_control_updates,track_sequence,track_start_count;
volatile int line_out_center=-1,line_out_servo_duty,line_out_left_speed,line_out_right_speed;
volatile int line_out_flags;
volatile float line_out_deviation,line_out_steering;
volatile unsigned line_out_lost_count,line_out_black_pixels;
volatile int line_speed_scale_pct=100,line_steer_kp_scale_pct=100;  /* v123: restored 100% (user request, v117 slowdown reverted) */
volatile int ring_out_mode,ring_out_dir,ring_out_frames,ring_out_hole=-1,ring_out_sig,ring_out_phase,ring_out_width;
volatile int line_ring_armed,line_out_segs,line_out_gap,line_out_white,line_pick_left;
volatile int bigring_out_state,bigring_peak,bigring_cross,line_out_speed_target,gyro_out_val,bw_cnt;
volatile unsigned bw_latch_odo,bw_prev_odo;
extern volatile uint8 servo_released;
extern volatile int line_enc5,line_enc6,line_gyro_f,line_gyro_ready,line_imu_ok,line_tilt_stop,line_stall,line_yaw10;
extern volatile unsigned line_enc_seq,line_enc_dt_ms,line_odo_run,line_run_elapsed_ms;
void TrackVisionPublish(const VTFrame *v)
{
 unsigned irq=interrupt_global_disable();frame=*v;track_sequence=v->sequence;track_confidence=v->confidence;interrupt_global_enable(irq);
}
unsigned TrackNow(void){return milliseconds;}
unsigned TrackEdgeMode(void){return big_ring_enabled&&big_ring_phase>=2u&&big_ring_phase<=4u?0u:vt_edge_mode(&controller);}
unsigned TrackRunning(void){return controller.active;}
void TrackHeartbeat(void){heartbeat=milliseconds;}
void TrackRemoteSeen(unsigned valid){remote_stamp=milliseconds;remote_valid=valid;}
void TrackRemoteRequired(unsigned required){remote_required=required;remote_previous=uart_receiver.channel[2];}
void PID_Reset(PID_Controller *p){p->error=p->last_error=p->integral=p->derivative=p->output=0;}
void PID_Init(PID_Controller *p,float kp,float ki,float kd,float imax,float omax)
{p->param.Kp=kp;p->param.Ki=ki;p->param.Kd=kd;p->param.integral_max=imax;p->param.output_max=omax;PID_Reset(p);}
float PID_Calculate(PID_Controller *p,float target,float actual)
{
 float e=target-actual;p->derivative=.35f*p->derivative+.65f*(e-p->last_error);p->last_error=e;p->error=e;
 p->output=p->param.Kp*e+p->param.Kd*p->derivative;
 if(p->output>p->param.output_max)p->output=p->param.output_max;
 if(p->output< -p->param.output_max)p->output=-p->param.output_max;
 return p->output;
}
void SpeedLoop_Reset(void)
{
 unsigned irq=interrupt_global_disable();vt_control_reset(&controller,milliseconds);track_fault=0;line_stall=0;track_epoch++;
 line_out_left_speed=line_out_right_speed=line_out_servo_duty=0;line_out_lost_count=0;interrupt_global_enable(irq);
}
void RingReset(void)
{
 unsigned irq=interrupt_global_disable();vt_control_stop(&controller,VT_OK);
 entry_odo=bw_latch_odo=bw_prev_odo=0;bw_cnt=0;ring_out_mode=ring_out_phase=ring_out_frames=0;
 bigring_out_state=bigring_peak=bigring_cross=0;track_phase=VT_NORMAL;interrupt_global_enable(irq);
}
int CalculateCenterLine(uint8 image[1][128])
{(void)image;return frame.valid?(int)(frame.mid*128/188):-1;}
float CalculateDeviation(uint8 image[1][128],int center)
{(void)image;return center>=0?(float)(center-64)/64.0f:0;}
void LineFollow_PID(PID_Controller *ps,PID_Controller *pv,uint8 image[1][128])
{(void)ps;(void)pv;(void)image;/* Fixed-period timer owns all actuator writes. */}
void LineFollow_PID_End(PID_Controller *ps,PID_Controller *pv,uint8 image[1][128])
{(void)ps;(void)pv;(void)image;line_ring_armed=0;}
void RingDiagReadController(unsigned *flags,unsigned *count,unsigned *candidate,unsigned *entry)
{
 *flags=RD_ACTION_ENABLED|(line_ring_armed?RD_ARMED:0u)|(frame.fork?RD_MOUTH:0u);
 *count=(unsigned)controller.phase;*candidate=controller.phase>=VT_RING_BEGIN&&controller.phase<=VT_RING_END;
 *entry=entry_odo;
}
static void apply(void)
{
 int turn=controller.steering,duty=abs(turn);
 DRV_8701_MOTOR_DRIVER_D((int)controller.pwm[0],(int)controller.pwm[1]);
 if(controller.active||!servo_released)servo_control(turn>0?'R':(turn<0?'L':'C'),duty);
 line_out_servo_duty=duty;line_out_left_speed=(int)controller.pwm[0];line_out_right_speed=(int)controller.pwm[1];
}
void TrackControlTick(void)
{
 uint16 l,r;unsigned old_phase,dt,now_tick=IfxStm_getLower(&MODULE_STM0);
 if(!ticks_per_ms){ticks_per_ms=IfxStm_getFrequency(&MODULE_STM0)/1000u;last_tick=now_tick;}
 tick_remainder+=(unsigned)(now_tick-last_tick);last_tick=now_tick;
 milliseconds+=tick_remainder/ticks_per_ms;tick_remainder%=ticks_per_ms;
 if(line_ring_armed&&remote_required){
  unsigned channel=uart_receiver.channel[2];
  if((channel>=992u&&remote_previous<992u)||!remote_valid||milliseconds-remote_stamp>500u){
   vt_control_stop(&controller,VT_MANUAL);line_ring_armed=0;track_fault=VT_MANUAL;apply();
  }
  remote_previous=channel;
 }
 if(!gpio_get_level(P11_3)&&line_ring_armed){
  vt_control_stop(&controller,VT_MANUAL);line_ring_armed=0;track_fault=VT_MANUAL;apply();
 }
 if(milliseconds-last_control<20u)return;
 dt=milliseconds-last_control;last_control=milliseconds;
 l=(uint16)IfxGpt12_T5_getTimerValue(&MODULE_GPT120);r=(uint16)IfxGpt12_T6_getTimerValue(&MODULE_GPT120);
 if(!encoder_initialized){encoder_old[0]=l;encoder_old[1]=r;encoder_initialized=1;}
 line_enc5=(int)(int16)(uint16)(l-encoder_old[0]);line_enc6=(int)(int16)(uint16)(r-encoder_old[1]);
 encoder_old[0]=l;encoder_old[1]=r;line_enc_dt_ms=dt;line_enc_seq++;
 if(line_ring_armed&&!controller.active&&!track_fault){
  if(frame.valid&&milliseconds-frame.stamp<=150u){vt_control_start(&controller,milliseconds);track_start_count++;}
 }
 if(!line_ring_armed&&controller.active)vt_control_stop(&controller,VT_MANUAL);
 old_phase=controller.phase;
 vt_step(&controller,(const VTFrame *)&frame,milliseconds,dt,line_enc5,line_enc6,line_yaw10,line_gyro_f,
         (unsigned)(line_imu_ok&&line_gyro_ready),milliseconds-heartbeat,(unsigned)line_tilt_stop,
         (unsigned)line_speed_scale_pct,(unsigned)line_steer_kp_scale_pct);
 if(controller.fault){line_ring_armed=0;track_fault=controller.fault;}
 if(old_phase==VT_NORMAL&&controller.phase==VT_RING_BEGIN)entry_odo=line_odo_run;
 track_phase=controller.phase;track_control_updates=controller.updates;track_frame_age=milliseconds-frame.stamp;track_heartbeat_age=milliseconds-heartbeat;
 ring_out_mode=controller.phase>=VT_RING_BEGIN&&controller.phase<=VT_RING_END;
 ring_out_dir=controller.ring_direction;ring_out_phase=(int)controller.phase;ring_out_frames=(int)(milliseconds-controller.phase_ms)/20;
 ring_out_hole=frame.hole;ring_out_sig=frame.fork;ring_out_width=frame.width;
 bigring_out_state=ring_out_mode?(controller.phase>=VT_RING_OUT?2:1):0;bigring_cross=(int)controller.ring_completed;
 line_out_center=frame.valid?frame.mid*128/188:-1;line_out_flags=(frame.valid?1:0)|(frame.degraded?2:0)|(frame.all_white?4:0)|(frame.fork?8:0)|(frame.hole?16:0)|(frame.corner_left?32:0)|(frame.corner_right?64:0)|(frame.finish?128:0);line_out_deviation=frame.valid?(float)(frame.mid-94)/94.0f:0;
 line_out_steering=(float)controller.steering/10.0f;line_out_lost_count=controller.fault==VT_LOST?31u:0u;
 line_out_speed_target=(int)((controller.target[0]+controller.target[1])/2u);line_stall=controller.fault==VT_STALL;
 apply();
 track_report[3]++;track_report[0]=0x56543130u;track_report[1]=1;track_report[2]=29u*4u;
 track_report[4]=milliseconds;track_report[5]=controller.active;track_report[6]=track_fault;track_report[7]=controller.phase;
 track_report[8]=track_frame_age;track_report[9]=track_heartbeat_age;track_report[10]=track_confidence;track_report[11]=track_sequence;
 track_report[12]=track_start_count;track_report[13]=track_control_updates;track_report[14]=controller.pwm[0];track_report[15]=controller.pwm[1];
 track_report[16]=(unsigned)line_enc5;track_report[17]=(unsigned)line_enc6;track_report[18]=(unsigned)controller.steering;
 track_report[19]=controller.target[0];track_report[20]=controller.target[1];track_report[21]=dt;track_report[22]=line_imu_ok&&line_gyro_ready;
 /* v104：中线质量（可信测量行 / 模型插值补出行），用来在板上确认"中线有多少是真看到的" */
 track_report[24]=frame.measured;track_report[25]=frame.model_rows;
 /* v105：二值化可观测。26=大津法级别(1/2)；27=二值后白像素占比(%)。
  * 蓝白赛道正常 35~55；若 26 恒为 1 且 27 超过 80，说明亮类没被二次切开，
  * 蓝底很可能又被判成白——回读一眼就能确认两级 Otsu 有没有生效。 */
 track_report[26]=cam_otsu_level;
 track_report[27]=(unsigned)cam_bin_white*100u/(unsigned)(CAM_W*CAM_H);
 /* v105：28=单边贴边补线行数。急弯里白路跑出画面边缘时，这一侧边界不可见，
  * 靠"可见另一边 + 学习路宽"把中线撑住。这个数明显大于 0 就说明补线在工作。 */
 track_report[28]=frame.clip_rows;
 track_report[3]++;track_report[23]=track_report[3];
}
#pragma section all restore

