/* callback.h */
#ifndef SimpleCALLBACK_H
#define SimpleCALLBACK_H

#include "reference.h"
#include "ustring.h"
#include "func_ref.h"
#include <string.h>
#include <mumlib/Callback.hpp>
#include <vector>

class SimpleCallback : public mumlib::Callback, public Reference {
    GDCLASS(SimpleCallback,Reference);

private:
    FuncRef *audio_handler;
    FuncRef *text_handler;

protected:
//    bool _set(const StringName &p_name, const Variant &p_value);
//    bool _get(const StringName &p_name, Variant &r_ret) const;
//    void _get_property_list(List<PropertyInfo> *p_list) const;
    static void _bind_methods();

public:

    SimpleCallback();
    ~SimpleCallback();
    virtual void audio(int target,
                       int sessionId,
                       int sequenceNumber,
                       int16_t *pcm_data,
			uint32_t pcm_data_size)  ;
    virtual void textMessage(
            uint32_t actor,
            std::vector<uint32_t> session,
            std::vector<uint32_t> channel_id,
            std::vector<uint32_t> tree_id,
    	    std::string message);
    void setAudioHandler( Object * handle);
    void setTextHandler( Object * handle);

};

#endif
