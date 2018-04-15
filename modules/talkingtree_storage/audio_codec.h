#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include "reference.h"
#include "io/treecursion_types.h"
#include "variant.h"
#include <opus.h>

class AudioCodec : public Reference {
	GDCLASS(AudioCodec, Reference);

protected:
	AudioCodec(AUDIO_CODEC type, int sampleRate, int bitrate, int frameSize);
	int _sampleRate;
	int _bitRate;
	int _frameSize;
private:
	AUDIO_CODEC codec_type;

public:
	int virtual encode( const PoolByteArray &pcm_data, unsigned char * buffer, int bufferSize );
	int virtual decode( const PoolByteArray &pcm_data,  int16_t *pcm_buf, int buf_len );
	int virtual decode( const unsigned char * compressed, int size, int16_t *pcm_buf, int buf_len );
	void reset_encoder();
	AudioCodec(int sampleRate, int bitrate, int frameSize);
	~AudioCodec();
};

class OpusCodec : public AudioCodec {
	GDCLASS(OpusCodec, AudioCodec);
	OpusEncoder *_opusEncoder;
	OpusDecoder *_opusDecoder;
private:
	void reset_opus();
public:
	OpusCodec(int sampleRate, int bitrate, int frameSize);
	~OpusCodec();
	int decode( const unsigned char * compressed, int size, int16_t *pcm_buf, int buf_len );
	int encode( const PoolByteArray &pcm_data, unsigned char * buffer, int bufferSize );
	int decode( const PoolByteArray &pcm_data,  int16_t *pcm_buf, int buf_len );
};

#endif