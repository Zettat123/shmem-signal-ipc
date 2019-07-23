#include "sndcapture.h"

long long CaptureSound(char* mptr)
{
	int i;
	int err;
    int dir;
    long long ret_size = 0;
	char *buffer;
    char *oldptr = mptr;
    int buffer_size;
    long loops;
    int capture_time = 10; //second
    int periods_per_buffer = 1;
    unsigned int period_time;
	unsigned int rate = 44100;			// 采样频率：	44100Hz
    unsigned int channels = 2;

    snd_pcm_uframes_t frames = 128;
	snd_pcm_t *capture_handle;			// 一个指向PCM设备的句柄
	snd_pcm_hw_params_t *hw_params;		// 此结构包含有关硬件的信息，可用于指定PCM流的配置
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;		// 采样位数：16bit、LE格式

	// 打开音频采集卡硬件，并判断硬件是否打开成功，若打开失败则打印出错误提示
	if ((err = snd_pcm_open (&capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0) 
	{
		fprintf (stderr, "cannot open pcm device (%s)\n",  snd_strerror (err));
		return -1;
	}

	// 分配一个硬件变量对象，并判断是否分配成功
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) 
	{
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (err));
		return -1;
	}
	
	// 按照默认设置对硬件对象进行设置，并判断是否设置成功
	if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) 
	{
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
		return -1;
	}

	/*
		设置数据为交叉模式，并判断是否设置成功
		interleaved/non interleaved:交叉/非交叉模式。
		表示在多声道数据传输的过程中是采样交叉的模式还是非交叉的模式。
		对多声道数据，如果采样交叉模式，使用一块buffer即可，其中各声道的数据交叉传输；
		如果使用非交叉模式，需要为各声道分别分配一个buffer，各声道数据分别传输。
	*/
	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) 
	{
		fprintf (stderr, "cannot set access type (%s)\n", snd_strerror (err));
		return -1;
	}

	// 设置数据编码格式为PCM、有符号、16bit、LE格式，并判断是否设置成功
	if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) 
	{
		fprintf (stderr, "cannot set sample format (%s)\n",  snd_strerror (err));
		return -1;
	}

	// 设置采样频率，并判断是否设置成功
	if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) 
	{
		fprintf (stderr, "cannot set sample rate (%s)\n", snd_strerror (err));
		return -1;
	}

	//  设置为双声道，并判断是否设置成功
	if ((err = snd_pcm_hw_params_set_channels_near (capture_handle, hw_params, &channels)) < 0) 
	{
		fprintf (stderr, "cannot set channel count (%s)\n", snd_strerror (err));
		return -1;
	}

    snd_pcm_hw_params_set_period_size_near(capture_handle, hw_params, &frames, &dir);


	// 将配置写入驱动程序中，并判断是否配置成功
	if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) 
	{
		fprintf (stderr, "cannot set parameters (%s)\n", snd_strerror (err));
		return -1;
	}
	//fprintf(stdout, "hw_params setted\n");

    snd_pcm_hw_params_get_period_size(hw_params, &frames, &dir);
    //printf("Frame: %lu\n", frames);

    snd_pcm_hw_params_get_period_time(hw_params, &period_time, &dir);
    //printf("Period_time: %d\n", period_time);


	// 使采集卡处于空闲状态
	snd_pcm_hw_params_free (hw_params);

	// 准备音频接口，并判断是否准备好
	if ((err = snd_pcm_prepare (capture_handle)) < 0) 
	{
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror (err));
		return -1;
	}
	//fprintf(stdout, "audio interface prepared\n");

	// 配置一个数据缓冲区用来缓冲数据
    buffer_size = frames * periods_per_buffer * snd_pcm_format_width(format) * channels / 8;
	buffer = (char *) malloc(buffer_size);
    //printf("Buffer size: %d\n",buffer_size);
    printf("Start capturing...\n");

    loops = capture_time * 1000000 / period_time;
	// 开始采集音频pcm数据
	while (loops > 0) 
	{
        loops--;
		// 读取
        err = snd_pcm_readi (capture_handle, buffer, frames * periods_per_buffer);

        if (err == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(capture_handle);
        } else if (err < 0) {
            fprintf(stderr, "error from read: %s\n", snd_strerror(err));
        } else if (err != frames * periods_per_buffer) {
            fprintf(stderr, "short read, read %d frames\n", err);
        }

        memcpy(oldptr, buffer, buffer_size);
        oldptr += buffer_size;

        ret_size += buffer_size;
	}
    printf("Ret size: %lld bytes\n", ret_size);
	// 释放数据缓冲区
	free(buffer);


	// 关闭音频采集卡硬件
	snd_pcm_close (capture_handle);
	fprintf(stdout, "Audio interface closed.\n");

	return ret_size;
}