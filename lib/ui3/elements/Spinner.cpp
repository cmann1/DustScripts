#include 'SelectBase.cpp';
#include 'Button.cpp';
#include 'shapes/Arrow.cpp';

class Spinner : SelectBase
{
	
	bool allow_wrap = true;
	float button_size = 16;
	float button_speed = 7;
	float button_pause = button_speed * 5;
	float fade_buttons = 0;
	
	protected float button_timer;
	protected bool first_button_press;
	
	protected Button@ _left_button;
	protected Button@ _right_button;
	protected Arrow@ _left_arrow;
	protected Arrow@ _right_arrow;
	
	Spinner(UI@ ui, const string placeholder_text='',
		const string placeholder_icon_set='', const string placeholder_icon_name='',
		const float placeholder_icon_width=-1, const float placeholder_icon_height=-1,
		const float placeholder_icon_offset_x=0, const float placeholder_icon_offset_y=0)
	{
		super(ui, placeholder_text,
			placeholder_icon_set, placeholder_icon_name,
			placeholder_icon_width, placeholder_icon_height,
			placeholder_icon_offset_x, placeholder_icon_offset_y);
		
		@_left_button  = Button(ui, @_left_arrow = Arrow(ui));
		@_right_button = Button(ui, @_right_arrow = Arrow(ui));
		_left_button.name = 'left';
		_right_button.name = 'right';
		_left_button.draw_border = DrawOption::Never;
		_right_button.draw_border = DrawOption::Never;
		_left_button.draw_background = DrawOption::Hover;
		_right_button.draw_background = DrawOption::Hover;
		_left_arrow.rotation = -180;
		_right_arrow.rotation = 0;
		_left_arrow.alpha = fade_buttons < 1 ? fade_buttons : 1;
		_right_arrow.alpha = fade_buttons < 1 ? fade_buttons : 1;
		Container::add_child(_left_button, 0);
		Container::add_child(_right_button);
		
		_label.mouse_enabled = false;
		children_mouse_enabled = true;
	}
	
	string element_type { get const override { return 'Spinner'; } }
	
	int selected_index
	{
		set override
		{
			if(_num_values == 0)
				return;
			
			if(allow_wrap)
			{
				if(value < 0)
					value = _num_values - 1;
				else if(value >= _num_values)
					value = 0;
			}
			else
			{
				if(value < 0)
					value = 0;
				else if(value > _num_values - 1)
					value = _num_values - 1;
			}
			
			if(_selected_index == value)
				return;
			
			_selected_index = value;
			
			update_label();
			update_icon();
			ui._dispatch_event(@change, EventType::CHANGE, this);
		}
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(fade_buttons < 1)
		{
			const float fade_speed = 0.15;
			
			if(pressed || hovered)
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
		
		const float border_size = max(0, ui.style.border_size);
		const bool show_buttons = _left_arrow.alpha > 0;
		
		if(show_buttons)
		{
			_left_button._x = border_size;
			_left_button._y = border_size;
			_left_button._width  = button_size;
			_left_button._height = _height - border_size * 2;
			
			_right_button._x = _width - border_size - button_size;
			_right_button._y = border_size;
			_right_button._width  = _left_button._width;
			_right_button._height = _left_button._height;
		}
		
		_label._x = border_size + (show_buttons ? button_size : 0);
		_label._y = border_size;
		_label._width = _width - (show_buttons ? button_size * 2 : 0) - border_size * 2;
		_label._height = _height - border_size * 2;
		
		if(_left_button.pressed || _right_button.pressed)
		{
			if(button_timer-- == 0)
			{
				selected_index = _selected_index + (_left_button.pressed ? -1 : 1);
				button_timer = first_button_press ? button_pause : button_speed;
				first_button_press = false;
			}
		}
		else
		{
			button_timer = 0;
			first_button_press = true;
		}
		
		if(_show_icons && _icon.visible)
		{
			_icon._x = border_size + (show_buttons ? button_size : 0);
			_icon._y = border_size;
			_icon._width = min(_height, _width) - border_size * 2;
			_icon._height = _icon._width;
			
			_label._x += _icon._width;
			_label._width -= _icon._width;
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_interactive_element(
			x1, y1, x2, y2,
			false, false, false, disabled);
	}
	
}