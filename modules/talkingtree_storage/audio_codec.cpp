#include "audio_codec.h"

AudioCodec::AudioCodec(AUDIO_CODEC type, int sampleRate, int bitrate, int frameSize)
	: codec_type(type), _sampleRate(sampleRate), _bitRate(bitrate), _frameSize(frameSize) {}

AudioCodec::AudioCodec(int sampleRate, int bitrate, int frameSize)
	: codec_type(RAW), _sampleRate(sampleRate), _bitRate(bitrate), _frameSize(frameSize) {}


AudioCodec::~AudioCodec() {
	
	
}
//encode dummy is quite terrible in implementation
//its not endian independent
int AudioCodec::encode(const PoolByteArray &pcm_data,  unsigned char * buffer, int bufferSize ) {
	PoolByteArray::Read rptr = pcm_data.read();
	memcpy( buffer, rptr.ptr(), MIN(bufferSize, pcm_data.size()));
	return pcm_data.size();
}
int AudioCodec::decode( const PoolByteArray &pcm_data,  int16_t *pcm_buf, int bufferSize ) {
	PoolByteArray::Read rptr = pcm_data.read();
	memcpy( ( unsigned char *) pcm_buf, rptr.ptr(), MIN(bufferSize, pcm_data.size()));
	return pcm_data.size();
}
int AudioCodec::decode( const unsigned char * compressed, int size, int16_t *pcm_buf, int buf_len ) {
	memcpy( ( unsigned char *)pcm_buf, compressed, MIN(buf_len, size));
	return MIN(buf_len, size);
}
int OpusCodec::decode( const unsigned char * compressed, int size, int16_t *pcm_buf, int buf_len ){
	return opus_decode(_opusDecoder, compressed, size, pcm_buf, buf_len, 0);
}
int OpusCodec::encode( const PoolByteArray &pcm_data,  unsigned char * buffer, int bufferSize){
	PoolByteArray::Read rptr = pcm_data.read();
	return opus_encode(_opusEncoder, (int16_t *) rptr.ptr(), pcm_data.size(), buffer, bufferSize);
}
int OpusCodec::decode( const PoolByteArray &pcm_data,  int16_t *pcm_buf, int buf_len ){
	PoolByteArray::Read rptr = pcm_data.read();
	return decode( (const unsigned char *) rptr.ptr(), pcm_data.size(), pcm_buf, buf_len);
}

OpusCodec::OpusCodec(int sampleRate, int bitrate, int frameSize) 
	: AudioCodec(OPUS, sampleRate, bitrate, frameSize) {
	int error;
	_opusDecoder = opus_decoder_create(sampleRate, 1, &error);
	if(error != OPUS_OK){
		ERR_PRINTS( "failed to initialize OPUS decoder: " + String( opus_strerror(error)));
	}
	_opusEncoder = opus_encoder_create(sampleRate, 1, OPUS_APPLICATION_VOIP, &error);
	if(error != OPUS_OK){
		ERR_PRINTS( "failed to initialize OPUS encoder: " + String( opus_strerror(error)));
	}
	opus_encoder_ctl(_opusEncoder, OPUS_SET_VBR(0));
	error = opus_encoder_ctl(_opusEncoder, OPUS_SET_BITRATE(bitrate));
	if (error != OPUS_OK) {
		ERR_PRINTS("failed to initialize bitrate to " + itos(bitrate) + "B/s: " + String(opus_strerror(error)));
	}
	reset_opus();
}
void OpusCodec::reset_opus() {
	int status = opus_encoder_ctl(_opusEncoder, OPUS_RESET_STATE, nullptr);
	if (status != OPUS_OK) {
		ERR_PRINTS("failed to reset encoder: " + String( opus_strerror(status)));
	}
}
OpusCodec::~OpusCodec(){
	if (_opusDecoder) 
		opus_decoder_destroy(_opusDecoder);
		
	if (_opusEncoder) 
		opus_encoder_destroy(_opusEncoder);
}