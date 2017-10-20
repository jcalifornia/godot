#include "callback.h"
#include "utils.h"
#include "variant.h"
#include "print_string.h"
#include "math/vector3.h"
#include "scene/resources/audio_stream_sample.h"



void MyCallBack::audio( int target,
	int sessionId, int sequenceNumber, int16_t *pcm_data,
	uint32_t pcm_data_size, float position[3]){
	Variant sid(sessionId);
	Variant snum(sequenceNumber);
	Ref<AudioStreamSample> sam = Ref<AudioStreamSample>(utils::pcm2Sample(pcm_data, pcm_data_size));    
	Variant pcm(sam);
	Variant tar(target);
	Vector3 pos(position[0],position[1],position[2]);
//	Variant::CallError err;
	Variant result = _mumble_ref.emit_signal( "audio_message", pcm, pos, tar, sid );
}


void MyCallBack::textMessage(
			uint32_t actor,
			std::vector<uint32_t> session,
			std::vector<uint32_t> channel_id,
			std::vector<uint32_t> tree_id,
			std::string message) {
	Variant s = utils::cpp_uint32vec2Variant(session);
	Variant c = utils::cpp_uint32vec2Variant(channel_id);
	Variant t = utils::cpp_uint32vec2Variant(tree_id);
	Variant a(actor);
	Variant m(String(message.c_str()));
//		Variant::CallError err;
//		const Variant *args[5] = {&a, &s, &c, &t, &m};
	Variant result =  _mumble_ref.emit_signal( "text_message", m, a );

}
void MyCallBack::version(
				uint16_t major,
				uint8_t minor,
				uint8_t patch,
				std::string release,
				std::string os,
				std::string os_version){ }


