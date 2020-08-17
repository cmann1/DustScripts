#include '../enums/GVB.cpp';
#include '../enums/VK.cpp';
#include '../editor/common.cpp';
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
	
	private editor_api@ editor;
	
	private array<int> gvb;
	private array<int> gvb_modifiers;
	private int num_gvb;
	
	private array<int> vk;
	private array<int> vk_modifiers;
	private int num_vk;
	
	private int pressed_key;
	private bool pressed_gvb;
	private int pressed_timer;
	private int pressed_modifiers;
	
	Keyboard(IKeyboardFocusListener@ focus_listener=null)
	{
		@editor = get_editor_api();
		
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
		
		ctrl  = editor.key_check_gvb(GVB::Control);
		shift = editor.key_check_gvb(GVB::Shift);
		alt   = editor.key_check_gvb(GVB::Alt);
		modifiers = (ctrl ? int(ModifierKey::Ctrl) : 0) | (shift ? int(ModifierKey::Shift) : 0) | (alt ? int(ModifierKey::Alt) : 0);
		
		if(allow_navigation && @_navigable != null)
		{
			const NavigateOn navigate_on = _navigable.navigate_on;
			
			if(navigation::consume(editor, navigate_on, Tab, GVB::Tab, consume_gvb))
			{
				navigate();
			}
			else if(ctrl && navigation::consume(editor, navigate_on, CtrlReturn, GVB::Return, consume_gvb))
			{
				navigate(Accepted);
			}
			else if(!ctrl && navigation::consume(editor, navigate_on, Return, GVB::Return, consume_gvb))
			{
				navigate(Accepted);
			}
			else if(navigation::consume(editor, navigate_on, Escape, GVB::Escape, consume_gvb))
			{
				set_focus(null, Cancelled);
			}
		}
		
		if(@focus == null)
			return;
		
		for(int i = num_vk - 1; i >= 0; i--)
		{
			const int key = vk[i];
			const int key_modifiers = vk_modifiers[i];
			
			if(!editor.key_check_pressed_vk(key))
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
			
			if((modifiers & ~pressed_modifiers) == 0)
			{
				focus.on_key_press(@this, key, false);
			}
		}
		
		for(int i = num_gvb - 1; i >= 0; i--)
		{
			const int key = gvb[i];
			const int key_modifiers = gvb_modifiers[i];
			
			if(!editor.key_check_pressed_gvb(key))
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
			
			if(consume_gvb)
			{
				editor.key_clear_gvb(key);
			}
			
			if((modifiers & ~pressed_modifiers) == 0)
			{
				focus.on_key_press(@this, key, true);
			}
		}
		
		if(pressed_key != -1)
		{
			if(pressed_timer == press_delay || pressed_gvb && editor.key_check_gvb(pressed_key) || !pressed_gvb && editor.key_check_vk(pressed_key))
			{
				if(pressed_timer-- == 0)
				{
					if((modifiers & ~pressed_modifiers) == 0)
					{
						focus.on_key(@this, pressed_key, pressed_gvb);
					}
					
					pressed_timer = repeat_period;
				}
			}
			else
			{
				if((modifiers & ~pressed_modifiers) == 0)
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
	
	/// Registers all the GVB keys in the given range.
	void register_gvb(int start_index_gvb, int end_index_gvb=-1, const int allowed_modifiers=-1)
	{
		if(start_index_gvb < 0 || start_index_gvb > GVB::EditorAux)
			return;
		
		if(end_index_gvb == -1)
		{
			end_index_gvb = start_index_gvb;
		}
		
		if(end_index_gvb < start_index_gvb)
		{
			const int end_index_gvb_t = end_index_gvb;
			end_index_gvb = start_index_gvb;
			start_index_gvb = end_index_gvb_t;
		}
		
		const int count = end_index_gvb - start_index_gvb + 1;
		
		if(num_gvb + count >= int(gvb.length()))
		{
			gvb.resize(num_gvb + count + 16);
			gvb_modifiers.resize(num_gvb + count + 16);
		}
		
		for(int i = start_index_gvb; i <= end_index_gvb; i++)
		{
			gvb[num_gvb] = i;
			gvb_modifiers[num_gvb++] = allowed_modifiers == -1 ? int(ModifierKey::All) : allowed_modifiers;
		}
	}
	
	/// Registers all the VK keys in the given range.
	void register_vk(int start_index_vk, int end_index_vk=-1, const int allowed_modifiers=-1)
	{
		if(start_index_vk < 0 || start_index_vk > VK::OemClear)
			return;
		
		if(end_index_vk == -1)
		{
			end_index_vk = start_index_vk;
		}
		
		if(end_index_vk < start_index_vk)
		{
			const int end_index_gvb_t = end_index_vk;
			end_index_vk = start_index_vk;
			start_index_vk = end_index_gvb_t;
		}
		
		const int count = end_index_vk - start_index_vk + 1;
		
		if(num_vk + count >= int(vk.length()))
		{
			vk.resize(num_vk + count + 16);
			vk_modifiers.resize(num_vk + count + 16);
		}
		
		for(int i = start_index_vk; i <= end_index_vk; i++)
		{
			vk[num_vk] = i;
			vk_modifiers[num_vk++] = allowed_modifiers == -1 ? int(ModifierKey::All): allowed_modifiers;
		}
	}
	
	/// Registers all GVB arrows keys
	void register_arrows_gvb(const int allowed_modifiers=-1)
	{
		register_gvb(GVB::UpArrow, GVB::RightArrow, allowed_modifiers);
	}
	
	private void reset()
	{
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
	
}