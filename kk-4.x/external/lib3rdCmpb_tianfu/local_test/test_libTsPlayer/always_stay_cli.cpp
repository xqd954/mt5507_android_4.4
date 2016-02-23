#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <getopt.h>

/*the variables bellow was define in getopt.h
extern char *optarg;  //选项的参数指针
extern int optind;    //下一次调用getopt的时，从optind存储的位置处重新开始检查选项
extern int opterr;    //当opterr=0时，getopt不向stderr输出错误信息
extern int optopt;

默认情况下getopt会重新排列命令行参数的顺序，所以到最后所有不包含选项的命令行参数都排到最后
*/


#include "TsPlayer.h"

#define LOG_DBG(fmt, ...)  fprintf(stderr, "[iptv_play]]line:%d "fmt,  __LINE__, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)  fprintf(stderr, "[iptv_play]line:%d Err: "fmt,  __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  fprintf(stdout, fmt, ##__VA_ARGS__)

#define MAX_USR_INPUT_LEN     (128)


//#define BUF_SIZE (48*1024)
#define BUF_SIZE (37600)


static FILE* _pfile_data = NULL;
static FILE* pfile_normal = NULL;
static FILE* pfile_fast = NULL;

static volatile int _write_data_runing = 1;
static volatile int _write_data_start = 0;
static volatile unsigned int ui4_wrt_tms = 0;


static const char* cp_change_av_files[] =
{
    "cctv.ts",
    "iptv_record.ts",
    "ts_dump_hd_cctv1hd.ts",
    "ts_dump_hd_cctvhd.ts",
    "sd_wydht_2011-02-12-13-56-57.ts",
    "shoe.ts",
    "ts_dump_sd_last_3.ts"
};

#ifdef __cplusplus
extern "C" {
#endif
int info_get(const char* cp_info_file, const char*  cp_class_name, int (*pfunc_get)(const char* /*cp_class_name*/, const char* /*cp_value*/, int /*i4_error*/));

#ifdef __cplusplus
}
#endif

struct G_APP_ARGS_T
{
    char* cp_t_str;                  /* -t option string */
    char* cp_k_str;                  /* -k option string */
    char* cp_f_str;                  /* -f option string */
    long i4_d_num;                    /* -d option integer*/
    long i4_p_num;                    /* -p option integer*/
    long i4_s_num;                    /* -s option integer*/
    //const char* cp_out_file;         /* -o option */
    char** cpp_input_files;          /* input files */
    int i4_num_of_files;             /* number of input files */
    //unsigned long ui4_valid_bits;    /*valid bits*/
} _g_app_args;

static const char*   cp_opt_string = ":t:f:k:d:p:s:h";  /*-h option is used for usage*/

static void _show_app_args(void)
{
    char** cpp_inputs = _g_app_args.cpp_input_files;
    int i = _g_app_args.i4_num_of_files;

    LOG_INFO("current app args(0807):\n");
    if (_g_app_args.cp_t_str)
    {
        LOG_INFO("  -t: %s\n", _g_app_args.cp_t_str);
    }
    if (_g_app_args.cp_k_str)
    {
        LOG_INFO("  -k: %s\n", _g_app_args.cp_k_str);
    }

    if (_g_app_args.cp_f_str)
    {
        LOG_INFO("  -f: %s\n", _g_app_args.cp_f_str);
    }

    if (_g_app_args.i4_d_num > 0)
    {
        LOG_INFO("  -d: %ld\n", _g_app_args.i4_d_num);
    }

    if (_g_app_args.i4_p_num > 0)
    {
        LOG_INFO("  -p: %ld\n", _g_app_args.i4_p_num);
    }

    if (_g_app_args.i4_s_num > 0)
    {
        LOG_INFO("  -s: %ld\n", _g_app_args.i4_s_num);
    }

    if (i > 0)
    {
        LOG_INFO("  files: ");

        do
        {
            LOG_INFO("%s ", *cpp_inputs);
            cpp_inputs++;
            i--;
        }
        while (i != 0);

        LOG_INFO(" \n");
        if (_g_app_args.i4_num_of_files > 1)
        {
            LOG_INFO("Too many file for play, the first: %s is played\n", *(_g_app_args.cpp_input_files));
        }
    }

}

static void _input_files_init(int i4_start_id, int i4_end_next_id, char* argv[])
{
    int i;
    char** cpp_inputs = NULL;

    i = i4_end_next_id - i4_start_id;

    if (i >= 0)
    {
        _g_app_args.i4_num_of_files = i;
    }

    if (_g_app_args.i4_num_of_files)
    {
        cpp_inputs = (char**)calloc(sizeof(char**), _g_app_args.i4_num_of_files);
        if (!cpp_inputs)
        {
            LOG_ERR("out of memory!\n");
            exit(1);
        }

        _g_app_args.cpp_input_files = cpp_inputs;


        for (i = i4_start_id; i < i4_end_next_id; i++)
        {
            *cpp_inputs = argv[i];
            cpp_inputs++;
        }
    }
}

static void _input_files_destroy(void)
{
    char** cpp_inputs = _g_app_args.cpp_input_files;
    int i = _g_app_args.i4_num_of_files;
    if (i)
    {
        if (cpp_inputs)
        {
            free(cpp_inputs);
            _g_app_args.cpp_input_files = NULL;
        }
    }
}

static void _show_usage(const char* cp_prompt_str)
{
    if (cp_prompt_str)
    {
        LOG_INFO("%s\n", cp_prompt_str);
    }

    LOG_INFO("Usage: [OPTION]... FILE\n");
    LOG_INFO("test elf for shanghai iptv\n\n");
    LOG_INFO("  -t,          source type: 0, iptv, mpeg_real\n");
    LOG_INFO("  -k,          ff/fb file\n");
    LOG_INFO("  -f,          file name of TS info\n");
    LOG_INFO("  -d,          delay time(ms) to send data: cctv*HD: 28000 other HD:32000 SD: 18000\n");
    LOG_INFO("  -p,          play time(ms), just for change ts\n");
    LOG_INFO("  -s,          first (32 tiimes)delay time(ms) to send data: HD: 20000 SD: 120000 is for default\n");
    LOG_INFO("  -h, --help   show this information\n");

    LOG_INFO("\nOther input args will be treated as [FILE]\n");
    LOG_INFO("\nExamples:\n");
    LOG_INFO("  iptv_play_test.elf -t IPTV -f info_ts.txt -d 28000 ts_dump_hd_cctvhd.ts\n\n");

    _input_files_destroy();
    exit(0);
}






static void _show_help_info(void)
{
    LOG_INFO("0 -- quit\n");
    LOG_INFO("1 -- init\n");
    LOG_INFO("2 -- play\n");
    LOG_INFO("3 -- pause\n");
    LOG_INFO("7 -- resume\n");
    LOG_INFO("4 -- stop\n");
    LOG_INFO("5 -- fast\n");
    LOG_INFO("6 -- stopFast\n");
    LOG_INFO("8 -- autoChange\n");
    LOG_INFO("9 -- changeTs\n");
    LOG_INFO("\nCmd>$$ ");
}

static long _get_usr_input(void)
{
    char c_input_buf[MAX_USR_INPUT_LEN];
    long i4_str = 0;
    int i4_len = 0;
    char* pc_endptr = NULL;

    while (1)
    {
        memset(c_input_buf, 0, sizeof(c_input_buf));
        fgets(c_input_buf, MAX_USR_INPUT_LEN - 1, stdin);

        if (c_input_buf[0] == '\n')
        {
            _show_help_info();
            continue;
        }

        i4_len = strlen(c_input_buf);

        if (c_input_buf[i4_len - 1] == '\n' || c_input_buf[i4_len - 1] == '\r')
        {
            c_input_buf[i4_len - 1] = '\0';
        }

        //URI_LOG_INFO("==>%s[E](%d)\n", c_input_buf, '\n');

        if (c_input_buf[0] == '-' || (c_input_buf[0] >= '0' && c_input_buf[0] <= '9'))
        {
            i4_str = strtol(c_input_buf, &pc_endptr, 10);
            if (!pc_endptr || pc_endptr[0] == 0)
            {
                break;
            }
            else
            {
                LOG_INFO("strtol err:%s[end]%d\n", pc_endptr, pc_endptr[0]);
            }
        }

        _show_help_info();
        continue;
    }

    return i4_str;
}

static int func_get_ts_info(const char* cp_class_name, const char* cp_value, int i4_error)
{
    int i, i4_value_len;
    char* cp_id_str = NULL;
    char* cp_num_str = NULL;
    char* cp_value_str = NULL;
    int i4_ret = 0;

    if (!i4_error && cp_value)
    {
        cp_value_str = strdup(cp_value);
        if (!cp_value_str)
        {
            LOG_ERR("out of memory\n");
            return -1;
        }

        i4_value_len = strlen(cp_value_str);
        i = 0;



        while (i < i4_value_len)
        {
            if (*(cp_value_str + i) == '=')
            {
                *(cp_value_str + i) = '\0';
                break;
            }
            i++;
        }

        if (i == i4_value_len)
        {
            LOG_ERR("STRING %s IS INVALID,please use '=' to set value!!\n", cp_value_str);
            free(cp_value_str);
            return -1;
        }

        cp_id_str = cp_value_str;
        cp_num_str = cp_value_str + i + 1;        
       
        LOG_ERR("STRING %s=%s\n", cp_id_str, cp_num_str);       

        free(cp_value_str);

    }

    return i4_ret;

}

static char* get_base_name(const char* cp_file_name)
{
    int i, i4_str_len = 0;
    int i4_find_comment = 0;
    char* cp_ret = NULL;

    if (cp_file_name)
    {
        i4_str_len = strlen(cp_file_name);

        i = i4_str_len - 1;

        while (i > 0)
        {
            if ((*(cp_file_name + i) == '/'))
            {
                cp_ret = (char*)(cp_file_name + i + 1);
                break;
            }
            i--;
        }

        if (!cp_ret)
        {
            cp_ret = (char*)cp_file_name;
        }
    }

    return cp_ret;
}

int main(int argc, char* argv[])
{
    char c_out_buf[64];
    int i4_opt_char = -1;

    int running = 1;
    long cmd = 0;
    int ret = -1;
    int i4_fast_enable = 0;
    char  c_file_name[24] = {0};
    int i4_rechk = 0;

    unsigned int i4_change_id = 0;
    char* cp_change_file = NULL;


    if (argc < 2)
    {
        _show_usage(NULL);
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            _show_usage(NULL);
        }
    }

    memset(&_g_app_args, 0, sizeof(_g_app_args));
    _g_app_args.i4_d_num = -1;
    _g_app_args.i4_p_num = 1000; //1s
    _g_app_args.i4_s_num = 20000;
    _g_app_args.cp_t_str = (char*)"IPTV";

    while ((i4_opt_char = getopt(argc, argv, cp_opt_string)) != -1)
    {
        switch (i4_opt_char)
        {

            case 't':
                _g_app_args.cp_t_str = optarg;;
                break;

            case 'k':
                _g_app_args.cp_k_str = optarg;
                break;

            case 'f':
                _g_app_args.cp_f_str = optarg;
                break;

            case 'd':
                _g_app_args.i4_d_num = strtol(optarg, NULL, 0);
                break;

           case 'p':
                _g_app_args.i4_p_num = strtol(optarg, NULL, 0);
                break;

            case 's':
                _g_app_args.i4_s_num = strtol(optarg, NULL, 0);
                break;

            case 'h':
                _show_usage(NULL);
                break;

            case ':':
                memset(c_out_buf, 0, sizeof(c_out_buf));
                sprintf(c_out_buf, "option -%c requires a parameter", optopt);
                _show_usage(c_out_buf);
                break;

            case '?':
            default:
                memset(c_out_buf, 0, sizeof(c_out_buf));
                sprintf(c_out_buf, "option: -%c is invalid", optopt);
                _show_usage(c_out_buf);
                break;
        }
    }

    _input_files_init(optind, argc, argv);

    _show_app_args();

    //check valid
    if (strcmp(_g_app_args.cp_t_str, "IPTV") != 0)
    {
        if (strcmp(_g_app_args.cp_t_str, "mpeg_real") != 0 )
        {
            if (*(_g_app_args.cp_t_str) != '0')
            {
                LOG_INFO("srcType:%s is error\n", _g_app_args.cp_t_str);
                _show_usage(NULL);
            }
        }
    }

    if (_g_app_args.i4_d_num < 0)
    {
        _show_usage("-d must set");
    }

    if (! _g_app_args.cpp_input_files)
    {
        _show_usage("NO input play file");
    }

    if (! _g_app_args.cp_f_str)
    {
        _show_usage("NO ts info file");
    }

    if (info_get(_g_app_args.cp_f_str, get_base_name(*(_g_app_args.cpp_input_files)), func_get_ts_info))
    {
        LOG_ERR("get info fail\n");
        goto exit_lable;
    }
    else
    {
        LOG_DBG("get info OK\n");
    }


    pfile_normal = fopen(*(_g_app_args.cpp_input_files), "rb");
    if (! pfile_normal)
    {
        LOG_ERR("Open file %s\n", *(_g_app_args.cpp_input_files));
        goto exit_lable;
    }

    if (_g_app_args.cp_k_str)
    {
        pfile_fast = fopen(_g_app_args.cp_k_str, "rb");
        if (! pfile_fast)
        {
            LOG_ERR("Open file %s\n", argv[3]);
            goto exit_lable;
        }
        i4_fast_enable = 1;
    }

    _pfile_data = pfile_normal;

    while (running)
    {
        _show_help_info();

        cmd = _get_usr_input();
        
        switch (cmd)
        {
            case 0:  /*exit*/
            	running = 0;
              break;

            case 1: /*init*/

            case 2: /*play*/

            case 3: /*pause*/

            case 4: /*stop*/

            case 5:  /*fast*/

            case 6:  /*stopfast*/

            case 7: /*resume*/

            case 8: /*change Ts*/                

            case 9: /*change Ts*/
                LOG_DBG("command %ld received!\n", cmd);
								break;
								
            default:
                LOG_ERR("invalid command!\n");
                break;
        }
    }

exit_lable:

    if (pfile_normal)
    {
        fclose(pfile_normal);
    }
    if (pfile_fast)
    {
        fclose(pfile_fast);
    }

    _input_files_destroy();

    LOG_INFO("main exit!\n");
    return 0;
}


