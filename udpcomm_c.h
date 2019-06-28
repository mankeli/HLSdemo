#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#define FRAMETYPE_TILEDATA 1
#define FRAMETYPE_SWAP 2
typedef struct
{
  union
  {
  struct
  {
    uint8_t type;
    uint8_t frame;
    union
    {
      struct
      {
        uint16_t xpos;
        uint16_t ypos;
      };
      struct
      {
      };
    };
  };
    char siz[8];
  };
} packethdr_t;


typedef struct
{
	packethdr_t hdr;
	uint8_t payload[2048];
} packet_t;

typedef struct
{
	int x,y,w,h;
	struct sockaddr_in si_other;
} paneldef_t;

typedef struct
{
	int panelcount;
	paneldef_t panels[17];
	int sock;
} hlsudpcomm_t;

static void initpanel(paneldef_t* panel, int x, int y, int w, int h, char* ip, int port)
{
		panel->x = x;
		panel->y = y;
		panel->w = w;
		panel->h = h;

		memset((char *) &panel->si_other, 0, sizeof(struct sockaddr_in));
		panel->si_other.sin_family = AF_INET;
		panel->si_other.sin_port = htons(port);
		if (inet_aton(ip, &panel->si_other.sin_addr)==0)
		{
			printf("inet_aton() failed\n");
		}
}

static int getpanelconfig(hlsudpcomm_t* ctx)
{
	int x,y,w,h;
	char ip[256];
	int port;


	char* panelcfgpath;
	panelcfgpath = getenv ("PANELCONFIG");
	if (!panelcfgpath)
	{
		fprintf(stderr,"set PANELCONFIG env var\n");
		return 1;
	}

	FILE *file = fopen(panelcfgpath, "r");

	ctx->panelcount = 0;

	for (int i = 0; i < 17; i++)
	{
		if (6 != fscanf(file, "%d,%d,%d,%d, %[^,] ,%d", &x, &y, &w, &h, ip, &port))
			break;

		printf ("got panel %i: %i %i %i %i %s %i\n", i, x,y,w,h,ip,port);

		initpanel(&ctx->panels[i], x, y, w, h, ip, port);
		ctx->panelcount++;
	}

	return 0;
}


static hlsudpcomm_t* hlsudp_open(void)
{
	hlsudpcomm_t* ctx = (hlsudpcomm_t*)malloc(sizeof(hlsudpcomm_t));

	if ((ctx->sock=socket(AF_INET, SOCK_DGRAM/* | SOCK_NONBLOCK*/, IPPROTO_UDP)) == -1)
	{
		printf("no socket created\n");
		ctx->sock = 0;
		return NULL;
	}

	int bcflag=1;
	int ret = setsockopt(ctx->sock, SOL_SOCKET, SO_BROADCAST, &bcflag, sizeof(bcflag));

	if (ret)
	{
		printf("couldn't set sock to broadcast\n");
	}

	int sndbufsiz = 64*1024*1024;
	socklen_t sndbufsiz_siz = sizeof(sndbufsiz);
	setsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF, &sndbufsiz, sndbufsiz_siz);
	getsockopt(ctx->sock, SOL_SOCKET, SO_SNDBUF, &sndbufsiz, &sndbufsiz_siz);
	printf("sndbufsiz %i, %i\n", sndbufsiz, sndbufsiz_siz);

	if (getpanelconfig(ctx))
	{
		printf("getpanelconfig error\n");
		return NULL;
	}
	return ctx;
}


static int hlsudp_send(hlsudpcomm_t* ctx, paneldef_t* panel, uint8_t *buf, size_t size)
{
	//printf("sending %i bytes to s %i\n", size, m_s);

	int sent = sendto(ctx->sock, buf, size, 0, (const struct sockaddr *)&panel->si_other, sizeof(struct sockaddr_in));  
	if (sent == -1)
	{
//		printf("your buffer got lost! :-D\n");
	}

	if (size > 0 && 0)
	{
		int i;
		for (i = 0; i < sent; i++)
		{
			printf("%02X ", buf[i]);

		}
		printf("\n");
	}

	//exit(1);


	return sent;
}


static void hlsudp_sendtile(hlsudpcomm_t* ctx, uint8_t *pixels, int pixelsize, int frame, int xo, int yo)
{
	int tilesize_x = 16;
	int tilesize_y = 16;

	packet_t tmp;
	tmp.hdr.type = FRAMETYPE_TILEDATA;
	tmp.hdr.frame = frame & 255;
	memcpy(&tmp.payload, pixels, tilesize_x*tilesize_y*pixelsize);

	size_t packsiz = ((uintptr_t)tmp.payload - (uintptr_t)&tmp) + tilesize_x*tilesize_y*pixelsize;

//	if (packsiz > 1200)
//		packsiz = 1200;
	for (int i = 1; i < ctx->panelcount; i++)
	{
		int drx = xo+tilesize_x;
		int dry = yo+tilesize_y;

		if (xo >= (ctx->panels[i].x+ctx->panels[i].w) || yo >= (ctx->panels[i].y+ctx->panels[i].h))
			continue;
		if (drx <= ctx->panels[i].x || dry <= ctx->panels[i].y)
			continue;

		int xpos = xo - ctx->panels[i].x;
		int ypos = yo - ctx->panels[i].y;

		tmp.hdr.xpos = xpos;
		tmp.hdr.ypos = ypos;

#if 0
		printf("sending to panel %i, %i-%i=%i=%i, %i-%i=%i=%i\n", i,
			xo, ctx->panels[i].x, xpos,tmp.hdr.xpos,
			yo, ctx->panels[i].y, ypos, tmp.hdr.ypos);
#endif
		
		hlsudp_send(ctx, &ctx->panels[i], (uint8_t*)&tmp, packsiz);
	}
}

static void hlsudp_sendswap(hlsudpcomm_t* ctx, int frame)
{
	packet_t tmp;
	tmp.hdr.type = FRAMETYPE_SWAP;
	tmp.hdr.frame = frame & 255;
	size_t packsiz = sizeof(packethdr_t);

	hlsudp_send(ctx, &ctx->panels[0], (uint8_t*)&tmp, packsiz);
}

static void hlsudp_shutdown(hlsudpcomm_t* ctx)
{
	 close(ctx->sock);
	 free(ctx);
}
