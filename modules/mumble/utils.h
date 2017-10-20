#include "ustring.h"
#include "array.h"
#include <string>
#include <vector>
#include "scene/resources/audio_stream_sample.h"
#include "variant.h"

namespace utils{
	std::string gstr2cpp_str(String s);
	String cpp_str2gstr(std::string s);
	Variant cpp_uint32vec2Variant(  const std::vector<uint32_t> &v );
	AudioStreamSample *pcm2Sample( const int16_t * pcm_data, uint32_t size);
}
