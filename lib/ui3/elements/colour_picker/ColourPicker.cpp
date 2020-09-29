#include '../../../fonts.cpp';
#include '../../../utils/colour.cpp';
#include '../LockedContainer.cpp';
#include '../Button.cpp';
#include '../Label.cpp';
#include '../TextBox.cpp';
#include 'ColourSlider.cpp';
#include 'BasicColourSwatch.cpp';

namespace ColourPicker { const string TYPE_NAME = 'ColourPicker'; }

class ColourPicker : LockedContainer, IStepHandler
{
	
	Event change;
	Event accept;
	
	protected ColourSlider@ slider_h, slider_s, slider_l;
	protected ColourSlider@ slider_r, slider_g, slider_b, slider_a;
	protected Label@ label_h, label_s, label_l;
	protected Label@ label_r, label_g, label_b, label_a;
	protected TextBox@ input_h, input_s, input_l;
	protected TextBox@ input_r, input_g, input_b, input_a;
	protected TextBox@ input_hex;
	protected BasicColourSwatch@ swatch;
	protected BasicColourSwatch@ swatch_previous;
	protected Button@ ok_button;
	protected Button@ cancel_button;
	
	protected NavigationGroup@ navigation_group;
	
	protected uint _colour = 0xffffffff;
	protected uint _previous_colour = 0xffffffff;
	protected int _r, _g, _b, _a;
	protected float _h, _s, _l;
	protected bool _show_alpha = true;
	protected bool _show_buttons = true;
	protected bool _accept_on_keybaord = false;
	
	protected EventCallback@ on_slider_change_delegate;
	protected EventCallback@ on_input_change_delegate;
	protected EventCallback@ on_input_accept_delegate;
	protected EventCallback@ on_button_click_delegate;
	
	ColourPicker(UI@ ui)
	{
		super(ui);
		
		@navigation_group = NavigationGroup(ui);
		@on_slider_change_delegate = EventCallback(on_slider_change);
		@on_input_change_delegate = EventCallback(on_input_change);
		@on_input_accept_delegate = EventCallback(on_input_accept);
		@on_button_click_delegate = EventCallback(on_button_click);
		
		@label_h = create_label('H');
		@label_s = create_label('S');
		@label_l = create_label('L');
		@label_r = create_label('R');
		@label_g = create_label('G');
		@label_b = create_label('B');
		@label_a = create_label('A');
		
		@slider_h = create_slider('H', H);
		@slider_s = create_slider('S', S);
		@slider_l = create_slider('L', L);
		@slider_r = create_slider('R', R, 255);
		@slider_g = create_slider('G', G, 255);
		@slider_b = create_slider('B', B, 255);
		@slider_a = create_slider('A', A, 255);
		
		@input_h = create_input('H', Decimal);
		@input_s = create_input('S', Decimal);
		@input_l = create_input('L', Decimal);
		
		@input_r = create_input('R', Integer);
		@input_g = create_input('G', Integer);
		@input_b = create_input('B', Integer);
		@input_a = create_input('A', Integer);
		
		@input_hex = create_input('Hex', Hex);
		input_hex.width = 95;
		
		@ok_button = Button(ui, 'OK', Centre);
		@cancel_button = Button(ui, 'Cancel', Centre);
		ok_button.name = EventType::ACCEPT;
		cancel_button.name = EventType::CANCEL;
		ok_button.mouse_click.on(on_button_click_delegate);
		cancel_button.mouse_click.on(on_button_click_delegate);
		Container::add_child(ok_button);
		Container::add_child(cancel_button);
		
		@swatch = BasicColourSwatch(ui);
		@swatch_previous = BasicColourSwatch(ui);
		Container::add_child(swatch);
		Container::add_child(swatch_previous);
		
		calculate_colour_values();
		update_all();
		
		mouse_self = false;
		
		_do_layout(null);
		fit_to_contents(true);
	}
	
	string element_type { get const override { return ColourPicker::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	/// Returns the current colours as an AARRGGBB interger
	uint colour
	{
		get const { return _colour; }
		set
		{
			if(_colour == value)
				return;
			
			_colour = value;
			calculate_colour_values();
			update_all();
			
			trigger_change();
		}
	}
	
	/// Returns the previous colours as an AARRGGBB interger
	uint previous_colour
	{
		get const { return _previous_colour; }
		set
		{
			_previous_colour = value;
			update_swatches();
		}
	}
	
	/// Returns the current hue in the range 0 ... 1
	float h
	{
		get const { return _h; }
		set { if(_h != value) update_from_hsl(value, H); }
	}
	
	/// Returns the current saturation in the range 0 ... 1
	float s
	{
		get const { return _s; }
		set { if(_s != value) update_from_hsl(value, S); }
	}
	
	/// Returns the current value in the range 0 ... 1
	float l
	{
		get const { return _l; }
		set { if(_l != value) update_from_hsl(value, L); }
	}
	
	/// Returns the current red in the range 0 ... 255
	int r
	{
		get const { return _r; }
		set { if(_r != value) update_from_rgb(value, R); }
	}
	
	/// Returns the current green in the range 0 ... 255
	int g
	{
		get const { return _g; }
		set { if(_g != value) update_from_rgb(value, G); }
	}
	
	/// Returns the current blue in the range 0 ... 255
	int b
	{
		get const { return _b; }
		set { if(_b != value) update_from_rgb(value, B); }
	}
	
	/// Returns the current alpha in the range 0 ... 255
	int a
	{
		get const { return _a; }
		set { if(_a != value) update_from_rgb(value, A); }
	}
	
	/// Whether or not to show the alpha slider and input
	bool show_alpha
	{
		get const { return _show_alpha; }
		set
		{
			if(_show_alpha == value)
				return;
			
			_show_alpha = value;
			label_a.visible = _show_alpha;
			input_a.visible = _show_alpha;
			slider_a.visible = _show_alpha;
			update_inputs();
			
			_do_layout(null);
			validate_layout = true;
			fit_to_contents(true);
			
			if(_show_alpha)
			{
				navigation_group.remove(input_a);
			}
			else
			{
				navigation_group.add_before(input_a, input_hex);
			}
		}
	}
	
	/// Whether or not to show the ok and cancel buttons
	bool show_buttons
	{
		get const { return _show_buttons; }
		set
		{
			if(_show_buttons == value)
				return;
			
			_show_buttons = value;
			ok_button.visible = _show_buttons;
			cancel_button.visible = _show_buttons;
			
			_do_layout(null);
			validate_layout = true;
			fit_to_contents(true);
		}
	}
	
	/// Set to true to start listening for the enter and escape key, and false to stop.
	/// The accept/cancel event will trigger when an input does not have focus, and the enter or escape key is pressed.
	bool accept_on_keybaord
	{
		get const { return _accept_on_keybaord; }
		set
		{
			if(_accept_on_keybaord == value)
				return;
			
			_accept_on_keybaord = value;
			
			if(ui._has_editor && _accept_on_keybaord)
			{
				ui._step_subscribe(this);
			}
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Methods
	// ///////////////////////////////////////////////////////////////////
	
	private Label@ create_label(const string text)
	{
		Label@ label = Label(ui, text);
		label.set_font(font::ENVY_BOLD, 20);
		label.text_align_h = TextAlign::Centre;
		label.align_h = GraphicAlign::Centre;
		label.align_v = GraphicAlign::Middle;
		label.mouse_enabled = false;
		label.width = 24;
		Container::add_child(label);
		
		return label;
	}
	
	private ColourSlider@ create_slider(const string name, const ColourSliderType type, const float value_multiplier=1)
	{
		ColourSlider@ slider = ColourSlider(ui);
		slider.name = name;
		slider.type = type;
		slider.value_multiplier = value_multiplier;
		slider.change.on(on_slider_change_delegate);
		
		Container::add_child(slider);
		
		return slider;
	}
	
	private TextBox@ create_input(const string name, const CharacterValidation validation)
	{
		TextBox@ text_box = TextBox(ui, '', font::ENVY_BOLD, 20);
		text_box.name = name;
		text_box.width = 65;
		text_box.character_validation = validation;
		text_box.allow_negative = false;
		text_box.change.on(on_input_change_delegate);
		text_box.accept.on(on_input_accept_delegate);
		Container::add_child(text_box);
		
		navigation_group.add_last(text_box);
		
		return text_box;
	}
	
	private void calculate_colour_values()
	{
		int_to_rgba(colour, _r, _g, _b, _a);
		rgb_to_hsv(_r, _g, _b, _h, _s, _l);
	}
	
	private void update_sliders()
	{
		slider_h.value = _h;
		slider_s.value = _s;
		slider_l.value = _l;
		slider_r.colour = _colour;
		slider_g.colour = _colour;
		slider_b.colour = _colour;
		slider_a.colour = _colour;
		slider_r.value = _r / 255.0;
		slider_g.value = _g / 255.0;
		slider_b.value = _b / 255.0;
		slider_a.value = _a / 255.0;
		
		slider_s.hue = _h;
		slider_l.hue = _h;
	}
	
	private void update_inputs(TextBox@ updated_from=null)
	{
		// HSL
		
		if(@updated_from != @input_h)
		{
			input_h.ignore_next_change();
			input_h.float_value = _h;
		}
		
		if(@updated_from != @input_s)
		{
			input_s.ignore_next_change();
			input_s.float_value = _s;
		}
		
		if(@updated_from != @input_l)
		{
			input_l.ignore_next_change();
			input_l.float_value = _l;
		}
		
		// RGBA
		
		if(@updated_from != @input_r)
		{
			input_r.ignore_next_change();
			input_r.int_value = _r;
		}
		
		if(@updated_from != @input_g)
		{
			input_g.ignore_next_change();
			input_g.int_value = _g;
		}
		
		if(@updated_from != @input_b)
		{
			input_b.ignore_next_change();
			input_b.int_value = _b;
		}
		
		if(@updated_from != @input_a)
		{
			input_a.ignore_next_change();
			input_a.int_value = _a;
		}
		
		// Hex
		
		if(@updated_from != @input_hex)
		{
			input_hex.ignore_next_change();
			input_hex.text = hex(_show_alpha ? _colour : _colour & 0xffffff, _show_alpha ? 8 : 6);
		}
	}
	
	private void update_swatches()
	{
		swatch.background_colour = _colour;
		swatch_previous.background_colour = _previous_colour;
		
		if(!_show_alpha)
		{
			swatch.background_colour = swatch.background_colour | 0xff000000;
			swatch_previous.background_colour = swatch_previous.background_colour | 0xff000000;
		}
	}
	
	private void update_from_hsl(const float value, const ColourSliderType type, TextBox@ updated_from=null)
	{
		const float h = _h;
		
		switch(type)
		{
			case H: _h = value; break;
			case S: _s = value; break;
			case L: _l = value; break;
		}
		
		_h = clamp01(_h);
		_s = clamp01(_s);
		_l = clamp01(_l);
		
		_colour = hsv_to_rgb(_h, _s, _l) | (_a << 24);
		int_to_rgba(colour, _r, _g, _b, _a);
		update_all(updated_from);
		
		trigger_change();
	}
	
	private void update_from_rgb(const float value, const ColourSliderType type, TextBox@ updated_from=null)
	{
		update_from_rgb(round_int(value * 255), type, updated_from);
	}
	
	private void update_from_rgb(const int value, const ColourSliderType type, TextBox@ updated_from=null)
	{
		switch(type)
		{
			case R: _r = value; break;
			case G: _g = value; break;
			case B: _b = value; break;
			case A: _a = value; break;
		}
		
		_r = clamp(_r, 0, 255);
		_g = clamp(_g, 0, 255);
		_b = clamp(_b, 0, 255);
		_a = clamp(_a, 0, 255);
		
		_colour = rgba(_r, _g, _b, _a);
		rgb_to_hsv(_r, _g, _b, _h, _s, _l);
		update_all(updated_from);
		
		trigger_change();
	}
	
	private void update_from_hex(const uint value, TextBox@ updated_from=null)
	{
		_colour = value;
		int_to_rgba(value, _r, _g, _b, _a);
		rgb_to_hsv(_r, _g, _b, _h, _s, _l);
		update_all(updated_from);
		
		trigger_change();
	}
	
	private void update_from_value(const string name, const float value, TextBox@ updated_from=null)
	{
		// HSL
		
		if(name == 'H')
		{
			update_from_hsl(value, H, updated_from);
		}
		else if(name == 'S')
		{
			update_from_hsl(value, S, updated_from);
		}
		else if(name == 'L')
		{
			update_from_hsl(value, L, updated_from);
		}
		
		// RGB
		
		if(name == 'R')
		{
			update_from_rgb(value / 255, R, updated_from);
		}
		else if(name == 'G')
		{
			update_from_rgb(value / 255, G, updated_from);
		}
		else if(name == 'B')
		{
			update_from_rgb(value / 255, B, updated_from);
		}
		else if(name == 'A')
		{
			update_from_rgb(value / 255, A, updated_from);
		}
	}
	
	private void update_all(TextBox@ updated_from=null)
	{
		update_sliders();
		update_inputs(updated_from);
		update_swatches();
	}
	
	private void trigger_change()
	{
		ui._dispatch_event(@change, EventType::CHANGE, this);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	bool ui_step()
	{
		if(ui._editor.key_check_gvb(GVB::Escape) && @ui.focused_element == null && !contains(ui.focused_element))
		{
			ui._dispatch_event(@accept, EventType::CANCEL, this);
		}
		else if(ui._editor.key_check_gvb(GVB::Return) && @ui.focused_element == null && !contains(ui.focused_element))
		{
			ui._dispatch_event(@accept, EventType::ACCEPT, this);
		}
		
		return _accept_on_keybaord;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		slider_h._x = 0;
		slider_h._y = 0;
		slider_s._x = slider_h._x;
		slider_s._y = slider_h._y + slider_h._height + ui.style.spacing;
		slider_l._x = slider_s._x;
		slider_l._y = slider_s._y + slider_s._height + ui.style.spacing;
		slider_r._x = slider_l._x;
		slider_r._y = slider_l._y + slider_l._height + ui.style.spacing;
		slider_g._x = slider_r._x;
		slider_g._y = slider_r._y + slider_r._height + ui.style.spacing;
		slider_b._x = slider_g._x;
		slider_b._y = slider_g._y + slider_g._height + ui.style.spacing;
		
		label_h._x = slider_h._x + slider_r._width + ui.style.spacing;
		label_h._y = slider_h._y;
		label_s._x = slider_s._x + slider_r._width + ui.style.spacing;
		label_s._y = slider_s._y;
		label_l._x = slider_l._x + slider_r._width + ui.style.spacing;
		label_l._y = slider_l._y;
		label_r._x = slider_r._x + slider_r._width + ui.style.spacing;
		label_r._y = slider_r._y;
		label_g._x = slider_g._x + slider_r._width + ui.style.spacing;
		label_g._y = slider_g._y;
		label_b._x = slider_b._x + slider_r._width + ui.style.spacing;
		label_b._y = slider_b._y;
		
		label_h._height = slider_h._height - 2;
		label_s._height = slider_s._height - 2;
		label_l._height = slider_l._height - 2;
		label_r._height = slider_r._height - 2;
		label_g._height = slider_g._height - 2;
		label_b._height = slider_b._height - 2;
		
		input_h._x = label_h._x + label_h._width + ui.style.spacing;
		input_h._y = label_h._y;
		input_s._x = label_s._x + label_s._width + ui.style.spacing;
		input_s._y = label_s._y;
		input_l._x = label_l._x + label_l._width + ui.style.spacing;
		input_l._y = label_l._y;
		
		input_r._x = label_r._x + label_r._width + ui.style.spacing;
		input_r._y = label_r._y;
		input_g._x = label_g._x + label_g._width + ui.style.spacing;
		input_g._y = label_g._y;
		input_b._x = label_b._x + label_b._width + ui.style.spacing;
		input_b._y = label_b._y;
		
		Element@ last_el = input_b;
		
		if(_show_alpha)
		{
			slider_a._x = slider_b._x;
			slider_a._y = slider_b._y + slider_b._height + ui.style.spacing;
			
			label_a._x = slider_a._x + slider_r._width + ui.style.spacing;
			label_a._y = slider_a._y;
			
			label_a._height = slider_a._height - 2;
			
			input_a._x = label_a._x + label_a._width + ui.style.spacing;
			input_a._y = label_a._y;
			
			@last_el = input_a;
		}
		
		input_hex._x = input_h._x + input_h._width + ui.style.spacing;
		
		swatch._x = input_s._x + input_s._width + ui.style.spacing;
		swatch._y = input_s._y;
		swatch._width = input_hex._width;
		swatch._height = input_s._height + ui.style.spacing * 0.5;
		swatch_previous._x = swatch._x;
		swatch_previous._y = swatch._y + swatch._height;
		swatch_previous._width = swatch._width;
		swatch_previous._height = swatch._height;
		
		cancel_button._x = last_el._x + last_el._width + ui.style.spacing;
		cancel_button._y = last_el._y;
		cancel_button._width = input_hex._width;
		cancel_button._height = last_el._height;
		
		ok_button._x = cancel_button._x;
		ok_button._y = cancel_button._y - cancel_button._height - ui.style.spacing;
		ok_button._width = cancel_button._width;
		ok_button._height = cancel_button._height;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void on_slider_change(EventInfo@ event)
	{
		ColourSlider@ slider = cast<ColourSlider@>(event.target);
		
		update_from_value(slider.name, slider.value);
	}
	
	void on_input_change(EventInfo@ event)
	{
		TextBox@ input = cast<TextBox@>(event.target);
		
		if(input.name == 'Hex')
		{
			update_from_hex(input.rgb_value, input);
		}
		else
		{
			update_from_value(input.name, input.float_value, input);
		}
	}
	
	void on_input_accept(EventInfo@ event)
	{
		if(event.type != EventType::CANCEL)
			return;
		
		on_input_change(event);
	}
	
	void on_button_click(EventInfo@ event)
	{
		ui._dispatch_event(@accept, event.target.name, this);
	}
	
}