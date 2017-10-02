#include "ustring.h"
#include "array.h"
#include <string>
#include <vector>

namespace utils{
   std::string gstr2cpp_str(String s);
   String cpp_str2gstr(std::string s);
   Variant *cpp_vec2garr( const std::vector<uint32_t> &v );
   Variant *short2byte( const int16_t * pcm_data, uint32_t size);
}
