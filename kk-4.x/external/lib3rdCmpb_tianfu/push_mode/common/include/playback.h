#ifndef _HANDLE_KEY_H_
#define _HANDLE_KEY_H_

#define bool int
#include "IMtkPb_Ctrl.h"
#include "IMtkPb_Ctrl_DTV.h"

typedef uint64_t uint64;
typedef int64_t int64;
typedef uint32_t uint32;
typedef IMTK_PB_CB_ERROR_CODE_T (*event_callback_fct)(IMTK_PB_CTRL_EVENT_T, void*, unsigned int);

#define KEY_COMMAND_QUIT        'q'
#define KEY_COMMAND_PAUSE        '.'
#define KEY_COMMAND_RESUME        'r'
#define KEY_COMMAND_FF        'f'
#define KEY_COMMAND_STOPFF        's'
#define KEY_COMMAND_SEEK        't'
#define KEY_COMMAND_GETPTS        'g'
#define KEY_COMMAND_GETAUDTRACK        'a'
#define KEY_COMMAND_AUDIOBALANCE        'c'

typedef enum
{
    CMPB_OK = 0,
    CMPB_ERROR = -1,
    CMPB_QUIT = -2,
    CMPB_PENDING = -3,
}CMPB_ERROR_CODE;

CMPB_ERROR_CODE playback_init(char * playFile, char * ffFile, int speed, event_callback_fct pf_callback);

CMPB_ERROR_CODE process_key(char);

CMPB_ERROR_CODE playback_uinit();

CMPB_ERROR_CODE playback_open();

CMPB_ERROR_CODE playback_play(int);

void playback_speedUpdate();

CMPB_ERROR_CODE playback_stop();

CMPB_ERROR_CODE playback_close();

CMPB_ERROR_CODE playback_push_thread_start();

CMPB_ERROR_CODE playback_push_thread_start_with_file(char * file) ;

CMPB_ERROR_CODE playback_push_thread_stop();

CMPB_ERROR_CODE playback_getCurrentTime(unsigned int * cTime);

#endif  /*_HANDLE_KEY_H_*/
