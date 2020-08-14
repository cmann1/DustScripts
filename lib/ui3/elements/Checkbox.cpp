#include '../../math/math.cpp';
#include '../events/Event.cpp';
#include '../utils/CheckboxState.cpp';
#include 'Element.cpp';

class Checkbox : Element
{
	
	float _size = 14;
	
	Event change;
	
	protected CheckboxState _state = CheckboxState::Off;
	protected Element@ _label;
	protected EventCallback@ label_press_delegate;
	protected EventCallback@ label_click_delegate;
	protected bool _toggle_on_press;
	
	Checkbox(UI@ ui)
	{
		super(ui);
		
		@label_click_delegate = EventCallback(on_label_click);
		
		fit();
	}
	
	string element_type { get const { return 'Checkbox'; } }
	
	CheckboxState state
	{
		get const { return _state; }
		set
		{
			if(_state == value)
				return;
			
			_state = value;
			ui._dispatch_event(@change, EventType::CHANGE, this);
		}
	}
	
	bool checked
	{
		get const { return _state == CheckboxState::On; }
		set
		{
			const CheckboxState new_state = value ? CheckboxState::On : CheckboxState::Off;
			
			if(_state == new_state)
				return;
			
			_state = new_state;
			ui._dispatch_event(@change, EventType::CHANGE, this);
		}
	}
	
	/// A third state, neither on nor off.
	bool indeterminate
	{
		get const { return _state == CheckboxState::Indeterminate; }
		set
		{
			_state = value ? CheckboxState::Indeterminate : CheckboxState::Off;
			ui._dispatch_event(@change, EventType::CHANGE, this);
		}
	}
	
	/// An element associated with this checkbox. Clicking the element will also toggle this checkbox.
	Element@ label
	{
		get { return _label; }
		set
		{
			if(@_label == @value)
				return;
			
			clear_label_events();
			@_label = value;
			set_label_events();
		}
	}
	
	/// If true the checkbox will be toggled when pressed instead of when clicked.
	bool toggle_on_press
	{
		get const { return _toggle_on_press; }
		set
		{
			if(_toggle_on_press == value)
				return;
			
			_toggle_on_press = value;
			clear_label_events();
			set_label_events();
		}
	}
	
	/// Sets this checkbox's state without triggering the change event.
	void initialise_state(const CheckboxState state)
	{
		_state = state;
	}
	
	/// Sets this checkbox's checked state without triggering the change event.
	void initialise_state(const bool checked)
	{
		_state = checked ? CheckboxState::On : CheckboxState::Off;
	}
	
	void fit(float padding=NAN)
	{
		fit(padding, padding);
	}
	
	void fit(float padding_x, float padding_y)
	{
		if(is_nan(padding_x))
			padding_x = ui.style.spacing;
		if(is_nan(padding_y))
			padding_y = ui.style.spacing;
		
		width  = _size + padding_x * 2;
		height = _size + padding_y * 2;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		float x = x1 + (_width  - _size) * 0.5;
		float y = y1 + (_height - _size) * 0.5;
		const bool pressed = !_toggle_on_press && (this.pressed || (@_label != null && _label.pressed));
		
		style.draw_interactive_element(
			x, y, x + _size, y + _size,
			hovered || (@_label != null && _label.hovered) || pressed, false, pressed, disabled, true, true, false);
		
		if(_state == CheckboxState::On || _state == CheckboxState::Indeterminate)
		{
			const float spacing = 3;
			x += ui.style.border_size + spacing;
			y += ui.style.border_size + spacing;
			const float width  = _size - (spacing + ui.style.border_size) * 2;
			const float height = _state == CheckboxState::Indeterminate ? 2.0 : width;
			
			y = y + (width - height) * 0.5;
			
			style.draw_rectangle(
				x, y,
				x + width, y + height,
				0, ui.style.text_clr);
		}
	}
	
	protected void clear_label_events()
	{
		if(@_label == null)
			return;
		
		if(@label_click_delegate != null)
		{
			_label.mouse_click.off(label_click_delegate);
		}
		
		if(@label_press_delegate != null)
		{
			_label.mouse_press.off(label_press_delegate);
		}
	}
	
	protected void set_label_events()
	{
		if(@_label == null)
			return;
		
		if(@label_press_delegate == null)
			@label_press_delegate = EventCallback(on_label_press);
		if(@label_click_delegate == null)
			@label_click_delegate = EventCallback(on_label_click);
		
		_label.mouse_press.on(label_press_delegate);
		
		if(!_toggle_on_press)
			_label.mouse_click.on(label_click_delegate);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_press(const MouseButton button)
	{
		if(button != ui.primary_button)
			return;
		
		if(toggle_on_press)
		{
			checked = _state != CheckboxState::On;
		}
		else
		{
			@ui._active_mouse_element = @this;
		}
	}
	
	void _mouse_release(const MouseButton button) override
	{
		if(button != ui.primary_button)
			return;
		
		if(!_toggle_on_press)
		{
			@ui._active_mouse_element = @null;
		}
	}
	
	void _mouse_click() override
	{
		if(!toggle_on_press)
		{
			checked = _state != CheckboxState::On;
			@ui._active_mouse_element = @null;
		}
	}
	
	protected void on_label_press(EventInfo@ event)
	{
		if(event.button != ui.primary_button)
			return;
		
		if(toggle_on_press)
		{
			checked = _state != CheckboxState::On;
		}
		else
		{
			@ui._active_mouse_element = @this;
		}
	}
	
	protected void on_label_click(EventInfo@ event)
	{
		_mouse_click();
	}
	
}