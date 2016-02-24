#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/netlink.h>
#define NL_PARAMS_MAX 32
#define BUFFSIZE 1024
#define true 1
#define false 0


int main(int argc, char **argv)
{
	int sz = 64 * 1024;
	char *mParams[NL_PARAMS_MAX];
	int ntlk_sock;
        int len;
	char buf[BUFFSIZE];
	struct sockaddr_nl sa;
	memset(&sa, 0, sizeof(sa));
	struct iovec iov = { buf, sizeof(buf) };
        sa.nl_family = AF_NETLINK;
        sa.nl_pid = getpid();
        sa.nl_groups = 0xffffffff;
	struct msghdr msg = { (void *)&sa, sizeof(sa), &iov, 1, NULL , 0, 0};
        //char *s;

        if((ntlk_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT)) < 0){
        	perror("create netlink socket failed: ");
        	exit(EXIT_FAILURE);
        }

    	if(setsockopt(ntlk_sock, SOL_SOCKET, SO_RCVBUF, &sz, sizeof(sz)) < 0){
        	perror("setsockopt failed: ");
        	exit(EXIT_FAILURE);
    	}

    	if(bind(ntlk_sock, (struct sockaddr *)&sa, sizeof(sa)) < 0){
        	perror("bind netlink socket failed: ");
         	exit(EXIT_FAILURE);
    	}
       
        struct nlmsghdr *nh = (struct nlmsghdr *) buf;
        while(1){
        	if((len = recvmsg(ntlk_sock, &msg, 0)) < 0){
                	perror("receive msg errer: ");
                	exit(EXIT_FAILURE);
        	}
		char *s = NLMSG_DATA(nh);
		char *end = s + len;
                printf("********************************begin msg from kernel************************************************\n");
 		while(s < end){
			if(strcmp(s, " "))
			fprintf(stdout, "%s\n", s);
			s = s + strlen(s) + 1;
		}
                printf("********************************end msg from kernel**************************************************\n");
                printf("\n");
	}
	return 0;
}
