#include '../../utils/colour.cpp';
#include 'colour_picker/BasicColourSwatch.cpp';

namespace ColourSwatch { const string TYPE_NAME = 'ColourSwatch'; }

class ColourSwatch : BasicColourSwatch
{
	
	Event activate;
	Event change;
	
	/* If true the colour can be quickly copied and pasted with Right mouse and Alt+Right mouse respectively
     * without needing to open the colour picker. */
	bool allow_quick_copy = true;
	
	protected float _h, _s, _l;
	protected float _h_prev, _s_prev, _l_prev;
	protected int _a;
	protected int _a_prev;
	protected uint _colour = 0xffaa4444;
	
	private ColourPickerSettings picker_setting;
	protected bool selected;
	
	ColourSwatch(UI@ ui)
	{
		super(ui);
		
		update_from_rgb();
		_h_prev = _h;
		_s_prev = _s;
		_l_prev = _l;
		_a_prev = _a;
		
		mouse_enabled = true;
		
		background_colour = _colour;
		border_size = ui.style.border_size;
		
		@picker_setting.on_change_callback = EventCallback(on_colour_picker_change);
		@picker_setting.on_accept_callback = picker_setting.on_change_callback;
	}
	
	string element_type { get const override { return ColourSwatch::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	/* If true uses the HSL values instead of the `colour` for cases where setting specific
	 * values are needed, e.g. a hue of 0/1 are equivalent so it's not possible to explicity choose 1
	 * when assigning the `colour` value. */
	bool force_hsl
	{
		get const { return picker_setting.force_hsl; }
		set
		{
			if(picker_setting.force_hsl == value)
				return;
			
			picker_setting.force_hsl = value;
			update_colour_picker();
		}
	}
	
	/// Sets the current colour.
	uint colour
	{
		get const { return _colour; }
		set
		{
			if(_colour == value)
				return;
			
			_colour = value;
			background_colour = _colour;
			
			update_from_rgb();
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	float h
	{
		get const { return _h; }
		set
		{
			value = clamp01(value);
			if(value == _h)
				return;
			
			_h = value;
			update_from_hsl();
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	float s
	{
		get const { return _s; }
		set
		{
			value = clamp01(value);
			if(value == _s)
				return;
			
			_s = value;
			update_from_hsl();
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	float l
	{
		get const { return _l; }
		set
		{
			value = clamp01(value);
			if(value == _l)
				return;
			
			_l = value;
			update_from_hsl();
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	int a
	{
		get const { return _a; }
		set
		{
			value = clamp(value, 0, 255);
			if(value == _a)
				return;
			
			_a = value;
			_colour = (_colour & 0x00ffffff) | ((_a & 0xff) << 24);
			background_colour = _colour;
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	/* Show the HSL inputs. */
	bool show_hsl
	{
		get const { return picker_setting.show_hsl; }
		set
		{
			if(picker_setting.show_hsl == value)
				return;
			
			picker_setting.show_hsl = value;
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	/* Show the RGB inputs. */
	bool show_rgb
	{
		get const { return picker_setting.show_rgb; }
		set
		{
			if(picker_setting.show_rgb == value)
				return;
			
			picker_setting.show_rgb = value;
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	/* Show the alpha inputs. */
	bool show_alpha
	{
		get const { return picker_setting.show_alpha; }
		set
		{
			if(picker_setting.show_alpha == value)
				return;
			
			picker_setting.show_alpha = value;
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	/* Show the hex input. */
	bool show_hex
	{
		get const { return picker_setting.show_hex; }
		set
		{
			if(picker_setting.show_hex == value)
				return;
			
			picker_setting.show_hex = value;
			update_colour_picker();
			
			dispatch_change();
		}
	}
	
	void set_hsl(float h, float s, float l)
	{
		set_hsl(h, s, l, _a);
	}
	
	void set_hsl(float h, float s, float l, int a)
	{
		h = clamp01(h);
		s = clamp01(s);
		l = clamp01(l);
		a = clamp(a, 0, 255);
		
		if(h == _h && s == _s && l == _l && a == _a)
			return;
		
		_h = h;
		_s = s;
		_l = l;
		_a = a;
		update_from_hsl();
		update_colour_picker();
		
		dispatch_change();
	}
	
	void set_rgb(int r, int g, int b)
	{
		set_rgb(r, g, b, _a);
	}
	
	void set_rgb(int r, int g, int b, int a)
	{
		r = clamp(r, 0, 255);
		g = clamp(g, 0, 255);
		b = clamp(b, 0, 255);
		a = clamp(a, 0, 255);
		
		_colour = rgba(r, g, b, a);
		update_from_rgb();
		update_colour_picker();
		
		dispatch_change();
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Methods
	// ///////////////////////////////////////////////////////////////////
	
	/// Show the colour picker popup
	void choose()
	{
		if(selected)
			return;
		
		selected = true;
		show_colour_picker();
		
		ui.hide_tooltip(this);
		
		ui._dispatch_event(@activate, EventType::OPEN, this);
	}
	
	/// Hide the colour picker popup
	void hide(const bool accept=true)
	{
		if(!selected)
			return;
		
		_colour = accept
			? ui.colour_picker_instance.colour
			: ui.colour_picker_instance.previous_colour;
		background_colour = _colour;
		_h = accept ? ui.colour_picker_instance.h : _h_prev;
		_s = accept ? ui.colour_picker_instance.s : _s_prev;
		_l = accept ? ui.colour_picker_instance.l : _l_prev;
		_a = accept ? ui.colour_picker_instance.a : _a_prev;
		_h_prev = _h;
		_s_prev = _s;
		_l_prev = _l;
		_a_prev = _a;
		
		selected = false;
		ui._dispatch_event(@change, accept ? EventType::ACCEPT : EventType::CANCEL, this);
		ui._dispatch_event(@activate, EventType::CLOSE, this);
	}
	
	/// Is the colour picker popup currently open
	bool open
	{
		get const { return selected; }
	}
	
	protected void show_colour_picker()
	{
		picker_setting.colour = _colour;
		picker_setting.set_hsl(_h, _s, _l, _a);
		
		ui.show_colour_picker(picker_setting);
	}
	
	protected void update_colour_picker()
	{
		if(selected)
		{
			show_colour_picker();
		}
	}
	
	protected void update_from_hsl()
	{
		_colour = hsv_to_rgb(_h, _s, _l) | ((_a & 0xff) << 24);
		background_colour = _colour;
	}
	
	protected void update_from_rgb()
	{
		int _r, _g, _b;
		int_to_rgba(_colour, _r, _g, _b, _a);
		rgb_to_hsv(_r, _g, _b, _h, _s, _l);
	}
	
	protected void dispatch_change()
	{
		ui._dispatch_event(@change, EventType::CHANGE, this);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		border_colour = style.get_interactive_element_border_colour(hovered, selected, false, disabled);
		border_size = selected ? style.selected_border_size : style.border_size;
		BasicColourSwatch::_draw(style, ctx);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_button_click(EventInfo@ event) override
	{
		if(ui.has_input && allow_quick_copy && event.button == MouseButton::Right)
		{
			if(ui.input.key_check_gvb(GVB::Alt))
			{
				colour = string::try_parse_rgb(ui.input.clipboard);
			}
			else
			{
				ui.input.clipboard = hex(_colour);
			}
		}
	}
	
	void _mouse_click(EventInfo@ event) override
	{
		choose();
	}
	
	void on_colour_picker_change(EventInfo@ event)
	{
		if(event.type == EventType::CLOSE)
		{
			hide(true);
		}
		else if(event.type == EventType::CHANGE)
		{
			if(!picker_setting.force_hsl)
			{
				_colour = ui.colour_picker_instance.colour;
				background_colour = _colour;
				update_from_rgb();
			}
			else
			{
				_h = ui.colour_picker_instance.h;
				_s = ui.colour_picker_instance.s;
				_l = ui.colour_picker_instance.l;
				_a = ui.colour_picker_instance.a;
				update_from_hsl();
			}
			
			@event.target = this;
			change.dispatch(event);
		}
		else
		{
			hide(event.type == EventType::ACCEPT);
		}
	}
	
}
