#include "common.h"

//////////////////////////////////////////////
//本代码参考：https://github.com/samirchen/TestFFmpeg/blob/master/tutorial04/tutorial04.c 
//////////////////////////////////////////////

int audio_buff_size = 0;
int out_buffer_size = 0;
int thread_exit = 0;
int thread_pause = 0;

AVFrame *pFrameYUV;

uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;


//int sfp_refresh_thread(void *opaque)
//{
//	thread_exit = 0;
//	thread_pause = 0;
//
//	while (!thread_exit)
//	{
//		if (!thread_pause)
//		{
//			SDL_Event event;
//			event.type = FF_REFRESH_EVENT;
//			SDL_PushEvent(&event);
//		}
//		SDL_Delay(40); //clw note：视频一般都是每秒25帧
//	}
//	thread_exit = 0;
//	thread_pause = 0;
//	//Break
//	SDL_Event event;
//	event.type = FF_BREAK_EVENT;
//	SDL_PushEvent(&event);
//
//	return 0;
//}

// 包队列初始化
void packet_queue_init(PacketQueue* q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->last_pkt = nullptr;
	q->first_pkt = nullptr;
	q->mutext = SDL_CreateMutex();
	q->cond = SDL_CreateCond();
}

// 放入packet到队列中，不带头指针的队列
int packet_queue_put(PacketQueue*q, AVPacket *pkt)
{
	AVPacketList *pktl;
	/*clw note:
	注意，在调用packet_queue_put时传递的是指针，也就是形参pkt和实参packet中的data引用的是同一个数据缓存。
	但是在循环调用av_read_frame的时候，会将packet中的data释放掉，以便于读取下一个帧数据。
	所以就需要对data引用的数据缓存进行处理，保证在读取下一个帧数据的时候，其data引用的数据空间没有被释放。
	有两种方法，复制一份data引用的数据缓存，或者给data引用的缓存空间加一个引用计数。
	这里采用的是第一种方法，av_dup_packet
	*/
	if (av_dup_packet(pkt) < 0) //在FFmpeg3已废弃，该函数将pkt中data引用的数据缓存复制一份给队列节点中的AVPacket。
		return -1;

	pktl = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if (!pktl)
		return -1;

	pktl->pkt = *pkt;
	pktl->next = nullptr;

	SDL_LockMutex(q->mutext);

	if (!q->last_pkt) // 队列为空，新插入元素为第一个元素
		q->first_pkt = pktl; //那么就让链表的头指针指向第一个元素
	else // 如果队列不为空，则插入队尾
		q->last_pkt->next = pktl; //当前尾指针对应链表节点的next指向即将插入的packet

	q->last_pkt = pktl; //更新链表尾指针，指向插入的packet

	q->nb_packets++; //nb_packets指的是队列内拥有的packet个数，++是因为插入了1个packet

	q->size += pkt->size;  // 队列中的size属性指的是插入的packet的总大小，因此是+=
							//clw note：也可以写成pkl->pkt->size

	SDL_CondSignal(q->cond); //通过我们的条件变量为一个接收函数（如果它在等待）发出一个信号
							 //来告诉它现在已经有数据了，接着就会解锁互斥量并让队列可以自由访问
	SDL_UnlockMutex(q->mutext);

	return 0;
}

// 从队列中取出packet
int packet_queue_get(PacketQueue* q, AVPacket* pkt, bool block)
{
	AVPacketList* pktl;
	int ret;

	SDL_LockMutex(q->mutext);

	while (true)
	{
		if (quit)
		{
			ret = -1;
			break;
		}

		pktl = q->first_pkt;  //pkt指向链表的头指针
		if (pktl)    //如果头指针非空，说明队列中是有packet的，可以准备取出packet
		{
			q->first_pkt = pktl->next;  //在把第一个packet取出来之前，要先让链表头指针指向下一个节点
			if (!q->first_pkt)  //如果当前队列只有1个packet，那么此时链表头指针指向下一个节点会是NULL
				q->last_pkt = nullptr;  //因此把尾指针写为NULL，代表队列为空。这样下次packet进队列，判断
			                            //q->last_pkt是否为空的时候就有了依据，具体可见方法packet_queue_put

			q->nb_packets--;          //取出了1个packet，队列中packet的总数减少
			q->size -= pktl->pkt.size;  //队列中所有packet占总的size大小也相应减少了pktl->pkt.size

			*pkt = pktl->pkt;  //将packet的地址，也就是最开始时候的q->first_pkt对应的packet的对象写入pkt这个指针所指向的内存空间
			av_free(pktl); //释放链表的头结点
			ret = 1;   //代表取出packet的个数为1，最后会return出来
			break;
		}
		else if (!block)  //代表如果是非阻塞的，而且头指针还是空的（因为有else），则直接return 0，代表根本没有取出packet。
		{
			ret = 0;
			break;
		}
		else  //还有一种情况是头指针还是空的，却是阻塞的状况，说明虽然队列目前长度为0，但是正在把packet放入队列，只是还没完成，那么
		{
			SDL_CondWait(q->cond, q->mutext); //。注意函数SDL_CondWait()是如何按照我们的要求让
											 //函数阻塞block的（例如一直等到队列中有数据）。
		}
	}

	SDL_UnlockMutex(q->mutext);

	return ret;
}

double get_audio_clock(VideoState *is) 
{
	double pts;
	int hw_buf_size, bytes_per_sec, n;

	pts = is->audio_clock; // Maintained in the audio thread.
	hw_buf_size = is->audio_buf_size - is->audio_buf_index;
	bytes_per_sec = 0;
	n = is->audio_st->codec->channels * 2;
	if (is->audio_st) {
		bytes_per_sec = is->audio_st->codec->sample_rate * n;
	}
	if (bytes_per_sec) {
		pts -= (double)hw_buf_size / bytes_per_sec;
	}
	return pts;
}

int audio_decode_frame(AVCodecContext* aCodecCtx, uint8_t* _audio_buff, int buf_size, VideoState *is, double *pts_ptr)
{
	static AVPacket pkt;
	static uint8_t* audio_pkt_data = nullptr;
	static int audio_pkt_size = 0;
	static AVFrame frame;

	int len1;
	int data_size = 0;
	static int audioCnt = 0;
	double pts;
	int n;
	/************************************************************************/
	/* clw note:
	新版本中使用了最新版本的FFMPEG类库（2014.5.7）。FFMPEG在新版本中的音频解码方面
	发生了比较大的变化。如果将旧版的主程序和新版的类库组合使用的话，会出现听到的
	都是杂音这一现象。经过研究发现，新版中avcodec_decode_audio4()解码后输出的
	音频采样数据格式为AV_SAMPLE_FMT_FLTP（float, planar）而不再是
	AV_SAMPLE_FMT_S16（signed 16 bits）。因此无法直接使用SDL进行播放。
	最后的解决方法是使用SwrContext对音频采样数据进行转换之后，再进行输出播放，
	问题就可以得到解决了。
	作者：雷霄骅  https ://blog.csdn.net/leixiaohua1020/article/details/10528443
																			*/
																			/************************************************************************/

	SwrContext* swr_ctx = nullptr;

	while (true)
	{
		while (audio_pkt_size > 0)
		{
			int got_frame = 0;
			/************************************************************************/
			/* clw note:
			需要注意的是，一般网上的代码只有int ret = avcodec_decode_audio4(pCodecCtx_A,
			pFrame_A, &frameFinished, packet);
			然后调用swr_convert进行转换成输出的PCM格式，这样的方法对于ADPCM压缩的音频文件或
			其他某些文件不奏效，声音会出现断裂从而听上去像加速播放的效果。
			这种方法实际上是理想化了，int nRead=av_read_frame(pFormatCtx, packet)分离（demux）
			出来的音频帧有可能并不止1帧，所以解码那里必须有个死循环，通过
			int ret = avcodec_decode_audio4(pCodecCtx_A, pFrame_A, &frameFinished, packet);
			解码的返回值判断解码了多少数据，然后改变解码数据的指针继续解码剩下的音频帧，
			直至解码完成！
			作者：周星星的星  https://blog.csdn.net/xjb2006/article/details/78980628
																					*/
																					/************************************************************************/
			len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);
			if (len1 < 0) // 出错，跳过
			{
				audio_pkt_size = 0;
				break;
			}

			audio_pkt_data += len1;
			audio_pkt_size -= len1;
			data_size = 0;
			if (got_frame)
			{
				data_size = out_buffer_size;
				assert(data_size <= buf_size); //clw note：这两个size的含义？
				memcpy(_audio_buff, frame.data[0], data_size); //clw note：frame.linesize[0]是8192，data_size是4096，用哪个？好像都没问题													 
				printf("audio index:%5d\t pts:%lld\t packet size:%d\n", audioCnt, pkt.pts, pkt.size);
				audioCnt++;
				/************************************************************************/
				/*
					解决杂音问题
					http://bbs.csdn.net/topics/390700255?page=1
					注意一点，ffmpeg新的版本，音频也像视频那样分为packed、plannar存储格式。
					作如下修改后，声音正常。
																						*/
																						/************************************************************************/
				if (frame.channels > 0 && frame.channel_layout == 0)
					frame.channel_layout = av_get_default_channel_layout(frame.channels);
				else if (frame.channels == 0 && frame.channel_layout > 0)
					frame.channels = av_get_channel_layout_nb_channels(frame.channel_layout);

				AVFrame wanted_frame;
				wanted_frame.format = AV_SAMPLE_FMT_S16;
				wanted_frame.sample_rate = frame.sample_rate;
				wanted_frame.channel_layout = av_get_default_channel_layout(frame.channels);
				wanted_frame.channels = frame.channels;

				if (swr_ctx)
				{
					swr_free(&swr_ctx);
					swr_ctx = nullptr;
				}
				swr_ctx = swr_alloc_set_opts(nullptr, wanted_frame.channel_layout, (AVSampleFormat)wanted_frame.format, wanted_frame.sample_rate,
					frame.channel_layout, (AVSampleFormat)frame.format, frame.sample_rate, 0, nullptr);
				if (!swr_ctx || swr_init(swr_ctx) < 0)
				{
					cout << "swr_init failed:" << endl;
					break;
				}
				int dst_nb_samples = (int)av_rescale_rnd(swr_get_delay(swr_ctx, frame.sample_rate) + frame.nb_samples,
					wanted_frame.sample_rate, wanted_frame.format, AVRounding(1));

				/*clw note：
				使用swr_convert转换成所需要的PCM格式，输出到第二个参数_audio_buff中。
				如果想输出PCM格式文件，可以添加如下代码（参考雷神）：
				#if OUTPUT_PCM
					//Write PCM
					fwrite(out_buffer, 1, out_buffer_size, pFile);
				#endif
				自注：这里
				out_buffer指向192000*2字节的内存空间；
				1代表要写入内容的单字节数：这里是按1个字节写入，因为
					out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);
					uint8_t其实就是unsigned char，占用1个字节的空间；
				out_buffer_size大小为1024*2*2=4096，应该就是回调函数每次传入Stream
					的字节大小，也就是所谓的audio_len
				*/
				int len2 = swr_convert(swr_ctx, &_audio_buff, dst_nb_samples, (const uint8_t**)frame.data, frame.nb_samples);
				if (len2 < 0)
				{
					cout << "swr_convert failed\n";
					break;
				}
				return wanted_frame.channels * len2 * av_get_bytes_per_sample((AVSampleFormat)wanted_frame.format);


				if (data_size <= 0)
					continue; // No data yet,get more frames

				pts = is->audio_clock;
				*pts_ptr = pts;
				n = 2 * is->audio_st->codec->channels;
				is->audio_clock += (double)data_size / (double)(n * is->audio_st->codec->sample_rate);

				return data_size; // we have data,return it and come back for more later
			}
		}
		if (pkt.data)
			av_free_packet(&pkt);

		if (quit)
			return -1;

		if (packet_queue_get(&is->audioq, &pkt, true) < 0)
			return -1;

		audio_pkt_data = pkt.data; //TODO：原文是pkt->data指针对象
		audio_pkt_size = pkt.size;

		// If update, update the audio clock w/pts. 音频时钟
		if (pkt.pts != AV_NOPTS_VALUE) 
		{
			is->audio_clock = av_q2d(is->audio_st->time_base) * pkt.pts;
		}
	}
}

// 解码后的回调函数
void audio_callback(void* userdata, Uint8* stream, int len)
{
	if (len <= 0)  
		return;   //TODO：个人认为应该加上这两句

	VideoState *is = (VideoState *)userdata;

	//AVCodecContext* aCodecCtx = (AVCodecContext*)userdata;
	int len1, audio_size;
	double pts;

	static unsigned int audio_buf_size = 0;
	static unsigned int audio_buf_index = 0;

	SDL_memset(stream, 0, len);

	while (len > 0)
	{
		if (audio_buf_index >= audio_buf_size)
		{
			audio_size = audio_decode_frame(is->audio_st->codec, audio_buff, audio_buff_size/*sizeof(audio_buff)*/, is, &pts); //clw note：返回4096，即1024*2*2
			if (audio_size < 0)
			{
				audio_buf_size = 1024;
				memset(audio_buff, 0, audio_buf_size);
			}
			else
				audio_buf_size = audio_size;

			audio_buf_index = 0;
		}
		len1 = audio_buf_size - audio_buf_index;
		if (len1 > len)
			len1 = len;

		SDL_MixAudio(stream, audio_buff + audio_buf_index, len, SDL_MIX_MAXVOLUME);


		//memcpy(stream, (uint8_t*)(audio_buff + audio_buf_index), audio_buf_size);
		len -= len1;
		stream += len1;
		audio_buf_index += len1;
	}
}

Uint32 sdl_refresh_timer_cb(Uint32 interval, void *opaque) 
{
	SDL_Event event;
	event.type = FF_REFRESH_EVENT;
	event.user.data1 = opaque;
	SDL_PushEvent(&event);
	return 0; // 0 means stop timer.
}

// Schedule a video refresh in 'delay' ms.
void schedule_refresh(VideoState *is, int delay) 
{
	int ret = SDL_AddTimer(delay, sdl_refresh_timer_cb, is); //这个定时器会触发一个事件来让 main 函数的事件处理逻辑去从 picture queue 取得一帧数据来显示出来
}

void video_display(VideoState *is) //由于我们的屏幕可以是任意尺寸（我们自己设置的是 640x480，但是这个对用户应该是可以改变的），
                                   //所以我们需要能够动态地计算我们要显示图像的尺寸。
{
	VideoPicture *vp;

	vp = &is->pictq[is->pictq_rindex];
	if (vp->bmp)
	{
		//clw note：旧版代码已弃用
		/*
		由于我们的屏幕可以是任意尺寸（我们自己设置的是 640x480，但是这个对用户应该是可以改变的），
		所以我们需要能够动态地计算我们要显示图像的尺寸。首先，我们需要计算出视频的 aspect ratio，
		即宽度和高度的比例(width/height)。但是有一些 codec 有很奇怪的 sample aspect ration，即
		单像素(单采样)的宽高比(width/height)，又由于我们的 AVCodecContext 中的宽度和高度是以像素
		为单位来表示的，那么这时候 actual aspect ratio 应该是 aspect ratio 乘上 sample aspect ratio。
		有的 codec 的 aspect ratio 值是 0，这表示的是每个像素的尺寸是 1x1。

		接下来，我们放大视频来尽量适配我们的屏幕。代码中的 & -3 位操作可以将数值调整到最接近 4 的倍数，
		然后我们将视频居中，并调用 SDL_DisplayYUVOverlay()，这里要确保我们通过 screen_mutex 来加锁。
																						        */
		/*if (is->video_st->codec->sample_aspect_ratio.num == 0) 
		{
			aspect_ratio = 0;
		}
		else 
		{
			aspect_ratio = av_q2d(is->video_st->codec->sample_aspect_ratio) * is->video_st->codec->width / is->video_st->codec->height;
		}
		if (aspect_ratio <= 0.0) 
		{
			aspect_ratio = (float)is->video_st->codec->width / (float)is->video_st->codec->height;
		}
		h = screen_h;
		w = ((int)rint(h * aspect_ratio)) & -3;
		if (w > screen_w)
		{
			w = screen_w;
			h = ((int)rint(w / aspect_ratio)) & -3;
		}
		x = (screen_w - w) / 2;
		y = (screen_h - h) / 2;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;*/


		//clw modify 20190301：main函数中增加SDL_WINDOWEVENT事件和SDL_GetWindowSize函数，
		//                     一旦窗口大小改变立刻修改对应的screen_w和screen_h，然后在这里进行更新
		sdlRect.x = 0;
		sdlRect.y = 0;
		sdlRect.w = screen_w;
		sdlRect.h = screen_h;

		//SDL Begin---------------------------
		SDL_LockMutex(screen_mutex);
		SDL_UpdateTexture(vp->bmp, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]); //clw note：之前第三个参数是从.YUV文件中读进来的输入存入的一个buffer;这里pFrameYUV->linesize[0]是视频的宽，如640*480的视频，该值就是640；
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, vp->bmp, NULL, &sdlRect);
		SDL_RenderPresent(sdlRenderer);
		SDL_UnlockMutex(screen_mutex);
		//SDL End-----------------------
	}
}

void video_refresh_timer(void *userdata)   	//根据PTS计算刷新时间间隔
{
	/*
	现在假设我们有一个 get_audio_clock 函数来返回我们 audio clock，那当我们拿到这个值，
	我们怎么去处理音视频不同步的情况呢？如果只是简单的尝试跳到正确的 packet 来解决并不是
	一个很好的方案。我们要做的是调整下一次刷新的时机：如果视频播慢了我们就加快刷新，如果
	视频播快了我们就减慢刷新。既然我们调整好了刷新时间，接下来用 frame_timer 跟电脑的时钟
	做一下比较。frame_timer 会一直累加在播放过程中我们计算的延时。换而言之，这个 
	frame_timer 就是播放下一帧的应该对上的时间点。我们简单的在 frame_timer 上累加新计算的 
	delay，然后和电脑的时钟比较，并用得到的值来作为时间间隔去刷新。这段逻辑需要好好阅读一下
	下面的代码

	*/


	VideoState *is = (VideoState *)userdata;
	VideoPicture *vp;
	double actual_delay, delay, sync_threshold, ref_clock, diff;

	if (is->video_st) 
	{
		if (is->pictq_size <= 0) //TODO：原为==0
		{
			schedule_refresh(is, 1);
		}
		else  //当 pictq 队列有数据时就取出 VideoPicture，设置显示下一帧图像的 timer，
			  //调用 video_display() 来将视频显示出来，增加队列的计数器，更新队列的 size。
		{
			vp = &is->pictq[is->pictq_rindex];

			delay = vp->pts - is->frame_last_pts; // The pts from last time.
			if (delay <= 0 || delay >= 1.0) {
				// If incorrect delay, use previous one.
				delay = is->frame_last_delay;
			}
			// Save for next time.
			is->frame_last_delay = delay;
			is->frame_last_pts = vp->pts;

			// Update delay to sync to audio.
			ref_clock = get_audio_clock(is);
			diff = vp->pts - ref_clock;

			// Skip or repeat the frame. Take delay into account FFPlay still doesn't "know if this is the best guess."
			sync_threshold = (delay > AV_SYNC_THRESHOLD) ? delay : AV_SYNC_THRESHOLD;
			if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
				if (diff <= -sync_threshold) {
					delay = 0;
				}
				else if (diff >= sync_threshold) {
					delay = 2 * delay;
				}
			}
			is->frame_timer += delay;
			// Computer the REAL delay.
			actual_delay = is->frame_timer - (av_gettime() / 1000000.0);
			if (actual_delay < 0.010) {
				// Really it should skip the picture instead.
				actual_delay = 0.010;
			}
			schedule_refresh(is, (int)(actual_delay * 1000 + 0.5));

			// Now, normally here goes a ton of code about timing, etc. we're just going to guess at a delay for now. You can increase and decrease this value and hard code the timing - but I don't suggest that ;) We'll learn how to do it for real later..
			// 之前为schedule_refresh(is, 80);

			// Show the picture!
			video_display(is);

			// Update queue for next picture!
			if (++is->pictq_rindex == VIDEO_PICTURE_QUEUE_SIZE) 
			{
				is->pictq_rindex = 0;
			}
			SDL_LockMutex(is->pictq_mutex);
			is->pictq_size--;
			SDL_CondSignal(is->pictq_cond);
			SDL_UnlockMutex(is->pictq_mutex);

			SDL_LockMutex(is->pictq_mutex);
			SDL_CondSignal(is->pictq_cond);
			SDL_UnlockMutex(is->pictq_mutex);

		}
	}
	else 
	{
		schedule_refresh(is, 1);
	}
}

void alloc_picture(void *userdata)
{
	VideoState *is = (VideoState *)userdata;
	VideoPicture *vp;

	vp = &is->pictq[is->pictq_windex];
	if (vp->bmp)
	{
		SDL_DestroyTexture(vp->bmp); //TODO：尚未确定是否正确
	}


	//clw note：后两个参数的宽和高应该是数据帧的宽和高，也就是屏幕的宽和高，也就是视频的宽和高
	//          SDL_CreateTexture在SDL1.2中为SDL_CreateYUVOverlay，同样对应关系还有SDL_Overlay————SDL_Texture，
	//          SDL_SetVideoMode()————SDL_CreateWindow()，SDL_Surface————SDL_Window等；
	SDL_LockMutex(screen_mutex);
	vp->bmp = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, is->video_st->codec->width, is->video_st->codec->height);
	SDL_UnlockMutex(screen_mutex);

	vp->width = is->video_st->codec->width;  //TODO：确保视频尺寸不会发生改变？
	vp->height = is->video_st->codec->height;

	SDL_LockMutex(is->pictq_mutex);
	vp->allocated = 1;
	SDL_CondSignal(is->pictq_cond);
	SDL_UnlockMutex(is->pictq_mutex);

}



int decode_thread(void *arg) //clw note：参数是在SDL_CreateThread配置的，这里传入VideoState*类型对象is
{
	VideoState *is = (VideoState*)arg;
	uint8_t *out_buffer;
	struct SwsContext *img_convert_ctx;

	// 打开文件，读取流信息
	AVFormatContext* pFormatCtx = nullptr;
	// 读取文件头，将格式相关信息存放在AVFormatContext结构体中
	if (avformat_open_input(&pFormatCtx, FILENAME, nullptr, nullptr) != 0)
		return -1; // 打开失败

	is->pFormatCtx = pFormatCtx; //clw note：存入VideoState类型对象

	// 检测文件的流信息
	if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)
		return -1; // 没有检测到流信息 stream infomation

	// 在控制台输出文件信息
	av_dump_format(pFormatCtx, 0, FILENAME, 0);

	//查找视频流与音频流对应的Index
	int audioStreamIndex = -1;
	int videoStreamIndex = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStreamIndex = i;
		}
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamIndex = i;
		}
	}
	if (audioStreamIndex == -1 || videoStreamIndex == -1)
	{
		SDL_Event event;
		event.type = FF_QUIT_EVENT;
		event.user.data1 = is;
		SDL_PushEvent(&event);
		printf("没有查找到音频流或视频流!!!\n");
		return -1; // 没有查找到音频流或视频流
	}

	AVCodecContext* pCodecCtx = nullptr;
	AVCodec* pCodec = nullptr;

	//  处理视频流
	// （1）得到指向视频流编解码器上下文的指针，然后打开解码器
	pCodecCtx = pFormatCtx->streams[videoStreamIndex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);//找出解码器，比如是H.264或MPEG2的
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) //打开解码器
	{
		printf("Could not open codec.\n");
		return -1;
	}

	pFrameYUV = av_frame_alloc();
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	///////////////////////////////////////////////////
	//clw note：保存视频流信息
	is->videoStreamIndex = videoStreamIndex;
	is->video_st = pFormatCtx->streams[videoStreamIndex];
	
	is->frame_timer = (double)av_gettime() / 1000000.0;
	is->frame_last_delay = 40e-3;

	packet_queue_init(& is->videoq);
	is->video_tid = SDL_CreateThread(video_thread, "video_thread", is);
	is->sws_ctx = img_convert_ctx;
	///////////////////////////////////////////////////


	//  处理音频流
	// （2）得到指向音频流编解码器上下文的指针
	pCodecCtx = pFormatCtx->streams[audioStreamIndex]->codec; // codec context
	// 找到audioStreamIndex的decoder
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id); // 3. 根据读取到的流信息查找相应的解码器并打开
	if (!pCodec)
	{
		cout << "Unsupported codec!" << endl;
		return -1;
	}

	// Open codec
	avcodec_open2(pCodecCtx, pCodec, nullptr);

	/************************************************************************/
	/* 学习雷神                                                  */
	/************************************************************************/
	//Out Audio Param
	uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	//AAC:1024  MP3:1152
	int out_nb_samples = pCodecCtx->frame_size; //clw note：一帧包含的采样点个数，AAC编码一般是1024
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_sample_rate = 44100; //clw note：采样率，常见的包括22050,44100和48000Hz
	int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer Size：大小为1024*2*2 = 4096
	out_buffer_size = av_samples_get_buffer_size(nullptr, out_channels, out_nb_samples, out_sample_fmt, 1);
	audio_buff_size = MAX_AUDIO_FRAME_SIZE * 2; //clw note：也有乘3/2的
	audio_buff = (uint8_t*)av_malloc(audio_buff_size);
	/************************************************************************/

	// Set audio settings from codec info
	SDL_AudioSpec wanted_spec, spec; //clw note：spec暂时没用到
	wanted_spec.freq = out_sample_rate;
	wanted_spec.format = AUDIO_S16SYS;  //clw note：.format 告诉SDL我们将要给的格式。
									   //在S16SYS中的S表示有符号的signed，
									  //16表示每个样本是16位长的，SYS表示大端/小端的顺序
									  //是与使用的系统相同的。这些格式是由avcodec_decode_audio
									  //为我们给出来的输入音频的格式。
	wanted_spec.channels = out_channels;  //双声道值为2
	wanted_spec.silence = 0;              //0代表非静音模式
	wanted_spec.samples = out_nb_samples; //这是当我们想要更多声音的时候，我们想让SDL给出来的声音缓冲区的尺寸
										 //一个比较合适的值在512到8192之间；一般取1024。
	wanted_spec.userdata = is;
	wanted_spec.callback = audio_callback; //这个是SDL供给回调函数运行的参数。我们将让回调函数得到整个编解码的上下文
	//这个回调函数负责不断的播放声音,那么这个函数从那里取出声音呢?这时候我需要申请一块内存用来存放声音,
	//回调函数不断的读取数据从我申请的内存中, 这块内存就是队列PacketQueue.
	//PacketQueue通过小链表AVPacketList把音频帧AVPacket组成一个顺序队列，nb_packets为AVPacket的数量,size为AVPacket.size的总大小

	//如果你的程序能够处理不同的音频格式，把一个SDL_AudioSpec的指针作为SDL_OpenAudio() 的第二个参数可以取得硬件真正的音频格式。如果第二个参数是NULL，音频数据将在运行时被转换成硬件格式。
	if (SDL_OpenAudio(&wanted_spec, /*&spec*/ NULL) < 0)
		//clw note：这里是个问题，如果写了spec，就要把频率降为一半？？
	{
		cout << "Open audio failed:" << SDL_GetError() << endl;
		return -1;
	}

	///////////////////////////////////////////////////
	//clw note：保存音频流信息  TODO
	is->audioStreamIndex = audioStreamIndex;
	is->audio_st = pFormatCtx->streams[audioStreamIndex];
	//is->audio_buf_size = 0;
	//is->audio_buf_index = 0;
	//memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
	///////////////////////////////////////////////////


	/************************************************************************/
	/* 学习雷神                                                  */
	/************************************************************************/
	//FIX:Some Codec's Context Information is missing
	int64_t in_channel_layout = av_get_default_channel_layout(pCodecCtx->channels);
	//Swr
	struct SwrContext *au_convert_ctx;
	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, out_channel_layout, out_sample_fmt/*out：比如AV_SAMPLE_FMT_S16*/, out_sample_rate,
		in_channel_layout, pCodecCtx->sample_fmt /*in：比如我这里是AV_SAMPLE_FMT_FLTP*/, pCodecCtx->sample_rate, 0, NULL);
	swr_init(au_convert_ctx);
	/************************************************************************/
	packet_queue_init(&is->audioq);
	SDL_PauseAudio(0);  //clw note：开启音频处理部分，会反复在一定时间后回调audio_callback


	AVPacket *packet = (AVPacket *)malloc(sizeof(AVPacket));
	av_init_packet(packet);
	int count_read_frame = 0;
	int count_audioStream = 0;
	int count_videoStream = 0;

	//clw modify 20190301 
	while (1)
	{
		if (is->quit) 
		{
			break;
		}

		// Seek stuff goes here.   TODO
		if (is->audioq.size > MAX_AUDIOQ_SIZE || is->videoq.size > MAX_VIDEOQ_SIZE) 
		{
			SDL_Delay(10);
			continue;
		}
		if (av_read_frame(is->pFormatCtx, packet) < 0) //clw note：读取1帧压缩数据存入packet，之后packet_queue_put
		{
			if (is->pFormatCtx->pb->error == 0) //TODO:???
			{
				SDL_Delay(100);// No error; wait for user input.  //TODO这个延迟100ms没影响么
				continue;
			}
			else
				break; 
		}
			
		//else
		//{
			if (packet->stream_index == audioStreamIndex)
			{
				packet_queue_put(&is->audioq, packet);//如果是音频流则将读到Packet调用packet_queue_put插入到队列，
				count_audioStream++;
			}
			else if (packet->stream_index == videoStreamIndex)
			{
				packet_queue_put(&is->videoq, packet);  //TODO：改成&is->videoq，F5宕掉？？
				count_videoStream++;
			}
			else
				av_free_packet(packet);//如果不是音频流则调用av_free_packet释放已读取到的AVPacket数据。
			count_read_frame++;
		//}
	}

	swr_free(&au_convert_ctx);//clw note：二级指针做输出的典型用法，修改一级指针的值
							  //Free the given SwrContext and set the pointer to NULL.

	SDL_CloseAudio(); //Close SDL
	SDL_Quit();
	av_free(audio_buff);
	avcodec_close(pCodecCtx);

	// All done - wait for it.
	while (!is->quit) {
		SDL_Delay(100);
	}

	return 0;
}

double synchronize_video(VideoState *is, AVFrame *src_frame, double pts) 
{
	double frame_delay;

	if (pts != 0) 
	{
		// If we have pts, set video clock to it.
		is->video_clock = pts;
	}
	else 
	{
		// If we aren't given a pts, set it to the clock.
		pts = is->video_clock;
	}
	// Update the video clock.
	frame_delay = av_q2d(is->video_st->codec->time_base); //TODO：av_q2d的含义？
	// If we are repeating a frame, adjust clock accordingly.
	frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
	is->video_clock += frame_delay;
	return pts;
}


int our_get_buffer(struct AVCodecContext *c, AVFrame *pic, int flags) 
{
	int ret = avcodec_default_get_buffer2(c, pic, 0);
	uint64_t *pts = (uint64_t *)av_malloc(sizeof(uint64_t));
	*pts = global_video_pkt_pts;
	pic->opaque = pts;
	return ret;
}

int video_thread(void *arg)
{
	static int videoCnt = 0;
	VideoState *is = (VideoState*)arg;
	AVPacket pkt1;
	AVPacket *packet = &pkt1;
	int frameFinished;
	AVFrame *pFrame;
	double pts;

	pFrame = av_frame_alloc();  //TODO：frame的free函数？
	
	while (1)
	{
		if (packet_queue_get(&is->videoq, packet, 1) < 0)
		{
			// Means we quit getting packets.
			break;
		}

		pts = 0;

		// Save global pts to be stored in pFrame in first call.
		global_video_pkt_pts = packet->pts;


		//解码一帧压缩数据packet，得到AVFrame*类型的pFrame
		int ret = avcodec_decode_video2(is->video_st->codec, pFrame, &frameFinished, packet);
		if (ret < 0)
		{
			printf("Decode Error.\n");
			return -1;
		}
		if (packet->dts == AV_NOPTS_VALUE && pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE) 
		{
			pts = *(uint64_t *)pFrame->opaque;  //TODO：warning C4244: “=”: 从“uint64_t”转换到“double”，可能丢失数据
		}
		else if (packet->dts != AV_NOPTS_VALUE) 
		{
			pts = packet->dts;  //TODO：warning C4244: “=”: 从“int64_t”转换到“double”，可能丢失数据
		}
		else 
		{
			pts = 0;  //当我们无法计算 PTS 时就设置它为 0。
		}
		pts *= av_q2d(is->video_st->time_base);



		if (frameFinished)  //TODO：之后替换为got_picture
		{
			pts = synchronize_video(is, pFrame, pts);

			//这里直接去掉了queue_picture函数
			VideoPicture *vp;
			// Wait until we have space for a new pic.
			SDL_LockMutex(is->pictq_mutex);
			while (is->pictq_size >= VIDEO_PICTURE_QUEUE_SIZE && !is->quit) 
			{
				SDL_CondWait(is->pictq_cond, is->pictq_mutex);  //clw note：先等待解锁，之后到下面alloc分配
			}
			SDL_UnlockMutex(is->pictq_mutex);

			if (is->quit)  //TODO：是否必要？
			{
				break;
			}

			// windex is set to 0 initially.
			vp = &is->pictq[is->pictq_windex];

			// Allocate or resize the buffer!
			if (!vp->bmp)
			{
				SDL_Event event;
				event.type = FF_ALLOC_EVENT;
				event.user.data1 = is;
				SDL_PushEvent(&event);

				// Wait until we have a picture allocated.
				SDL_LockMutex(is->pictq_mutex);
				while (!vp->allocated && !is->quit)
				{
					SDL_CondWait(is->pictq_cond, is->pictq_mutex);
				}
				SDL_UnlockMutex(is->pictq_mutex);

				if (is->quit)  //TODO：是否必要？
				{
					break;
				}
			}
			else // We have a place to put our picture on the queue.
			{
				//视频像素数据格式转换
				sws_scale(is->sws_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, is->video_st->codec->height, pFrameYUV->data, pFrameYUV->linesize);
				vp->pts = pts;
				// 现在我们需要通知视频显示部分，告诉它图片队列现在已经有图片了，
				// 所以下面调用is->pictq_size++，然后在video_refresh_timer中判断is->pictq_size>0则显示视频
				if (++is->pictq_windex == VIDEO_PICTURE_QUEUE_SIZE) 
				{
					is->pictq_windex = 0;
				}
				SDL_LockMutex(is->pictq_mutex);
				is->pictq_size++;  //读写操作都会依赖 is->pictq_size 的值，所以这里我们要给它加锁
				SDL_UnlockMutex(is->pictq_mutex);

				printf("videoCnt index:%5d\t pts:%lld\t packet size:%d\n", videoCnt, packet->pts, packet->size);
				videoCnt++;
			}
		}
		//else
		//{
		//	printf("clw note：frame hasn't finish yet!\n");
		//}
		av_packet_unref(packet); //TODO：是否必要？
	}
	av_free_packet(packet);
	return 0;
}

