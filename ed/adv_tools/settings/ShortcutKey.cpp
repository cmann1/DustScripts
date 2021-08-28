#include '../../../lib/enums/VK.cpp';
#include '../../../lib/input/ModifierKey.cpp';
#include '../../../lib/utils/vk_from_name.cpp';

class ShortcutKey
{
	
	AdvToolScript@ script;
	
	int modifiers = ModifierKey::None;
	int vk = -1;
	int priority = 0;
	
	ShortcutKey@ init(AdvToolScript@ script)
	{
		@this.script = script;
		return this;
	}
	
	private void _set(const int vk, const int modifiers, const int priority)
	{
		this.vk = vk;
		this.modifiers = modifiers;
		this.priority = priority;
	}
	
	ShortcutKey@ set(const int vk, const int modifiers=ModifierKey::None, const int priority=0)
	{
		@this.script = script;
		
		_set(vk, modifiers, priority);
		return this;
	}
	
	ShortcutKey@ from_string(const string &in str, const int priority=0)
	{
		_set(-1, ModifierKey::None, priority);
		
		// Parse key string in the format MODIFIER+KEY
		array<string> parts = str.split('+');
		
		string key_str = string::trim(parts[int(parts.length) - 1]);
		if(key_str == '')
			return this;
		
		vk = VK::from_name(key_str);
		
		// The key itself must not be a modifier
		if(
			vk <= 0 ||
			(vk >= VK::Shift && vk <= VK::Alt) ||
			(vk >= VK::LeftShift && vk <= VK::RightMenu))
		{
			vk = -1;
			return this;
		}
		
		// Parse modifier keys
		for(int i = 0, length = int(parts.length) - 1; i < length; i++)
		{
			key_str = string::trim(parts[i]);
			if(key_str == '')
				continue;
			
			const int mod_vk = VK::from_name(key_str);
			
			switch(mod_vk)
			{
				case VK::Control: modifiers |= ModifierKey::Ctrl; break;
				case VK::Shift: modifiers |= ModifierKey::Shift; break;
				case VK::Alt: modifiers |= ModifierKey::Alt; break;
			}
		}
		
		return this;
	}
	
	ShortcutKey@ from_config(const string &in name, const string &in default_str='', const int priority=0)
	{
		return from_string(script.config.get_string(name, default_str), priority);
	}
	
	string to_string()
	{
		string str = VK::to_name(vk);
		
		if(str == '')
			return str;
		
		if((modifiers & ModifierKey::Alt) != 0)
			str = VK::to_name(VK::Alt) + '+' + str;
		if((modifiers & ModifierKey::Shift) != 0)
			str = VK::to_name(VK::Shift) + '+' + str;
		if((modifiers & ModifierKey::Ctrl) != 0)
			str = VK::to_name(VK::Control) + '+' + str;
		
		return str;
	}
	
	bool is_set() const
	{
		return vk > 0;
	}
	
	bool check()
	{
		if(vk <= 0 || @script == null)
			return false;
		
		if(
			script.ctrl != ((modifiers & ModifierKey::Ctrl) != 0) ||
			script.shift != ((modifiers & ModifierKey::Shift) != 0) ||
			script.alt != ((modifiers & ModifierKey::Alt) != 0)
		)
			return false;
		
		return script.input.key_check_pressed_vk(vk);
	}
	
}
