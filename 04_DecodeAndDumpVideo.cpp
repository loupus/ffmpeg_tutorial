#include <stdio.h>


extern "C"
{
	// include format and codec headers
#include <libavformat\avformat.h>
#include <libavcodec\avcodec.h>

}

void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize, FILE *f)
{
	// write header
	fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
	// loop until all rows are written to file
	for (int i = 0; i < ysize; i++)
		fwrite(buf + i * wrap, 1, xsize, f);	
}

void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,	FILE *f)
{
	char buf[1024];
	int ret;

	//send packet to decoder
	ret = avcodec_send_packet(dec_ctx, pkt);
	if (ret < 0) {
		fprintf(stderr, "Error sending a packet for decoding\n");
		exit(1);
	}
	while (ret >= 0) {
		// receive frame from decoder
		// we may receive multiple frames or we may consume all data from decoder, then return to main loop
		ret = avcodec_receive_frame(dec_ctx, frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			// something wrong, quit program
			fprintf(stderr, "Error during decoding\n");
			exit(1);
		}
		printf("saving frame %3d\n", dec_ctx->frame_number);
		fflush(stdout);
		// send frame info to writing function
		pgm_save(frame->data[0], frame->linesize[0],frame->width, frame->height, f);
	}
}


int main()
{

	// declare format and codec contexts, also codec for decoding
	AVFormatContext *fmt_ctx = NULL;
	AVCodecContext *codec_ctx = NULL;
	AVCodec *Codec = NULL;
	int ret;
	char *infilename = "D:\\Wildlife.wmv";
	char *outfilename = "D:\\Wildlife.yuv";
	int VideoStreamIndex = -1;

	FILE *fin = NULL;
	FILE *fout = NULL;

	AVFrame *frame = NULL;
	AVPacket *pkt = NULL;


	av_register_all();

	// open input file
	if (ret = avformat_open_input(&fmt_ctx, infilename, NULL, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot open input file\n");
		goto end;
	}

	//get stream info
	if (ret = avformat_find_stream_info(fmt_ctx, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "cannot get stream info\n");
		goto end;
	}

	// get video stream index
	for (int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			VideoStreamIndex = i;
			break;
		}
	}

	if (VideoStreamIndex < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "No video stream\n");
		goto end;
	}

	// dump video stream info
	av_dump_format(fmt_ctx, VideoStreamIndex, infilename, false);

	//alloc memory for codec context
	codec_ctx = avcodec_alloc_context3(NULL);

	// retrieve codec params from format context
	if (ret = avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[VideoStreamIndex]->codecpar) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot get codec parameters\n");
		goto end;
	}

	// find decoding codec
	Codec = avcodec_find_decoder(codec_ctx->codec_id);

	if (Codec == NULL)
	{
		av_log(NULL, AV_LOG_ERROR, "No decoder found\n");
		goto end;
	}

	// try to open codec
	if (ret = avcodec_open2(codec_ctx, Codec, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
		goto end;
	}

	printf("\nDecoding codec is : %s\n", Codec->name);

	//init packet
	pkt = av_packet_alloc();
	av_init_packet(pkt);
	if (!pkt)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot init packet\n");
		goto end;
	}

	// init frame
	frame = av_frame_alloc();
	if (!frame)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot init frame\n");
		goto end;
	}

	//open input file
	fin = fopen(infilename, "rb");
	if (!fin)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
		goto end;
	}

	// open output file
	fout = fopen(outfilename, "w");
	if (!fout)
	{
		av_log(NULL, AV_LOG_ERROR, "Cannot open output file\n");
		goto end;
	}

	// main loop
	while (1)
	{
		// read an encoded packet from file
		if (ret = av_read_frame(fmt_ctx, pkt) < 0)
		{
			av_log(NULL, AV_LOG_ERROR, "cannot read frame");
			break;
		}
		// if packet data is video data then send it to decoder
		if (pkt->stream_index == VideoStreamIndex)
		{
			decode(codec_ctx, frame, pkt, fout);
		}

		// release packet buffers to be allocated again
		av_packet_unref(pkt);
	}

	//flush decoder
	decode(codec_ctx, frame, NULL, fout);

	// clear and out
end:
	if (fin)
		fclose(fin);
	if (fout)
		fclose(fout);
	if (codec_ctx)
		avcodec_close(codec_ctx);
	if (fmt_ctx)
		avformat_close_input(&fmt_ctx);
	if (frame)
		av_frame_free(&frame);
	if (pkt)
		av_packet_free(&pkt);
	system("PAUSE");

	return 0;
}