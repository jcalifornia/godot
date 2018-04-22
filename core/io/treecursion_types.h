#ifndef TREECURSION_TYPES_H
#define TREECURSION_TYPES_H

#include "core/variant.h"
#include "reference.h"
#include "vector.h"
#include "variant_parser.h"
#include "io/json.h"
#include "dictionary.h"

enum AUDIO_CODEC { RAW = 0, OPUS = 1  };

enum HTOGG_SERIALNO { HTOGG_ENGINE = 0, HTOGG_VOIP = 1, HTOGG_END };


class TreecursionWriteTask : public Reference {
	GDCLASS(TreecursionWriteTask, Reference);

public:
	enum Types {
		ERROR = 0,
		SET_TASK = 1,
		CALL_TASK,
		VOICE_TASK,
		ENGINE_HEADER_TASK,
		VOICE_HEADER_TASK
	};
protected:
	TreecursionWriteTask( uint64_t t, uint64_t u, Types ty ) : time(t), user_id(u), type(ty){

	}

public:

	
	uint64_t get_user_id() const { return user_id; }
	uint64_t get_time() const { return time; }
	virtual String toJson() const { 
		return String("");
	}
	virtual String toString() const { 
		return String("");
	}
	Types get_type() const { return type; }
protected:
	Types type;
	uint64_t time;
	uint64_t user_id;


};
class TreecursionCommandTask : public TreecursionWriteTask {
protected:
	String node_path;
	String name;
public:
	TreecursionCommandTask(const String &n, const String &node, uint64_t t, uint64_t u, Types ty) 
		: TreecursionWriteTask(t, u, ty), node_path(node), name(n){

	}

	String get_node_path() const { return node_path; }
	String get_name() const { return name; }
	virtual String toJson() const { 
		return String("");
	}
	virtual String toString() const { 
		return String("");
	}
};


class TreecursionSetTask : public TreecursionCommandTask {
	Variant value;
public:
	TreecursionSetTask( const String n, const String &node, Variant v, uint64_t t, uint64_t u) 
		: TreecursionCommandTask( n, node, t, u, SET_TASK ), value(v) {
	}
	TreecursionSetTask( const TreecursionSetTask& other ) 
		: TreecursionCommandTask( other.get_name(), other.get_node_path(), other.get_time(), other.get_user_id(), other.get_type() ), 
		value(other.get_value()) {
	}
	TreecursionSetTask( const Dictionary& map ) 
		: TreecursionCommandTask( map["name"], map["node"], map["time"], map["user_id"], SET_TASK ), 
		value(map["value"]) {
			
	}

	Variant get_value() const { return value; }

	virtual String toJson() const {
		Dictionary dict;
		dict["time"] = time;
		dict["value"] = value;
		dict["node"] = node_path;
		dict["user_id"] = user_id;
		dict["name"] = name;
		dict["type"] = int32_t(type);
		JSON a;
		String ret = a.print(dict);
		return ret;
	}

	virtual String toString() const{
		Dictionary dict;

		dict["time"] = time;
		dict["value"] = value;
		dict["node"] = node_path;
		dict["user_id"] = user_id;
		dict["name"] = name;
		dict["type"] = int32_t(type);

		VariantWriter w;
		String ret;
		Error err = w.write_to_string(dict, ret);
		return ret;
	}
};

class TreecursionCallTask : public TreecursionCommandTask {
	Vector<Variant> args;
public:
	TreecursionCallTask( const String n, const String &node, const Vector<Variant> &a, uint64_t t, uint64_t u) 
		: TreecursionCommandTask( n, node, t, u, CALL_TASK ), args(a) {
	}

	TreecursionCallTask( const TreecursionCallTask& other) 
		: TreecursionCommandTask( other.get_name(), other.get_node_path(), other.get_time(), other.get_user_id(), CALL_TASK ), args( other.args) {
	}

	TreecursionCallTask(  const Dictionary& map ) 
		: TreecursionCommandTask( map["name"], map["node"], map["time"], map["user_id"], CALL_TASK ), args( map["args"]) {
	}

	Vector<Variant> get_args() const { return args; }

	virtual String toJson() const {
		Dictionary dict;
		dict["time"] = time;
		dict["args"] = args;
		dict["node"] = node_path;
		dict["user_id"] = user_id;
		dict["name"] = name;
		dict["type"] = int32_t(type);
		JSON a;
		String ret = a.print(dict);
		return ret;
	}

	virtual String toString() const{
		Dictionary dict;

		dict["time"] = time;
		dict["args"] = args;
		dict["node"] = node_path;
		dict["user_id"] = user_id;
		dict["name"] = name;
		dict["type"] = int32_t(type);

		VariantWriter w;
		String ret;
		Error err = w.write_to_string(dict, ret);
		return ret;
	}

};


class TreecursionVoiceTask : public TreecursionWriteTask {
	PoolByteArray pcm_data;
public:
	TreecursionVoiceTask( uint64_t t, uint64_t u, PoolByteArray data )
		: TreecursionWriteTask(t, u, VOICE_TASK), pcm_data(data) {
	}
	TreecursionVoiceTask( uint64_t t, uint64_t u, const uint8_t *data, int size )
		: TreecursionWriteTask(t, u, VOICE_TASK) {
			pcm_data.resize(size);
			memcpy(pcm_data.write().ptr(), data, size); 
	}
	PoolByteArray get_data() const {
		return pcm_data;
	}
	virtual String toJson() const{
		Dictionary dict;

		dict["time"] = time;
		dict["user_id"] = user_id;
		dict["pcm_size"] = pcm_data.size();
		dict["type"] = int32_t(type);

		JSON a;
		String ret = a.print(dict);
		return ret;
	}
	virtual String toString() const{
		Dictionary dict;

		dict["time"] = time;
		dict["user_id"] = user_id;
		dict["pcm_size"] = pcm_data.size();
		dict["type"] = int32_t(type);

		VariantWriter w;
		String ret;
		Error err = w.write_to_string(dict, ret);
		return ret;
	}
};

class TreecursionHeaderTask : public TreecursionWriteTask {
public:
	TreecursionHeaderTask( uint64_t t, uint64_t u, Types ty ) : TreecursionWriteTask(t, u, ty ){

	}
};

class TreecursionEngineHeaderTask : public TreecursionHeaderTask {
	Dictionary init_vars;
public:
	TreecursionEngineHeaderTask(  uint64_t time, const Dictionary& map )
		: TreecursionHeaderTask( time, 0, ENGINE_HEADER_TASK), init_vars(map) {
	}
	virtual String toString() const {
		VariantWriter w;
		String ret;
		Error err = w.write_to_string(init_vars, ret);
		return ret;
	}
	Dictionary get_vars() const {
		return init_vars.duplicate();
	}
};

class TreecursionVoiceHeaderTask : public TreecursionHeaderTask {
	int32_t sample_rate;
	int32_t bit_rate;
	int32_t frame_size;
	AUDIO_CODEC codec;
public:
	TreecursionVoiceHeaderTask(  int codec, int32_t sample_rate, int32_t bit_rate, int32_t frame_size)
		: TreecursionHeaderTask( 0, 0, VOICE_HEADER_TASK)
		,  codec((AUDIO_CODEC)codec), sample_rate(sample_rate), bit_rate(bit_rate), frame_size(frame_size) {
	}
	int32_t get_sample_rate() const {
		return sample_rate;
	}
	int32_t get_bit_rate() const {
		return bit_rate;
	}
	int32_t get_frame_size() const {
		return frame_size;
	}
	int32_t get_codec() const {
		return codec;
	}
};
#endif
