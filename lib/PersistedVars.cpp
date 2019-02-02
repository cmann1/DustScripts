#include 'math/Vec2.cpp';

enum PersistedVarType
{
	None,
	Bool,
	Byte,
	Int,
	UInt,
	Long,
	Float,
	Double,
	String,
	Vec2,
}

class PersistedVars
{
	
	protected dictionary variables;
	protected dictionary variables_checkpoint;
	protected dictionary listeners;
	
	bool active = false;
	
	void checkpoint_save()
	{
		variables_checkpoint = variables;
	}
	
	void checkpoint_load()
	{
		variables = variables_checkpoint;
		listeners.deleteAll();
	}
	
	bool exists(string name)
	{
		return variables.exists(name);
	}
	
	protected void add_listener(string name, PersistedVarListener@ listener, int index)
	{
		array<PersistedVarListenerWrapper@>@ event_listeners;
		
		if(!listeners.exists(name))
		{
			@event_listeners = array<PersistedVarListenerWrapper@>();
			@listeners[name] = @event_listeners;
		}
		else
		{
			@event_listeners = cast<array<PersistedVarListenerWrapper@>>(listeners[name]);
		}
		
		event_listeners.insertLast(PersistedVarListenerWrapper(listener, index));
	}
	
	// Bool
	void register(string name, PersistedVarListener@ listener, const bool &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_bool(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_bool(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	// Byte
	void register(string name, PersistedVarListener@ listener, const int8 &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_byte(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_byte(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	// Int
	void register(string name, PersistedVarListener@ listener, const int &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_int(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_int(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	// UInt
	void register(string name, PersistedVarListener@ listener, const uint &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_uint(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_uint_a(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	// Long
	void register(string name, PersistedVarListener@ listener, const int64 &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_long(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_long(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	// Float
	void register(string name, PersistedVarListener@ listener, const float &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_float(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_float(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	// Double
	void register(string name, PersistedVarListener@ listener, const double &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_double(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_double(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	// String
	void register(string name, PersistedVarListener@ listener, const string &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_string(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_string(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	// Vec2
	void register(string name, PersistedVarListener@ listener, Vec2@ &in initial_value, int index=-1)
	{
		if(!active) return;
		
		add_listener(name, listener, index);
		
		if(index < 0)
		{
			if(get_vec2(name, initial_value) != initial_value)
				listener.on_var_change(name, index);
		}
		else if(get_vec2(name, index, initial_value) != initial_value)
				listener.on_var_change(name, index);
	}
	
	void unregister(string name, PersistedVarListener@ listener)
	{
		if(!active) return;
		
		if(listeners.exists(name))
		{
			array<PersistedVarListenerWrapper@>@ event_listeners = cast<array<PersistedVarListenerWrapper@>>(listeners[name]);
			for(int i = int(event_listeners.size()) - 1; i >= 0; i--)
			{
				if(event_listeners[i] == listener)
				{
					event_listeners.removeAt(i);
					break;
				}
			}
		}
	}
	
	protected void trigger_change(const string name, int index=-1)
	{
		array<PersistedVarListenerWrapper@>@ event_listeners = cast<array<PersistedVarListenerWrapper@>>(listeners[name]);
		if(event_listeners !is null)
		{
			for(int i = int(event_listeners.size()) - 1; i >= 0; i--)
			{
				PersistedVarListenerWrapper@ wrapper = @event_listeners[i];
				if(index == -1 || wrapper.index == -1 || wrapper.index == index)
					wrapper.listener.on_var_change(name, index);
			}
		}
	}
	
	// Bool
	// --------------------------------------------------------
	bool get_bool(string name, const bool &in default_value=false)
	{
		return variables.exists(name)
			? bool(variables[name])
			: default_value;
	}
	void set_bool(string name, const bool &in value)
	{
		if(!variables.exists(name) || bool(variables[name]) != value)
		{
			variables[name] = value;
			trigger_change(name);
		}
	}
	// Array
	bool get_bool(string name, uint index, const bool &in default_value=false)
	{
		array<bool>@ list = cast<array<bool>>(variables[name]);
		return list !is null && index < list.size()
			? bool(list[index])
			: default_value;
	}
	void set_bool(string name, uint index, const bool &in value)
	{
		bool has_changed = false;
		
		array<bool>@ list = cast<array<bool>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<bool>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		if(list[index] != value)
		{
			list[index] = value;
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
	// Byte
	// --------------------------------------------------------
	int8 get_byte(string name, const int8 &in default_value=0)
	{
		return variables.exists(name)
			? int8(variables[name])
			: default_value;
	}
	void set_byte(string name, const int8 &in value)
	{
		if(!variables.exists(name) || int8(variables[name]) != value)
		{
			variables[name] = value;
			trigger_change(name);
		}
	}
	// Array
	int8 get_byte(string name, uint index, const int8 &in default_value=false)
	{
		array<int8>@ list = cast<array<int8>>(variables[name]);
		return list !is null && index < list.size()
			? int8(list[index])
			: default_value;
	}
	void set_byte(string name, uint index, const int8 &in value)
	{
		bool has_changed = false;
		
		array<int8>@ list = cast<array<int8>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<int8>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		if(list[index] != value)
		{
			list[index] = value;
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
	// Int
	// --------------------------------------------------------
	int get_int(string name, const int &in default_value=0)
	{
		return variables.exists(name)
			? int(variables[name])
			: default_value;
	}
	void set_int(string name, const int &in value)
	{
		if(!variables.exists(name) || int(variables[name]) != value)
		{
			variables[name] = value;
			trigger_change(name);
		}
	}
	// Array
	int get_int(string name, uint index, const int &in default_value=false)
	{
		array<int>@ list = cast<array<int>>(variables[name]);
		return list !is null && index < list.size()
			? int(list[index])
			: default_value;
	}
	void set_int(string name, uint index, const int &in value)
	{
		bool has_changed = false;
		
		array<int>@ list = cast<array<int>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<int>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		if(list[index] != value)
		{
			list[index] = value;
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
	// UInt
	// --------------------------------------------------------
	uint get_uint(string name, const uint &in default_value=0)
	{
		return variables.exists(name)
			? uint(variables[name])
			: default_value;
	}
	void set_uint(string name, const uint &in value)
	{
		if(!variables.exists(name) || uint(variables[name]) != value)
		{
			variables[name] = value;
			trigger_change(name);
		}
	}
	// Array
	uint get_uint_a(string name, uint index, const uint &in default_value=0)
	{
		array<uint>@ list = cast<array<uint>>(variables[name]);
		return list !is null && index < list.size()
			? uint(list[index])
			: default_value;
	}
	void set_uint_a(string name, uint index, const uint &in value)
	{
		bool has_changed = false;
		
		array<uint>@ list = cast<array<uint>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<uint>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		if(list[index] != value)
		{
			list[index] = value;
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
	// Long
	// --------------------------------------------------------
	int64 get_long(string name, const int64 &in default_value=0)
	{
		return variables.exists(name)
			? int64(variables[name])
			: default_value;
	}
	void set_long(string name, const int64 &in value)
	{
		if(!variables.exists(name) || int64(variables[name]) != value)
		{
			variables[name] = value;
			trigger_change(name);
		}
	}
	// Array
	int64 get_long(string name, uint index, const int64 &in default_value=0)
	{
		array<int64>@ list = cast<array<int64>>(variables[name]);
		return list !is null && index < list.size()
			? int64(list[index])
			: default_value;
	}
	void set_long(string name, uint index, const int64 &in value)
	{
		bool has_changed = false;
		
		array<int64>@ list = cast<array<int64>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<int64>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		if(list[index] != value)
		{
			list[index] = value;
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
	// Float
	// --------------------------------------------------------
	float get_float(string name, const float &in default_value=0.0)
	{
		return variables.exists(name)
			? float(variables[name])
			: default_value;
	}
	void set_float(string name, const float &in value)
	{
		if(!variables.exists(name) || float(variables[name]) != value)
		{
			variables[name] = value;
			trigger_change(name);
		}
	}
	// Array
	float get_float(string name, uint index, const float &in default_value=0.0)
	{
		array<float>@ list = cast<array<float>>(variables[name]);
		return list !is null && index < list.size()
			? float(list[index])
			: default_value;
	}
	void set_float(string name, uint index, const float &in value)
	{
		bool has_changed = false;
		
		array<float>@ list = cast<array<float>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<float>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		if(list[index] != value)
		{
			list[index] = value;
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
	// Double
	// --------------------------------------------------------
	double get_double(string name, const double &in default_value=0.0)
	{
		return variables.exists(name)
			? double(variables[name])
			: default_value;
	}
	void set_double(string name, const double &in value)
	{
		if(!variables.exists(name) || double(variables[name]) != value)
		{
			variables[name] = value;
			trigger_change(name);
		}
	}
	// Array
	double get_double(string name, uint index, const double &in default_value=0.0)
	{
		array<double>@ list = cast<array<double>>(variables[name]);
		return list !is null && index < list.size()
			? double(list[index])
			: default_value;
	}
	void set_double(string name, uint index, const double &in value)
	{
		bool has_changed = false;
		
		array<double>@ list = cast<array<double>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<double>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		if(list[index] != value)
		{
			list[index] = value;
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
	// String
	// --------------------------------------------------------
	string get_string(string name, const string &in default_value='')
	{
		return variables.exists(name)
			? string(variables[name])
			: default_value;
	}
	void set_string(string name, const string &in value)
	{
		if(!variables.exists(name) || string(variables[name]) != value)
		{
			variables[name] = value;
			trigger_change(name);
		}
	}
	// Array
	string get_string(string name, uint index, const string &in default_value='')
	{
		array<string>@ list = cast<array<string>>(variables[name]);
		return list !is null && index < list.size()
			? string(list[index])
			: default_value;
	}
	void set_string(string name, uint index, const string &in value)
	{
		bool has_changed = false;
		
		array<string>@ list = cast<array<string>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<string>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		if(list[index] != value)
		{
			list[index] = value;
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
	// Vec2
	// --------------------------------------------------------
	Vec2@ get_vec2(string name, Vec2@ &in default_value=null)
	{
		return variables.exists(name)
			? cast<Vec2>(variables[name])
			: default_value;
	}
	void set_vec2(string name, const float &in x, const float &in y)
	{
		if(!variables.exists(name) || !cast<Vec2>(variables[name]).equals(x, y))
		{
			cast<Vec2>(variables[name]).set(x, y);
			trigger_change(name);
		}
	}
	// Array
	Vec2@ get_vec2(string name, uint index, Vec2@ &in default_value=null)
	{
		array<Vec2@>@ list = cast<array<Vec2@>>(variables[name]);
		return list !is null && index < list.size()
			? cast<Vec2>(list[index])
			: default_value;
	}
	void set_vec2(string name, uint index, const float &in x, const float &in y)
	{
		bool has_changed = false;
		
		array<Vec2@>@ list = cast<array<Vec2@>>(variables[name]);
		if(list is null)
		{
			@variables[name] = @list = array<Vec2@>();
			has_changed = true;
		}
		
		if(index >= list.size())
		{
			list.resize(index + 1);
			has_changed = true;
		}
		
		Vec2@ value = @list[index];
		if(value is null)
		{
			@value = Vec2(x, y);
			has_changed = true;
		}
		else if(!value.equals(x, y))
		{
			value.set(x, y);
			has_changed = true;
		}
		
		if(has_changed) trigger_change(name, index);
	}
	
}

interface PersistedVarListener
{
	void on_var_change(string name, int index);
}
class PersistedVarListenerWrapper
{
	PersistedVarListener@ listener;
	int index;
	
	PersistedVarListenerWrapper(PersistedVarListener@ listener, int index)
	{
		@this.listener = listener;
		this.index = index;
	}
	
	bool opEquals(const PersistedVarListener@ &in other)
	{
		return @listener == @other;
	}
}

class SetPersistedVarTrigger : trigger_base
{
	
	script@ script;
	scripttrigger@ self;
	
	[text] string var_name = '';
	[option,0:None,1:Bool,2:Byte,3:Int,4:UInt,5:Long,6:Float,7:Double,8:String,9:Vec2]
	PersistedVarType var_type = PersistedVarType::None;
	[text] int index = -1;
	[text] bool once = false;
	
	[text] bool bool_value = false;
	[text] int int_value = 0;
	[text] uint uint_value = 0;
	[text] int64 long_value = 0;
	[text] float float_value = 0;
	[text] string string_value = '';
	[text] float x_value = 0;
	[text] float y_value = 0;
	
	SetPersistedVarTrigger(){}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
	}
	
	void activate(controllable@ e)
	{
		if(e.player_index() == -1)
			return;
		
		switch(var_type)
		{
			case PersistedVarType::Bool:
				if(index == -1)
					script.vars.set_bool(var_name, bool_value);
				else
					script.vars.set_bool(var_name, index, bool_value);
				break;
			case PersistedVarType::Byte:
				if(index == -1)
					script.vars.set_byte(var_name, int_value);
				else
					script.vars.set_byte(var_name, index, int_value);
				break;
			case PersistedVarType::Int:
				if(index == -1)
					script.vars.set_int(var_name, int_value);
				else
					script.vars.set_int(var_name, index, int_value);
				break;
			case PersistedVarType::UInt:
				if(index == -1)
					script.vars.set_uint(var_name, uint_value);
				else
					script.vars.set_uint_a(var_name, index, uint_value);
				break;
			case PersistedVarType::Long:
				if(index == -1)
					script.vars.set_long(var_name, long_value);
				else
					script.vars.set_long(var_name, index, long_value);
				break;
			case PersistedVarType::Float:
				if(index == -1)
					script.vars.set_float(var_name, float_value);
				else
					script.vars.set_float(var_name, index, float_value);
				break;
			case PersistedVarType::Double:
				if(index == -1)
					script.vars.set_double(var_name, float_value);
				else
					script.vars.set_double(var_name, index, float_value);
				break;
			case PersistedVarType::String:
				if(index == -1)
					script.vars.set_string(var_name, string_value);
				else
					script.vars.set_string(var_name, index, string_value);
				break;
			case PersistedVarType::Vec2:
				if(index == -1)
					script.vars.set_vec2(var_name, x_value, y_value);
				else
					script.vars.set_vec2(var_name, index, x_value, y_value);
				break;
		}
		
		if(once)
			get_scene().remove_entity(self.as_entity());
	}
	
}