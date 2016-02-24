#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <sys/un.h>
 
#include <errno.h>
#include <sys/stat.h>

#ifdef USED_ANDROID_LOG
#define LOG_TAG "uds_server"
#include <cutils/log.h>

#else

#define ALOGI(x...) fprintf(stdout, x);
#define ALOGE(x...) fprintf(stderr, x);
#endif

#define CLIENT_NUM 8 
#define BUFFSZ 1024

#define SERVER_NAME "sserver"
#define SOCKET_DEV_DIR "/dev/socket/"

static int create_socket(const char *name, int type, mode_t perm, uid_t uid, gid_t gid)
{
    struct sockaddr_un addr;
    int fd, ret;
    char *secon;

    fd = socket(PF_UNIX, type, 0);
    if (fd < 0) {
        ALOGE("Failed to open socket '%s': %s\n", name, strerror(errno));
        return -1;
    }

    memset(&addr, 0 , sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), SOCKET_DEV_DIR"%s", name);

    ret = unlink(addr.sun_path);
    if (ret != 0 && errno != ENOENT) {
        ALOGE("Failed to unlink old socket '%s': %s\n", name, strerror(errno));
        goto out_close;
    }

    /*
    secon = NULL;
    if (sehandle) {
        ret = selabel_lookup(sehandle, &secon, addr.sun_path, S_IFSOCK);
        if (ret == 0)
            setfscreatecon(secon);
    }
    */

    ret = bind(fd, (struct sockaddr *) &addr, sizeof (addr));
    if (ret) {
        ALOGE("Failed to bind socket '%s': %s\n", name, strerror(errno));
        goto out_unlink;
    }

    //setfscreatecon(NULL);
    //freecon(secon);

    chown(addr.sun_path, uid, gid);
    chmod(addr.sun_path, perm);

    ALOGI("Created socket '%s' with mode '%o', user '%d', group '%d'\n",
         addr.sun_path, perm, uid, gid);

    return fd;

out_unlink:
    unlink(addr.sun_path);
out_close:
    close(fd);
    return -1;
}

static void proc_cmd(int sock, char buf[])
{
    char cmdbuf[BUFFSZ];
    char fpath[64] = "/tmp/sserver_log";
    FILE *fp = NULL;

    ALOGI("proc_cmd now\n");
    sprintf(cmdbuf,"%s 2> %s", buf, fpath);
    if (-1 == system(cmdbuf)) {
        ALOGI("system call failed\n");
        exit(1);
    }

    fp = fopen(fpath, "r");
    if (fp  == NULL) {
        ALOGI("open file %s failed\n", fpath);
        exit(1);
    }

    char* ret = fgets(cmdbuf, sizeof(cmdbuf), fp);
    char *result = (ret == NULL) ? "ok" : "fail";
    if (send(sock, result, strlen(result), 0) < 0) {
        perror("send error");
        exit(1);
    }
}

void signal_handler_fun(int signal_num)
{
	ALOGE("catch signal %d\n",signal_num);
}

int main(int argc, char* argv[])
{
	int fd;
	int s;
	char buff[BUFFSZ];
    struct sockaddr_un addr;
    socklen_t addr_size = sizeof(addr);

    //added by huoph@tcl.com for catch SIGPIPE signal
    sighandler_t p_signal_handler = signal_handler_fun;
    signal(SIGPIPE,p_signal_handler);
    //end by huoph@tcl.com

    fd = create_socket(SERVER_NAME, SOCK_STREAM, 0666, 0, 1000);

	if (listen(fd, CLIENT_NUM) != 0) {
		ALOGI("listen error\n");
		exit(1);
	}
 
    ALOGI("listening\n");
	
	while (1) {
        if ((s = accept(fd, (struct sockaddr *) &addr, &addr_size)) < 0) {
            ALOGE("accept: %s\n", strerror(errno));
            return -1;
        }

		int rlen;
        memset(buff, 0, sizeof(buff));
		while ((rlen = recv(s, buff, BUFFSZ-1, 0)) > 0) {
			buff[rlen] = '\0';
			ALOGI("received data = %s\n",buff);
			ALOGI("received bytes = %d\n", rlen);

			if ((strcmp(buff, "quit") == 0) || (strcmp(buff, "close") == 0)) {
				ALOGI("sserver exit\n");
				break;
			}
			

			if(strstr(buff,"remount") != NULL){
                ALOGI("skip remount ");
                break;
            }
			proc_cmd(s, buff);
		}

		close(s);
	}
	close(fd);
	return 0;
}


