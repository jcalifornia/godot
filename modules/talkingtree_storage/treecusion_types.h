#include "core/variant.h"


enum AUDIO_CODEC { OPUS };


class TreecusionWriteTask {
	uint64_t time;
	uint64_t user_id;
};

class TreecursionCommandTask : TreecusionWriteTask {
	String func;
	List<String> args;
};

class TreecursionVoiceTask : TreecusionWriteTask {
	PoolByteArray pcm_data;
};

class TreecursionHeaderTask : TreecusionWriteTask {
	AUDIO_CODEC codec;
	int bit_rate;
	int frame_size;
	int version_major;
	int version_minor;
};

