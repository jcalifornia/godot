#ifndef TREECURSION_TYPES
#define TREECURSION_TYPES

#include "core/variant.h"
#include "reference.h"
#include "vector.h"
#include "variant_parser.h"
#include "io/json.h"
#include "dictionary.h"

enum AUDIO_CODEC { OPUS, RAW };


class TreecursionWriteTask : public Reference {
	GDCLASS(TreecursionWriteTask, Reference);
protected:
	const uint64_t time;
	const uint64_t user_id;
protected:
	TreecursionWriteTask( uint64_t t, uint64_t u ) : time(t), user_id(u){

	}
	String _string2Json(String key, String value) const {
		return String("\"") + key + String("\": \"") + value.json_escape() + String("\""); 
	}
	String _int2Json(String key, uint64_t value) const {
		return String("\"") + key + String("\": ") + itos(value); 
	}
	String _addQuotes(String s) const {
		return String("\"") + s.json_escape() + String("\"");
	}
	String _variant2Json(String key, Variant v) const {
		String var;
		VariantWriter::write_to_string(v, var);
		String("\"") + key + String("\": ") + _addQuotes(var); 
	}

public:
	uint64_t get_user_id() const { return user_id; }
	uint64_t get_time() const { return time; }
	virtual String toJson() const { 
		return String("");
	}
	


};
class TreecursionCommandTask : public TreecursionWriteTask {
protected:
	const String node_path;
	const String name;
public:
	TreecursionCommandTask(const String &n, const String &node, uint64_t t, uint64_t u ) 
		: TreecursionWriteTask(t, u), node_path(node), name(n){

	}
	String get_node_path() const { return node_path; }
	String get_name() const { return name; }
	virtual String toJson() const { 
		return String("");
	}
};

class TreecursionSetTask : public TreecursionCommandTask {
	const Variant value;
public:
	TreecursionSetTask( const String n, const String &node, Variant v, uint64_t t, uint64_t u) 
		: TreecursionCommandTask( n, node, t, u ), value(v) {
	}
	TreecursionSetTask( const TreecursionSetTask& other ) 
		: TreecursionCommandTask( other.get_name(), other.get_node_path(), other.get_time(), other.get_user_id() ), 
		value(other.get_value()) {
	}

	bool get_value() const { return value; }

	String toString() const {
		String var;
		VariantWriter::write_to_string(value, var);
		String ret = String("time: ") + itos(time) + String(", user_id : ") + itos(user_id) + String(", name : ") + name + String(", node : ") + node_path;
		ret += String(", value: ")  + value;
		return ret;
	}
	virtual String toJson() const {
		Dictionary dict;
		dict["time"] = time;
		dict["value"] = value;
		dict["node"] = node_path;
		dict["user_id"] = user_id;
		dict["name"] = name;
		JSON a;
		String ret = a.print(dict);
		return ret;
	}
/*
	virtual String toJson() const { 
		return String("{") + _string2Json("name", name) + String(",")
			 + _string2Json("path", node_path) + String(",")
			 + _variant2Json("value", value) + String(",")
			 + _int2Json("time", time) + String(",") 
			 + _int2Json("user_id", user_id) + String("}");
	}
*/
};

class TreecursionCallTask : public TreecursionCommandTask {
	const Vector<Variant> args;
public:
	TreecursionCallTask( const String n, const String &node, const Vector<Variant> &a, uint64_t t, uint64_t u) 
		: TreecursionCommandTask( n, node, t, u ), args(a) {
	}

	TreecursionCallTask( const TreecursionCallTask& other) 
		: TreecursionCommandTask( other.get_name(), other.get_node_path(), other.get_time(), other.get_user_id() ), args( other.args) {
	}

	Vector<Variant> get_args() const { return args; }

	String toString() const {
		String ret = String("time: ") + itos(time) + String(", user_id : ") + itos(user_id) + String(", name : ") + name + String(", node : ") + node_path;
		for( int i = 0; i < args.size(); i++){
			String var;
			VariantWriter::write_to_string(args[i], var);
			ret += String(", args : ") + var;
		}
		return ret;
	}
	virtual String toJson() const {
		Dictionary dict;
		dict["time"] = time;
		dict["args"] = args;
		dict["node"] = node_path;
		dict["user_id"] = user_id;
		dict["name"] = name;
		JSON a;
		String ret = a.print(dict);
		return ret;
	}
/*
	virtual String toJson() const { 
		return String("{") + _string2Json("name", name) + String(",")
			 + _string2Json("path", node_path) + String(",")
			 + _variant2Json("args", args) + String(",")
			 + _int2Json("time", time) + String(",") 
			 + _int2Json("user_id", user_id) + String("}");
	}
*/
};


class TreecursionVoiceTask : public TreecursionWriteTask {
	PoolByteArray pcm_data;
	
};

class TreecursionHeaderTask : public TreecursionWriteTask {
	AUDIO_CODEC codec;
	int bit_rate;
	int frame_size;
	int version_major;
	int version_minor;
};
#endif
