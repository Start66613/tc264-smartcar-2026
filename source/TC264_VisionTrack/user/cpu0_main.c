/*********************************************************************************************************************


* TC264 Opensourec Library ¼´£¨TC264 ¿ªÔ´¿â£©ÊÇÒ»¸ö»ùÓÚ¹Ù·½ SDK ½Ó¿ÚµÄµÚÈı·½¿ªÔ´¿â


* Copyright (c) 2022 SEEKFREE Öğ·É¿Æ¼¼


*


* ±¾ÎÄ¼şÊÇ TC264 ¿ªÔ´¿âµÄÒ»²¿·Ö


*


* TC264 ¿ªÔ´¿â ÊÇÃâ·ÑÈí¼ş


* Äú¿ÉÒÔ¸ù¾İ×ÔÓÉÈí¼ş»ù½ğ»á·¢²¼µÄ GPL£¨GNU General Public License£¬¼´ GNUÍ¨ÓÃ¹«¹²Ğí¿ÉÖ¤£©µÄÌõ¿î


* ¼´ GPL µÄµÚ3°æ£¨¼´ GPL3.0£©»ò£¨ÄúÑ¡ÔñµÄ£©ÈÎºÎºóÀ´µÄ°æ±¾£¬ÖØĞÂ·¢²¼ºÍ/»òĞŞ¸ÄËü


*


* ±¾¿ªÔ´¿âµÄ·¢²¼ÊÇÏ£ÍûËüÄÜ·¢»Ó×÷ÓÃ£¬µ«²¢Î´¶ÔÆä×÷ÈÎºÎµÄ±£Ö¤


* ÉõÖÁÃ»ÓĞÒşº¬µÄÊÊÏúĞÔ»òÊÊºÏÌØ¶¨ÓÃÍ¾µÄ±£Ö¤


* ¸ü¶àÏ¸½ÚÇë²Î¼û GPL


*


* ÄúÓ¦¸ÃÔÚÊÕµ½±¾¿ªÔ´¿âµÄÍ¬Ê±ÊÕµ½Ò»·İ GPL µÄ¸±±¾


* Èç¹ûÃ»ÓĞ£¬Çë²ÎÔÄ<https://www.gnu.org/licenses/>


*


* ¶îÍâ×¢Ã÷£º


* ±¾¿ªÔ´¿âÊ¹ÓÃ GPL3.0 ¿ªÔ´Ğí¿ÉÖ¤Ğ­Òé ÒÔÉÏĞí¿ÉÉêÃ÷ÎªÒëÎÄ°æ±¾


* Ğí¿ÉÉêÃ÷Ó¢ÎÄ°æÔÚ libraries/doc ÎÄ¼ş¼ĞÏÂµÄ GPL3_permission_statement.txt ÎÄ¼şÖĞ


* Ğí¿ÉÖ¤¸±±¾ÔÚ libraries ÎÄ¼ş¼ĞÏÂ ¼´¸ÃÎÄ¼ş¼ĞÏÂµÄ LICENSE ÎÄ¼ş


* »¶Ó­¸÷Î»Ê¹ÓÃ²¢´«²¥±¾³ÌĞò µ«ĞŞ¸ÄÄÚÈİÊ±±ØĞë±£ÁôÖğ·É¿Æ¼¼µÄ°æÈ¨ÉùÃ÷£¨¼´±¾ÉùÃ÷£©


*


* ÎÄ¼şÃû³Æ          cpu0_main


* ¹«Ë¾Ãû³Æ          ³É¶¼Öğ·É¿Æ¼¼ÓĞÏŞ¹«Ë¾


* °æ±¾ĞÅÏ¢          ²é¿´ libraries/doc ÎÄ¼ş¼ĞÄÚ version ÎÄ¼ş °æ±¾ËµÃ÷


* ¿ª·¢»·¾³          ADS v1.10.2


* ÊÊÓÃÆ½Ì¨          TC264D


* µêÆÌÁ´½Ó          https://seekfree.taobao.com/


*


* ĞŞ¸Ä¼ÇÂ¼


* ÈÕÆÚ              ×÷Õß                ±¸×¢


* 2022-09-15       pudding            first version


********************************************************************************************************************/


#include "zf_common_headfile.h"


#include "line_ui_assets.h"


#include "ring_diag.h"


#include "control_time.h"
#include "track_adapter.h"


#pragma section all "cpu0_dsram"


// ½«±¾Óï¾äÓë#pragma section all restoreÓï¾äÖ®¼äµÄÈ«¾Ö±äÁ¿¶¼·ÅÔÚCPU0µÄRAMÖĞ





// **************************** ´úÂëÇøÓò ****************************





// Ñ§³¤ 6.0 Ñ­¼£³ÌĞò¡ª¡ªµ±Ç°½ÓÏßÊÊÅä°æ£¨Ëã·¨ĞŞÕı + Í¼ĞÎ½çÃæ + Æô¶¯±£»¤£©¡£


//   1) ĞŞµôÈ«ºÚ/È«°×ÎóÅĞ¾ÓÖĞ¡¢¶şÖµ»¯ãĞÖµ±ß½ç£¨code/TX.c¡¢code/PID.c£©£»


//   2) Á¬Ğø¶ªÏß³¬¹ıãĞÖµÁ¢¼´Í£³µ£»


//   3) P11.3 Æô¶¯/¼±Í££ºÉÏµç´ı»ú£¬°´Ò»ÏÂ½øÈë¡°µÈ´ıºÚÏß¡±£¬¿´µ½Á¬ĞøºÚÏß²Å×Ô¶¯Æğ²½¡£


// ÉĞÎ´Íê³É£º±àÂëÆ÷ËÙ¶È±Õ»·£¨Ñ§³¤Ô­³ÌĞòÎª¿ª»·£©¡£





PID_Controller pid_steering;


PID_Controller pid_speed;





#define RUN_KEY         (P11_3)      /* Æô¶¯ / ¼±Í£ */


#define SERVO_KEY       (P11_2)      /* ¶æ»úËÉ¿ª / Ëø¶¨ ÇĞ»» */


#define REC_KEY         (P20_7)      /* ¿ªÊ¼ / Í£Ö¹ ÇúÏß¼ÇÂ¼£¨ÊµÎïÈ·ÈÏ P20.7£© */


#define PAGE_KEY        (P20_6)      /* Ñ­»·ÇĞ»»Ò³Ãæ£ºÖ÷½çÃæ ¡ú ²¨ĞÎ ¡ú ¼ÇÂ¼ */


#define KEY_DEBOUNCE_MS (30u)
#define KEY_LONG_MS (600u)    /* v116: P20.7 short/long split, ms */

/* ¡ïv079£º¿ØÖÆÁ´×ø±ê¾µÏñ£¨ÖÎ"Âú¶æÂÒÅÜ"£©¡ï
   Êµ²â£¨ÓÃ»§ 2026-10-07 ÊÖÖ¸²âÊÔ + ips200 ÏÔÊ¾ÎŞ·­×ªÈ·ÈÏ£©£ºÉãÏñÍ·Í¼Ïñ¡¾·Ç¾µÏñ¡¿£¬
   ¼´"Í¼Ïñ×ó = ³µÉí×ó"¡£¶ø PID.c ÕûÌõ¿ØÖÆÁ´°´"Í¼Ïñ×ó = ³µÉíÓÒ"£¨CCD ¾µÏñÄê´ú£©
   Éè¼Æ ¡ª¡ª Ö±½ÓÎ¹Èë»áµ¼ÖÂ×ªÏòÕı·´À¡£¨Ô½ĞŞÔ½Æ«£¬Âú¶æÂÒÅÜ£©¡£
   ±¾¿ª¹Ø°Ñ"Î¹¸ø¿ØÖÆÁ´µÄ×ø±ê"·­×ª»Ø¾µÏñÔ¼¶¨£¨Í¼Ïñ×ó = ³µÉíÓÒ£©¡£
   Èô½«À´¸ü»»°²×°·½Ïò»ò»ØÍË´úÂë£¬°Ñ´Ëºê¸Ä 0 ¼´¿É¡£ */
#define CTRL_MID_MIRROR   1





// ×´Ì¬»ú£º´ı»ú -> (°´¼ü) µÈ´ıºÚÏß -> (Á¬ĞøÓĞĞ§Ïß) ÔËĞĞÖĞ -> (¶ªÏß/¼±Í£) Í£³µ


#define ST_IDLE (0u)


#define ST_ARM  (1u)


#define ST_RUN  (2u)


#define ST_STOP (3u)


#define ST_FIN  (4u)


#define ARM_FRAMES 5u          // Á¬ĞøÕâÃ´¶à¸öÓĞĞ§Ö¡²ÅÔÊĞíÆğ²½





#define PANEL_BG     0x0841


#define PANEL_WHITE  0xffff


#define PANEL_CYAN   0x5eff


#define PANEL_AMBER  0xfd20


#define PANEL_GREEN  0x07e0


#define PANEL_RED    0xf800





// ÔËĞĞ±¨¸æ£º¹Ì¶¨Ğ¡¶Ë 32 Î»×Ö£¬¹© DAS ÈÈ attach Ö»¶Á»Ø¶Á¡£


#define LINE_REPORT_SIGNATURE (0x4C463631u)   // 'LF61'


typedef struct {


    unsigned signature, version, size, sequence;


    unsigned uptime_ms, loop_count, state;


    unsigned center, deviation_milli, steering_milli;


    unsigned servo_duty, left_speed, right_speed;


    unsigned lost_count, threshold, black_pixels, key_presses;


    unsigned enc5, enc6, gyro_z;          /* ±àÂëÆ÷ T5/T6 ¼ÆÊı + ÍÓÂİ Z Öá */


    unsigned ring_mode, ring_dir, ring_frames, ring_hole, ring_sig;  /* Ô²»·×´Ì¬ */


    unsigned ring_phase, ring_width;            /* »·½×¶Î / °×Â·×Ü¿í */


    unsigned segs, gap;                         /* °×¶ÎÊı / ×î´ó°µÇø¼äÏ¶ */


    unsigned white;                             /* °×Â·×Ü¿í£¨°×ÏñËØÊı£© */


    unsigned bigring;                           /* ´ó»·×´Ì¬£º0=ÆÕÍ¨ 1=»·ÄÚ 2=³ö»· */


    unsigned bigring_peak, bigring_cross;       /* ½ø»·ÒÔÀ´×î´ó¿í¶È / ²í¿Ú´ÎÊı */


    unsigned speed_target;                      /* ËÙ¶È±Õ»·Ä¿±ê£¨¼ÆÊı/20ms£© */


    unsigned imu_ok;                            /* IMU ³õÊ¼»¯×´Ì¬ 1=Õı³£ 0=Ê§°Ü */


    unsigned gyro_bias, gyro_ready;             /* ÍÓÂİÁãÆ« / ±ê¶¨Íê³É(1=ÊÇ) */


    unsigned gyro_f;                            /* ÂË²¨ºóÍÓÂİ Z£¨×èÄáÓÃ£© */


    unsigned odometer, stall;                   /* Àï³Ì / ¶Â×ª±êÖ¾ */


    unsigned acc_x, acc_y, acc_z;               /* ¼ÓËÙ¶È¼ÆÈıÖá£¨ÉÏ±¨£© */


    unsigned enc_peak;                          /* ±àÂëÆ÷·åÖµ¼ÆÊı/20ms£¨ÖĞÖµÂË²¨ºó£© */


    unsigned enc_avg;                           /* ±àÂëÆ÷ÔËĞĞÆ½¾ù¼ÆÊı/20ms£¨±ê¶¨²Î¿¼£© */


    unsigned cring_cnt, cring_w, cring_in;      /* ¡ïv043 ´ó»·£º»·¿Ú¼ÆÊı/Æ¬¿í/ÊÇ·ñÔÚ»·ÄÚ¡ï */


    unsigned yaw10;                             /* ¡ïv043£ºÍÓÂİ»ı·Ö×ª½Ç£¨¶È¡Á10£©¡ï */


    unsigned sequence_end;


} LineSeniorReport;


volatile LineSeniorReport line_senior_report;





// CCD Ô­Ê¼»Ò¶ÈÇúÏß£¨128 ÏñËØ£©£¬¹©Ö÷»ú»Ø¶Á»­Í¼£¬Õï¶ÏÉãÏñÍ·µ½µ×¿´µ½ÁËÊ²Ã´¡£


volatile unsigned char line_ccd_raw[128];


volatile int line_cam_ok = 0;         /* ¡ïv066£ºÉãÏñÍ·³õÊ¼»¯ÊÇ·ñ³É¹¦£¨ÉÏ±¨£©¡ï */





/* ¡ïv069£ºÉãÏñÍ·Õï¶ÏÒÇ±íÅÌ£¨¾µÏñµ½ CPU0 ¿É¶ÁÇø£¬DAS ¿É»Ø¶Á / ÆÁÄ»¿ÉÏÔÊ¾£©¡ï */


volatile int  line_cam_finish = 0, line_cam_lost = 0, line_cam_locked = 0;


volatile int  line_cam_dma_int = 0, line_cam_init_flag = 0, line_cam_llnum = 0;


volatile unsigned line_cam_claims = 0, line_cam_polls = 0, line_cam_claims_1s = 0;


volatile unsigned line_cam_reinit_n = 0;


volatile int  line_cam_fps_x10 = 0;


volatile int  line_loop_hz = 0, line_loop_max_ms = 0;


/* ¡ïv072£ºÆô¶¯½ø¶È±ê¼Ç£¨¼´Ê¹ÆÁÄ»²»ÁÁ£¬DAS Ò²ÄÜ¶ÁËüÅĞ¶Ï¿¨ÔÚÄÄÒ»²½£©¡ï


   1=Ê±ÖÓ 2=°´¼ü 3=±àÂëÆ÷ 4=ÆÁÄ» 5=ÉãÏñÍ· 6=IMU 7=Ö´ĞĞÆ÷ 9=Ê×ÆÁ 10=½øÈëÖ÷Ñ­»· */


volatile unsigned line_boot_stage = 0;


volatile unsigned line_loop_count = 0;


/* v075£ºÖ¡ĞòºÅÎÕÊÖ */


volatile unsigned line_frame_changes = 0;    /* Ö÷Ñ­»·È¡µ½ĞÂÖ¡µÄÀÛ¼Æ´ÎÊı */


volatile unsigned line_frame_last_seq = 0;   /* Ö÷Ñ­»·ÒÑ´¦ÀíµÄÖ¡ĞòºÅ */


/* v070: ²É¼¯Á´Â·¼ÆÊıÆ÷£¨Õï¶Ï£© */


volatile int  line_cam_fin_seen = 0, line_cam_init_seen = 0, line_cam_dint_max = 0;


volatile unsigned line_isr_vsync_n = 0, line_isr_dma_n = 0;


volatile int line_cam_hole = 0;       /* ¡ïv066£º2D »·ĞÄ£¨Á½²à°×+ÖĞ¼ä°µ£©¡ï */


volatile int line_cam_wide = 0;       /* ¡ïv066£º2D ¿íÇø£¨Ç°·½´óÆ¬°×£©¡ï */


volatile int line_cam_hole_y = -1;    /* »·ĞÄËùÔÚĞĞ */


volatile int line_cam_rows = 0;       /* ÓĞĞ§ÖĞÏßĞĞÊı */
volatile int line_cam_mid_dbg = -1;   /* ¡ïv079£ºÔ­Ê¼Í¼Ïñ×ø±êµÄ½ü´¦ÖĞÏß£¨188 Ïµ£¬Î´·­×ª£»¹©·½ÏòºË¶Ô£©¡ï */
volatile int line_cam_dev_dbg = 0;    /* ¡ïv079£ºÔ­Ê¼Í¼Ïñ×ø±êÆ«²î£¨Ç§·Ö±È£»Õı = ÏßÔÚÍ¼ÏñÓÒ£©¡ï */





/* ÍÑ»úÇúÏß¼ÇÂ¼£º°´¼ü´¥·¢£¬Ë³ĞòĞ´Èë TRACE_MAX Ö¡ CCD »Ò¶È(128 µã)¡£


 * ÊÖ¶¯ÍÆ³µ / ×Ô¶¯ĞĞÊ»¶¼ÄÜÂ¼£¬Â¼ÍêÓÃ DAS »Ø¶ÁÀëÏß·ÖÎö¡£


 * Êµ²â£ºCCD Êµ²â 31 Hz£¨32.7ms/Ö¡£©¡£°´¡°CCD Ö¡¡±½µ²ÉÑù£¨Ã¿ REC_DIV_FRAMES Ö¡´æ 1 Ö¡£©£¬


 * ÓëÖ÷Ñ­»·¿ìÂıÎŞ¹Ø£ºÃ¿ 2 Ö¡Ò»´æ ¡Ö 65ms/Ñù±¾ ¡ú 2000 Ö¡ ¡Ö 130 Ãë¡£ */


#define TRACE_MAX 2000


#define REC_DIV_FRAMES 2      /* v130: Ã¿ 2 Ö¡Ò»´æ ¡Ö 65ms/Ñù±¾£¨Êµ²â 31Hz£©£»2000 Ö¡ ¡Ö 130 Ãë */


volatile unsigned char line_trace[CAM_H][CAM_W];   /* v129: save-frame buffer (120x188) only, decoupled from TRACE_MAX */
volatile unsigned cam_saved_valid = 0u;
volatile unsigned cam_saved_request = 0u;
volatile unsigned cam_saved_sequence = 0u;
volatile short cam_saved_mid[CAM_H];
/* v142 ç›²åŒºè‡ªåŠ¨å­˜å¸§ï¼ˆè¯Šæ–­ï¼‰ï¼šè½¦è¿è¡Œä¸­è¿ç»­ 3 ä¸ªé‡‡æ ·æ‹ï¼ˆâ‰ˆ195msï¼‰æ— æœ‰æ•ˆçº¿ â†’ è‡ªåŠ¨æŠ“ä¸€å¸§ï¼Œ
   å­˜å…¥ line_trace/cam_saved_midï¼ˆå¤ç”¨ P20.7 å­˜å¸§é€šé“ï¼‰ã€‚ç¼“å†²æœªè¯»å‡ºæ—¶ä¸è¦†ç›–ï¼›
   çº¿æ¢å¤åå†ç›²å¯å†æŠ“ã€‚å…ƒæ•°æ®ï¼šauto 0=æ‰‹åŠ¨ 1=ç›²åŒºè‡ªåŠ¨ã€‚ */
#define CAM_BLIND_AUTO_TICKS 3u
volatile unsigned cam_saved_auto = 0u;
volatile unsigned cam_saved_meta_flags = 0u;
volatile unsigned cam_saved_meta_br2 = 0u;
volatile unsigned cam_saved_meta_ms = 0u;
volatile unsigned cam_saved_meta_recf = 0u;


volatile unsigned line_trace_count = 0;    /* ÒÑ¼ÇÂ¼Ö¡Êı */


/* ×´Ì¬×·×Ù£ºÓë line_trace Í¬²½¼ÇÂ¼×´Ì¬»ú¿ìÕÕ£¬¹© DAS »Ø¶ÁÕï¶Ï */


volatile unsigned char trace_speed[TRACE_MAX];    /* v130b: avg PWM /12 (byte*12 => 0..3060 speed units) */


volatile unsigned char trace_br2[TRACE_MAX];    /* v134 br2 diag: b0 arm b1 mouth b2 exit; hi bits = phase */


volatile signed char   trace_center[TRACE_MAX];   /* ÖĞÏß -1..127 */


volatile short         trace_gyro[TRACE_MAX];      /* ÂË²¨ºóÍÓÂİ£¨×èÄáµ÷ÊÔÓÃ£© */


volatile short         trace_cl[TRACE_MAX];        /* ¡ïv095£º»·¿ÚÖ¸ÎÆ ×óºá¶ÏÂÊ ¡ï */


volatile short         trace_cr[TRACE_MAX];        /* ¡ïv095£º»·¿ÚÖ¸ÎÆ ÓÒºá¶ÏÂÊ ¡ï */


volatile signed char   trace_steer[TRACE_MAX];    /* v116: steering command (-100..100), U-turn diagnosis */

volatile unsigned char trace_flags[TRACE_MAX];   /* v116: bit0 valid / bit1 degraded / bit2 all_white */

volatile unsigned ccd_frames = 0;   // TSL1401 ²É¼¯Ö¡¼ÆÊı£¨¶¨ÒåÔÚ´ËÒÔ±ã»Ø¶Á£©


volatile int line_enc5 = 0, line_enc6 = 0;   // ±àÂëÆ÷ T5/T6 Ã¿ 20ms ¼ÆÊı


volatile unsigned line_enc_dt_ms = 20u, line_run_elapsed_ms = 0u;


volatile unsigned line_enc_seq = 0;          // ±àÂëÆ÷²ÉÑùĞòºÅ£¨ËÙ¶È±Õ»·ÓÃ£©


volatile int line_gyro_z = 0;              // IMU660RA ÍÓÂİ Z Öá


volatile int line_imu_ok = 0;              // IMU ³õÊ¼»¯½á¹û£º1=Õı³£ 0=Ê§°Ü(×Ô¶¯½µ¼¶)


volatile int line_gyro_f = 0;              // ÂË²¨ºóÍÓÂİ Z£¨È¥ÁãÆ«£¬×èÄáÓÃ£©


volatile int line_gyro_bias = 0;           // ÍÓÂİÁãÆ«£¨¿ª»ú¾²Ö¹±ê¶¨Öµ£©


volatile int line_gyro_ready = 0;          // 1=ÁãÆ«±ê¶¨Íê³É£¨Ô¼ÉÏµç2Ãëºó£©


volatile unsigned line_odometer = 0;       // Àï³Ì£¨±àÂëÆ÷¼ÆÊıÀÛ¼Æ£©


volatile int line_stall = 0;               // ¶Â×ª±êÖ¾£º1=ÓĞÊä³öµ«³µ²»¶¯


volatile int line_enc_peak = 0;            // ¡ï±àÂëÆ÷·åÖµ(ÖĞÖµÂË²¨ºó)¡ª¡ª±ê¶¨±Õ»·Ä¿±êÓÃ¡ï


volatile int line_enc_avg = 0;             // ¡ï±àÂëÆ÷ÔËĞĞÆ½¾ù¡ª¡ª±ê¶¨²Î¿¼¡ï


/* v046: ³µÍ·×ó²à"´óÆ¬°×"¼ì²â£¨= Í¼ÏñÓÒ²àµÄ´óÆ¬°×£©¡£ÓÃ»§Êµ²â£ºÕâ¸ö¿Ú×Ó¸ÃÖ±ĞĞ¡£


   ÒÀ¾İ£¨³µÍ£ÔÚ»·¿Ú×¥µÄÔ­Ê¼Í¼£©£º°×¶Î 42..121 -> ¿í 80¡¢ÖĞĞÄ 81¡£


   ÅĞ¾İ£º×î¿í°×¶Î ¿í¶È >= 70 ÇÒ ÖĞĞÄ >= 72£¨Ã÷ÏÔÆ«Í¼ÏñÓÒ²à = Ã÷ÏÔÔÚ³µÍ·×ó²à£©¡£


   ·ÅÔÚÖ÷Ñ­»·Àï -> ´ı»úÊ±Ò²ÊµÊ±¸üĞÂ£¬¿É"°Ñ³µ·ÅÔÚ»·¿ÚÖ±½ÓÑéÖ¤ãĞÖµ"¡£ */


#define BW_W_WIDE      70


#define BW_CENTER_MIN  72


#define BW_HOLD_MS     1500u


volatile int line_bigwhite = 0;


volatile int line_bigwhite_w = 0;


volatile int line_bigwhite_c = 0;


volatile int line_bigwhite_hold = 0;


volatile unsigned line_odo_run = 0;  /* v050£º´ÓÆğÅÜË²¼ä¿ªÊ¼ÀÛ¼ÆµÄÀï³Ì£¨Ö»Ëã RUN ÆÚ¼ä£©*/


volatile int line_tilt_stop = 0;     /* v061£º²à·­±£»¤£¨1=Çã½Ç¹ı´ó£¬Í£³µ£©


                                      * ÓÃ"×ÜÇã½Ç"ÅĞ¶¨£¬Óë IMU °²×°·½ÏòÎŞ¹Ø£º


                                      * cos(Çã½Ç)=az/|a|£¬¼´ az*az < 0.36*|a|*|a| ±íÊ¾Çã½Ç>53¶È¡£*/


static int tilt_cnt = 0;


volatile int line_allbright = 0;     /* v047£ºÕû·ùÈ«ÁÁ£¨Õ¾ÔÚ´ó»·°×ÃæÉÏ=µÚ2¸ö¿ÚµÄÌØÕ÷£©*/


static uint32 bw_since = 0;





volatile int line_yaw10 = 0;               // ¡ïv043£ºÍÓÂİ»ı·Ö×ª½Ç(¶È¡Á10)¡ª¡ª´ó»·ÅĞ360¡ã³ö»·¡ï


volatile int line_rec_trigger = 0;         // ¡ïv043£ºÖÃ1Á¢¿Ì¿ªÊ¼ÇúÏß¼ÇÂ¼£¨½ø»·×Ô¶¯´¥·¢£©¡ï


static float yaw_acc = 0.0f;               // ÍÓÂİ»ı·ÖÀÛ¼ÓÆ÷£¨¶È£©


static int  enc_med_buf[5] = {0,0,0,0,0};  // ÖĞÖµÂË²¨´°¿Ú£¨Ïûµ¥Ö¡Ê±Ğò¼â·å£©


static int  enc_med_i = 0, enc_med_n = 0;


static unsigned enc_sum = 0, enc_cnt = 0;  // ÔËĞĞÆÚÀÛ¼Æ£¨ÇóÆ½¾ù£©





/* ÍÓÂİ/±àÂëÆ÷´¦ÀíµÄÄÚ²¿×´Ì¬ */


#define GYRO_CAL_N   100                   /* ÁãÆ«±ê¶¨Ñù±¾Êı£¨100¡Á20ms=2s£© */


#define GYRO_FILT_N  4                     /* ÍÓÂİ»¬¶¯Æ½¾ù´°¿Ú */


static int  gyro_buf[GYRO_FILT_N] = {0,0,0,0};


static int  gyro_buf_i = 0, gyro_buf_n = 0;


static int  gyro_cal_sum = 0, gyro_cal_cnt = 0;





static int  imu_dead_cnt = 0;              /* gyro+acc È«ÁãÁ¬Ğø¼ÆÊı£¨SPIÊ§²½¼ì²â£© */


static int  imu_reinit_cd = 0;             /* IMU ÖØ³õÊ¼»¯ÀäÈ´£¨¡Á20ms£© */


volatile int imu_dbg = 0;                  /* Õï¶Ï£ºÉÏ´ÎÖØ³õÊ¼»¯½á¹û£¨0=»Ö¸´ 1=ÈÔÊ§Áª£© */


extern volatile int ring_out_mode, ring_out_dir, ring_out_frames, ring_out_hole, ring_out_sig;


extern volatile int ring_out_phase, ring_out_width;


extern volatile int line_ring_armed;


extern volatile int line_out_segs, line_out_gap, line_out_white;


extern volatile int bw_cnt;   /* v059£º»·¿ÚÑØ±ä¼ÆÊı£¨PID.c ¶¨Òå£©*/


extern volatile unsigned bw_latch_odo;  /* v060£º»·¿ÚÀï³ÌËø´æ */


extern volatile int bigring_out_state;


extern volatile int bigring_peak, bigring_cross;


extern volatile int line_out_speed_target;


void SpeedLoop_Reset(void);


void RingReset(void);


extern volatile int ring_out_phase, ring_out_width;


volatile int line_acc_x = 0, line_acc_y = 0, line_acc_z = 0;





static uint16 panel_rows[320 * 4];


static uint32 ticks_per_ms, boot_tick, last_draw, last_enc;


typedef struct { uint8 last, stable, pressed; uint32 change; uint32 down_ms; } KeyState;


static KeyState k_run   = {1u, 1u, 0u, 0u, 0u};


static KeyState k_servo = {1u, 1u, 0u, 0u, 0u};


volatile uint8 servo_released = 0u;    /* ¶æ»úÊÇ·ñ´¦ÓÚËÉ¿ª×´Ì¬ */


static uint8  ui_page    = 0u;        /* 0=Ö÷½çÃæ 1=²¨ĞÎÒ³ 2=¼ÇÂ¼Ò³ 3=ÉãÏñÍ·Ò³£¨20.6 Ñ­»·ÇĞ»»£© */


static uint8  state = ST_IDLE;        /* v082£º×´Ì¬´Ó core0_main ¾Ö²¿ÌáÉıÎªÎÄ¼ş¼¶£¨rc Ä£Ê½º¯ÊıĞèÒª·ÃÎÊ£© */


static uint16 rcb_prev3 = 0u;         /* v083£ºCH3 ÉÏ´ÎÖµ£¨±ßÑØ¼ì²â£© */


static uint8  rc_sync = 0u;           /* ¡ïv086£ºÒ£¿ØÊ×Ö¡Í¬²½±êÖ¾£¨·À"¿ª»ú×Ô¶¯·¢³µ"£©¡ï */


static uint8  rc_ever = 0u;           /* ¡ïv086£ºÊÇ·ñÔøÊÕµ½Ò£¿ØÖ¡ */


static uint32 rc_last_rx = 0u;        /* ¡ïv086£º×î½üÊÕµ½Ò£¿ØÖ¡µÄÊ±´Á£¨Ö¡ĞÂÏÊ¶È£© */


static uint16 rcb_prev1 = 0u;         /* ¡ïv096£ºCH1 ÉÏ´ÎÖµ£¨Â¼ÖÆ¼ü±ßÑØ¼ì²â£©¡ï */


static uint8  rc_rec_req = 0u;        /* ¡ïv096£ºÒ£¿ØÂ¼ÖÆÇĞ»»ÇëÇó£¨µÈĞ§ P20.7£©¡ï */


static uint16 rcb_prev5 = 0u;         /* v083£ºCH5 ÉÏ´ÎÖµ */


static uint16 rcb_prev6 = 0u;         /* v083£ºCH6 ÉÏ´ÎÖµ */


static uint8  rc_run_req = 0u;        /* v083£ºÒ£¿Ø·¢³µ/¼±Í£ÇëÇó£¨µÈĞ§À¶¼üÒ»´Î°´ÏÂ£© */


static uint8  rec_active = 0u;        /* ÊÇ·ñÕıÔÚ¼ÇÂ¼ */


static uint32 rec_frames = 0u;        /* ÒÑ¼ÇÂ¼Ö¡Êı */


static unsigned rec_last_frame = 0u; /* ÉÏ´Î´æÈëµÄ CCD Ö¡ºÅ£¨°´Ö¡½µ²ÉÑùÓÃ£© */


static KeyState k_rec  = {1u, 1u, 0u, 0u, 0u};
static uint8 rec_ev;                  /* v116: P20.7 event this loop (1 short / 2 long) */
static uint8 page_ev;                 /* v122: P20.6 event (long press = big-ring module toggle) */


static KeyState k_page = {1u, 1u, 0u, 0u, 0u};


static uint32 key_presses = 0u, loop_count = 0u, report_sequence = 0u;


static uint32 run_started = 0u;      // ½øÈë RUN µÄÊ±¿Ì£¨ÓÃÓÚÖÕµã¼ì²âÆğÅÜ±£»¤£©





static uint32 now_ticks(void) { return IfxStm_getLower(&MODULE_STM0); }


static uint32 age_ms(uint32 now, uint32 then) { return (uint32)(now - then) / ticks_per_ms; }





// ------------------------------ ÏÔÊ¾ ------------------------------


static void fill_rect(uint16 x, uint16 y, uint16 w, uint16 h, uint16 color)


{


    uint16 row, index, rows;


    if (w == 0 || h == 0) return;          /* ¿í/¸ßÎª 0 »á¿¨ËÀÆÁÄ»Çı¶¯£¬Ö±½ÓÌø¹ı */


    if (x >= 320 || y >= 240) return;      /* ÍêÈ«ÔÚÆÁÄ»Íâ */


    if ((uint32)x + w > 320) w = (uint16)(320 - x);   /* ÓÒ±ß½ç²Ã¼ô£ºÔ½½ç»á´¥·¢ÆÁÄ»¿â¶ÏÑÔËÀ»ú */


    if ((uint32)y + h > 240) h = (uint16)(240 - y);   /* ÏÂ±ß½ç²Ã¼ô */


    for (row = 0; row < h; row += rows)


    {


        rows = (uint16)(h - row < 4 ? h - row : 4);


        for (index = 0; index < w * rows; ++index) panel_rows[index] = color;


        ips200_show_rgb565_image(x, (uint16)(y + row), panel_rows, w, rows, w, rows, 0);


    }


}


static uint16 paint_label(LineLabel id, uint16 x, uint16 y, uint16 color)


{


    const LineBitmap *b = line_labels + id;


    uint16 row, column, dy, rows;


    if ((uint32)x + b->width > 320 || (uint32)y + b->height > 240) return 0;   /* Ô½½ç²»»­£¨·À¶ÏÑÔËÀ»ú£© */


    for (row = 0; row < b->height; row += rows)


    {


        rows = (uint16)(b->height - row < 4 ? b->height - row : 4);


        for (dy = 0; dy < rows; ++dy)


            for (column = 0; column < b->width; ++column)


                panel_rows[dy * b->width + column] =


                    (b->bits[(row + dy) * b->stride + column / 8] & (0x80 >> (column % 8))) ? color : PANEL_BG;


        ips200_show_rgb565_image(x, (uint16)(y + row), panel_rows, b->width, rows, b->width, rows, 0);


    }


    return b->width;


}


// »­ÎŞ·ûºÅÕûÊı£¬big=1 ÓÃ 36 ºÅ´ó×Ö£¬·ñÔò 24 ºÅ


static uint16 draw_digits(unsigned value, uint16 x, uint16 y, uint16 color, int big)


{


    char text[8];


    uint16 start = x;


    uint8 i;


    sprintf(text, "%u", value);


    for (i = 0; text[i]; ++i)


    {


        LineLabel gid = (LineLabel)((big ? UI_D36_0 : UI_D24_0) + (text[i] - '0'));


        if ((uint32)x + line_labels[gid].width > 320) break;   /* ·Å²»ÏÂ¾ÍÍ£Ö¹£¨·À¶ÏÑÔËÀ»ú£© */


        x += paint_label(gid, x, y, color);


    }


    return (uint16)(x - start);


}


static uint16 draw_signed_int(int value, uint16 x, uint16 y, uint16 color)


{


    uint16 start = x;


    if (value < 0) { x += paint_label(UI_MINUS, x, y, color); value = -value; }


    else           { x += paint_label(UI_PLUS, x, y, color); }


    x += draw_digits((unsigned)value, x, y, color, 0);


    return (uint16)(x - start);


}


static uint16 draw_signed_float(float v, uint16 x, uint16 y, uint16 color)


{


    uint16 start = x;


    int scaled = (int)(v * 100.0f);


    unsigned frac;


    if (scaled < 0) { x += paint_label(UI_MINUS, x, y, color); scaled = -scaled; }


    else            { x += paint_label(UI_PLUS, x, y, color); }


    frac = (unsigned)(scaled % 100);


    x += draw_digits((unsigned)(scaled / 100), x, y, color, 0);


    x += paint_label(UI_DOT, x, y, color);


    if (frac < 10) x += paint_label(UI_D24_0, x, y, color);


    x += draw_digits(frac, x, y, color, 0);


    return (uint16)(x - start);


}


static LineLabel state_glyph(uint8 st)


{


    switch (st) {


        case ST_ARM:  return UI_ST_ARM;


        case ST_RUN:  return UI_ST_RUN;


        case ST_STOP: return UI_ST_LOST;


        case ST_FIN:  return UI_ST_FIN;


        default:      return UI_ST_IDLE;


    }


}





static void draw_state_row(uint8 st)


{


    uint16 color = (st == ST_RUN) ? PANEL_GREEN : (st == ST_STOP ? PANEL_RED : PANEL_CYAN);


    fill_rect(0, 34, 320, 32, PANEL_BG);


    paint_label(UI_LB_STATE, 8, 40, PANEL_WHITE);


    paint_label(state_glyph(st), 60, 36, color);





    /* ¡ïv066£ºÉãÏñÍ·×´Ì¬ + 2D »·ÌØÕ÷£¨ÓÒÉÏ½Ç£¬×Ö·û´®Ö±»­£©¡ï */


    ips200_set_color(PANEL_WHITE, PANEL_BG);


    if (line_cam_ok) ips200_show_string(190, 36, "CAM OK ");


    else             ips200_show_string(190, 36, "CAMFAIL");


    {


        char buf[24];


        /* ÓÃÇáÁ¿Æ´½Ó£¨²»ÒÀÀµ sprintf µÄ¸¡µã£©£ºHOLE/WIDE/ÖĞÏßĞĞÊı */


        const char *tag = cam_ring_hole ? "HOLE" : (cam_ring_wide ? "WIDE" : " -- ");


        int n = 0;


        buf[n++]='R'; buf[n++]=':';


        buf[n++]=tag[0]; buf[n++]=tag[1]; buf[n++]=tag[2]; buf[n++]=tag[3];


        buf[n++]=' ';


        buf[n++]='r';


        /* ĞĞÊı 0~120 ÈıÎ»Êı */


        buf[n++]=(char)('0' + (cam_mid_rows/100)%10);


        buf[n++]=(char)('0' + (cam_mid_rows/10)%10);


        buf[n++]=(char)('0' + cam_mid_rows%10);


        buf[n]='\0';


        ips200_show_string(190, 52, buf);


    }


}


static void draw_data_rows(void)


{


    // ÖĞÏß£¨´ó×Ö£©


    fill_rect(0, 66, 320, 44, PANEL_BG);


    paint_label(UI_LB_CENTER, 8, 76, PANEL_WHITE);


    if (line_out_center < 0) paint_label(UI_ST_ARM, 60, 76, PANEL_AMBER);


    else                     draw_digits((unsigned)line_out_center, 60, 68, PANEL_CYAN, 1);


    /* ¡ïv083£ºÒ£¿Ø×´Ì¬ + ËÙ¶ÈËõ·Å£¨ÖĞÏßĞĞÓÒ²à£©¡ï */


    {


        char rcbuf[16];


        sprintf(rcbuf, "RC:%s", (uart_receiver.channel[0] != 0u) ? "OK" : "--");


        ips200_set_color((uart_receiver.channel[0] != 0u) ? PANEL_GREEN : PANEL_AMBER, PANEL_BG);


        ips200_show_string(232, 74, rcbuf);


        if (uart_receiver.channel[3] >= 1500u)


            sprintf(rcbuf, "K%3d%%", (int)line_steer_kp_scale_pct);


        else


            sprintf(rcbuf, "S%3d%%", (int)line_speed_scale_pct);


        ips200_set_color(PANEL_CYAN, PANEL_BG);


        ips200_show_string(232, 92, rcbuf);


    }





    // Æ«²î + Æ«²îÖ¸Ê¾Ìõ£¨ÖĞĞÄ=0£¬ÏòÓÒ/Ïò×óÑÓÉì£¬Ö±¹Û¿´³µÆ«ÄÄ±ß£©


    fill_rect(0, 112, 320, 32, PANEL_BG);


    paint_label(UI_LB_DEV, 8, 118, PANEL_WHITE);


    {


        int cx = 232, half = 52, off;


        draw_signed_float(line_out_deviation, 60, 116, PANEL_WHITE);


        off = (int)(line_out_deviation * (float)half);


        if (off >  half) off =  half;


        if (off < -half) off = -half;


        fill_rect((uint16)(cx - half), 122, (uint16)(half * 2), 10, 0x2104);   /* ¹ìµÀµ×É« */


        fill_rect((uint16)(cx - 1), 122, 2, 10, PANEL_WHITE);                  /* ÖĞĞÄ¿Ì¶È */


        if (off >  0) fill_rect((uint16)cx, 122, (uint16)off, 10, PANEL_CYAN);


        else if (off < 0) fill_rect((uint16)(cx + off), 122, (uint16)(-off), 10, PANEL_AMBER);


        /* off == 0 Ê±²»»­£¨¿í¶È 0 »á¿¨Çı¶¯£© */


    }





    // ¶æ»ú / ±àÂëÆ÷²âËÙ£¨Ô­¡°µç»ú¡±¸ñ¸ÄÏÔÊ¾±àÂëÆ÷Æ½¾ùËÙ¶È£©


    fill_rect(0, 144, 320, 32, PANEL_BG);


    paint_label(UI_LB_SERVO, 8, 150, PANEL_WHITE);


    draw_signed_int(line_out_servo_duty, 60, 148, PANEL_CYAN);


    paint_label(UI_LB_MOTOR, 160, 150, PANEL_WHITE);


    {


        int a = line_enc5 < 0 ? -line_enc5 : line_enc5;


        int b = line_enc6 < 0 ? -line_enc6 : line_enc6;


        unsigned spd = (unsigned)((a + b) / 2);


        if (spd > 9999u) spd = 9999u;                 /* ÏŞ·ù£¬·ÀÔ½½ç */


        draw_digits(spd, 212, 148, PANEL_WHITE, 0);


    }





    // ãĞÖµ / ºÚµã / ¶ªÏß


    fill_rect(0, 176, 320, 30, PANEL_BG);


    paint_label(UI_LB_TH, 8, 182, PANEL_WHITE);


    draw_digits((unsigned)img_threshold, 52, 180, PANEL_WHITE, 0);


    paint_label(UI_LB_BLACK, 112, 182, PANEL_WHITE);


    draw_digits(line_out_black_pixels, 156, 180, PANEL_WHITE, 0);


    paint_label(UI_LB_LOST, 212, 182, PANEL_WHITE);


    draw_digits(line_out_lost_count, 256, 180, PANEL_AMBER, 0);


}


static void draw_hint_row(void)


{


    fill_rect(0, 206, 320, 30, PANEL_BG);


    paint_label(UI_HINT, (uint16)((320 - line_labels[UI_HINT].width) / 2), 210, PANEL_AMBER);


}


/* ¼ÇÂ¼½çÃæ£ºÏÔÊ¾¼ÇÂ¼×´Ì¬¡¢Ö¡Êı¡¢ÖĞÏß/ËÙ¶È£¬ÒÔ¼°°´¼üÌáÊ¾ */


static void draw_rec_page(void)


{


    paint_label(UI_LB_REC, (uint16)((320 - line_labels[UI_LB_REC].width) / 2), 2, PANEL_CYAN);





    fill_rect(0, 44, 320, 36, PANEL_BG);


    if (rec_active)                    paint_label(UI_REC_ON,   20, 48, PANEL_GREEN);


    else if (rec_frames >= TRACE_MAX)  paint_label(UI_REC_FULL, 20, 48, PANEL_AMBER);


    else                               paint_label(UI_REC_OFF,  20, 48, PANEL_WHITE);





    fill_rect(0, 88, 320, 36, PANEL_BG);


    paint_label(UI_LB_FRAMES, 20, 96, PANEL_WHITE);


    {


        uint16 x = 110;


        x += draw_digits(rec_frames, x, 92, PANEL_CYAN, 0);


        x += paint_label(UI_SLASH, (uint16)(x + 4), 92, PANEL_WHITE);


        x += draw_digits(TRACE_MAX, (uint16)(x + 4), 92, PANEL_WHITE, 0);


    }





    fill_rect(0, 132, 320, 32, PANEL_BG);


    paint_label(UI_LB_CENTER, 20, 140, PANEL_WHITE);


    if (line_out_center < 0) paint_label(UI_ST_ARM, 74, 140, PANEL_AMBER);


    else                     draw_digits((unsigned)line_out_center, 74, 136, PANEL_CYAN, 0);


    paint_label(UI_LB_SPEED, 184, 140, PANEL_WHITE);


    {


        int a = line_enc5 < 0 ? -line_enc5 : line_enc5;


        int b = line_enc6 < 0 ? -line_enc6 : line_enc6;


        unsigned spd = (unsigned)((a + b) / 2);


        if (spd > 9999u) spd = 9999u;                 /* ÏŞ·ù£¬·À 5 Î»ÊıÔ½½ç */


        draw_digits(spd, 232, 136, PANEL_WHITE, 0);


    }





    fill_rect(0, 186, 320, 54, PANEL_BG);


    paint_label(UI_KEY_START, 24, 190, PANEL_AMBER);


    paint_label(UI_KEY_BACK,  24, 216, PANEL_AMBER);


}





/* ================= ²¨ĞÎÒ³£¨°´ 20.6 ÇĞ»»µ½ÕâÀï£© =================


 * Ö±½Ó»Ø´ğ¡°Ïà»úµ½µ×¿´µ½ÁËÊ²Ã´¡±£º


 *   ÉÏ°ë ¡ª¡ª CCD »Ò¶ÈÇúÏß£¨128 µã£¬Öù¸ß = ÁÁ¶È£©


 *   »ÆÏß ¡ª¡ª ¶şÖµ»¯ãĞÖµ£»  ÇàÏß ¡ª¡ª µ±Ç°ÖĞÏßÎ»ÖÃ


 *   ÏÂÌõ ¡ª¡ª ¶şÖµ»¯½á¹û£ºÂÌ = °×(ÈüµÀ)  ºì = ºÚ(·ÇÈüµÀ)


 * ¿´¡°»·¡±µÄ¹Ø¼ü£ºÈç¹û³öÏÖ¡°°× | °µ | °×¡±Èı¶Î£¬ÕâÀï»áÖ±½ÓÏÔÊ¾³öÀ´£»


 * ÇàÉ«ÖĞÏßÓĞÃ»ÓĞÕæµÄ¡°Ìø¶Î¡±Ò²Ò»ÑÛ¿É¼û¡£ */


#define WV_X   8


#define WV_W   304


#define WV_GY  68


#define WV_GH  112


#define WV_BY  182


#define WV_BH  20





static void draw_wave_page(void)


{


    int band, r, cx, px, gy, bar, row_th, marker;


    uint16 color;





    paint_label(UI_LB_WAVE, (uint16)((320 - line_labels[UI_LB_WAVE].width) / 2), 2, PANEL_CYAN);





    /* Êı¾İĞĞ£ºÖĞÏß / ãĞÖµ / °×¿í£¨°×¿íÊÇµ÷Ô²»·ÅĞ¾İµÄ¹Ø¼ü¶ÁÊı£© */


    fill_rect(0, 34, 320, 28, PANEL_BG);


    paint_label(UI_LB_CENTER, 8, 40, PANEL_WHITE);


    if (line_out_center < 0) paint_label(UI_ST_ARM, 56, 40, PANEL_AMBER);


    else                     draw_digits((unsigned)line_out_center, 56, 36, PANEL_CYAN, 0);


    paint_label(UI_LB_TH, 118, 40, PANEL_WHITE);


    draw_digits((unsigned)img_threshold, 158, 36, PANEL_WHITE, 0);


    paint_label(UI_LB_W, 228, 40, PANEL_WHITE);


    draw_digits((unsigned)line_out_white, 256, 36, PANEL_GREEN, 0);   /* 256+3Î»60=316£¬·ÀÔ½½ç */





    marker = line_out_center;            /* CCD ÏñËØ×ø±ê 0..127£»-1=ÎŞÏß */


    row_th = (img_threshold > 0)    ? (WV_GH - 1 - (int)img_threshold * WV_GH / 255) : -1;





    /* »Ò¶ÈÇúÏß */


    for (band = 0; band < WV_GH; band += 4)


    {


        for (r = 0; r < 4; ++r)


        {


            gy = band + r;


            for (cx = 0; cx < WV_W; ++cx)


            {


                px = cx * 128 / WV_W;


                bar = (int)base_image[0][px] * WV_GH / 255;


                color = (gy >= WV_GH - bar) ? 0x7bef : PANEL_BG;


                if (row_th >= 0 && gy == row_th) color = PANEL_AMBER;


                if (marker >= 0 && px >= marker - 1 && px <= marker + 1) color = PANEL_CYAN;


                panel_rows[r * WV_W + cx] = color;


            }


        }


        ips200_show_rgb565_image(WV_X, (uint16)(WV_GY + band), panel_rows, WV_W, 4, WV_W, 4, 0);


    }





    /* ¶şÖµ»¯½á¹ûÌõ */


    for (band = 0; band < WV_BH; band += 4)


    {


        for (r = 0; r < 4; ++r)


        {


            for (cx = 0; cx < WV_W; ++cx)


            {


                px = cx * 128 / WV_W;


                color = (image[0][px] == 255) ? PANEL_GREEN : PANEL_RED;


                if (marker >= 0 && (px == marker || px == marker - 1)) color = PANEL_CYAN;


                panel_rows[r * WV_W + cx] = color;


            }


        }


        ips200_show_rgb565_image(WV_X, (uint16)(WV_BY + band), panel_rows, WV_W, 4, WV_W, 4, 0);


    }





    fill_rect(0, 210, 320, 30, PANEL_BG);


    paint_label(UI_KEY_BACK, (uint16)((320 - line_labels[UI_KEY_BACK].width) / 2), 214, PANEL_AMBER);


}





/* ¡ïv067£ºÉãÏñÍ·Ô¤ÀÀÒ³£¨P20.6 ÇĞµ½£¬ui_page==3£©¡ï


 * Ä¿µÄ£ºÒ»ÑÛÅĞ¶Ï"³µÂÒÅÜ"ÊÇ¡¾Í¼Ïñ·½Ïò/ÆØ¹âÎÊÌâ¡¿»¹ÊÇ¡¾ÖĞÏßËã·¨ÎÊÌâ¡¿¡£


 * ×óÉÏ = ÉãÏñÍ·»Ò¶ÈÍ¼£¨Ô­³ß´ç 188x120£©£»»Æµã = ¸ú×Ù³öµÄÖĞÏß£»ÓÒ²à = Êı¾İ */


static const uint16 cam_dot2[4] = { RGB565_YELLOW, RGB565_YELLOW,


                                    RGB565_YELLOW, RGB565_YELLOW };


static void draw_cam_page(void)


{


    uint32 y;





    ips200_show_gray_image(0, 0, cam_saved_valid ? (const uint8 *)&line_trace[0][0] : &cam_gray[0][0], CAM_W, CAM_H, CAM_W, CAM_H, 0);
    ips200_show_string(192, 200, cam_saved_valid ? "SAVED P20.7" : (rec_active ? "REC P20.7 " : "LIVE P20.7 "));
    ips200_show_string(192, 180, big_ring_enabled ? "BIG ON " : "BIG OFF");
    ips200_show_uint(270, 180, big_ring_phase, 1);





    for (y = 0; y < CAM_H; y++)


    {


        if ((cam_saved_valid ? cam_saved_mid[y] : cam_mid[y]) >= 0)


            ips200_show_rgb565_image((uint16)(cam_saved_valid ? cam_saved_mid[y] : cam_mid[y]), (uint16)y,


                                     cam_dot2, 2, 2, 2, 2, 0);


    }





    ips200_set_color(RGB565_WHITE, RGB565_BLACK);


    ips200_show_string(192, 0,   "== CAM ==");


    ips200_show_string(192, 20,  line_cam_ok ? "CAM: OK  " : "CAM: FAIL");


    ips200_show_string(192, 40,  cam_ring_hole ? "R: HOLE  " :


                                 (cam_ring_wide ? "R: WIDE  " : "R: ----  "));


    ips200_show_string(192, 60,  "rows:");


    ips200_show_uint(236, 60, cam_mid_rows, 3);


    ips200_show_string(192, 80,  "midN:");


    ips200_show_int(236, 80, cam_mid_near, 4);


    ips200_show_string(192, 100, "devN:");


    ips200_show_int(236, 100, cam_dev_near, 5);


    ips200_show_string(192, 120, "thr:");


    ips200_show_uint(228, 120, cam_thr, 3);


    /* ¡ïv105£º´ó½ò·¨¼¶±ğ¡£L1=Ö»ÅÜÁËÒ»¼¶ãĞÖµ£»L2=¶ÔÁÁÀàÓÖÅÜÁËÒ»´Î Otsu£¬
     * ÒÑ°Ñ"À¶µ×"ºÍ"°×É«ÅÜµÀ"·Ö¿ª£¨À¶°×ÈüµÀ¹ıÂ·¿Ú/¿´Ô¶·½Ê±Õı³£Ó¦Îª L2£©¡£ */
    ips200_show_string(252, 120, "L");


    ips200_show_uint(264, 120, cam_otsu_level, 1);


    ips200_show_string(192, 136, "fps");


    ips200_show_uint(216, 136, (uint32)(line_cam_fps_x10 / 10), 2);


    /* ¡ïv105£º¶şÖµ»¯ºó°×ÏñËØÕ¼±È(%)¡£Õı³£À¶°×ÈüµÀ ¡Ö35~55£»½Ó½ü 100 ËµÃ÷ÕûÖ¡
     * Ã»ÓĞºÚ±ß½ç£¨³å³öÈüµÀ/¶Ô°×Ç½£©£¬´ËÊ±ÏÂÓÎ»á×ß all_white ¶µµ×¡£ */
    ips200_show_string(240, 136, "wp");


    ips200_show_uint(264, 136, (uint32)cam_bin_white * 100u / (uint32)(CAM_W * CAM_H), 3);


    ips200_show_string(192, 152, "F");


    ips200_show_uint(204, 152, (uint32)line_cam_finish, 1);


    ips200_show_string(216, 152, "L");


    ips200_show_uint(228, 152, (uint32)line_cam_lost, 1);


    ips200_show_string(240, 152, "K");


    ips200_show_uint(252, 152, (uint32)line_cam_locked, 1);


    ips200_show_string(192, 168, "rnt");


    ips200_show_uint(228, 168, line_cam_reinit_n, 3);


    /* ¡ïv105£ºS=1 ±íÊ¾¶şÖµ»¯ºó°×Õ¼±È >72%£¬ºÜ¿ÉÄÜÕûÖ¡²»ÊÇÈüµÀ£¨Õï¶ÏÓÃ£©¡£ */
    ips200_show_string(252, 168, "S");


    ips200_show_uint(264, 168, cam_bin_suspect, 1);


    ips200_show_string(192, 184, "fr");


    ips200_show_uint(216, 184, (uint32)ccd_frames, 5);


    ips200_show_string(192, 200, "P20.6 next");
    ips200_show_string(192, 216, "av");
    ips200_show_uint(204, 216, cam_gavg, 3);
    ips200_show_string(224, 216, "mx");
    ips200_show_uint(236, 216, cam_gmax, 3);


}





static void screen_all(uint8 st)


{


    fill_rect(0, 0, 320, 240, PANEL_BG);


    if (ui_page == 3u)


    {


        draw_cam_page();       /* ¡ïv067£ºÉãÏñÍ·Ô¤ÀÀÒ³¡ï */


    }


    else if (ui_page == 2u)


    {


        draw_rec_page();


    }


    else if (ui_page == 1u)


    {


        draw_wave_page();


    }


    else


    {


        paint_label(UI_TITLE, (uint16)((320 - line_labels[UI_TITLE].width) / 2), 2, PANEL_CYAN);


        draw_state_row(st);


        draw_data_rows();


        draw_hint_row();


    }


    last_draw = now_ticks();


}





// ------------------------------ °´¼ü / ±¨¸æ ------------------------------


/* Í¨ÓÃ°´¼üÏû¶¶£º·µ»Ø 1 ±íÊ¾Ò»´Î¡°°´ÏÂ²¢ËÉ¿ª¡±¡£pin ÓÉµ÷ÓÃÕßÖ¸¶¨£¬Ö§³Ö¶à¸ö°´¼ü¡£ */


static uint8 key_event(KeyState *k, uint16 pin, uint32 now)


{


    uint8 raw = (uint8)gpio_get_level(pin);


    if (raw != k->last) { k->last = raw; k->change = now; }


    if (raw != k->stable && age_ms(now, k->change) >= KEY_DEBOUNCE_MS)


    {


        k->stable = raw;


        if (!raw) { k->pressed = 1u; k->down_ms = now; }


        else if (k->pressed) { k->pressed = 0u; return (age_ms(now, k->down_ms) >= KEY_LONG_MS) ? 2u : 1u; }


    }


    return 0u;


}


static void report_publish(uint32 now, uint8 state)


{


    report_sequence += 2u;


    line_senior_report.sequence = report_sequence - 1u;


    line_senior_report.sequence_end = report_sequence - 1u;


    line_senior_report.signature       = LINE_REPORT_SIGNATURE;


    line_senior_report.version         = 1u;


    line_senior_report.size            = sizeof(LineSeniorReport);


    line_senior_report.uptime_ms       = age_ms(now, boot_tick);


    line_senior_report.loop_count      = loop_count;


    line_senior_report.state           = state;


    line_senior_report.center          = (line_out_center < 0) ? 0xFFFFFFFFu : (unsigned)line_out_center;


    line_senior_report.deviation_milli = (unsigned)(int)(line_out_deviation * 1000.0f);


    line_senior_report.steering_milli  = (unsigned)(int)(line_out_steering * 1000.0f);


    line_senior_report.servo_duty      = (line_out_servo_duty < 0) ? 0u : (unsigned)line_out_servo_duty;


    line_senior_report.left_speed      = (line_out_left_speed < 0) ? 0u : (unsigned)line_out_left_speed;


    line_senior_report.right_speed     = (line_out_right_speed < 0) ? 0u : (unsigned)line_out_right_speed;


    line_senior_report.lost_count      = line_out_lost_count;


    line_senior_report.threshold       = (unsigned)img_threshold;


    line_senior_report.black_pixels    = line_out_black_pixels;


    line_senior_report.key_presses     = key_presses;


    line_senior_report.enc5            = (unsigned)(int)line_enc5;


    line_senior_report.enc6            = (unsigned)(int)line_enc6;


    line_senior_report.gyro_z          = (unsigned)(int)line_gyro_z;


    line_senior_report.ring_mode       = (unsigned)ring_out_mode;


    line_senior_report.ring_dir        = (unsigned)(int)ring_out_dir;


    line_senior_report.ring_frames     = (unsigned)ring_out_frames;


    line_senior_report.ring_hole       = (unsigned)(int)ring_out_hole;


    line_senior_report.ring_sig        = (unsigned)ring_out_sig;


    line_senior_report.ring_phase      = (unsigned)ring_out_phase;


    line_senior_report.ring_width      = (unsigned)ring_out_width;


    line_senior_report.segs            = (unsigned)line_out_segs;


    line_senior_report.gap             = (unsigned)line_out_gap;


    line_senior_report.white           = (unsigned)line_out_white;


    line_senior_report.bigring         = (unsigned)bigring_out_state;


    line_senior_report.bigring_peak    = (unsigned)bigring_peak;


    line_senior_report.bigring_cross   = (unsigned)bigring_cross;


    line_senior_report.speed_target    = (unsigned)line_out_speed_target;


    line_senior_report.imu_ok          = (unsigned)line_imu_ok;


    line_senior_report.gyro_bias       = (unsigned)(int)line_gyro_bias;


    line_senior_report.gyro_ready      = (unsigned)line_gyro_ready;


    line_senior_report.gyro_f          = (unsigned)(int)line_gyro_f;


    line_senior_report.odometer        = (unsigned)line_odometer;


    line_senior_report.stall           = (unsigned)line_stall;


    line_senior_report.acc_x           = (unsigned)(int)line_acc_x;


    line_senior_report.acc_y           = (unsigned)(int)line_acc_y;


    line_senior_report.acc_z           = (unsigned)(int)line_acc_z;


    line_senior_report.enc_peak        = (unsigned)line_enc_peak;


    line_senior_report.enc_avg         = (unsigned)(int)line_enc_avg;


    line_senior_report.cring_cnt       = (unsigned)bw_cnt;   /* v059£º»·¿Ú¼ÆÊı(Õï¶Ï) */


    line_senior_report.cring_w         = (unsigned)bw_latch_odo;  /* v060£ºÉÏÒ»¸ö»·¿ÚµÄÀï³Ì(Õï¶Ï) */


    line_senior_report.cring_in        = (unsigned)line_odo_run;   /* v050£ºÆğÅÜÀï³Ì(Õï¶Ï) */


    line_senior_report.yaw10           = (unsigned)(int)line_yaw10;


    line_senior_report.sequence_end    = report_sequence;


    line_senior_report.sequence        = report_sequence;


}


static void count_black(void)


{


    int i, bl = 0;


    for (i = 0; i < 128; i++) if (image[0][i] == 0) bl++;


    line_out_black_pixels = (unsigned)bl;


}





int core0_main(void)


{


    /* v082£ºstate ÒÑÌáÉıÎªÎÄ¼ş¼¶ static£¨¼û±äÁ¿Çø£©£¬´Ë´¦²»ÔÙ¶¨Òå */


    uint16 good_frames = 0;


    uint32 now;
    uint8 frame_new;





    clock_init();
    DRV8701_Init_D();


    line_boot_stage = 1;


    debug_init();                   // ³õÊ¼»¯Ä¬ÈÏµ÷ÊÔ´®¿Ú





    gpio_init(RUN_KEY,   GPI, GPIO_HIGH, GPI_PULL_UP);   // Æô¶¯/¼±Í£


    gpio_init(SERVO_KEY, GPI, GPIO_HIGH, GPI_PULL_UP);   // ¶æ»úËÉ/Ëø


    gpio_init(REC_KEY,   GPI, GPIO_HIGH, GPI_PULL_UP);   // ¼ÇÂ¼¿ª¹Ø


    gpio_init(PAGE_KEY,  GPI, GPIO_HIGH, GPI_PULL_UP);   // ¼ÇÂ¼½çÃæÇĞ»»


    line_boot_stage = 2;





    // ±àÂëÆ÷£¨´¿¶¨Ê±Æ÷£¬²»Õ¼ SPI£¬ÓëÆÁÄ»ÎŞ³åÍ»£©


    encoder_dir_init(TIM5_ENCODER, TIM5_ENCODER_CH1_P10_3, TIM5_ENCODER_CH2_P10_1);


    encoder_dir_init(TIM6_ENCODER, TIM6_ENCODER_CH1_P20_3, TIM6_ENCODER_CH2_P20_0);


    encoder_clear_count(TIM5_ENCODER);


    encoder_clear_count(TIM6_ENCODER);


    line_boot_stage = 3;


    /* ¡ïv066£ºCCD ÍËÒÛ ¡ú 2D ÉãÏñÍ·£¨×Ü×ê·ç MT9V03X£©¡ï


       Ë³Ğò£ºÆÁÄ»ÏÈ ¡ú ÉãÏñÍ·ºó£¨Í¬ IMU µÄÔ¼¶¨£»ÆÁÄ»ÓëÉãÏñÍ·¶¼ÓĞ SPI/Òı½Å×ÊÔ´£©¡£


       ×¢Òâ£ºCCD ±ØĞëÎïÀí°Îµô£¨TSL1401 ÓÃ P00.0/P00.1£¬ÓëÉãÏñÍ·Êı¾İ D0/D1 ³åÍ»£©¡£ */


    ips200_set_dir(IPS200_CROSSWISE); ips200_init(IPS200_TYPE_SPI);  // ÆÁÄ»ÏÈ


    line_boot_stage = 4;


    {


        uint8 cam_retry = 0;


        while (mt9v03x_init())


        {


            system_delay_ms(100);


            if (++cam_retry >= 30u) break;             /* Ê§°ÜÒ²²»¿¨ËÀ£¬ÆÁÄ»ÉÏÄÜ¿´µ½ */


        }


        line_cam_ok = (cam_retry >= 30u) ? 0 : 1;


        line_boot_stage = 5;


    }





    /* IMU660RA£¨v029£©£º¡ïÊ±ĞòĞŞÕı¡ï ¡ª¡ª Å²µ½¡°ÆÁÄ»³õÊ¼»¯Ö®ºó¡±£¬


       ÓëÒÑÑéÖ¤µÄ Codex IMUCheck ¹¤³ÌÍêÈ«Ò»ÖÂ£¨ËüµÄË³ĞòÊÇ£ºÆÁÄ»ÏÈ ¡ú IMU ºó£¬


       ÇÒ³É¹¦¶Áµ½ chip_id=0x24£©¡£´ËÇ°µÄ¡°IMU ÏÈ¡¢ÆÁÄ»ºó¡±Ë³ĞòÒÉÎª»¨ÆÁ/Ê§ÁªÓÕÒò¡£


       - SPI Ä£Ê½£¨µ±Ç°£©£º¿â×Ô¼º¹ÜÀí CS£¬ÎŞĞèÊÖ¶¯ÉèµçÆ½£»


       - Èí IIC Ä£Ê½£¨±¸Ñ¡£©£ºÊÖ¶¯°Ñ CS(P20_13)/SDO(P20_12) À­¸ß


         £¨CS¡úÑ¡Ôñ IIC ½Ó¿Ú£»SDO¡ú¸ßµçÆ½Ê¹ IIC µØÖ· = 0x69£©¡£


       Ê§°Ü×Ô¶¯½µ¼¶£¨line_imu_ok=0£©£¬²»Ó°ÏìÑ­¼£/ËÙ¶È±Õ»·¡£ */


#if IMU660RA_USE_SOFT_IIC


    gpio_init(IMU660RA_CS_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);


    gpio_init(P20_12,          GPO, GPIO_HIGH, GPO_PUSH_PULL);


    system_delay_ms(5);


#endif


    {


        /* ÁÙ½çÇø£ºCCD ÖĞ¶ÏÃ¿ 10ms ÅÜ ~1.8ms£¬ÈôÇĞËé IMU µÄ SPI ÅäÖÃÊÂÎñ


           »áÔì³É½Ó¿ÚÊ§²½£¨Ïê¼û 20ms ¿é×¢ÊÍ£©¡£ */


        uint32 pm0 = interrupt_global_disable();


        line_imu_ok = (imu660ra_init() == 0) ? 1 : 0;


        interrupt_global_enable(pm0);


    }


    DRV8701_Init_D();


    line_boot_stage = 7;


    servo_init();                                      // ¶æ»ú³õÊ¼»¯


    servo_control('C', 0);                             // ÉÏµç¶æ»ú»ØÖĞÎ»





    ticks_per_ms = IfxStm_getFrequency(&MODULE_STM0) / 1000u;


    boot_tick = now_ticks();


    k_run.last   = k_run.stable   = (uint8)gpio_get_level(RUN_KEY);


    k_servo.last = k_servo.stable = (uint8)gpio_get_level(SERVO_KEY);





        /* ¡ïv040£º»Ö¸´Ñ§³¤£¨¹úÈü£©Ô­Öµ Kp=30¡ï


       ÎÒÃÇÖ®Ç°°ÑËü½µµ½ 15 À´"·À¹ı³å"£¬´ú¼ÛÊÇ×ªÍäÊ±¶æÁ¿²»¹»¡¢Ò»Ö±ÂÔ³öµÀ¡£


       ²éÖ¤Ñ§³¤Ô­°æ cpu0_main.c È·Îª PID_Init(&pid_steering, 30, 0.1, 15, 100, 50)¡£


       ×¢Òâ£º×ªÏò PID Êµ¼ÊÖ»ÓÃ P¡¢D Á½Ïî£¨»ı·ÖÎ´Ê¹ÓÃ£©¡£ */


    PID_Init(&pid_steering, 30.0f, 0.1f, 20.0f, 100.0f, 10.0f);   /* ¡ïv092£ºKd 15¡ú20£¨ÏàÎ»³¬Ç°¼ÓÇ¿£»Kp ·Ö¶Î¼û PID.c£©¡ï */   /* ¡ïv090£º50¡ú10 Á¿¸Ù¶ÔÆë£¨Âú¶æ=|out|¡Á10=100 ¡ú ÓĞĞ§Á¿³Ì=10£»Ô­ 50 ÓĞ 80% ËÀÇø£©¡ï */


    PID_Init(&pid_speed,    1.0f, 0.01f, 0.05f, 50.0f, 10000.0f);


    /* v083£ºÒ£¿Ø½ÓÊÕ»úÆôÓÃ£¨Ò£¿Ø·¢³µ/¼±Í£/µ÷ËÙ£»½ÓÊÕ»ú TX -> P10.6 / UART2£© */


    uart_receiver_init();





    RingDiagReset(); RingDiagFreeze(0u);


    screen_all(state);





    pit_ms_init(CCU60_CH0,1u);
    cpu_wait_event_ready();         // µÈ´ıËùÓĞºËĞÄ³õÊ¼»¯Íê±Ï





    while (TRUE)


    {
        frame_new=0;TrackHeartbeat();
        if(state==ST_RUN&&track_fault){
            line_ring_armed=0;DRV_8701_MOTOR_DRIVER_D(0,0);
            state=track_fault==VT_FINISH?ST_FIN:ST_STOP;
            RingDiagFreeze(track_fault==VT_FINISH?3u:2u);
            screen_all(state);
        }



        now = now_ticks();


        line_boot_stage = 10; line_loop_count++;


        loop_count++;





        rec_ev = key_event(&k_rec, REC_KEY, now);
        if (rec_ev != 0u || (rc_rec_req != 0u))   /* ¡ïv096£ºP20.7 »ò Ò£¿ØCH1´òµ½µ× ¡ï */


        {


            rc_rec_req = 0u;                                            /* ¡ïv096£ºÏû·ÑÒ£¿ØÇëÇó ¡ï */


            if (rec_ev == 2u) {              /* v116: long press = save/clear one frame (stop rec first) */
                rec_active = 0u; rec_frames = 0u; line_trace_count = 0u;
                if (cam_saved_valid || cam_saved_request) {
                    cam_saved_request = 0u; cam_saved_valid = 0u; cam_saved_auto = 0u;
                } else { cam_saved_request = 1u; cam_saved_auto = 0u; }
            }
            else if (rec_active) { rec_active = 0u; }
            else { cam_saved_valid = 0u; cam_saved_request = 0u; cam_saved_auto = 0u; rec_frames = 0u; line_trace_count = 0u; rec_last_frame = ccd_frames; rec_active = 1u; }  /* ÖØĞÂ¿ªÊ¼ */


            screen_all(state);


        }





        page_ev = key_event(&k_page, PAGE_KEY, now);
        if (page_ev == 2u)
        {
            big_ring_enabled = big_ring_enabled ? 0u : 1u;   /* v122 (CQU-style): long press toggles ring module */
            track_epoch++;
            screen_all(state);
        }
        else if (page_ev == 1u)        // 20.6£ºÑ­»·ÇĞ»»Ò³Ãæ


        {


            ui_page = (uint8)((ui_page + 1u) % 4u);  /* v067: +cam page */


            screen_all(state);


        }





        if (key_event(&k_servo, SERVO_KEY, now))      /* v116: restore plain servo release/lock; camera-page big-ring toggle removed */
        {
            servo_released = servo_released ? 0u : 1u;


            if (servo_released) servo_release();


            else                servo_control('C', 0);


            screen_all(state);


        }
        /* ¡ïv084£ºÒ£¿ØÊÂ¼ş£¨CH3=·¢³µ/¼±Í££»CH5/CH6=µ÷²Î£»CH4 ÉÏµµ=×ªÏòK£¬ÖĞ/ÏÂ=ËÙ¶ÈS£©¡ï */


        {


            uint16 c3 = uart_receiver.channel[2];


            uint16 c5 = uart_receiver.channel[4];


            uint16 c6 = uart_receiver.channel[5];


            uint16 c4 = uart_receiver.channel[3];


            uint16 c1 = uart_receiver.channel[0];


            /* ¡ïv086£ºÒ£¿ØÔÚÏßÅĞ¶¨£¨Ö¡ĞÂÏÊ¶È 500ms£©+ ÉÏÏßÊ×Ö¡Ö»Í¬²½¡ª¡ª·À"¿ª»ú×Ô¶¯·¢³µ"¡ï */


            if (uart_receiver.finsh_flag != 0u)


            {


                TrackRemoteSeen(uart_receiver.state);
                uart_receiver.finsh_flag = 0u;      /* Ïû·ÑÒ»´Î£º±íÊ¾×î½üÓĞĞÂÖ¡ */


                rc_ever = 1u;


                rc_last_rx = now;


            }


            if (rc_ever && (age_ms(now, rc_last_rx) < 500u))


            {


                if (!rc_sync) { rc_sync = 1u; rcb_prev1 = c1; }  /* ÉÏÏßÊ×Ö¡£ºÖ»¼Ç»ù×¼£¨º¬CH1£©£¬²»ÅĞ±ßÑØ */


                else


                {


                    /* ¡ïv096£ºCH1 Ò¡¸Ë´òµ½µ×£¨ÈÎÒâÒ»±ß£©= ¿ªÊ¼/Í£Ö¹¼ÇÂ¼£¨µÈĞ§ P20.7£©¡ï */


                    if (((c1 >= 1800u) && (rcb_prev1 < 1800u)) || ((c1 <= 200u) && (rcb_prev1 > 200u)))


                    { rc_rec_req = 1u; }


                    if ((c3 >= 992u) && (rcb_prev3 < 992u)) { rc_run_req = 1u; }


                    if ((c5 >= 992u) && (rcb_prev5 < 992u))


                    {


                        if (c4 >= 1500u)                /* CH4 ÉÏµµ£º×ªÏòÁ¦¶È K */


                        {


                            line_steer_kp_scale_pct -= 5;


                            if (line_steer_kp_scale_pct < 70) line_steer_kp_scale_pct = 70;


                        }


                        else                            /* CH4 ÖĞ/ÏÂ£ºËÙ¶È S */


                        {


                            line_speed_scale_pct -= 5;


                            if (line_speed_scale_pct < 50) line_speed_scale_pct = 50;


                        }


                    }


                    if ((c6 >= 992u) && (rcb_prev6 < 992u))


                    {


                        if (c4 >= 1500u)


                        {


                            line_steer_kp_scale_pct += 5;


                            if (line_steer_kp_scale_pct > 130) line_steer_kp_scale_pct = 130;


                        }


                        else


                        {


                            line_speed_scale_pct += 5;


                            if (line_speed_scale_pct > 200) line_speed_scale_pct = 200;


                        }


                    }


                }


                rcb_prev3 = c3; rcb_prev5 = c5; rcb_prev6 = c6; rcb_prev1 = c1;


            }


            else


            {


                rc_sync = 0u;                       /* ÀëÏß/Î´ÊÕµ½Ö¡£ºÖØĞÂÎä×°£¨»Ö¸´ºóÊ×Ö¡ÔÙÍ¬²½£© */


            }


        }


        if (key_event(&k_run, RUN_KEY, now) || (rc_run_req != 0u))


        {


            TrackRemoteRequired(rc_run_req != 0u);
            rc_run_req = 0u;


            key_presses++;


            switch (state)


            {


                case ST_IDLE: state = ST_ARM;  good_frames = 0; break;   // ½øÈëµÈ´ıºÚÏß


                case ST_ARM:  state = ST_IDLE; break;                    // È¡Ïû


                case ST_RUN:  RingDiagFreeze(1u); DRV_8701_MOTOR_DRIVER_D(0, 0);             // ¼±Í£


                              line_ring_armed = 0;


                              RingReset();


                              servo_control('C', 0);


                              state = ST_STOP; break;


                default:      state = ST_IDLE; break;                    // Í£³µ/ÖÕµãºóÖØÖÃ


            }


            screen_all(state);


        }





        /* ¡ïv066£º2D ÉãÏñÍ·²É¼¯£¨Ìæ´ú CCD£©¡ï


           1) µÈÒ»Ö¡Íê³É£¨×î¶à 40ms£¬³¬Ê±ÔòÑØÓÃÉÏÒ»Ö¡£©


           2) cam_process£º¸´ÖÆ + Ostu + ¶şÖµ


           3) cam_line_all£ºÖğĞĞ°×¶Î + ÖĞÏß¸ú×Ù + ½ü/Ô¶Æ«²î + »·ÌØÕ÷


           4) ¡ïÇÅ½Ó¡ï£º°Ñ 2D µÄ"½ü´¦ÖĞÏß"ºÏ³ÉÎªµÈĞ§µÄ 1D Í¼ÏñÎ¹¸ø¾ÉÑ²ÏßÁ´


              £¨ÔÚ mid ¸½½ü ¡À15 ÏñËØ»­°×¶Î£©¡ú CalculateCenterLine »áËã³öÍ¬Ò»¸ö mid£¬


              ÓÚÊÇ PID.c/´ó»·×´Ì¬»úÒ»ĞĞ¶¼²»ÓÃ¸Ä£¬µ«ÖĞÏßÖÊÁ¿´Ó"Ò»ÌõÏß"Éı¼¶Îª"¶àĞĞÈÚºÏ"¡£ */


        {


            /* ¡ï¡ï v068£ºDMA Ö¡Ëø£¨ĞŞ¸´"»­ÃæËºÁÑ/½Ó·ì"£©¡ï¡ï


               À´Ô´£ºCodex Õï¶Ï ¡ª¡ª "ÉÏÒ»Ö¡Íê³É±êÖ¾¿ÉÄÜ»¹ÔÚ£¬ÏÂÒ»Ö¡ DMA ÒÑ¾­¿ªÊ¼


               ¸²¸ÇÍ¬Ò»»º³åÇø"£¨È·ÇĞµÄ¾ºÌ¬£©¡£ĞŞ·¨£º


                 claim  ¡ú Ëø×¡ÕâÒ»Ö¡£¨VSYNC ÖĞ¶Ï²»ÔÙÖØĞÂÎä×° DMA£¬»º³åÇø²»ÔÙ±»¸ÄĞ´£©


                 cam_process ¡ú ÔÚËø±£»¤ÏÂ°ÑÍ¼Ïñ¸´ÖÆµ½ cam_gray£¨¹Ø¼ü£º¸´ÖÆÆÚ¼ä²»±»¸²¸Ç£©


                 release ¡ú ·Å¿ª£¬DMA ¼ÌĞø²ÉÏÂÒ»Ö¡


               Ö®ºó cam_line_all ÔÚ cam_gray/cam_bin£¨ÎÒÃÇ×Ô¼ºµÄ¸±±¾£©ÉÏËã£¬ÎŞĞèËø¡£ */


            /* ¡ïv069£º¾µÏñÉãÏñÍ·ÄÚ²¿±êÖ¾£¨Õï¶ÏÓÃ£©¡ï */


            line_cam_finish    = (int)mt9v03x_finish_flag;


            line_cam_lost      = (int)mt9v03x_lost_flag;


            line_cam_locked    = (int)mt9v03x_frame_locked;


            line_cam_dma_int   = (int)mt9v03x_dma_int_num;


            line_cam_init_flag = (int)mt9v03x_dma_init_flag;


            line_cam_llnum     = (int)mt9v03x_link_list_num;





            /* v070: Á´Â·¹Û²ì¼ÆÊı */


            if (mt9v03x_finish_flag) line_cam_fin_seen++;


            if (mt9v03x_dma_init_flag) line_cam_init_seen++;


            if ((int)mt9v03x_dma_int_num > line_cam_dint_max) line_cam_dint_max = (int)mt9v03x_dma_int_num;





            line_cam_polls++;


            /* *** v075£ºÖ¡»ñÈ¡¸ÄÎª"ĞòºÅ±È¶Ô" ***


               Ô­ claim ·½°¸Êµ²â¼¸ºõ´Ó²»³É¹¦£»¶ø ISR µÄÖ¡Íê³É¼ÆÊı


               g_isr_dma_fin Ò»Ö±Õı³££¨65 ´Î/Ãë£©¡£ĞÂ·½°¸£ºÖ÷Ñ­»·¶Á


               g_isr_dma_fin£¨cpu0_dsram£¬Óë ISR Í¬Çø£©£¬ĞòºÅ±ä»¯ÇÒ²»ÔÚ


               ´«Êä´°¿Ú£¨!g_lost_mirror£©¡ú ÓĞĞÂÖ¡ ¡ú Á¢¿Ì´¦Àí¡£


               ¸Ã·½°¸²»ÊÜ²ÉÑùÏàÎ»Ó°Ïì£ºĞòºÅ±ä»¯±£³Öµ½±»´¦ÀíÎªÖ¹¡£ */


            /* ¡ï¡ï¡ï v076£ºÈ¥µô lost ÃÅ£¨¸ùÒòÈ·ÈÏ£©¡ï¡ï¡ï


               Êµ²âÊı¾İ£º


                 g_isr_dma_fin +64/s£¨DMA Ã¿Ö¡¶¼Íê³É£©


                 ¶ø g_lost_mirror ºãÎª 1¡¢line_frame_changes ½ö 0.6/s


               ¸ùÒò£º±¾ÉãÏñÍ·µÄ DMA ´«ÊäÔÚ¡¾Ò»Ö¡µÄ×îÄ©Î²¡¿Íê³É£¨½ôÌùÏÂÒ»¸ö


                     VSYNC£©£¬ËùÒÔ finish/lost µÄÓĞĞ§´°¿ÚÖ»ÓĞ¼¸Ê®Î¢Ãë¡ª¡ª


                     ¹Ù·½Àı³Ì¿¿"È«ËÙ¿Õ×ªµÄÖ÷Ñ­»·"×²ÉÏËü£¬


                     ÎÒÃÇµÄÖ÷Ñ­»·ÓĞ 1ms ÑÓ³Ù + ¸÷ÖÖÈÎÎñ£¨974Hz£©£¬×²²»ÉÏ¡£


               ĞŞ¸´£ºÓÃµ¥µ÷µİÔöµÄ"Ö¡ĞòºÅ"£¨g_isr_dma_fin£¬Ö»Ôö²»Çå£©×öÎÕÊÖ£º


                     ĞòºÅ±ä»¯ÔÚÄÚ´æÀï¡¾Ò»Ö±±£³Ö¡¿£¬Ö÷Ñ­»·ÈÎºÎÊ±ºò¿´µ½¶¼ÄÜ


                     Á¢¿Ì×¥×¡£¬³¹µ×°ÚÍÑ"¼«Õ­´°¿Ú"ÎÊÌâ¡£


                     »º³åÇø°²È«£ºÖ¡Êı¾İÔÚÍê³ÉºóÔ¼Ò»ÕûÖ¡ÖÜÆÚÄÚ²»»á±»¸ÄĞ´¡£ */


            /* ¡ïv079£º¸ÄÓÃ"¿ìÕÕ¾ÍĞ÷"´¥·¢£¨cam_take_snapshot ÄÚ²¿ÅĞ¶Ï g_snap_seq£©¡£
               È¡µ½µÄÖ¡±ØÀ´×Ô"Íê³ÉÖĞ¶ÏÄÚµÄÍêÕû¿ìÕÕ"£¬²»ÔÙËºÁÑ£»
               g_isr_dma_fin ĞòºÅ¼ÌĞøÓÃÓÚ±¨¸æ/ÏÔÊ¾¡£ */
            if (cam_take_snapshot(cam_gray))


            {


                line_frame_last_seq = g_isr_dma_fin;


                line_frame_changes++;


                cam_process();          /* v079£ºÍ³¼Æ + Ostu + ¶şÖµ£¨cam_gray ÒÑÓÉ¿ìÕÕ¾ÍĞ÷£© */


                cam_line_all();
                if (cam_saved_request) {
                    unsigned si;
                    volatile unsigned char *saved = &line_trace[0][0];
                    for (si = 0u; si < CAM_W * CAM_H; si++) saved[si] = (&cam_gray[0][0])[si];
                    for (si = 0u; si < CAM_H; si++) cam_saved_mid[si] = cam_mid[si];
                    cam_saved_sequence++;
                    cam_saved_request = 0u;
                    cam_saved_valid = 1u;
                }
                frame_new=1;


                ccd_frames++;


                line_cam_claims++;


                line_cam_claims_1s++;


            }





            {
                int i2;unsigned w=0;
                for(i2=0;i2<128;i2++){
                    int sx=i2*CAM_W/128;
                    image[0][i2]=cam_bin[CAM_H-25][sx];
                    base_image[0][i2]=cam_gray[CAM_H-25][sx];
                    line_ccd_raw[i2]=base_image[0][i2];
                    if(image[0][i2])w++;
                }
                line_out_white=(int)w;
            }


        }





        /* ¡ï¡ï¡ï v066£º2D ¼¸ºÎÅĞ¾İ£¨Ìæ´ú 1D Ê±´úµÄ´àÈõÅĞ¾İ£©¡ï¡ï¡ï


           ¾ÉµÄ line_bigwhite£¨"Í¼ÏñÓÒ²àÒ»Æ¬°×"£©ºÍ line_allbright£¨"Õû·ùÈ«ÁÁ"£©


           ¶¼ÊÇÎªÒ»ÌõÉ¨ÃèÏßÉè¼ÆµÄ£¬ÔÚ 2D Ê±´ú²»ºÏÀí¡£ÏÖÔÚ¸ÄÎª£º


             line_bigwhite ¡û 2D »·ÌØÕ÷£º»·ĞÄ£¨Á½²à°×+ÖĞ¼ä°µ£©»ò ¿íÇø£¨Ç°·½´ó°×Ãæ£©


             line_allbright ¡û 2D ¿íÇø£¨Ç°·½ÕûÆ¬°×£¬¼´»·¿ÚÆ½Ãæ£©


           ±äÁ¿Ãû±£³Ö²»±ä ¡ú PID.c µÄ´ó»·×´Ì¬»ú£¨ÑØ±ä¼ÆÊı/Àï³ÌÕ¢ÃÅ£©Ò»ĞĞ¶¼²»ÓÃ¸Ä¡£ */


        line_cam_hole   = cam_ring_hole;


        line_cam_wide   = cam_ring_wide;


        line_cam_hole_y = cam_ring_hole_y;


        line_cam_rows   = cam_mid_rows;
            line_cam_mid_dbg = (int)cam_mid_near;   /* v079: Ô­Ê¼×ø±ê£¨Î´·­×ª£© */
            line_cam_dev_dbg = (int)cam_dev_near;   /* v079: Ô­Ê¼×ø±êÆ«²î */


        {


            line_bigwhite_w = cam_ring_hole ? 120 : (cam_ring_wide ? 150 : 0);


            line_bigwhite_c = (int)cam_ring_hole_y;


            line_bigwhite   = (cam_ring_hole || cam_ring_wide) ? 1 : 0;


            line_allbright  = cam_ring_wide ? 1 : 0;


            if (line_bigwhite) { if (bw_since == 0u) bw_since = now; }


            else               { bw_since = 0u; }


            line_bigwhite_hold = (line_bigwhite && bw_since != 0u


                                  && age_ms(now, bw_since) < BW_HOLD_MS) ? 1 : 0;


        }





        /* ---- ¾ÉµÄ 1D ÅĞ¾İ£¨ÒÑÍ£ÓÃ£¬±£Áô´úÂëÒÔ±ã¶ÔÕÕ/»ØÍË£©---- */


#if 0


        {


            int i2 = 0, wmax = 0, cmax = 64;


            while (i2 < 128)


            {


                int b2, w2;


                if (image[0][i2] != 255) { i2++; continue; }


                b2 = i2;


                while (i2 < 128 && image[0][i2] == 255) i2++;


                w2 = i2 - b2;


                if (w2 > wmax) { wmax = w2; cmax = (b2 + i2 - 1) / 2; }


            }


            line_bigwhite_w = wmax;


            line_bigwhite_c = cmax;


            line_bigwhite   = (wmax >= BW_W_WIDE && cmax >= BW_CENTER_MIN) ? 1 : 0;





            /* v047£ºÕû·ùÈ«ÁÁ¼ì²â£¨µÚ2¸ö¿Ú/´ó»·ÉÏ£©¡£Êµ²âµÚ2¿Ú rmin=63(È«ÁÁ)£¬


               µÚ1¿Ú rmin=12(×ó±ßÓĞ°µ) ¡ú ÓÃÔ­Ê¼»Ò¶È×îĞ¡ÖµÇø·Ö¡£ */


            {


                int k3, rmin = 255;


                for (k3 = 0; k3 < 128; k3++)


                    if (base_image[0][k3] < rmin) rmin = base_image[0][k3];


                line_allbright = (rmin > 50) ? 1 : 0;


            }





            if (line_bigwhite) { if (bw_since == 0u) bw_since = now; }


            else               { bw_since = 0u; }


            line_bigwhite_hold = (line_bigwhite && bw_since != 0u


                                  && age_ms(now, bw_since) < BW_HOLD_MS) ? 1 : 0;


        }


#endif  /* v066£º¾É 1D ÅĞ¾İÒÑÍ£ÓÃ */





        if (line_rec_trigger)   /* ¡ïv043¡ï£º¿´µ½»·¿Ú×Ô¶¯¿ªÊ¼¼ÇÂ¼ */


        {


            line_rec_trigger = 0;


            rec_frames = 0u; line_trace_count = 0u;


            rec_last_frame = ccd_frames;
            if (!cam_saved_valid && !cam_saved_request) rec_active = 1u;


        }





        /* °´¼ü´¥·¢µÄÇúÏß¼ÇÂ¼£ºÊÖ¶¯ÍÆ³µ / ×Ô¶¯ĞĞÊ»¶¼ÄÜÂ¼¡£


           °´¡°CCD Ö¡¡±½µ²ÉÑù£¨Ã¿ REC_DIV_FRAMES Ö¡´æ 1 Ö¡£©£¬ÓëÖ÷Ñ­»·¿ìÂıÎŞ¹Ø£º


           Êµ²â CCD Êµ²â 31Hz ¡ú Ã¿ 2 Ö¡Ò»´æ ¡Ö 65ms/Ñù±¾ ¡ú 2000 Ö¡ ¡Ö 130 Ãë¡£ */


        if (rec_active && rec_frames < TRACE_MAX)


        {


            if ((unsigned)(ccd_frames - rec_last_frame) >= REC_DIV_FRAMES)


            {


                rec_last_frame = ccd_frames;


                /* Í¬²½¼ÇÂ¼×´Ì¬»ú¿ìÕÕ£¨Õï¶ÏÓÃ£© */


                if (rec_frames < TRACE_MAX)


                {


                    {int sp_tmp=(line_out_left_speed+line_out_right_speed)/2/12;if(sp_tmp<0)sp_tmp=0;if(sp_tmp>255)sp_tmp=255;trace_speed[rec_frames]=(unsigned char)sp_tmp;}


                    trace_br2[rec_frames]   = (unsigned char)((big_ring_hits & 7u) | ((big_ring_phase & 31u) << 3));


                    trace_center[rec_frames] = (signed char)(line_out_center <= 127 ? line_out_center : 127);


                    trace_gyro[rec_frames]   = (short)line_gyro_f;


                    trace_cl[rec_frames]     = (short)cam_ring_cl;


                    trace_cr[rec_frames]     = (short)cam_ring_cr;


                    {int st_t = (int)(line_out_steering * 10.0f); if (st_t > 100) st_t = 100; if (st_t < -100) st_t = -100; trace_steer[rec_frames] = (signed char)st_t;}


                    trace_flags[rec_frames]  = (unsigned char)(line_out_flags & 0xFF);   /* v118: +fork/hole/corner bits */


                }


                rec_frames++;


                line_trace_count = rec_frames;


            }


        }





        /* v142 ç›²åŒºè‡ªåŠ¨å­˜å¸§ï¼ˆè¯Šæ–­ï¼Œåªè¯»è¡Œè½¦çŠ¶æ€ã€ä¸å‚ä¸æ§åˆ¶ï¼‰ï¼š
           è½¦è¿è¡Œä¸­è¿ç»­ CAM_BLIND_AUTO_TICKS ä¸ªé‡‡æ ·æ‹æ— æœ‰æ•ˆçº¿ï¼ˆâ‰ˆ195msï¼‰â†’ è‡ªåŠ¨æŠ“æ‹ä¸€å¸§ã€‚
           ç¼“å†²æœªè¯»å‡ºï¼ˆvalid/request å·²ç½®ï¼‰æ—¶ä¸è¦†ç›–ï¼›çº¿æ¢å¤åå†ç›²å¯å†æŠ“ï¼ˆæ¯æ®µåªè§¦å‘ä¸€æ¬¡ï¼‰ã€‚ */
        {
            static unsigned blind_auto_last = 0u;
            static unsigned blind_sr = 0u;
            if ((unsigned)(ccd_frames - blind_auto_last) >= REC_DIV_FRAMES) {
                blind_auto_last = ccd_frames;
                if (state == ST_RUN && (line_out_left_speed + line_out_right_speed) > 0) {
                    if (line_out_center < 0) { if (blind_sr < 60000u) blind_sr++; }
                    else blind_sr = 0u;
                    if (blind_sr == CAM_BLIND_AUTO_TICKS && !cam_saved_valid && !cam_saved_request) {
                        cam_saved_request = 1u;
                        cam_saved_auto = 1u;
                        cam_saved_meta_flags = line_out_flags;
                        cam_saved_meta_br2 = (big_ring_hits & 7u) | ((big_ring_phase & 31u) << 3);
                        cam_saved_meta_ms = age_ms(now, boot_tick);
                        cam_saved_meta_recf = line_trace_count;
                    }
                } else blind_sr = 0u;
            }
        }


        if (state == ST_ARM || state == ST_IDLE)


        {


            if (!servo_released && !line_ring_armed) servo_control('C', 0);   /* Timer exclusively owns steering after arming. */


        }


        if (state == ST_ARM && frame_new)


        {


            // µÈ´ıºÚÏß£º²»¶¯µç»ú£¬Ö±µ½Á¬Ğø¿´µ½ ARM_FRAMES Ö¡ÓĞĞ§Ïß²ÅÆğ²½


            int c = CalculateCenterLine(image);


            line_out_center = c;


            count_black();


            DRV_8701_MOTOR_DRIVER_D(0, 0);


            if (c >= 0 && line_imu_ok && line_gyro_ready)


            {


                if (++good_frames >= ARM_FRAMES)


                {


                    servo_released = 0u;                  /* v082£ºÆğÅÜÇ°Ç¿ÖÆÊÕ¶æ£¨ĞŞ P0-1 ÒÅÁô£© */


                    PID_Reset(&pid_steering);


                    PID_Reset(&pid_speed);


                    SpeedLoop_Reset();                 // ËÙ¶È±Õ»·»Øµ½°²È«³õÖµ


                    RingReset();                       // Çå¿ÕÔ²»·×´Ì¬£¬±ÜÃâ²ĞÁô


                    line_ring_armed = 1;               // Ö»ÓĞ RUN ²ÅÔÊĞíÅĞ»·


                    RingDiagReset();


                    line_odo_run = 0u;                  // v050£ºÆğÅÜÀï³ÌÇåÁã


                    line_out_lost_count = 0;


                    run_started = now;


                    state = ST_RUN;


                    screen_all(state);


                }


            }


            else { good_frames = 0; }


        }


        else if (state == ST_RUN)


        {


            /* ¡ïv038¡ï£ºÑÏ¸ñ¡¸Ã¿ CCD Ö¡¿ØÖÆÒ»´Î¡¹¡£


               Ô­ÊµÏÖÃ¿¸öÖ÷Ñ­»·¶¼µ÷ÓÃ£¨Êµ²âÖ÷Ñ­»·¡Ö140Hz¡¢CCD¡Ö72Hz ¡ú Ô¼ 1.9 ´Î/Ö¡£©£¬


               µ¼ÖÂÈı´¦"Ã¿Ö¡"Éè¼ÆÈ«²¿Ê§Õæ£º


                 ¢Ù ÖĞÏßµÍÍ¨ 0.6/0.4 Êµ¼ÊµÈĞ§ ~0.72/0.28 ¡ú Æ½»¬±äÈõ ¡ú Ö±Ïß¶¶


                 ¢Ú ¡À20 ÏñËØ/Ö¡ÏŞ·ùÊµ¼Ê±ä ¡À38/Ö¡ ¡ú ÖĞÏßÌø±ä¸ü´ó


                 ¢Û »·×´Ì¬»úËùÓĞ"Á¬Ğø N Ö¡"¼ÆÊıÆ÷¿ìÔ¼ 2 ±¶ ¡ú ãĞÖµÈ«²¿²»¶Ô


               ¸ÄÎª°´ ccd_frames ±ä»¯´¥·¢£¬ÉÏÊöÈı´¦¶¼»Øµ½Éè¼ÆÖµ£¬


               ĞĞÎªÒ²²»ÔÙËæÖ÷Ñ­»·¿ìÂıÆ¯ÒÆ¡£ */


            static unsigned ctrl_frame = 0xFFFFFFFFu;


            if (ccd_frames != ctrl_frame)


            {


                ctrl_frame = ccd_frames;


                line_run_elapsed_ms = age_ms(now_ticks(), run_started);


                LineFollow_PID(&pid_steering, &pid_speed, image);


                {


                    RingDiagSample sample;


                    RingDiagReadController(&sample.flags, &sample.mouth_count,


                                           &sample.candidate, &sample.entry_odo);


                    sample.ccd_frame = ccd_frames;


                    sample.odo = line_odo_run;


                    sample.yaw10 = (unsigned)line_yaw10;


                    sample.white = (unsigned)line_out_white;


                    sample.wide_width = (unsigned)line_bigwhite_w;


                    sample.wide_center = (unsigned)line_bigwhite_c;


                    sample.center = (unsigned)line_out_center;


                    sample.deviation_milli = (unsigned)(int)(line_out_deviation * 1000.0f);


                    sample.steering_milli = (unsigned)(int)(line_out_steering * 1000.0f);


                    sample.servo_duty = (unsigned)line_out_servo_duty;


                    sample.left_pwm = (unsigned)line_out_left_speed;


                    sample.right_pwm = (unsigned)line_out_right_speed;


                    sample.enc5 = (unsigned)line_enc5;


                    sample.enc6 = (unsigned)line_enc6;


                    sample.gyro_f = (unsigned)line_gyro_f;


                    sample.enc_sequence = line_enc_seq;


                    sample.sample_dt_ms = line_enc_dt_ms;


                    RingDiagCapture(&sample, base_image[0]);


                }


            }


            if (line_out_lost_count > LOST_GIVEUP)              // ³¤Ê±¼äÕÒ²»µ½Ïß


            {


                /* ×Ô¶¯»Øµ½¡°µÈ´ıºÚÏß¡±£º³µÍ£ÏÂºó£¬Ö»ÒªÖØĞÂ·Å»ØÈüµÀ/ÊÓÒ°Àï³öÏÖÏß£¬


                   ¾Í»áÏñ¸Õ°´¼üÒ»Ñù×Ô¶¯ÖØĞÂÆğ²½£¬²»ÓÃÔÙÊÖ¶¯°´ P11.3¡£


                   ÆÚ¼ä PID ÒÑ¾­°´ÏÂ·½ÏòµÍËÙÕõÔú¹ı£¨¼û PID.c£©¡£ */


                RingDiagFreeze(2u);


                PID_Reset(&pid_steering);


                PID_Reset(&pid_speed);


                SpeedLoop_Reset();


                line_out_lost_count = 0;


                good_frames = 0;


                run_started = now;


                line_ring_armed = 0;               // Í£Ö¹ÅĞ»·


                RingReset();


                DRV_8701_MOTOR_DRIVER_D(0, 0);


                state = ST_STOP;


                screen_all(state);


            }


            else if (line_out_lost_count > LOST_STOP_LIMIT)     // ¶ªÏßÕõÔúÖĞ


            {


                draw_state_row(ST_STOP);          // ÆÁÄ»ÉÏÏÔÊ¾¡°¶ªÏßÍ£³µ¡±£¨ÕõÔúÖĞ£©


            }


            


        }


        else


        {


            DRV_8701_MOTOR_DRIVER_D(0, 0);                      // ´ı»ú/Í£³µ£ºµç»úÊä³öºãÎª 0


        }





        if (age_ms(now, last_enc) >= 20)          // 20ms ¶Á±àÂëÆ÷ + IMU


        {


            uint32 enc_tick = now_ticks();


            unsigned imu_dt_ms = age_ms(enc_tick, last_enc);


            if (!imu_dt_ms) imu_dt_ms = 1u;


            /* Timer task owns encoder reads; do not clear its counters here. */


            


            /* Sequence is published by the fixed-period task. */                        // Í¨ÖªËÙ¶È±Õ»·ÓĞĞÂÑù±¾





            /* ---- Àï³ÌÀÛ¼Æ£¨±àÂëÆ÷£©£ºÁ½ÂÖ¼ÆÊı¾ø¶ÔÖµÈ¡Æ½¾ù ---- */


            {


                int a = line_enc5 < 0 ? -line_enc5 : line_enc5;


                int b = line_enc6 < 0 ? -line_enc6 : line_enc6;


                line_odometer += (unsigned)((a + b) / 2);


            }





            /* v050£ºÆğÅÜÀï³Ì£¨´ó»·Àï³Ì´¥·¢ÓÃ£© */


            if (line_ring_armed)


            {


                int a = line_enc5 < 0 ? -line_enc5 : line_enc5;


                int b = line_enc6 < 0 ? -line_enc6 : line_enc6;


                line_odo_run += (unsigned)((a + b) / 2);


            }





            /* ---- ¶Â×ª¼ì²â£ºÄ¿±êËÙ¶È²»µÍµ«Êµ¼Ê²»¶¯£¬³ÖĞø 1 Ãë ¡ú ±ê¼Ç ---- */


            {


                int a = line_enc5 < 0 ? -line_enc5 : line_enc5;


                int b = line_enc6 < 0 ? -line_enc6 : line_enc6;


                int act = (a + b) / 2;


                /* ¡ï±ê¶¨Ì½Õë¡ï£ºÏÈ 5 Ö¡ÖĞÖµÂË²¨ÔÙÈ¡·åÖµ ¡ª¡ª


                   v033 Êµ²â enc_peak=2122 µ«È«³ÌÆ½¾ù½ö ~400£¬ÅĞ¶¨Îªµ¥Ö¡Ê±Ğò¼â·å


                   £¨Ö÷Ñ­»·Å¼·¢ÑÓ³Ù >20ms Ê±£¬Ò»´Î¶Áµ½Á½¸öÖÜÆÚµÄ¼ÆÊı£©¡£ */


                {


                    int tmp[5], k2, j2;


                    enc_med_buf[enc_med_i] = act;


                    enc_med_i = (enc_med_i + 1) % 5;


                    if (enc_med_n < 5) enc_med_n++;


                    for (k2 = 0; k2 < 5; k2++) tmp[k2] = enc_med_buf[k2];


                    for (k2 = 0; k2 < enc_med_n; k2++)


                        for (j2 = k2 + 1; j2 < enc_med_n; j2++)


                            if (tmp[j2] < tmp[k2]) { int t3 = tmp[k2]; tmp[k2] = tmp[j2]; tmp[j2] = t3; }


                    if (enc_med_n > 0 && tmp[enc_med_n / 2] > line_enc_peak)


                        line_enc_peak = tmp[enc_med_n / 2];


                }


                /* ÔËĞĞÆÚÆ½¾ù£¨Ö»Í³¼Æ RUN£¬ÅÅ³ı´ı»ú 0 Öµ£© */


                if (line_ring_armed) {


                    enc_sum += (unsigned)act; enc_cnt++;


                    if (enc_cnt) line_enc_avg = (int)(enc_sum / enc_cnt);


                }


                /* Ö»ÔÚ RUN£¨line_ring_armed£©Ê±ÅĞ¶¨£º´ı»úÊ±Ä¿±êÖµÊÇ²ĞÁôÖµ£¬»á¼Ù±¨¾¯ */





                /* Single-wheel stall is latched by track_core. */   /* 50¡Á20ms = 1s */


            }





            /* ---- IMU£ºÍÓÂİ + ¼ÓËÙ¶È£¨¡ïÁÙ½çÇø±£»¤ + Ê§²½×ÔÓú¡ï£©----


             * ¸ùÒò£¨v029 Êµ²â£©£ºCCD ²É¼¯ÖĞ¶ÏÃ¿ 10ms Ö´ĞĞ ~1.8ms£»ÈôËüÇ¡ºÃ²åÔÚ


             * IMU µÄ SPI ÊÂÎñÖĞ¼ä£¬IMU µÄ CS ±»³ÖĞøÀ­µÍ³¬¹ıĞ¾Æ¬³¬Ê± ¡ú SPI ½Ó¿Ú


             * Ê§²½ ¡ú ´Ëºó¶ÁÊıºãÎª 0£¨ÏÖÏó£ºÉÏµç±ê¶¨Õı³£ bias=3£¬ÔËĞĞÊ®ÓàÃëºó


             * gyro_z Í»±äÎªºã 0£©¡£ĞŞ·¨£ºÆÁ±ÎÖĞ¶Ï 20 Î¢ÃëÍê³ÉÁ½´Î¶ÁÈ¡¼´¿É±ÜÃâ£»


             * ´ú¼Û½öÊÇ CCD ÖĞ¶Ï×î¶àÍí 20 Î¢Ãë£¬ÎŞÈÎºÎ¸±×÷ÓÃ¡£


             * ×ÔÓú¶µµ×£ºgyro+acc È«Áã£¨¾²Ö¹Ê± acc_z Ô¼ 1g£¬²»¿ÉÄÜÎª 0£©³ÖĞø 0.5s


             * ¡ú Èôµ±Ç°Î´¿ªÅÜ£¨line_ring_armed=0£©£¬ÖØĞÂ³õÊ¼»¯ IMU »Ö¸´Í¨Ñ¶¡£ */


            if (line_imu_ok)


            {


                int g;


                uint32 pm1 = interrupt_global_disable();


                imu660ra_get_gyro();


                imu660ra_get_acc();


                interrupt_global_enable(pm1);





                g = imu660ra_gyro_z;


                line_gyro_z = g;                   /* Ô­Ê¼Öµ£¨ÉÏ±¨/Õï¶ÏÓÃ£© */


                line_acc_x = imu660ra_acc_x;


                line_acc_y = imu660ra_acc_y;


                line_acc_z = imu660ra_acc_z;





                /* Ê§²½¼ì²â£ºËÄÏîÈ«Áã = SPI Ê§²½£¨¾²Ö¹Ê± acc_z Ô¼ 1g ¾ø²»Îª 0£© */


                if (g == 0 && line_acc_x == 0 && line_acc_y == 0 && line_acc_z == 0)


                {   if (imu_dead_cnt < 1000) imu_dead_cnt++;   }


                else


                {   imu_dead_cnt = 0;   }





                /* ×ÔÓú£ºÈ·ÈÏÊ§²½ÇÒÎ´¿ªÅÜ ¡ú ÖØĞÂ³õÊ¼»¯£¨±ÜÃâ RUN ÖĞ¿¨¶Ù£© */


                if (imu_dead_cnt >= 25 && imu_reinit_cd == 0 && !line_ring_armed)


                {


                    uint32 pm2 = interrupt_global_disable();


                    imu_dbg = (int)imu660ra_init();    /* 0=»Ö¸´Í¨Ñ¶ 1=ÈÔÊ§Áª */


                    interrupt_global_enable(pm2);


                    imu_dead_cnt = 0;


                    imu_reinit_cd = 100;               /* 2s ÀäÈ´ */


                }


                if (imu_reinit_cd > 0) imu_reinit_cd--;





                if (imu_dead_cnt >= 25)


                {


                    /* ÒÑÈ·ÈÏÊ§²½£º½ûÓÃ×èÄáÊı¾İ£¨±ÜÃâÓÃ»µÊı¾İ´ò¶æ£©£¬µÈ×ÔÓú */


                    line_gyro_f = 0;


                    gyro_buf_n = 0;


                }


                else if (!line_gyro_ready)


                {


                    /* ÉÏµç¾²Ö¹±ê¶¨£ºÀÛ¼Ó GYRO_CAL_N ¸öÑù±¾È¡Æ½¾ùÎªÁãÆ« */


                    gyro_cal_sum += g;


                    if (++gyro_cal_cnt >= GYRO_CAL_N)


                    {


                        line_gyro_bias = gyro_cal_sum / GYRO_CAL_N;


                        line_gyro_ready = 1;


                    }


                    line_gyro_f = 0;


                }


                else


                {


                    /* È¥ÁãÆ« + 4 ÅÄ»¬¶¯Æ½¾ù */


                    int k, sum = 0;


                    gyro_buf[gyro_buf_i] = g - line_gyro_bias;


                    gyro_buf_i = (gyro_buf_i + 1) % GYRO_FILT_N;


                    if (gyro_buf_n < GYRO_FILT_N) gyro_buf_n++;


                    for (k = 0; k < gyro_buf_n; k++) sum += gyro_buf[k];


                    line_gyro_f = (gyro_buf_n > 0) ? (sum / gyro_buf_n) : 0;


                }


            }


            else


            {


                line_gyro_z = 0; line_gyro_f = 0; line_gyro_ready = 0;


            }





            /* ---- v061 ²à·­±£»¤£º×ÜÇã½Ç = acos(az/|a|) ----


               ÓÃÆ½·½±È½Ï±ÜÃâ¿ª·½£ºaz*az < 0.36*|a|*|a| µÈ¼ÛÓÚ Çã½Ç > 53 ¶È¡£


               Á¬Ğø 10 Ö¡(Ô¼ 0.14s)²ÅÈ·ÈÏ£¬·Àµ¥Ö¡³å»÷ÎóÅĞ¡£ */


            {


                long long ax = line_acc_x, ay = line_acc_y, az = line_acc_z;


                long long mag2 = ax*ax + ay*ay + az*az;


                int tipped = (mag2 > 1000000L) && (az*az < (mag2 * 36L) / 100L);


                if (tipped) { if (tilt_cnt < 20) tilt_cnt++; }


                else        { tilt_cnt = 0; }


                line_tilt_stop = (tilt_cnt >= 10) ? 1 : 0;


            }





            /* ---- ¡ïv043£ºÍÓÂİ»ı·Ö×ª½Ç£¨¶È¡Á10£©£¬¹©´ó»·ÅĞ 360¡ã ³ö»· ---- */


            yaw_acc += LineYawDelta(imu660ra_gyro_transition(line_gyro_f), imu_dt_ms);


            line_yaw10 = (int)(yaw_acc * 10.0f);





            last_enc = enc_tick;


        }





        if (age_ms(now_ticks(), last_draw) >= (uint32)((ui_page == 3u) ? 25u : 100u))   /* v078: cam 25ms, ~30fps */


        {


            if      (ui_page == 3u) { draw_cam_page(); }   /* v069£ºÉãÏñÍ·Ò³Ò²ÒªË¢ĞÂ£¨Ô­À´Â©ÁË£© */


            else if (ui_page == 2u) { draw_rec_page(); }


            else if (ui_page == 1u) { draw_wave_page(); }


            else                    { draw_data_rows(); }


            last_draw = now_ticks();


        }





        /* ¡ïv069£ºÃ¿ÃëÍ³¼Æ£¨Ö¡ÂÊ / Ö÷Ñ­»·ÆµÂÊ / ×î´ó¼ä¸ô£©+ ²É¼¯Í£°Ú×Ô¶¯»Ö¸´¡ï */


        {


            static uint32 stat_tick = 0, loop_cnt = 0, loop_max_run = 0, prev_it = 0, last_cam_reinit = 0;


            loop_cnt++;


            if (prev_it) { uint32 g = age_ms(now, prev_it); if (g > loop_max_run) loop_max_run = g; }


            prev_it = now;


            if (stat_tick == 0) stat_tick = now;


            if (age_ms(now, stat_tick) >= 1000)


            {


                uint32 dt = age_ms(now, stat_tick); if (!dt) dt = 1;


                line_cam_fps_x10 = (int)((line_cam_claims_1s * 10000u) / dt);


                line_loop_hz     = (int)((loop_cnt * 1000u) / dt);


                line_loop_max_ms = (int)loop_max_run;


                line_cam_claims_1s = 0; loop_cnt = 0; loop_max_run = 0;


                stat_tick = now;





                /* v070: ÔİÊ±¹Ø±Õ×Ô¶¯»Ö¸´£¨ÏÈ²âÁ¿£¬±ÜÃâ 443ms ×èÈû¸ÉÈÅ¼ÆÊı£© */


                (void)last_cam_reinit;


            }


        }





        report_publish(now, state);


        /* ¡ï¡ï¡ï v074£º5ms ¡ú 1ms£¨¸ùÒòĞŞ¸´£¡£©¡ï¡ï¡ï


        ÏÖÏó£ºDMA Õı³££¨65Ö¡/Ãë¶¼ÔÚÖÃ finish_flag£©£¬µ«Ö÷Ñ­»·¼¸ºõ×¥²»µ½Ö¡£¬


              10 ÃëÖ» claim ³É¹¦ 3 ´Î¡£


        Ô­Òò£ºÖ÷Ñ­»·ÖÜÆÚ£¨5ms ÑÓ³Ù + Ë¢ÆÁ/IMU ¿ªÏú£©¡Ö 15ms£¬ÓëÖ¡ÖÜÆÚ 15.4ms


              ¼¸ºõÏàÍ¬ ¡ú ¡¾ÏàÎ»Ëø¶¨¡¿¡ú Ã¿´Î¶¼Ç¡ºÃÔÚ"Ö¡¸ÕÖØĞÂ¿ªÊ¼"µÄÊ±¿Ì²ÉÑù£¬


              ÓÀÔ¶µÈ²»µ½"Ö¡Íê³É"±êÖ¾¡£


              CamLine£¨ÄÜ¹¤×÷£©Ã»ÓĞÕâ¸ö¹Ì¶¨ÑÓ³Ù£¬Ö÷Ñ­»·ÅÜµ½ÉÏÇ§ Hz ±ØÈ»ÃüÖĞ¡£


        ĞŞ¸´£ºÑÓ³Ù½µµ½ 1ms ¡ú Ö÷Ñ­»· ~ ¼¸°Ù Hz£¬Ô¶¸ßÓÚÖ¡ÂÊ ¡ú ±ØÈ»ÃüÖĞ¡£*/


        system_delay_ms(1);


    }


}





#pragma section all restore


// **************************** ´úÂëÇøÓò ****************************


