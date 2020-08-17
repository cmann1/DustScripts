#include '../enums/GVB.cpp';
#include '../enums/VK.cpp';
#include '../editor/common.cpp';
#include 'navigation/navigation.cpp';
#include 'IKeyboardFocusListener.cpp';
#include 'IKeyboardFocus.cpp';
#include 'navigation/INavigable.cpp';

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
	
	/// If true the ctrl, shift, and alt fields will be set accordingly each frame.
	/// This is reset each time a new element is focused.
	bool update_modifiers;
	
	/// Is the control key down this frame
	bool ctrl;
	/// Is the shift key down this frame
	bool shift;
	/// Is the alt key down this frame
	bool alt;
	
	IKeyboardFocusListener@ focus_listener;
	IKeyboardFocus@ _focus;
	INavigable@ _navigable;
	
	private editor_api@ editor;
	
	private array<int> gvb;
	private int num_gvb;
	private array<int> vk;
	private int num_vk;
	
	private array<int> gvb_up;
	private int num_gvb_up;
	private array<int> vk_up;
	private int num_vk_up;
	
	private array<int> gvb_down;
	private int num_gvb_down;
	private array<int> vk_down;
	private int num_vk_down;
	
	private int pressed_key;
	private bool pressed_gvb;
	private int pressed_timer;
	
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
		
		if(update_modifiers)
		{
			ctrl  = editor.key_check_gvb(GVB::Control);
			shift = editor.key_check_gvb(GVB::Shift);
			alt   = editor.key_check_gvb(GVB::Alt);
		}
		
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
		
		for(int i = num_vk_up - 1; i >= 0; i--)
		{
			const int key = vk_up[i];
			
			if(!editor.key_check_pressed_vk(key))
				continue;
			
			vk_up[i] = vk_up[--num_vk_up];
			
			if(pressed_key != -1)
			{
				if(pressed_gvb)
				{
					gvb_up[num_gvb_up++] = pressed_key;
				}
				else
				{
					vk_up[num_vk_up++] = pressed_key;
				}
			}
			
			focus.on_key_press(@this, key, false);
			pressed_key = key;
			pressed_gvb = false;
			pressed_timer = press_delay;
		}
		
		for(int i = num_gvb_up - 1; i >= 0; i--)
		{
			const int key = gvb_up[i];
			
			if(!editor.key_check_pressed_gvb(key))
				continue;
			
			gvb_up[i] = gvb_up[--num_gvb_up];
			
			if(consume_gvb)
			{
				editor.key_clear_gvb(key);
			}
			
			if(pressed_key != -1)
			{
				if(pressed_gvb)
				{
					gvb_up[num_gvb_up++] = pressed_key;
				}
				else
				{
					vk_up[num_vk_up++] = pressed_key;
				}
			}
			
			focus.on_key_press(@this, key, true);
			pressed_key = key;
			pressed_gvb = true;
			pressed_timer = press_delay;
		}
		
		if(pressed_key != -1)
		{
			if(pressed_timer == press_delay || pressed_gvb && editor.key_check_gvb(pressed_key) || !pressed_gvb && editor.key_check_vk(pressed_key))
			{
				if(pressed_timer-- == 0)
				{
					focus.on_key(@this, pressed_key, pressed_gvb);
					pressed_timer = repeat_period;
				}
			}
			else
			{
				focus.on_key_release(@this, pressed_key, pressed_gvb);
				
				if(pressed_gvb)
				{
					gvb_up[num_gvb_up++] = pressed_key;
				}
				else
				{
					vk_up[num_vk_up++] = pressed_key;
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
	void register_gvb(int start_index_gvb, int end_index_gvb=-1)
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
			gvb_up.resize(num_gvb + count + 16);
		}
		
		for(int i = start_index_gvb; i <= end_index_gvb; i++)
		{
			gvb[num_gvb++] = i;
			gvb_up[num_gvb_up++] = i;
		}
	}
	
	/// Registers all the VK keys in the given range.
	void register_vk(int start_index_vk, int end_index_vk=-1)
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
			vk_up.resize(num_vk + count + 16);
		}
		
		for(int i = start_index_vk; i <= end_index_vk; i++)
		{
			vk[num_vk++] = i;
			vk_up[num_vk_up++] = i;
		}
	}
	
	/// Registers all GVB arrows keys
	void register_arrows_gvb()
	{
		register_gvb(GVB::UpArrow, GVB::RightArrow);
	}
	
	private void reset()
	{
		update_modifiers = false;
		ctrl = false;
		shift = false;
		alt = false;
		
		num_gvb = 0;
		num_vk = 0;
		num_vk_up = 0;
		num_gvb_up = 0;
		
		pressed_key = -1;
	}
	
	private void navigate(const BlurAction type=BlurAction::None)
	{
		set_focus(cast<IKeyboardFocus@>(shift
			? _navigable.previous_navigable(@_navigable)
			: _navigable.next_navigable(@_navigable)), type);
	}
	
}