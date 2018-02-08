#include "core/variant.h"
#include "reference.h"
#include "vector.h"
#include "variant_parser.h"

enum AUDIO_CODEC { OPUS, RAW };


class TreecusionWriteTask : public Reference {
	GDCLASS(TreecusionWriteTask, Reference);
protected:
	const uint64_t time;
	const uint64_t user_id;
protected:
	TreecusionWriteTask( uint64_t t, uint64_t u ) : time(t), user_id(u){

	}
public:
	uint64_t get_user_id() const { return user_id; }
	uint64_t get_time() const { return time; }
};


class TreecursionCommandTask : public TreecusionWriteTask {
protected:
	const String node_path;
	const String name;
public:
	TreecursionCommandTask(String &n, String &node, uint64_t t, uint64_t u ) 
		: TreecusionWriteTask(t, u), node_path(node), name(n){

	}
	String get_node_path() const { return node_path; }
};

class TreecursionSetTask : public TreecursionCommandTask {
	const Variant value;
public:
	TreecursionSetTask(String n, String &node, Variant v, uint64_t t, uint64_t u) 
		: TreecursionCommandTask( n, node, t, u ), value(v) {
	}
	bool get_value() const { return value; }

	String toString() const {
		String var;
		VariantWriter::write_to_string(value, var);
		String ret = String("time: ") + itos(time) + String(", user_id : ") + itos(user_id) + String(", name : ") + name + String(", node : ") + node_path;
		ret += String(", value: ")  + value;
		return ret;
	}
};

class TreecursionCallTask : public TreecursionCommandTask {
	const Vector<Variant> args;
public:
	TreecursionCallTask(String n, String &node, const Vector<Variant> &a, uint64_t t, uint64_t u) 
		: TreecursionCommandTask( n, node, t, u ), args(a) {
	}
	String toString() const {
		String ret = String("time: ") + itos(time) + String(", user_id : ") + itos(user_id) + String(", name : ") + name + String(", node : ") + node_path;
		for( int i = 0; i < args.size(); i++){
			String var;
			VariantWriter::write_to_string(args[i], var);
			ret += String(", args : ") + var;
		}
		return ret;
	}
};


class TreecursionVoiceTask : public TreecusionWriteTask {
	PoolByteArray pcm_data;
};

class TreecursionHeaderTask : public TreecusionWriteTask {
	AUDIO_CODEC codec;
	int bit_rate;
	int frame_size;
	int version_major;
	int version_minor;
};

