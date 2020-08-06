#include '../events/Event.cpp';
#include '../utils/CheckboxState.cpp';
#include 'Element.cpp';

class Checkbox : Element
{
	
	float _size = 14;
	
	Event change;
	
	protected CheckboxState _state = CheckboxState::Off;
	protected Element@ _label;
	protected EventCallback@ label_click_delegate;
	
	Checkbox(UI@ ui)
	{
		super(ui);
		
		@label_click_delegate = EventCallback(on_label_click);
		
		_width  = _set_width  = _size + ui.style.spacing * 2;
		_height = _set_height = _set_width;
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
			dispatch_change_event();
		}
	}
	
	bool checked
	{
		get const { return _state == CheckboxState::On; }
		set
		{
			_state = value ? CheckboxState::On : CheckboxState::Off;
			dispatch_change_event();
		}
	}
	
	bool indeterminate
	{
		get const { return _state == CheckboxState::Indeterminate; }
		set
		{
			_state = value ? CheckboxState::Indeterminate : CheckboxState::Off;
			dispatch_change_event();
		}
	}
	
	Element@ label
	{
		get { return _label; }
		set
		{
			if(@_label == @value)
				return;
			
			if(@_label != null)
			{
				_label.mouse_click.off(label_click_delegate);
			}
			
			@_label = value;
			
			if(@_label != null)
			{
				_label.mouse_click.on(label_click_delegate);
			}
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		float x = x1 + (_width  - _size) * 0.5;
		float y = y1 + (_height - _size) * 0.5;
		const bool pressed = this.pressed || (@_label != null && _label.pressed);
		
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
	
	protected void dispatch_change_event()
	{
		ui._event_info.reset(EventType::CHANGE, this);
		change.dispatch(ui._event_info);
	}
	
	void _mouse_click() override
	{
		checked = _state != CheckboxState::On;
	}
	
	protected void on_label_click(EventInfo@ event)
	{
		_mouse_click();
	}
	
}