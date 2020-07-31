#include '../UI.cpp';
#include '../Style.cpp';
#include '../TextAlign.cpp';
#include '../../math/math.cpp';
#include '../../utils/colour.cpp';
#include '../elements/Button.cpp';
#include '../elements/Label.cpp';
#include '../elements/shapes/Arrow.cpp';
#include '../events/Event.cpp';
#include '../utils/Orientation.cpp';
#include 'LockedContainer.cpp';

class NumberSlider : LockedContainer
{
	
	float min_value;
	float max_value;
	float step;
	float drag_sensitivity = 0.25;
	bool drag_relative = true;
	
	bool show_fill;
	uint fill_colour = 0x00000000;
	uint fill_hover_colour = 0x00000000;
	uint fill_active_colour = 0x00000000;
	bool orient_label = true;
	uint label_precision = 4;
	bool trim_label_precision = true;
	float button_size = 10;
	float button_speed = 3;
	float button_pause = button_speed * 5;
	float fade_buttons = 0;
	
	Event change;
	
	protected Orientation _orientation;
	protected float _value = 0;
	protected bool _show_buttons;
	protected bool _show_text;
	protected float button_timer;
	protected bool first_button_press;
	
	protected bool busy_dragging;
	protected float drag_value;
	
	Label@ _label;
	Button@ _left_button;
	Button@ _right_button;
	Arrow@ _left_arrow;
	Arrow@ _right_arrow;
	
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
			value = clamp_value(value);
			
			if(value == _value)
				return;
			
			_value = value;
			
			if(_show_text)
			{
				update_label();
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
			
			if(@_label != null)
			{
				_label.rotation = (_orientation == Orientation::Horizontal || !orient_label) ? 0 : -90;
			}
			
			if(@_left_button != null)
			{
				_left_arrow.rotation = _orientation == Orientation::Horizontal ? -180 : 90;
				_right_arrow.rotation = _orientation == Orientation::Horizontal ? 0 : -90;
			}
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
				_label.rotation = (_orientation == Orientation::Horizontal || !orient_label) ? 0 : -90;
				update_label();
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
				@_left_button  = Button(ui, @_left_arrow = Arrow(ui));
				@_right_button = Button(ui, @_right_arrow = Arrow(ui));
				_left_button.draw_border = DrawOption::Never;
				_right_button.draw_border = DrawOption::Never;
				_left_button.draw_background = DrawOption::Hover;
				_right_button.draw_background = DrawOption::Hover;
				_left_arrow.rotation = _orientation == Orientation::Horizontal ? -180 : 90;
				_right_arrow.rotation = _orientation == Orientation::Horizontal ? 0 : -90;
				_left_arrow.alpha = fade_buttons < 1 ? fade_buttons : 1;
				_right_arrow.alpha = fade_buttons < 1 ? fade_buttons : 1;
				Container::add_child(_left_button);
				Container::add_child(_right_button);
			}
		}
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const bool is_horizontal = orientation == Orientation::Horizontal;
		const float border_size = max(0, ui.style.border_size);
		const float width  = is_horizontal ? _width  : _height;
		const float height = is_horizontal ? _height : _width;
		const float button_spacing = 2;
		
		if(_show_buttons && _left_button.visible)
		{
			if(is_horizontal)
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
			
			if(_left_button.pressed || _right_button.pressed)
			{
				if(button_timer-- == 0)
				{
					value = _value + (_left_button.pressed ? -step : step);
					button_timer = first_button_press ? button_pause : button_speed;
					first_button_press = false;
				}
			}
			else if(ui.mouse.primary_press && hovered)
			{
				// Check the hovered button states here because at this point pressed may not be set yet
				if((drag_sensitivity > 0 || drag_relative) && !_left_button.hovered && !_right_button.hovered)
				{
					busy_dragging = true;
					drag_value = _value;
					children_mouse_enabled = false;
					@ui._active_mouse_element = @this;
				}
			}
			else if(busy_dragging)
			{
				if(!ui.mouse.primary_down)
				{
					busy_dragging = false;
					children_mouse_enabled = true;
				}
				else
				{
					@ui._active_mouse_element = @this;
					
					if(drag_relative && !is_nan(min_value) && !is_nan(max_value))
					{
						value = min_value + (is_horizontal
							? ( ui.mouse.x - x1) / max(0.001, x2 - x1)
							: (-ui.mouse.y + y2) / max(0.001, y2 - y1)) * (max_value - min_value);
					}
					else
					{
						drag_value = clamp_value(drag_value + (is_horizontal ? ui.mouse.delta_x : -ui.mouse.delta_y) * drag_sensitivity * step, false);
						value = drag_value;
					}
				}
			}
			else
			{
				if(hovered)
				{
					int scroll_dir;
					
					if(ui.mouse.scrolled(scroll_dir))
					{
						value = _value - step * scroll_dir;
					}
				}
				
				button_timer = 0;
				first_button_press = true;
			}
			
			if(fade_buttons < 1)
			{
				const float fade_speed = 0.15;
				
				if((pressed || hovered) && !busy_dragging)
				{
					if(_left_arrow.alpha < 1)
					{
						_left_arrow.alpha = min(1, _left_arrow.alpha + fade_speed);
						_right_arrow.alpha = min(1, _right_arrow.alpha + fade_speed);
					}
				}
				else if(_left_arrow.alpha > fade_buttons)
				{
					_left_arrow.alpha = max(fade_buttons, _left_arrow.alpha - fade_speed);
					_right_arrow.alpha = max(fade_buttons, _right_arrow.alpha - fade_speed);
				}
			}
		}
		
		if(_show_text && _label.visible)
		{
			if(is_horizontal)
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
		const bool is_horizontal = orientation == Orientation::Horizontal;
		
		style.draw_interactive_element(this, false, false, false, disabled);
		
		if(show_fill && !is_nan(min_value) && !is_nan(max_value))
		{
			const float border_size = style.normal_border_clr != 0 && style.border_size > 0 ? style.border_size : 0;
			const float x1 = this.x1 + border_size;
			const float y1 = this.y1 + border_size;
			const float x2 = this.x2 - border_size;
			const float y2 = this.y2 - border_size;
			
			uint clr;
			
			if(busy_dragging)
			{
				clr = fill_active_colour != 0
					? multiply_alpha(fill_active_colour, disabled ? style.disabled_alpha : 1)
					: style.get_interactive_element_background_colour(hovered || pressed, true, true, disabled);
			}
			else if(hovered || _show_buttons && (_left_button.pressed || _right_button.pressed))
			{
				clr = fill_hover_colour != 0
					? multiply_alpha(fill_hover_colour, disabled ? style.disabled_alpha : 1)
					: style.get_interactive_element_background_colour(hovered || pressed, false, false, disabled);
			}
			else
			{
				clr = multiply_alpha(fill_colour != 0
					? fill_colour
					: style.secondary_bg_clr, disabled ? style.disabled_alpha : 1);
			}
			
			if(is_horizontal)
			{
				style.draw_rectangle(
					x1, y1,
					x1 + (x2 - x1) * (_value - min_value) / (max_value - min_value), y2,
					0, clr);
			}
			else
			{
				style.draw_rectangle(
					x1, y2 - (y2 - y1) * (_value - min_value) / (max_value - min_value),
					x2, y2,
					0, clr);
			}
		}
	}
	
	float clamp_value(float value, const bool clamp_to_step=true)
	{
		if(!is_nan(min_value) && value < min_value)
			value = min_value;
		else if(!is_nan(max_value) && value > max_value)
			value = max_value;
		
		if(clamp_to_step && !is_nan(step))
		{
			if(!is_nan(min_value))
				value = min_value + round((value - min_value) / step) * step;
			else
				value = round(value / step) * step;
		}
		
		return value;
	}
	
	protected void update_label()
	{
		string text = formatFloat(_value, '', 0, label_precision);
		
		if(trim_label_precision && label_precision > 0)
		{
			const int length = int(text.length());
			int end_index = length - 1;
			
			while(text[end_index] == 48) // "0"
				end_index--;
			
			while(text[end_index] == 46) // "."
				end_index--;
			
			if(end_index == -1)
			{
				text = '';
			}
			else if(end_index < length - 1)
			{
				text = text.substr(0, end_index + 1);
			}
		}
		
		_label.text = text;
	}
	
}