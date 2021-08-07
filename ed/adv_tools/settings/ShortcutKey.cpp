#include '../../../lib/enums/VK.cpp';
#include '../../../lib/utils/vk_from_name.cpp';

class ShortcutKey
{
	
	AdvToolScript@ script;
	
	bool ctrl;
	bool shift;
	bool alt;
	int key = -1;
	
	void set(AdvToolScript@ script, const int key, const bool ctrl=false, bool shift=false, const bool alt=false)
	{
		@this.script = script;
		
		this.key = key;
		this.ctrl = ctrl;
		this.shift = shift;
		this.alt = alt;
	}
	
	void set(AdvToolScript@ script, const string &in str)
	{
		@this.script = script;
		
		key = -1;
		ctrl = false;
		shift = false;
		alt = false;
		
		// Parse key string in the format MODIFIER+KEY
		array<string> parts = str.split('+');
		
		string key_str = string::trim(parts[int(parts.length) - 1]);
		if(key_str == '')
			return;
		
		key = VK::from_name(key_str);
		
		// The key itself must not be a modifier
		if(
			key == -1 ||
			(key >= VK::Shift && key <= VK::Alt) ||
			(key >= VK::LeftShift && key <= VK::RightMenu))
		{
			key = -1;
			return;
		}
		
		// Parse modifier keys
		for(int i = 0, length = int(parts.length) - 1; i < length; i++)
		{
			key_str = string::trim(parts[i]);
			if(key_str == '')
				continue;
			
			const int vk = VK::from_name(key_str);
			
			switch(vk)
			{
				case VK::Control: ctrl = true; break;
				case VK::Shift: shift = true; break;
				case VK::Alt: alt = true; break;
			}
		}
	}
	
	bool check()
	{
		if(key == -1 || @script == null)
			return false;
		
		if(script.ctrl != ctrl || script.shift != shift || script.alt != alt)
			return false;
		
		return script.input.key_check_pressed_vk(key);
	}
	
}
