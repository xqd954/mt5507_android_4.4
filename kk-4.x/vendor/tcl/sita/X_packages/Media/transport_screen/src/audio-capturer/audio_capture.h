/*
 * audio_capture.h
 *
 *  Created on: 2012-4-17
 *      Author: sujuan
 */

#ifndef AUDIO_CAPTURE_H_
#define AUDIO_CAPTURE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "limutil.h"

struct _AudioCapture;
typedef struct _AudioCapture AudioCapture;

typedef struct _AudioParameter
{
	/*audio capture parameter set of pcm*/
    unsigned int channels;
	unsigned int rate;
	unsigned int bits;

	/*audio queue */
	void *queue;
	lthread_mutex_t *queue_mutex;
	lthread_cond_t *queue_cond;
	
}AudioParameter;


 
/**
 * Init audio capture
 */
AudioCapture* audio_capture_init(AudioParameter * audioparameter);

/**
 * Start audio capture
 */
Ret audio_capture_start(AudioCapture * audio_capture);

/**
 * Pause audio capture
 */
Ret audio_capture_pause(AudioCapture * audio_capture);

/**
 * Close audio capture
 */
Ret audio_capture_close(AudioCapture * audio_capture);

 
#endif /* AUDIO_CAPTURE_H_ */
 
