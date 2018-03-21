#ifndef TREECURSION_TYPES
#define TREECURSION_TYPES

#include "core/variant.h"
#include "reference.h"
#include "vector.h"
#include "variant_parser.h"
#include "io/json.h"
#include "dictionary.h"

enum AUDIO_CODEC { OPUS = 1, RAW };

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
		VOIP_HEADER_TASK
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
};



class TreecursionTestData : public Resource{
    GDCLASS(TreecursionTestData, Resource);
    
public:
    bool eof_reached(){
		return _file->eof_reached();
	}
    Error set_file(const String &p_file);
	
    Ref<TreecursionWriteTask> next();
    Ref<TreecursionWriteTask> peek();
    bool has_next();
    TreecursionTestData();
    ~TreecursionTestData();
	

protected:
    static void _bind_methods();

private:
    Vector< Ref<TreecursionWriteTask> > commands;
    int counter;
    String file;
    FileAccess *_file;
    Ref<TreecursionWriteTask> current;
    Ref<TreecursionWriteTask> variant2write_task(const Variant & cmd);
};
#endif
