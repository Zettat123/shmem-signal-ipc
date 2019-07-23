#include "sndplay.h"

void PlaySound(char* mptr, long long snd_size)
{
    
	int rc;
	char * buffer;
  	int buffer_size;
  	int periods_per_buffer;
    char* oldptr = mptr;
  	snd_pcm_t *handle;
  	snd_pcm_hw_params_t *params;
  	snd_pcm_uframes_t frames;

  	unsigned int channels;
  	unsigned int rate;
    
    // Assign variables that were read from the wave file
	channels = 2;
	rate = 44100;
	periods_per_buffer = 2; // Down to user preference, depending on size of internal ring buffer of ALSA


  	// Open PCM device for playback
  	if ((rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) 
  	{
    	printf("ERROR: Cannot open pcm device. %s\n", snd_strerror(rc));
        return;
  	}

    
  	// Allocate hardware parameters
  	if ((rc = snd_pcm_hw_params_malloc(&params)) < 0)
  	{
  		printf("ERROR: Cannot allocate hardware parameters. %s\n", snd_strerror(rc));
        return;
  	}

    
  	// Initialize parameters with default values
  	if ((rc = snd_pcm_hw_params_any(handle, params)) < 0)
  	{
        printf("ERROR: Cannot initialize hardware parameters. %s\n", snd_strerror(rc));
        return;
    }

  	// Setting hardware parameters
  	if ((rc = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
  	{
  		printf("ERROR: Cannot set interleaved mode. %s\n", snd_strerror(rc));
        return;
  	}

  	if ((rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE)) < 0)
  	{
  		printf("ERROR: Cannot set PCM format. %s\n", snd_strerror(rc));
        return;
  	}

  	if ((rc = snd_pcm_hw_params_set_channels_near(handle, params, &channels)) < 0)
  	{
  		printf("ERROR: Cannot set number of channels. %s\n", snd_strerror(rc));
        return;
  	}

 	if ((rc = snd_pcm_hw_params_set_rate_near(handle, params, &rate, 0)) < 0)
 	{
 		printf("ERROR: Cannot set plyabck rate. %s\n", snd_strerror(rc));
        return;
 	}

 	if ((rc = snd_pcm_hw_params(handle, params)) < 0)
 	{
 		printf("ERROR: Cannot set hardware parameters. %s\n", snd_strerror(rc));
        return;
 	}


 	// Get hardware parameters
 	if ((rc = snd_pcm_hw_params_get_period_size(params, &frames, 0)) < 0)
	{
		printf("Playback ERROR: Can't get period size. %s\n", snd_strerror(rc));
        return;
	}
	printf("Frames: %lu\n", frames);

	if ((rc = snd_pcm_hw_params_get_channels(params, &channels)) < 0)
	{
		printf("Playback ERROR: Can't get channel number. %s\n", snd_strerror(rc));
        return;
	}
    printf("Channel: %u\n",channels);
	if ((rc = snd_pcm_hw_params_get_rate(params, &rate, 0)) < 0)
	{
		printf("ERROR: Cannot get rate. %s\n", snd_strerror(rc));
        return;
	}
    printf("Rate: %u\n",rate);

	// Free paraemeters
	snd_pcm_hw_params_free(params);

	// Create buffer
  	buffer_size = frames * periods_per_buffer * channels * sizeof(int16_t); /* 2 bytes/sample, 2 channels */
  	buffer = (char *) malloc(buffer_size);

  	// Send info to ALSA
    while (snd_size > 0)
 	{
        memcpy(buffer, oldptr, buffer_size);
        oldptr += buffer_size;
        snd_size -= buffer_size;
    	rc = snd_pcm_writei(handle, buffer, frames * periods_per_buffer);
    	if (rc == -EPIPE) 
    	{
      		fprintf(stderr, "underrun occurred\n");
      		snd_pcm_prepare(handle);
    	} 
    	else if (rc < 0) 
    	{
      		printf("ERROR: Cannot write to playback device. %s\n", strerror(rc));
    	}
  	}

  	printf("Info set: Device is now draining...\n");
  	snd_pcm_drain(handle);

  	printf("Done playing, closing connections.\n");
  	snd_pcm_close(handle);

  	free(buffer);

    return;
}

