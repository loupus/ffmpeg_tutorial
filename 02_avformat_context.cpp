#include <stdio.h>

// libavformat deals with the capsule of video

// if your code c++, you can include c headers with an extern encapsulation
extern "C"
{
#include <libavformat/avformat.h>
}

int main()
{	

	AVFormatContext *fmt_ctx = NULL;
	int ret;
	char *filename = "D:\\Wildlife.wmv";
	
	
	av_register_all();

	//fill format context with file info
	if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)))
		return ret;	

	// get streams info from format context
	if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
		return ret;
	}

	// loop streams and dump info
	for (int i = 0; i < fmt_ctx->nb_streams; i++)
	{
		av_dump_format(fmt_ctx, i, filename, false);		
	}

	// close format context
	avformat_close_input(&fmt_ctx);

	system("PAUSE");

	return 0;

}
