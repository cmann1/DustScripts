#include 'SelectBase.cpp';
#include 'Button.cpp';
#include 'shapes/Arrow.cpp';

class Spinner : SelectBase, IStepHandler
{
	
	bool allow_wrap = true;
	float button_size = 16;
	float button_speed = 7;
	float button_pause = button_speed * 5;
	float fade_buttons = 0;
	
	protected float button_timer;
	protected int button_hold_direction;
	
	protected Button@ _left_button;
	protected Button@ _right_button;
	protected Arrow@ _left_arrow;
	protected Arrow@ _right_arrow;
	
	protected bool step_subscribed;
	
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
		
		if(fade_buttons >= 0)
		{
			_left_button.visible = false;
			_right_button.visible = false;
		}
		
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
	
	bool ui_step() override
	{
		step_subscribed = false;
		
		if(button_hold_direction != 0)
		{
			if(_left_button.pressed || _right_button.pressed)
			{
				if(button_timer-- == 0)
				{
					selected_index = _selected_index + button_hold_direction;
					button_timer = button_speed;
				}
				
				step_subscribed = true;
			}
			else
			{
				button_hold_direction = 0;
			}
		}
		
		if(fade_buttons < 1)
		{
			const float fade_speed = 0.15;
			
			if(pressed || hovered)
			{
				_left_button.visible = true;
				_right_button.visible = true;
				
				if(_left_arrow.alpha < 1)
				{
					_left_arrow.alpha = min(1, _left_arrow.alpha + fade_speed);
					_right_arrow.alpha = min(1, _right_arrow.alpha + fade_speed);
					validate_layout = true;
					step_subscribed = true;
				}
			}
			else if(_left_arrow.alpha > fade_buttons)
			{
				_left_arrow.alpha = max(fade_buttons, _left_arrow.alpha - fade_speed);
				_right_arrow.alpha = max(fade_buttons, _right_arrow.alpha - fade_speed);
				validate_layout = true;
				step_subscribed = true;
				
				if(_left_arrow.alpha <= 0)
				{
					_left_button.visible = false;
					_right_button.visible = false;
				}
			}
		}
		
		return step_subscribed;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const float border_size = max(0, ui.style.border_size);
		const bool show_buttons = _left_arrow.alpha > 0;
		
		_left_button.x = border_size;
		_left_button.y = border_size;
		_left_button.width  = button_size * _left_arrow.alpha;
		_left_button.height = _height - border_size * 2;
		
		_right_button.x = _width - border_size - button_size * _left_arrow.alpha;
		_right_button.y = border_size;
		_right_button.width  = _left_button._width;
		_right_button.height = _left_button._height;
		
		_label.x = border_size + (show_buttons ? button_size * _left_arrow.alpha : 0);
		_label.y = border_size;
		_label.width = _width - (show_buttons ? button_size * 2 * _left_arrow.alpha : 0) - border_size * 2;
		_label.height = _height - border_size * 2;
		
		_left_arrow.scale_x = _left_arrow.alpha;
		_right_arrow.scale_x = _left_arrow.alpha;
		
		if(_show_icons && _icon.visible)
		{
			_icon.x = border_size + (show_buttons ? button_size * _left_arrow.alpha : 0);
			_icon.y = border_size;
			_icon.width = min(_height, _width) - border_size * 2;
			_icon.height = _icon._width;
			
			_label.x += _icon._width;
			_label.width -= _icon._width;
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_interactive_element(
			x1, y1, x2, y2,
			false, false, false, disabled);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_enter() override
	{
		if(fade_buttons >= 1)
			return;
		
		step_subscribed = ui._step_subscribe(@this, step_subscribed);
	}
	
	void _mouse_exit() override
	{
		if(fade_buttons >= 1)
			return;
		
		step_subscribed = ui._step_subscribe(@this, step_subscribed);
	}
	
	void _mouse_press(const MouseButton button) override
	{
		if(button != ui.primary_button)
			return;
		
		if(_left_button.check_mouse() || _right_button.check_mouse())
		{
			button_timer = button_pause;
			button_hold_direction = _left_button.check_mouse() ? -1 : 1;
			selected_index = _selected_index + button_hold_direction;
			
			@ui._active_mouse_element = @this;
			step_subscribed = ui._step_subscribe(@this, step_subscribed);
		}
	}
	
	void _mouse_scroll(const int scroll_dir) override
	{
		selected_index = _selected_index - scroll_dir;
	}
	
}