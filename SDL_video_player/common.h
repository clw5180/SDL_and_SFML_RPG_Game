#pragma once

#include <iostream>
#include <stdio.h>
#include <assert.h>

#include <SDL.h>
#include <SDL_thread.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h> //for av_gettime()
}

using namespace std;

#define MAX_AUDIO_FRAME_SIZE 192000 /*176400*/
#define SDL_AUDIO_BUFFER_SIZE 1024
#define FILENAME "1.mp4"

#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)  //TODO：怎么来的？
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)

#define AV_SYNC_THRESHOLD 0.01
#define AV_NOSYNC_THRESHOLD 10.0

//Refresh Event
#define FF_ALLOC_EVENT  (SDL_USEREVENT + 1)
#define FF_REFRESH_EVENT  (SDL_USEREVENT + 2)
#define FF_QUIT_EVENT (SDL_USEREVENT + 3)

#define VIDEO_PICTURE_QUEUE_SIZE 1

typedef struct PacketQueue
{
	AVPacketList *first_pkt; // 队头
	AVPacketList *last_pkt; // 队尾

	int nb_packets; //包的个数
	int size; // 占用空间的字节数
	SDL_mutex* mutext; // 互斥信号量
	SDL_cond* cond; // 条件变量
}PacketQueue;

typedef struct VideoPicture //clw note：保存每一帧图片的纹理，高、宽、是否已经分配等信息
{
	SDL_Texture *bmp;  //clw note：VideoState 中有个缓冲区用来存储 VideoPicture，
	                    //但是我们需要自己创建和分配 SDL_Overlay 的内存，
	                    //注意，下面的allocated 就是用来标记我们有没有做这件事。
	int width, height; // Source height & width.
	int allocated;
	double pts;
} VideoPicture;

//容纳所有音视频信息
typedef struct VideoState
{
	AVFormatContext *pFormatCtx; //格式信息

	/* video */
	int videoStreamIndex; //视频流索引标记
	double frame_timer;
	double frame_last_pts;
	double frame_last_delay;
	double video_clock; // pts of last decoded frame / predicted pts of next decoded frame.
	AVStream *video_st; //对应的视频流对象
	PacketQueue videoq; //视频数据队列
	VideoPicture pictq[VIDEO_PICTURE_QUEUE_SIZE];  //视频数据缓冲区，存储解码后的视频帧
	int pictq_size, pictq_rindex, pictq_windex; //TODO：具体含义不明

	/* audio */
	int audioStreamIndex; // 音频流索引标记
	AVStream* audio_st; //对应的音频流对象
	PacketQueue audioq; //音频数据队列
	uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
	unsigned int audio_buf_size;
	unsigned int audio_buf_index;
	AVFrame audio_frame;
	AVPacket audio_pkt;
	uint8_t *audio_pkt_data;
	int audio_pkt_size;
	double audio_clock;

	///* sdl */
	SDL_mutex *pictq_mutex; //视频渲染相关数据缓冲区 pictq 的锁
	                        //接着，初始化视频渲染相关数据缓冲区 pictq 的锁。因为事件循环调用我们的
	                        //渲染函数时，渲染逻辑就会从 pictq 获取数据，同时解码逻辑又会往 pictq 
	                        //写入数据，我们不知道谁会先到，所以这里需要通过锁机制来防止线程错乱
	SDL_cond *pictq_cond;

	SDL_Thread *parse_tid;
	SDL_Thread *video_tid;
	SDL_Thread *video_refresh_tid;

	char fileName[128];
	int quit; //退出标志

	AVIOContext *io_context;
	struct SwsContext *sws_ctx; //clw note:SwsContext(software scale)主要用于视频图像的转换，比如格式转换.
								//        SwrContext(software resample)主要用于音频重采样，比如采样率转换，声道转换。
}VideoState;

extern int quit;
extern uint8_t *audio_buff;
extern int screen_w;
extern int screen_h;
extern SDL_Window *screen;
extern SDL_Rect sdlRect;
extern SDL_Renderer *sdlRenderer;
extern SDL_mutex *screen_mutex;
//extern VideoState *global_video_state;

int sfp_refresh_thread(void *opaque);
// 包队列初始化
void packet_queue_init(PacketQueue* q);
// 放入packet到队列中，不带头指针的队列
int packet_queue_put(PacketQueue*q, AVPacket *pkt);
// 从队列中取出packet
/*static*/ int packet_queue_get(PacketQueue* q, AVPacket* pkt, bool block);

double get_audio_clock(VideoState *is);
// 把packet解码为frame
int audio_decode_frame(AVCodecContext* aCodecCtx, uint8_t* _audio_buff, int buf_size, VideoState *is, double *pts_ptr); //TODO：只要一个is变量就可以了，剩下都删掉；注意音频初始化的地方吧buf_size写了就可以
// 音频处理回调函数，过一定时间就会调用，取解码后的音频数据（如果有，没有则返回）给音频设备
void audio_callback(void* userdata, Uint8* stream, int len);
// 在指定的毫秒数之后运行的回调函数
Uint32 sdl_refresh_timer_cb(Uint32 interval, void *opaque); //TODO cb改成callback
//系统在指定的延时后调用回调函数sdl_refresh_timer_cb，来推送一个FF_REFRESH_EVENT事件，
//从而在主函数可以进行事件处理，调用刷新视频函数video_refresh_timer
void schedule_refresh(VideoState *is, int delay);
// 【子函数】显示视频
void video_display(VideoState *is);
// 刷新视频
void video_refresh_timer(void *userdata); 
// 分配图片
void alloc_picture(void *userdata);
// 解封装线程，packet进队列
int decode_thread(void *arg);

double synchronize_video(VideoState *is, AVFrame *src_frame, double pts);

// These are called whenever we allocate a frame buffer. We use this to store the global_pts in a frame at the time it is allocated.
int our_get_buffer(struct AVCodecContext *c, AVFrame *pic, int flags);
// 视频处理线程
int video_thread(void *arg);


