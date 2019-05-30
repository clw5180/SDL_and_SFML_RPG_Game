//   代码参考https://github.com/samirchen/TestFFmpeg/blob/master/tutorial04/tutorial04.c
//
//   clw note：几点注意：
//   1、main函数里做的事情应该只包括ffmpeg注册，SDL初始化，创建屏幕，创建线程和event loop；
//   2、其他事情比如读取数据包、进行视频解码等等都应该由其他的线程来执行；
//      （1）解码线程decode_thread来专门做数据包解码，这些数据包通过packet_queue_put分别被添加到
//           视频和音频队列中，并分别被音频处理和视频处理部分来读取和处理。     
//      （2）音频处理部分主要通过回调函数audio_callback，在其内部调用audio_decode_frame，
//           通过packet_queue_get读取音频数据包并进行处理；
//      （3）视频处理部分主要通过video_thread线程通过packet_queue_get读取视频数据包并进行处理，
//           显示部分主要是在调用schedule_refresh后，通过main函数接收到FF_REFRESH_EVENT事件后
//           调用video_refresh_timer函数中的video_display子函数进行显示；另外video_thread在
//           完成视频像素格式转换视频像素数据格式转换sws_scale之前
//           会先等待main函数调用alloc_picture函数，完成SDL_CreateTexture之后再进行。
//     流程图如下：
/*
              ________  audio   _______        _____
             |        | pkts   |       |      |     | to speaker.
             | DECODE | -----> | AUDIO | ---> | SDL | -- >
             |________|        |_______|      |_____|
                 |  video       _______
                 |  pkts       |       |
                 +---------- > | VIDEO |
               _______         |_______|      _______
              |       |            |         |       |
              | EVENT |            +------ > | VIDEO | to monitor.
              | LOOP  | -------------------> | DISP. | -- >
              |_______| <------FF_REFRESH----|_______|
                                                                         */

#include "common.h"

#ifdef __MINGW32__
#undef main // Prevents SDL from overriding main().
#endif 

//clw modify
#pragma comment(lib, "legacy_stdio_definitions.lib")
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

int quit = 0;
uint8_t *audio_buff = NULL;
int screen_w = 640 /*1152*/;
int screen_h = 480  /*720*/;
SDL_Window *screen;
SDL_mutex *screen_mutex;
SDL_Rect sdlRect;
SDL_Renderer *sdlRenderer;
/*
雷神给出的Audio Buffer结构
Buffer:
|-----------|-------------|
chunk-------pos---len-----|
static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;

1帧包含1024个samples，如果每个samples是16bit(2bype)的format存储，再是2通道的，
则1帧会占用1024*2*2 = 4096个byte;
采样率如果是44100个samples每秒，那么说明每一帧的执行时间为1024samples / 44100samples/s ≈ 23.2ms
另外1s执行的帧数为44100/1024 ≈ 43.1帧
*/

int main(int argv, char* argc[])
{
	//------------SDL----------------
	SDL_Event event;
	//--------------------------------
	VideoState *is;
	is = (VideoState *)av_mallocz(sizeof(VideoState));

	av_register_all();	//1.注册支持的文件格式及对应的codec

	/////////////////////////////////////////
	/* clw note： SDL部分开始*/
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
		exit(1);  
	}

	screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!screen)
	{
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}

	screen_mutex = SDL_CreateMutex();

	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//////////////////////////////////////////////////////
	is->pictq_mutex = SDL_CreateMutex();  
	is->pictq_cond = SDL_CreateCond();

	schedule_refresh(is, 40);

	is->parse_tid = SDL_CreateThread(decode_thread, "decode_thread", is);
	if (!is->parse_tid) //clw note：tid应该是thd，线程的意思吧 
	{
		av_free(is);
		return -1;
	}

	for (;;) 
	{
		SDL_WaitEvent(&event);
		switch (event.type) 
		{
		case SDL_QUIT:
			quit = 1;
			// If the video has finished playing, then both the picture and audio queues are waiting for more data.  Make them stop waiting and terminate normally..
			SDL_Quit();
			return 0;
		case FF_REFRESH_EVENT: //clw modify 20190228：暂时先不写video_thread，而把视频部分写在主线程
			//TODO
			video_refresh_timer(event.user.data1);
			break;
		case FF_ALLOC_EVENT:
			alloc_picture(event.user.data1);
			break;
		case SDL_WINDOWEVENT://clw modify 20190301：SDL内部包含的事件，在操作窗口的时候触发
			//If Resize
			SDL_GetWindowSize(screen, &screen_w, &screen_h); //clw note：通过重新获得拉伸后的screen_w和screen_h然后再传入SDL_RenderCopy，
			                                                 //这样可以起到类似拉伸视频播放窗口的效果！
			break;
		default:
			break;
		}
	}

	return 0;
}