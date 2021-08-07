#include '../enums/GVB.cpp';
#include '../enums/VK.cpp';
#include '../input/common.cpp';
#include '../string.cpp';
#include 'navigation/navigation.cpp';
#include 'navigation/INavigable.cpp';
#include 'IKeyboardFocusListener.cpp';
#include 'IKeyboardFocus.cpp';
#include 'ModifierKey.cpp';

class Keyboard
{
	
	/// How long (frames) to pause after the initial key press before the key starts repeating
	int press_delay = 25;
	/// While a key is pressed, this specifies the speed (in frames) at which it will trigger
	int repeat_period = 1;
	/// If true any registered GVB keys will be consumed/cleared
	bool consume_gvb = true;
	/// Allow navigation with enter and tab
	bool allow_navigation = true;
	
	/// Is the control key down this frame
	bool ctrl;
	/// Is the shift key down this frame
	bool shift;
	/// Is the alt key down this frame
	bool alt;
	/// A combined bit flag of all modifiers
	int modifiers;
	
	IKeyboardFocusListener@ focus_listener;
	IKeyboardFocus@ _focus;
	INavigable@ _navigable;
	
	private input_api@ input;
	
	private array<int> gvb;
	private array<int> gvb_modifiers;
	private int num_gvb;
	
	private array<int> vk;
	private array<int> vk_modifiers;
	private int num_vk;
	
	private bool process_input;
	private int pressed_key;
	private bool pressed_gvb;
	private int pressed_timer;
	private int pressed_modifiers;
	private bool pressed_only_modifier;
	
	Keyboard(input_api@ input, IKeyboardFocusListener@ focus_listener=null)
	{
		@this.input = input;
		
		@this.focus_listener = focus_listener;
		
		reset();
	}
	
	IKeyboardFocus@ focus
	{
		get { return @_focus; }
		set { set_focus(@value, BlurAction::None); }
	}
	
	void step()
	{
		if(@focus == null)
			return;
		
		ctrl  = input.key_check_gvb(GVB::Control);
		shift = input.key_check_gvb(GVB::Shift);
		alt   = input.key_check_gvb(GVB::Alt);
		modifiers = (ctrl ? int(ModifierKey::Ctrl) : 0) | (shift ? int(ModifierKey::Shift) : 0) | (alt ? int(ModifierKey::Alt) : 0);
		
		bool key_consumed = false;
		
		if(allow_navigation && @_navigable != null)
		{
			const NavigateOn navigate_on = _navigable.navigate_on;
			
			if(navigation::consume(input, navigate_on, Tab, GVB::Tab, consume_gvb))
			{
				navigate();
				key_consumed = true;
			}
			else if(ctrl && navigation::consume(input, navigate_on, CtrlReturn, GVB::Return, consume_gvb))
			{
				navigate(Accepted);
				key_consumed = true;
			}
			else if(!ctrl && navigation::consume(input, navigate_on, Return, GVB::Return, consume_gvb))
			{
				navigate(Accepted);
				key_consumed = true;
			}
			else if(navigation::consume(input, navigate_on, Escape, GVB::Escape, consume_gvb))
			{
				set_focus(null, Cancelled);
				key_consumed = true;
			}
		}
		
		if(@focus == null)
			return;
		
		if(!key_consumed)
		{
			for(int i = num_vk - 1; i >= 0; i--)
			{
				const int key = vk[i];
				const int key_modifiers = vk_modifiers[i];
				
				const bool only = (key_modifiers & ModifierKey::Only) != 0;
				
				if(only
					? ((modifiers & key_modifiers) != (key_modifiers & ~ModifierKey::Only))
					: ((modifiers & ~key_modifiers) != 0)
				)
					continue;
				
				if(!input.key_check_pressed_vk(key) || !pressed_gvb && pressed_key == key)
					continue;
				
				vk[i] = vk[num_vk - 1];
				vk_modifiers[i] = vk_modifiers[--num_vk];
				
				if(pressed_key != -1)
				{
					if(pressed_gvb)
					{
						gvb[num_gvb] = pressed_key;
						gvb_modifiers[num_gvb++] = pressed_modifiers;
					}
					else
					{
						vk[num_vk] = pressed_key;
						vk_modifiers[num_vk++] = pressed_modifiers;
					}
				}
				
				pressed_key = key;
				pressed_gvb = false;
				pressed_timer = press_delay;
				pressed_modifiers = key_modifiers;
				pressed_only_modifier = only;
				
				focus.on_key_press(@this, key, false, process_input_key(key));
			}
			
			for(int i = num_gvb - 1; i >= 0; i--)
			{
				const int key = gvb[i];
				const int key_modifiers = gvb_modifiers[i];
				
				const bool only = (key_modifiers & ModifierKey::Only) != 0;
				
				if(only
					? ((modifiers & key_modifiers) != (key_modifiers & ~ModifierKey::Only))
					: ((modifiers & ~key_modifiers) != 0)
				)
					continue;
				
				if(!input.key_check_pressed_gvb(key) || pressed_gvb && pressed_key == key)
					continue;
				
				gvb[i] = gvb[num_gvb - 1];
				gvb_modifiers[i] = gvb_modifiers[--num_gvb];
				
				if(pressed_key != -1)
				{
					if(pressed_gvb)
					{
						gvb[num_gvb] = pressed_key;
						gvb_modifiers[num_gvb++] = pressed_modifiers;
					}
					else
					{
						vk[num_vk] = pressed_key;
						vk_modifiers[num_vk++] = pressed_modifiers;
					}
				}
				
				pressed_key = key;
				pressed_gvb = true;
				pressed_timer = press_delay;
				pressed_modifiers = key_modifiers;
				pressed_only_modifier = only;
				
				if(consume_gvb)
				{
					input.key_clear_gvb(key);
				}
				
				if(pressed_only_modifier
					? ((modifiers & pressed_modifiers) == (pressed_modifiers & ~ModifierKey::Only))
					: ((modifiers & ~pressed_modifiers) == 0)
				)
				{
					focus.on_key_press(@this, key, true, '');
				}
			}
		}
		
		if(pressed_key != -1)
		{
			if(pressed_timer == press_delay || pressed_gvb && input.key_check_gvb(pressed_key) || !pressed_gvb && input.key_check_vk(pressed_key))
			{
				if(pressed_timer-- == 0)
				{
					if(pressed_only_modifier
						? ((modifiers & pressed_modifiers) == (pressed_modifiers & ~ModifierKey::Only))
						: ((modifiers & ~pressed_modifiers) == 0)
					)
					{
						focus.on_key(@this, pressed_key, pressed_gvb, pressed_gvb ? '' : process_input_key(pressed_key));
					}
					
					pressed_timer = repeat_period;
				}
			}
			else
			{
				if(pressed_only_modifier
					? ((modifiers & pressed_modifiers) == (pressed_modifiers & ~ModifierKey::Only))
					: ((modifiers & ~pressed_modifiers) == 0)
				)
				{
					focus.on_key_release(@this, pressed_key, pressed_gvb);
				}
				
				if(pressed_gvb)
				{
					gvb[num_gvb] = pressed_key;
					gvb_modifiers[num_gvb++] = pressed_modifiers;
				}
				else
				{
					vk[num_vk] = pressed_key;
					vk_modifiers[num_vk++] = pressed_modifiers;
				}
				
				pressed_key = -1;
			}
		}
	}
	
	void set_focus(IKeyboardFocus@ new_focus, const BlurAction type)
	{
		if(@_focus == @new_focus)
			return;
		
		if(@_focus != null)
		{
			_focus.on_blur(@this, type);
		}
		
		reset();
		
		@_focus = @new_focus;
		
		if(@_focus != null)
		{
			@_navigable = cast<INavigable@>(@_focus);
			_focus.on_focus(@this);
		}
		else
		{
			@_navigable = null;
		}
		
		if(@focus_listener != null)
		{
			focus_listener.on_keyboard_focus_change(_focus);
		}
	}
	
	/// Registers the specified GVB key.
	void register_gvb(int gvb, const int allowed_modifiers=-1)
	{
		register_range_gvb(gvb, gvb, allowed_modifiers);
	}
	
	/// Registers all the GVB keys in the given range.
	void register_range_gvb(int start_index_gvb, int end_index_gvb, const int allowed_modifiers=-1)
	{
		if(end_index_gvb < start_index_gvb)
		{
			const int end_index_gvb_t = end_index_gvb;
			end_index_gvb = start_index_gvb;
			start_index_gvb = end_index_gvb_t;
		}
		
		start_index_gvb = clamp(start_index_gvb, -1, GVB::EditorAux + 1);
		end_index_gvb   = clamp(end_index_gvb,   -1, GVB::EditorAux + 1);
		
		const int count = end_index_gvb - start_index_gvb + 1;
		
		if(num_gvb + count >= int(gvb.length()))
		{
			gvb.resize(num_gvb + count + 16);
			gvb_modifiers.resize(num_gvb + count + 16);
		}
		
		for(int i = start_index_gvb; i <= end_index_gvb; i++)
		{
			if(i < 0 || i > GVB::EditorAux)
				continue;
			
			gvb[num_gvb] = i;
			gvb_modifiers[num_gvb++] = allowed_modifiers == -1 ? int(ModifierKey::All) : allowed_modifiers;
		}
	}
	
	/// Registers all the VK keys in the given range.
	void register_vk(int key_vk, const int allowed_modifiers=-1)
	{
		register_range_vk(key_vk, key_vk, allowed_modifiers);
	}
	
	/// Registers all the VK keys in the given range.
	void register_range_vk(int start_index_vk, int end_index_vk, const int allowed_modifiers=-1)
	{
		if(end_index_vk < start_index_vk)
		{
			const int end_index_gvb_t = end_index_vk;
			end_index_vk = start_index_vk;
			start_index_vk = end_index_gvb_t;
		}
		
		start_index_vk = clamp(start_index_vk, -1, VK::OemClear + 1);
		end_index_vk   = clamp(end_index_vk,   -1, VK::OemClear + 1);
		
		const int count = end_index_vk - start_index_vk + 1;
		
		if(num_vk + count >= int(vk.length()))
		{
			vk.resize(num_vk + count + 16);
			vk_modifiers.resize(num_vk + count + 16);
		}
		
		for(int i = start_index_vk; i <= end_index_vk; i++)
		{
			if(i < 0 || i > VK::OemClear)
				continue;
			
			vk[num_vk] = i;
			vk_modifiers[num_vk++] = allowed_modifiers == -1 ? int(ModifierKey::All): allowed_modifiers;
		}
	}
	
	/// Registers all GVB arrows keys
	void register_arrows_gvb(const int allowed_modifiers=-1)
	{
		register_range_gvb(GVB::UpArrow, GVB::RightArrow, allowed_modifiers);
	}
	
	/// Registers all text input keys and automatically processes and returns the character in the
	/// IKeyboardFocus "input" param of on_key_press and on_key, or an empty string of the key does not have a textual representation.
	void register_inputs()
	{
		process_input = true;
		
		register_vk(VK::Return, ModifierKey::Shift);
		register_vk(VK::Space, ModifierKey::Shift);
		register_range_vk(VK::Numpad0, VK::Divide);
		register_range_vk(VK::Digit0, VK::Z, ModifierKey::Shift);
		register_range_vk(VK::Oem1, VK::Oem7);
	}
	
	private void reset()
	{
		process_input = false;
		
		ctrl = false;
		shift = false;
		alt = false;
		
		num_gvb = 0;
		num_vk = 0;
		
		pressed_key = -1;
	}
	
	private void navigate(const BlurAction type=BlurAction::None)
	{
		set_focus(cast<IKeyboardFocus@>(shift
			? _navigable.previous_navigable(@_navigable)
			: _navigable.next_navigable(@_navigable)), type);
	}
	
	private string process_input_key(const int key)
	{
		if(key == VK::Space)
			return ' ';
		
		if(key == VK::Return)
			return '\n';
			
		if(key >= VK::Multiply && key <= VK::Divide)
		{
			string s = ' ';
			s[0] = key - VK::Multiply + 42;
			return s;
		}
		
		if(key >= VK::Numpad0 && key <= VK::Numpad9)
		{
			string s = ' ';
			s[0] = key - VK::Numpad0 + VK::Digit0;
			return s;
		}
		
		if(key >= VK::Digit0 && key <= VK::Digit9)
		{
			if(shift)
			{
				switch(key)
				{
					case VK::Digit0: return ')';
					case VK::Digit1: return '!';
					case VK::Digit2: return '@';
					case VK::Digit3: return '#';
					case VK::Digit4: return '$';
					case VK::Digit5: return '%';
					case VK::Digit6: return '^';
					case VK::Digit7: return '&';
					case VK::Digit8: return '*';
					case VK::Digit9: return '(';
				}
			}
			else
			{
				string s = ' ';
				s[0] = key;
				return s;
			}
		}
		
		if(key >= VK::A && key <= VK::Z)
		{
			string s = ' ';
			s[0] = shift ? key : key + 32;
			return s;
		}
		
		if(key >= VK::Oem1 && key <= VK::Oem7)
		{
			switch(key)
			{
				case VK::Oem1: 	    return !shift ? ';'  : ':';
				case VK::OemPlus:   return !shift ? '='  : '+';
				case VK::OemComma:  return !shift ? ','  : '<';
				case VK::OemMinus:  return !shift ? '-'  : '_';
				case VK::OemPeriod: return !shift ? '.'  : '>';
				case VK::Oem2: 	 	return !shift ? '/'  : '?';
				case VK::Oem3: 	 	return !shift ? '`'  : '~';
				case VK::Oem4: 	 	return !shift ? '['  : '{';
				case VK::Oem5: 	 	return !shift ? '\\' : '|';
				case VK::Oem6: 	 	return !shift ? ']'  : '}';
				case VK::Oem7: 	 	return !shift ? '\'' : '"';
			}
		}
		
		return '';
	}
	
}
