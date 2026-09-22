#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>

#include <netdb.h>

#include <linux/sockios.h>
#include <net/if.h>

#include <ctype.h>
#include <string.h>

#include <pthread.h>

#include "rce.h"

#define MAXLEN 1024
#define DELAY 2

static unsigned int get_iface_ipaddr(char *p) ;
static unsigned long get_ip_addr(char * pIP) ;
static void rce_process(int recv_s) ;

static int autoenable ;
static char enable_string[64] ;
static char disable_string[64] ;
static unsigned long iface_ip  ;
static unsigned long mc_group ;
static unsigned short rce_port ;

static volatile int keepgoing = 0 ;

void set_rce_defaults()
{
	autoenable = 0 ;
	enable_string[0] = '\0' ;
	disable_string[0] = '\0' ;
	iface_ip = INADDR_ANY ;
	mc_group = inet_addr("239.1.2.3") ;
	rce_port = htons(10000) ;
}

void set_rce_autoenable(char *p)
{
	autoenable = 0 ;

	if (p)
	{
		if ((strcasecmp(p, "true") == 0) ||
			(strcasecmp(p, "yes") == 0) ||
			(strcmp(p, "1") == 0))
			autoenable = 1 ;
	}
}

void set_rce_enable_string(char *p)
{
	if (p)
		strncpy(enable_string, p, sizeof(enable_string) - 1) ;
}

void set_rce_disable_string(char *p)
{
	if (p)
		strncpy(disable_string, p, sizeof(disable_string) - 1) ;
}

void set_rce_interface(char *p)
{
	if (p)
		iface_ip = get_ip_addr(p) ;
}

void set_rce_mcgroup(char *p)
{
	if (p)
		mc_group = get_ip_addr(p) ;
}

void set_rce_port(char *p)
{
	if (p)
		rce_port = htons((unsigned short int)strtol(p, NULL, 0)) ;
}

int rce_init()
{
   u_int yes = 1;
   int recv_s;
   struct sockaddr_in mcast_group;
   struct sockaddr_in rcv_iface ;
   struct ip_mreq mreq;

   int doing_unicast = 0 ;
   unsigned long ul ;

   int rc = -1 ;

   memset(&mcast_group, 0, sizeof(mcast_group));
   mcast_group.sin_family = AF_INET;
   mcast_group.sin_port =  rce_port ;
   mcast_group.sin_addr.s_addr = mc_group ;

	ul = ntohl( mcast_group.sin_addr.s_addr) ;
	if (((ul >> 24) & 0x000000ff) < 224)
	{
		doing_unicast = 1 ;
		memcpy(&rcv_iface, &mcast_group, sizeof(rcv_iface)) ;
	}
	else
	{
		doing_unicast = 0 ;
		memset(&rcv_iface, 0, sizeof(rcv_iface));
		rcv_iface.sin_family = AF_INET;
		rcv_iface.sin_port = rce_port ;
		rcv_iface.sin_addr.s_addr = iface_ip ;
	}

	if ( (recv_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    	//perror ("recv socket");
    	return(rc);
	}
	if (setsockopt(recv_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
    	//perror("reuseaddr setsockopt");
    	return(rc);
	}

	if (bind(recv_s, (struct sockaddr*)&mcast_group,sizeof(mcast_group)) < 0){
		//perror ("bind");
		return(rc);
	}

	if (doing_unicast == 0)
	{
		/* Tell the kernel we want to join that multicast group. */
		mreq.imr_multiaddr = mcast_group.sin_addr;
		mreq.imr_interface.s_addr =  rcv_iface.sin_addr.s_addr /*: htonl(INADDR_ANY)*/ ;
		if (setsockopt(recv_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
			//perror ("add_membership setsockopt");
			return(rc);
		}
		rc = recv_s ;
	}

	return rc ;
}

static void* rce_thread(void* param)
{
	int s = (int) param ;

	rce_process(s) ;

	return NULL ;
}

void rce_start(int s)
{
	pthread_t	thstat ;

	// do not start RCE if it is not auto-enabled and no enable string defined...
	if ((autoenable != 0) || (enable_string[0] != '\0'))
		pthread_create(&thstat, NULL, &rce_thread, (void *) s) ;
}

void rce_stop(int s)
{
	keepgoing = 0;
	close(s) ;
	sleep(1) ;
}

static void rce_process(int recv_s)
{
	int is_enabled = 0 ;

	keepgoing = 1 ;

	if (autoenable)
		is_enabled = 1 ;

	while (keepgoing)
	{
		int n;
		int len;
		struct sockaddr_in from;
		char message [MAXLEN+1];

		for (;;)
		{
			len=sizeof(from);
			if ( (n=recvfrom(recv_s, message, MAXLEN-1, 0, (struct sockaddr*)&from, (unsigned *) &len)) > 0)
			{
				message[n] = 0;

#if 0
				printf("%s: Received %d-byte message from %s.\n", "RCE",
					n, inet_ntoa(from.sin_addr));
					printf("\t%s", message);
#endif

				if (is_enabled)
				{
					if (strcmp(&message[0], &disable_string[0]) == 0)
					{
						is_enabled = 0 ;
					}
					else if (strcmp(&message[0], &enable_string[0]) != 0)
					{
						if (message[n-1] != '&')
						{
							message[n] = '&' ;
							message[n+1] = '\0' ;
						}
						system(&message[0]) ;
					}
				}
				else if (strcmp(&message[0], &enable_string[0]) == 0)
				{
					is_enabled = 1 ;
				}
			}
		}
	}
}

static unsigned long get_ip_addr(char * pIP)
{
	unsigned long err = INADDR_NONE ;
	unsigned long ipaddr ;

	if (isdigit(pIP[0]))
	{
		ipaddr = inet_addr(pIP) ;
		return (ipaddr) ;
	}
	else
	{
		ipaddr = get_iface_ipaddr(pIP) ;
		if (ipaddr != INADDR_ANY)
		{
			err = ipaddr ;
		}
		else
		{
			//fprintf(stderr, "%s -- network interface not found\n", pIP) ;
			err = ipaddr ;
		}
	}

	return err ;
}

static unsigned int get_iface_ipaddr(char *p)
{
	unsigned long ipaddr = INADDR_ANY ;

	char buf [2048];
	struct ifconf ic;
	int i, n;
	int sock;
	struct ifreq *pir ;

	/* Create an unbound datagram socket to do the SIOCGIFADDR ioctl on. */
	if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("Can't create addrlist socket");
		return ipaddr ;
	}

	ic.ifc_len = sizeof buf;
	ic.ifc_ifcu.ifcu_buf = (caddr_t)buf;

	n = ioctl(sock, SIOCGIFCONF, &ic);

	if (n >= 0)
	{
		pir = ic.ifc_req ;
		n = ic.ifc_len / sizeof(struct ifreq) ;

		for (pir = ic.ifc_req, i = 0 ; i < n; i++, pir++)
		{
			if ((strcasecmp(p, pir->ifr_name)) == 0)
			{
				memcpy(&ipaddr, &pir->ifr_addr.sa_data[2], sizeof(ipaddr)) ; 	// skip sin_family member
				//printf("found %s, ip=%02.2X\n", pir->ifr_name, ipaddr ) ;
				break ;
			}
		}

	}
	//else
	//	perror("ioctl: SIOCGIFCONF: %m");

	close(sock) ;

    return ipaddr ;
}

