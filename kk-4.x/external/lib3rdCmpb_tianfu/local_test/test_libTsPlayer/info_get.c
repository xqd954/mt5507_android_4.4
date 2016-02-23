#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#if 0
#define LOG_DBG(fmt, ...)   fprintf(stderr, "[%s]line:%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...)
#endif
#define LOG_ERR(fmt, ...)   fprintf(stderr, "[%s]line:%d Err: "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  fprintf(stderr, fmt, ##__VA_ARGS__)


#define MAX_LINE_BUF_LEN  (1024)

#define CHAR_COMMENT  '#'
#define CHAR_START    '{'
#define CHAR_END      '}'
#define CHAR_SEPARATOR '='


static char* get_valide_string(char* cp_orig)
{
    int i = 0, i4_str_len = 0;
    int i4_find_comment = 0;
    char* cp_ret = NULL;

    if (cp_orig)
    {
        i4_str_len = strlen(cp_orig);

        while (i4_str_len > 1)
        {
            if ((*(cp_orig + i4_str_len - 1) == '\n') || (*(cp_orig + i4_str_len - 1) == '\r'))
            {
                *(cp_orig + i4_str_len - 1) = '\0'; /*drop last \n */
                i4_str_len--;
            }
            else
            {
                break;
            }
        }


        for (i = 0; i < i4_str_len; i++)
        {
            if (!cp_ret)
            {
                if ((*(cp_orig + i) != ' ') && (*(cp_orig + i) != '\t'))
                {
                    cp_ret = cp_orig + i;
                    if (*cp_ret == CHAR_COMMENT)
                    {
                        return NULL;
                    }
                }
            }
            else if (*(cp_orig + i) == CHAR_COMMENT)
            {
                *(cp_orig + i) = '\0';
                i4_find_comment = 1;
                break;
            }
        }
    }

    if (i4_find_comment)
    {
        memset(cp_orig + i, 0, i4_str_len - i);
    }

    while ((*(cp_orig + i) == ' ') || (*(cp_orig + i) == '\t'))
    {
        *(cp_orig + i) = '\0';
        i--;
        if (i < 0)
        {
            break;
        }
    }

    if (*cp_orig == '\0')
    {
        cp_ret = NULL;
    }

    return cp_ret;
}

int info_get(const char* cp_info_file, const char*  cp_class_name, int (*pfunc_get)(const char* /*cp_class_name*/, const char* /*cp_value*/, int /*i4_error*/))
{
    char c_buf_line[MAX_LINE_BUF_LEN];
    char* cp_buf_valid = NULL;
    FILE* pfile_get = NULL;
    int i4_get_ready = 0;  /*0--not find class  1--find class  2--find start  3--find end*/

    if (!cp_info_file || !cp_class_name || !pfunc_get)
    {
        LOG_ERR("arg error, cp_info_file=0x%x cp_class_name=0x%x pfunc_get=0x%x\n", (unsigned int)cp_info_file, (unsigned int)cp_class_name, (unsigned int)pfunc_get);
        if (pfunc_get)
        {
            pfunc_get(cp_class_name, NULL, 1);
        }
        return -1;
    }

    LOG_DBG("cp_info_file=%s cp_class_name=%s pfunc_get=0x%x\n", cp_info_file, cp_class_name, (unsigned int)pfunc_get);

    pfile_get = fopen(cp_info_file, "r");
    if (!pfile_get)
    {
        LOG_ERR("open file %s error\n", cp_info_file);
        if (pfunc_get)
        {
            pfunc_get(cp_class_name, NULL, 1);
        }
        return -1;
    }

    memset(c_buf_line, 0, MAX_LINE_BUF_LEN);

    while ((i4_get_ready != 3) && fgets(c_buf_line, MAX_LINE_BUF_LEN, pfile_get))
    {
        if ((cp_buf_valid = get_valide_string(c_buf_line)))
        {
#if 0  /*<!-- deeve 2012/3/12 1:49:21 PM */
            if (cp_buf_valid)
            {
                LOG_DBG("--> %s[%d]\n", cp_buf_valid, i4_get_ready);
            }
#endif /*0*//* deeve 2012/3/12 1:49:21 PM --!>*/

            if (i4_get_ready == 0)
            {

                if (strncmp(cp_buf_valid, cp_class_name, strlen(cp_class_name)) == 0)
                {
                    i4_get_ready = 1;
                }

            }
            else if (i4_get_ready == 1)
            {
                if (cp_buf_valid[0] == CHAR_START)
                {
                    i4_get_ready = 2;
                }

            }
            else if (i4_get_ready == 2)
            {
                if (cp_buf_valid[0] == CHAR_END)
                {
                    i4_get_ready = 3;
                }
                else
                {
                    LOG_DBG("==> %s\n", cp_buf_valid);
                    if (pfunc_get(cp_class_name, cp_buf_valid, 0))
                    {
                        break;
                    }
                }
            }
        }

        memset(c_buf_line, 0, MAX_LINE_BUF_LEN);
    }

    if (pfile_get)
    {
        fclose(pfile_get);
    }

    LOG_DBG("out OK\n");

    return ((i4_get_ready == 3) ? 0 : (-1));
}




