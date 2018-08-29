
#include <stdint.h>
#include <inttypes.h>
#include <windows.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
	//#include <libavutil/imgutils.h>
#include <libavutil/avstring.h>
#include <libswresample/swresample.h>
	//#include <libswscale/swscale.h>

#define SDL_MAIN_HANDLED

#include <sdl2/SDL.h>
#include <sdl2/SDL_thread.h>
#include <sdl2/SDL_syswm.h>
#include <sdl2/SDL_render.h>
#include <sdl2/SDL_audio.h>
#ifdef __cplusplus
}
#endif



typedef struct _PacketQueue
{
	AVPacketList *first, *last;
	int nb_packets, size;
} PacketQueue;

void PacketQueueInit(PacketQueue * pq)
{
	memset(pq, 0, sizeof(PacketQueue));
}

int PacketQueuePut(PacketQueue * pq, const AVPacket * srcPkt)
{
	AVPacketList *elt;
	
	int rv;
	if (!pq) return -1;

	elt = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if (!elt) return -1;
	elt->pkt = *srcPkt;
	elt->next = NULL;

	if (!pq->last)          // check queue is empty, use last item
		pq->first = elt;
	else
		pq->last->next = elt;
	pq->last = elt;
	pq->nb_packets++;
	pq->size += elt->pkt.size;

	return 0;
}

int PacketQueueGet(PacketQueue *pq, AVPacket *pkt)
{
	AVPacketList * elt;
	int rv;

	if (!pq || !pkt) return -1;

		elt = pq->first;
		if (elt)
		{
			pq->first = elt->next;
			if (!pq->first)
				pq->last = NULL;					// queue is empty
			pq->nb_packets--;
			pq->size -= elt->pkt.size;
			*pkt = elt->pkt;
			av_free(elt);
			rv = 1;
		
		}

	return rv;
}

PacketQueue audioq;

int main()
{
	PacketQueueInit(&audioq);
	int i;
	AVPacket* pkt;


	for (i = 0; i < 10; i++)
	{
		pkt = av_packet_alloc();
		pkt->pos = i;
		PacketQueuePut(&audioq, pkt);
	
	}

	for (i = 0; i < 10; i++)
	{
	
		PacketQueueGet(&audioq, pkt);	
		printf("Packet pos : %d\n", pkt->pos);
		
	}

	printf("Number Of Packets Left : %d\n", audioq.nb_packets);
	printf("Size of Queue : %d\n", audioq.size);

	


		system("PAUSE");
	
		return 0;
}
