#include '../UI.cpp';
#include '../Style.cpp';
#include '../TextAlign.cpp';
#include '../../math/math.cpp';
#include '../elements/Button.cpp';
#include '../elements/Label.cpp';
#include '../events/Event.cpp';
#include '../utils/Orientation.cpp';
#include 'LockedContainer.cpp';

class NumberSlider : LockedContainer
{
	
	float min_value;
	float max_value;
	float step;
	float drag_sensitivity = 0.25;
	
	bool show_fill;
	float button_size = 10;
	
	Event change;
	
	protected Orientation _orientation;
	protected float _value = 0;
	protected bool _show_buttons;
	protected bool _show_text;
	Label@ _label;
	Button@ _left_button;
	Button@ _right_button;
	
	NumberSlider(UI@ ui,
		const float value=0, const float min_value=NAN, const float max_value=NAN, const float step=1,
		Orientation orientation=Orientation::Horizontal,
		bool show_buttons=true, bool show_text=true, bool show_fill=true)
	{
		super(ui, 'numsldr');
		
		this.min_value = min_value;
		this.max_value = max_value;
		this.step = step;
		this.value = value;
		
		this.orientation = orientation;
		this.show_buttons = show_buttons;
		this.show_text = show_text;
		this.show_fill = show_fill;
		
		_set_width  = _width  = orientation == Orientation::Horizontal ? 60 : 30;
		_set_height = _height = orientation == Orientation::Horizontal ? 30 : 60;
	}
	
	float value
	{
		get const { return _value; }
		set
		{
			if(!is_nan(min_value) && value < min_value)
				value = min_value;
			else if(!is_nan(max_value) && value < max_value)
				value = max_value;
			else if(!is_nan(step))
			{
				if(!is_nan(min_value))
					value = min_value + floor((value - min_value) / step) * step;
				else
					value = floor(value / step);
			}
			
			if(value == _value)
				return;
			
			_value = value;
			
			if(_show_text)
			{
				_label.text = formatFloat(_value, '', 0, 0);
			}
			
			ui._event_info.reset(EventType::CHANGE, @this);
			change.dispatch(ui._event_info);
		}
	}
	
	Orientation orientation
	{
		get const { return _orientation; }
		set
		{
			if(_orientation == value)
				return;
			
			_orientation = value;
			
			float temp = _width;
			_width = _height;
			_height = temp;
			temp = _set_width;
			_set_width = _set_height;
			_set_height = temp;
			
			// TODO: Set label orientation;
			if(@_label != null)
			{
				_label.rotation = (_orientation == Orientation::Horizontal) ? 0 : -90;
			}
			
			// TODO: Set buttons orientation;
		}
	}
	
	bool show_text
	{
		get const { return _show_text; }
		set
		{
			if(value == _show_text)
				return;
			
			_show_text = value;
			
			if(!value)
			{
				if(@_label != null)
				{
					_label.visible = false;
				}
			}
			else if(@_label == null)
			{
				@_label = Label(ui, _value + '');
				_label.mouse_enabled = false;
				_label.sizing = ImageSize::ConstrainInside;
				_label.padding = ui.style.spacing;
				// TODO: Set label orientation;
				Container::add_child(_label);
			}
		}
	}
	
	bool show_buttons
	{
		get const { return _show_buttons; }
		set
		{
			if(value == _show_buttons)
				return;
			
			_show_buttons = value;
			
			if(!value)
			{
				if(@_left_button != null)
				{
					_left_button.visible = false;
					_right_button.visible = false;
				}
			}
			else if(@_left_button == null)
			{
				@_left_button  = Button(ui, '<');
				@_right_button = Button(ui, '>');
				_left_button.draw_border = false;
				_right_button.draw_border = false;
				Container::add_child(_left_button);
				Container::add_child(_right_button);
			}
		}
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const bool _is_horizontal = orientation == Orientation::Horizontal;
		const float border_size = max(0, ui.style.border_size);
		const float width  = _is_horizontal ? _width  : _height;
		const float height = _is_horizontal ? _height : _width;
		const float button_spacing = 2;
		
		if(_show_buttons && _left_button.visible)
		{
			// TODO: Set button icon rotation
			if(_is_horizontal)
			{
				_left_button._x = border_size;
				_left_button._y = border_size;
				_left_button.width  = button_size;
				_left_button.height = _height - border_size * 2;
				
				_right_button._x = _width - border_size - button_size;
				_right_button._y = border_size;
				_right_button.width  = _left_button._width;
				_right_button.height = _left_button._height;
			}
			else
			{
				_right_button._x = border_size;
				_right_button._y = border_size;
				_right_button.width  = _width - border_size * 2;
				_right_button.height = button_size;
				
				_left_button._x = border_size;
				_left_button._y = _height - border_size - button_size;
				_left_button.width  = _right_button._width;
				_left_button.height = _right_button._height;
			}
			
			// TODO: Step
			// TODO: Step once, then after a small delay step continuously
			if(_left_button.pressed)
			{
				value -= step;
			}
			else if(_right_button.pressed)
			{
				value += step;
			}
		}
		
		if(_show_text && _label.visible)
		{
			if(_is_horizontal)
			{
				if(_show_buttons && _left_button.visible)
				{
					_label._x = border_size + button_size;
					_label.width = _width - button_size * 2 - border_size * 2;
				}
				else
				{
					_label._x = border_size;
					_label.width = _width - border_size * 2;
				}
				
				_label._y = border_size;
				_label.height = _height - border_size * 2;
			}
			else
			{
				if(_show_buttons && _left_button.visible)
				{
					_label._y = border_size + button_size;
					_label.height = _height - button_size * 2 - border_size * 2;
				}
				else
				{
					_label._y = border_size;
					_label.height = _height - border_size * 2;
				}
				
				_label._x = border_size;
				_label.width = _width - border_size * 2;
			}
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_interactive_element(this, false, false, disabled);
	}
	
}