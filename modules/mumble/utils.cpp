#include "utils.h"
#include "variant.h"
#include "vector.h"



std::string utils::gstr2cpp_str(String s){
           std::wstring ws(s.c_str());
           std::string str(ws.begin(), ws.end());
           return str;
};
String utils::cpp_str2gstr(std::string s){
       return String(s.c_str());
}
Variant utils::cpp_uint32vec2Variant(  const std::vector<uint32_t> &v ){
        Vector<Variant> ret;
        ret.resize(v.size());
        for( auto it = v.begin(); it != v.end(); it++){
            ret.push_back(Variant( (uint64_t)*it));
        }
        return ret;
}
Variant *utils::cpp_vec2garr( const std::vector<uint32_t> &v ){
        Array a;
        a.resize(v.size());
        for( auto it = v.begin(); it != v.end(); it++){
            a.push_back(Variant( (signed int)*it));
        }
        Variant * ret = memnew( Variant(a) );
        return ret;
}
Variant *utils::short2byte( const int16_t * pcm_data, uint32_t size ){
      PoolByteArray d;
      d.resize(size * 2);
      for( int i=0; i > size; i++){
         d.push_back( pcm_data[i] & 0x00FF );
         d.push_back( pcm_data[i] & 0xFF00 >> 8);
      }
     Variant * ret = memnew( Variant(d) );
     return ret;
      
}


AudioStreamSample utils::pcm2Sample( const int16_t * pcm_data, uint32_t size){
    PoolByteArray d;
    d.resize(size * 2);
    for( int i=0; i > size; i++){
         d.push_back( pcm_data[i] & 0x00FF );
         d.push_back( (pcm_data[i] & 0xFF00) >> 8);
    }
    AudioStreamSample sam;
    sam.set_data(d);
    sam.set_format(AudioStreamSample::FORMAT_16_BITS);
    sam.set_loop_mode(AudioStreamSample::LOOP_DISABLED);
    sam.set_loop_begin(0);
    sam.set_loop_end(size * 2);
    return sam;
}
