#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpc_init.h"
#include "playback.h"

static IMTK_PB_CB_ERROR_CODE_T push_event_notify(IMTK_PB_CTRL_EVENT_T eEventType, void* pvTag, unsigned int u4Data)
{
    (void)pvTag;
    (void)u4Data;

    if(IMTK_PB_CTRL_EVENT_SPEED_UPDATE == eEventType)
    {
        fprintf(stdout, "IMTK_PB_CTRL_EVENT_SPEED_UPDATE!!!! \n");
        playback_speedUpdate();
        return IMTK_PB_CB_ERROR_CODE_OK;
    }

    switch (eEventType)
    {
    case IMTK_PB_CTRL_EVENT_CUR_TIME_UPDATE:
    {
        unsigned int cur_time = 0;
        playback_getCurrentTime(&cur_time);

        fprintf(stdout, "IMTK_PB_CTRL_EVENT_CUR_TIME_UPDATE : %d \n", cur_time);
        break;
    }
    case IMTK_PB_CTRL_EVENT_TOTAL_TIME_UPDATE:
    {
        fprintf(stdout, "IMTK_PB_CTRL_EVENT_TOTAL_TIME_UPDATE \n");
        break;
    }
    case IMTK_PB_CTRL_EVENT_BUFFER_UNDERFLOW:
    {
        fprintf(stdout, "IMTK_PB_CTRL_EVENT_BUFFER_UNDERFLOW \n");
        break;
    }
    case IMTK_PB_CTRL_EVENT_EOS:
    {
        fprintf(stdout, "IMTK_PB_CTRL_EVENT_EOS \n");
        break;
    }
    case IMTK_PB_CTRL_EVENT_STEP_DONE:
    {
        fprintf(stdout, "IMTK_PB_CTRL_EVENT_STEP_DONE \n");
        break;
    }
    case IMTK_PB_CTRL_EVENT_GET_BUF_READY:
    {
        fprintf(stdout, "IMTK_PB_CTRL_EVENT_GET_BUF_READY \n");
        break;
    }
    case IMTK_PB_CTRL_EVENT_PLAYBACK_ERROR:
    {
        fprintf(stdout, "IMTK_PB_CTRL_EVENT_ERROR \n");
        break;
    }
    case IMTK_PB_CTRL_EVENT_PLAY_DONE:
    {
        fprintf(stdout, "IMTK_PB_CTRL_EVENT_PLAYED \n");
        break;
    }
    default :
    {
        fprintf(stdout, "call back default %d \n", eEventType);
        break;
    }
    }

    fflush(stdout);
    return IMTK_PB_CB_ERROR_CODE_OK;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("-------------------Usage: ./cmpb_push.bin /mnt/usb/test.ts--------------------\n");
        return 0;
    }

    CMPB_ERROR_CODE ret = CMPB_OK;

    init_rpc(); //一个进程里只能调用一次

    printf("\n========================PUSH MODEL : argc[%d]========================\n", argc);

    if(argc >= 4)
    {
        int speed = atoi(argv[3]);
        printf("\n========================Speed : %d========================\n", speed);
        ret = playback_init(argv[1], argv[2], speed, push_event_notify);
    }
    else
    {
        ret = playback_init(argv[1], "", -1, push_event_notify);
    }
    if(CMPB_OK != ret)
    {
        printf("Error happened !!!!  File(%s), Line(%d)\n", __FILE__, __LINE__);
        goto exit;
    }

    ret = playback_open();
    if(CMPB_OK != ret)
    {
        printf("Error happened !!!!  File(%s), Line(%d)\n", __FILE__, __LINE__);
        goto exit;
    }

    ret = playback_play(0);
    if(CMPB_OK != ret)
    {
        printf("Error happened !!!!  File(%s), Line(%d)\n", __FILE__, __LINE__);
        goto exit;
    }

    ret = playback_push_thread_start();
    if(CMPB_OK != ret)
    {
        printf("Error happened !!!!  File(%s), Line(%d)\n", __FILE__, __LINE__);
        goto exit;
    }

    int quit = 0;
    char ch = '\0';

    while(!quit)
    {
        scanf("%c", &ch);
        if(process_key(ch) == CMPB_QUIT)
        {
            quit = 0;
            goto exit;
        }
    }

exit:
    ret = playback_push_thread_stop();
    if(CMPB_OK != ret)
    {
        printf("Error happened !!!!  File(%s), Line(%d)\n", __FILE__, __LINE__);
    }

    ret = playback_stop();
    if(CMPB_OK != ret)
    {
        printf("Error happened !!!!  File(%s), Line(%d)\n", __FILE__, __LINE__);
    }

    ret = playback_close();
    if(CMPB_OK != ret)
    {
        printf("Error happened !!!!  File(%s), Line(%d)\n", __FILE__, __LINE__);
    }

    ret = playback_uinit();
    if(CMPB_OK != ret)
    {
        printf("Error happened !!!!  File(%s), Line(%d)\n", __FILE__, __LINE__);
    }

    return CMPB_OK;
}

