#include '../../../lib/utils/chr.cpp';
#include '../../../lib/utils/vk_from_name.cpp';
#include '../../../lib/string.cpp';

#include 'ConfigState.cpp';

class Config
{
	
	bool EnableShortcuts;
	float ToolbarIconSize;
	uint UIIconColour;
	
	private dictionary values;
	
	bool load()
	{
		if(!load_embed(Settings::ConfigEmbedKey, Settings::ConfigFile))
		{
			init();
			return false;
		}
		
		values.deleteAll();
		const string data = get_embed_value(Settings::ConfigEmbedKey);
		
		//const uint t = get_time_us();
		
		int state = ConfigState::Start;
		string key = '', value = '';
		
		for(uint i = 0, length = data.length; i < length; i++)
		{
			int chr = data[i];
			
			if(chr == chr::CarriageReturn)
			{
				if(i < length - 1 && data[i + 1] == chr::NewLine)
					continue;
				
				chr = chr::NewLine;
			}
			
			switch(state)
			{
				case ConfigState::Start:
				{
					switch(chr)
					{
						case chr::SemiColon:
							state = ConfigState::Comment;
							break;
						case chr::Equals:
							state = ConfigState::Value;
							break;
						case chr::NewLine:
							break;
						default:
							key = string::chr(chr);
							state = ConfigState::Key;
					}
					break;
				}
				case ConfigState::Comment:
				{
					if(chr == chr::NewLine)
					{
						state = ConfigState::Start;
					}
					break;
				}
				case ConfigState::Key:
				{
					switch(chr)
					{
						case chr::NewLine:
							key = '';
							state = ConfigState::Start;
							break;
						case chr::Equals:
							state = ConfigState::Value;
							break;
						default:
							key += string::chr(chr);
							break;
					}
					break;
				}
				case ConfigState::Value:
				{
					switch(chr)
					{
						case chr::NewLine:
							add(key, value);
							key = '';
							value = '';
							state = ConfigState::Start;
							break;
						default:
							value += string::chr(chr);
							break;
					}
					break;
				}
			}
		}
		
		add(key, value);
		//puts((get_time_us() - t)/1000+'ms');
		
		init();
		return true;
	}
	
	private void init()
	{
		EnableShortcuts = get_bool('EnableShortcuts', true);
		ToolbarIconSize = round(get_float('ToolbarIconSize', 30));
		UIIconColour = get_colour('UIIconColour', 0xffffffff);
	}
	
	private void add(string &in key, const string &in value)
	{
		key = string::trim(key);
		if(key == '')
			return;
		
		values[key] = value;
	}
	
	bool has_value(const string &in name, const bool ignore_empty=true)
	{
		if(!values.exists(name))
			return false;
		
		if(!ignore_empty)
			return true;
		
		return string::trim(string(values[name])) != '';
	}
	
	bool get_bool(const string &in name, const bool default_value=false)
	{
		if(!values.exists(name))
			return default_value;
		
		return string(values[name]) == 'true';
	}
	
	float get_float(const string &in name, const float default_value=0)
	{
		if(!values.exists(name))
			return default_value;
		
		return parseFloat(string(values[name]));
	}
	
	int get_vk(const string &in name, const int default_value=-1)
	{
		if(!values.exists(name))
			return default_value;
		
		const string key_name = string::trim(string(values[name]));
		return key_name != '' ? VK::from_name(key_name) : -1;
	}
	
	uint get_colour(const string &in name, const uint default_value=0x00000000)
	{
		if(!values.exists(name))
			return default_value;
		
		return string::try_parse_rgb(string(values[name]), true, true);
	}
	
	bool compare_float(const string &in name, const float value)
	{
		return !values.exists(name) || get_float(name, value) == value;
	}
	
	bool compare_colour(const string &in name, const uint value)
	{
		if(!has_value(name))
			return true;
		
		return get_colour(name, value) == value;
	}
	
}
