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
        for( int i = 0; i < v.size(); i++){
            ret[i] = (Variant( (uint64_t)v[i]));
        }
        return ret;
}


AudioStreamSample *utils::pcm2Sample( const int16_t * pcm_data, uint32_t size){
    PoolByteArray d;
    d.resize(size * 2);
    AudioStreamSample *sam = memnew(AudioStreamSample);
    sam->set_format(AudioStreamSample::FORMAT_16_BITS);
    sam->set_loop_mode(AudioStreamSample::LOOP_DISABLED);
    for( int i=0; i < size; i++){
         uint16_t sh = (uint16_t) pcm_data[i];
         d.set( i*2 ,  (uint8_t) (sh & 0xFF) );
         d.set( i*2+1 , (uint8_t) (sh >> 8));
    }


    sam->set_data(d);
    return sam;
}
