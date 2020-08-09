#include '../../math/math.cpp';
#include '../../enums/Key.cpp';
#include '../../string.cpp';
#include 'LockedContainer.cpp';
#include 'Image.cpp';
#include '../events/Event.cpp';

class RotationWheel : Image
{
	
	bool allow_range = true;
	bool enable_mouse_wheel = true;
	bool drag_relative = true;
	
	float snap_big   = PI / 4;
	float snap_small = PI / 8;
	float snap_tiny  = PI / 36;
	
	float image_radius_inset = 3;
	
	protected bool _auto_tooltip = false;
	protected bool _auto_tooltip_range = true;
	protected string _tooltip_prefix = '';
	protected uint _tooltip_precision = 1;
	protected bool _tooltip_trim_trailing_zeros = true;
	protected bool _tooltip_degrees = true;
	
	Event change;
	Event range_change;
	
	protected float _angle;
	protected float _range;
	protected float _start_angle = -PI;
	protected float _end_angle   =  PI;
	
	protected bool drag_angle;
	protected bool drag_range;
	protected float drag_offset;
	
	RotationWheel(UI@ ui)
	{
		super(ui, 'editor', 'circle');
		sizing = ImageSize:: FitInside;
		padding = 0;
		
		_width = _set_width = 46;
		_height = _set_height = _width;
	}
	
	string element_type { get const override { return 'RotationWheel'; } }
	
	//
	
	float start_angle
	{
		get const { return _start_angle; }
		set
		{
			if(_start_angle == value)
				return;
			
			_start_angle = value;
			_end_angle = _start_angle + PI2;
			
			angle = _angle;
		}
	}
	
	float angle
	{
		get const { return _angle; }
		set
		{
			const float width		 = _end_angle - _start_angle;
			const float offset_value = value - _start_angle;
			
			value = (offset_value - ( floor( offset_value / width ) * width )) + _start_angle;
			value = snap(value);
			
			if(_angle == value)
				return;
			
			_angle = value;
			dispatch_change();
			
			if(auto_tooltip)
				update_tooltip();
		}
	}
	
	float degrees
	{
		get const { return _angle * RAD2DEG; }
		set { angle = value * DEG2RAD; }
	}
	
	float range
	{
		get const { return _range; }
		set
		{
			allow_range = true;
			value = clamp(snap(value), 0, PI);
			
			if(_range == value)
				return;
			
			_range = value;
			dispatch_range_change();
			
			if(auto_tooltip)
				update_tooltip();
		}
	}
	
	float range_degrees
	{
		get const { return _range * RAD2DEG; }
		set { range = value * DEG2RAD; }
	}
	
	// Auto tooltips
	
	bool auto_tooltip
	{
		get const { return _auto_tooltip; }
		set
		{
			if(_auto_tooltip == value)
				return;
			
			_auto_tooltip = value;
			update_tooltip();
		}
	}
	
	string tooltip_prefix
	{
		get const { return _tooltip_prefix; }
		set
		{
			if(_tooltip_prefix == value)
				return;
			
			_tooltip_prefix = value;
			update_tooltip();
		}
	}
	
	uint tooltip_precision
	{
		get const { return _tooltip_precision; }
		set
		{
			if(_tooltip_precision == value)
				return;
			
			_tooltip_precision = value;
			update_tooltip();
		}
	}
	
	bool tooltip_trim_trailing_zeros
	{
		get const { return _tooltip_trim_trailing_zeros; }
		set
		{
			if(_tooltip_trim_trailing_zeros == value)
				return;
			
			_tooltip_trim_trailing_zeros = value;
			update_tooltip();
		}
	}
	
	bool tooltip_degrees
	{
		get const { return _tooltip_degrees; }
		set
		{
			if(_tooltip_degrees == value)
				return;
			
			_tooltip_degrees = value;
			update_tooltip();
		}
	}
	
	//
	
	bool overlaps_point(const float x, const float y) override
	{
		if(Element::overlaps_point(x, y))
		{
			const float radius = min(_width, _height) * 0.5 - image_radius_inset;
			return dist_sqr(x, y, (x1 + x2) * 0.5, (y1 + y2) * 0.5) <= radius * radius;
		}
		
		return false;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		Image::_do_layout(ctx);
		
		int scroll_dir;
		
		if(drag_angle)
		{
			if(ui.mouse.primary_down)
			{
				angle = get_mouse_angle() - drag_offset;
				
				if(_auto_tooltip)
					tooltip.force_open = true;
			}
			else
			{
				drag_angle = false;
			}
		}
		else if(drag_range)
		{
			if(ui.mouse.secondary_down)
			{
				range = abs(shortest_angle(_angle, get_mouse_angle() - drag_offset));
				
				if(_auto_tooltip)
					tooltip.force_open = true;
			}
			else
			{
				drag_range = false;
			}
		}
		else if(hovered && (ui.mouse.primary_press || (allow_range && ui.mouse.secondary_press)))
		{
			if(ui.mouse.primary_press)
			{
				drag_angle = true;
			}
			else
			{
				drag_range = true;
			}
			
			const float mid_x = (x1 + x2) * 0.5;
			const float mid_y = (y1 + y2) * 0.5;
			const float dx = ui.mouse.x - mid_x;
			const float dy = ui.mouse.y - mid_y;
			const float length = magnitude(dx, dy);
			
			if(drag_relative)
			{
				drag_offset = get_mouse_angle();
				
				if(drag_range)
				{
					if(shortest_angle(_angle, drag_offset) >= 0)
					{
						drag_offset = shortest_angle(_angle + _range, drag_offset);
					}
					else
					{
						drag_offset = shortest_angle(_angle - _range, drag_offset);
					}
				}
				else
				{
					drag_offset = shortest_angle(_angle, drag_offset);
				}
			}
			else
			{
				drag_offset = 0;
			}
		}
		else if(enable_mouse_wheel && ui.mouse.scrolled(scroll_dir))
		{
			angle = (round(_angle * RAD2DEG) - scroll_dir) * DEG2RAD;
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		Image::_draw(style, ctx);
		
		const float radius = min(_width, _height) * 0.5 - image_radius_inset;
		const uint main_clr = drag_angle || (hovered && !drag_range) ? style.selected_highlight_border_clr : style.highlight_border_clr;
		
		const float mid_x = (x1 + x2) * 0.5;
		const float mid_y = (y1 + y2) * 0.5;
		
		float nx = cos(_angle);
		float ny = sin(_angle);
		style.draw_line(mid_x, mid_y, mid_x + nx * radius, mid_y + ny * radius, 1.5, main_clr);
		
		if(allow_range && _range != 0)
		{
			const uint range_clr = drag_range ? style.selected_highlight_border_clr : style.secondary_bg_clr;
			nx = cos(_angle - _range);
			ny = sin(_angle - _range);
			style.draw_line(mid_x, mid_y, mid_x + nx * radius, mid_y + ny * radius, 1, range_clr);
			nx = cos(_angle + _range);
			ny = sin(_angle + _range);
			style.draw_line(mid_x, mid_y, mid_x + nx * radius, mid_y + ny * radius, 1, range_clr);
		}
	}
	
	protected float snap(const float angle)
	{
		if(!ui._has_editor)
			return angle;
		
		if(ui._editor.key_check_vk(Key::Shift))
			return round(angle / snap_big) * snap_big;
			
		if(ui._editor.key_check_vk(Key::Control))
			return round(angle / snap_small) * snap_small;
			
		if(ui._editor.key_check_vk(Key::Menu))
			return round(angle / snap_tiny) * snap_tiny;
		
		return angle;
	}
	
	protected float get_mouse_angle()
	{
		const float mid_x = (x1 + x2) * 0.5;
		const float mid_y = (y1 + y2) * 0.5;
		const float dx = ui.mouse.x - mid_x;
		const float dy = ui.mouse.y - mid_y;
		
		return atan2(dy, dx);
	}
	
	protected void update_tooltip()
	{
		if(drag_range && !_auto_tooltip_range)
			return;
		
		if(!_auto_tooltip)
		{
			if(@tooltip != null)
			{
				tooltip.enabled = false;
			}
			
			return;
		}
		
		if(@tooltip == null)
		{
			@tooltip = PopupOptions(ui, null);
			tooltip.keep_open_while_pressed = true;
		}
		
		tooltip.force_open = true;
		tooltip.enabled = true;
		
		const float angle = drag_range ? _range : _angle;
		
		string str = string::nice_float(
			_tooltip_degrees ? angle * RAD2DEG : angle,
			_tooltip_precision, _tooltip_trim_trailing_zeros);
		
		if(drag_range)
		{
			str = '+/- ' + str;
		}
		
		tooltip.content_string = _tooltip_prefix + str;
		ui.update_tooltip(this);
	}
	
	protected void dispatch_change()
	{
		ui._event_info.reset(EventType::CHANGE, this);
		change.dispatch(ui._event_info);
	}
	
	protected void dispatch_range_change()
	{
		ui._event_info.reset(EventType::CHANGE_RANGE, this);
		range_change.dispatch(ui._event_info);
	}
	
}