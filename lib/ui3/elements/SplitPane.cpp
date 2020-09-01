#include 'LockedContainer.cpp';
#include '../utils/Orientation.cpp';
#include '../utils/SplitBehaviour.cpp';

namespace SplitPane { const string TYPE_NAME = 'SplitPane'; }

class SplitPane : LockedContainer, IStepHandler
{
	
	Event move;
	Event move_complete;
	
	protected Element@ _left;
	protected Element@ _right;
	protected Orientation _orientation = Orientation::Horizontal;
	protected float _position = 100;
	protected bool _resizable = true;
	protected SplitBehaviour _behaviour = SplitBehaviour::FixedLeft;
	protected float _left_min = 0;
	protected float _left_max = MAX_FLOAT;
	protected float _right_min = 0;
	protected float _right_max = MAX_FLOAT;
	
	protected float divider_spacing = 2;
	
	protected bool busy_dragging;
	protected float drag_offset;
	
	SplitPane(UI@ ui)
	{
		super(ui);
		
		_width  = _set_width  = 200;
		_height = _set_height = 200;
	}
	
	string element_type { get const override { return SplitPane::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	/// Get or set the left/top element
	Element@ left
	{
		get { return _left; }
		set
		{
			if(@_left == @value)
				return;
			
			if(@_left != null)
			{
				Container::remove_child(_left);
			}
			
			@_left = value;
			
			if(@_left != null)
			{
				Container::add_child(_left);
			}
			
			reposition();
		}
	}
	
	/// Alias for `left`
	Element@ top
	{
		get { return _left; }
		set { @left = value; }
	}
	
	/// Get or set the right/bottom element
	Element@ right
	{
		get { return _right; }
		set
		{
			if(@_right == @value)
				return;
			
			if(@_right != null)
			{
				Container::remove_child(_right);
			}
			
			@_right = value;
			
			if(@_right != null)
			{
				Container::add_child(_right);
			}
			
			reposition();
		}
	}
	
	/// Alias for `right`
	Element@ bottom
	{
		get { return _right; }
		set { @right = value; }
	}
	
	/// The direction the contents are displayed in, either left to right, or top to bottom.
	Orientation orientation
	{
		get const { return _orientation; }
		set
		{
			if(_orientation == value)
				return;
			
			_orientation = value;
			reposition();
		}
	}
	
	/// Set the position of the split. See SplitBehaviour for details
	float position
	{
		get const { return _position; }
		set
		{
			if(_position == value)
				return;
			
			_position = value;
			reposition();
		}
	}
	
	/// Can the divider by dragged to adjust the position
	bool resizable
	{
		get const { return _resizable; }
		set { _resizable = value; }
	}
	
	/// Controls the behaviour of the split position. See SplitBehaviour for details
	SplitBehaviour behaviour
	{
		get const { return _behaviour; }
		set
		{
			if(_behaviour == value)
				return;
			
			const float size = _orientation == Horizontal ? _width : _height;
			const float divider_hwidth = (ui.style.gripper_required_space + ui.style.spacing) * 0.5;
			
			if(value == Percentage && (_behaviour == FixedLeft || _behaviour == FixedRight))
			{
				if(_behaviour == FixedLeft)
				{
					_position = (_position + divider_hwidth) / size;
				}
				else if(_behaviour == FixedRight)
				{
					_position = (_position - divider_hwidth) / size;
				}
			}
			else if(_behaviour == Percentage && (value == FixedLeft || value == FixedRight))
			{
				if(value == FixedLeft)
				{
					_position = size * _position - divider_hwidth;
				}
				else if(value == FixedRight)
				{
					_position = size * _position + divider_hwidth;
				}
			}
			
			_behaviour = value;
			reposition();
		}
	}
	
	/// The minimum size of the left pane
	float left_min
	{
		get const { return _left_min; }
		set
		{
			if(_left_min == value)
				return;
			
			_left_min = value;
			reposition();
		}
	}
	
	/// The maximum size of the left pane
	float left_max
	{
		get const { return _left_max; }
		set
		{
			if(_left_max == value)
				return;
			
			_left_max = value;
			reposition();
		}
	}
	
	/// The minimum size of the right pane
	float right_min
	{
		get const { return _right_min; }
		set
		{
			if(_right_min == value)
				return;
			
			_right_min = value;
			reposition();
		}
	}
	
	/// The maximum size of the right pane
	float right_max
	{
		get const { return _right_max; }
		set
		{
			if(_right_max == value)
				return;
			
			_right_max = value;
			reposition();
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	bool ui_step() override
	{
		if(!ui.mouse.primary_down)
		{
			ui._dispatch_event(@move_complete, EventType::MOVE_COMPLETED, this);
			busy_dragging = false;
			return false;
		}
		
		const bool is_horizontal = _orientation == Horizontal;
		const float mouse_x = is_horizontal ? ui.mouse.x - x1 : ui.mouse.y - y1;
		const float size = is_horizontal ? _width : _height;
		float new_position;
		
		if(_behaviour == FixedLeft)
		{
			new_position = clamp(mouse_x - drag_offset, 0, size);
		}
		else if(_behaviour == FixedRight)
		{
			const float divider_size = ui.style.gripper_required_space + divider_spacing * 2;
			new_position = clamp(size - (mouse_x - drag_offset + divider_size), 0, size);
		}
		else
		{
			new_position = clamp01(clamp(mouse_x - drag_offset, 0, size) / size);
		}
		
		if(new_position != _position)
		{
			position = new_position;
			ui._dispatch_event(@move, EventType::MOVE, this);
		}
		
		return true;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		reposition();
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		Element::_draw(style, ctx);
		// TODO: highlight divider area when hovered
		
		float left, right;
		calculate_sizes(left, right);
		const bool is_horizontal = _orientation == Horizontal;
		const float height = is_horizontal ? _height : _width;
		const float gripper_size = 8;
		const float x = is_horizontal ? x1 : y1;
		const float y = is_horizontal ? y1 : x1;
		const bool divider_hovered = _resizable && !disabled && is_mouse_over_divider(left, right);
		
		if(divider_hovered || busy_dragging)
		{
			if(is_horizontal)
			{
				style.draw_interactive_element(x1 + left, y1, x2 - right, y2, divider_hovered, busy_dragging, busy_dragging, false, true, false, false);
			}
			else
			{
				style.draw_interactive_element(x1, y1 + left, x2, y2 - right, divider_hovered, busy_dragging, busy_dragging, false, true, false, false);
			}
		}
		
		if(_resizable)
		{
			style.draw_gripper(_orientation, x + left + divider_spacing,
				max(y + height * 0.5 - gripper_size ,y),
				min(y + height * 0.5 + gripper_size, is_horizontal ? y2 : x2),
				divider_hovered, busy_dragging);
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Private
	// ///////////////////////////////////////////////////////////////////
	
	protected void calculate_sizes(float &out left, float &out right)
	{
		const float divider_size = ui.style.gripper_required_space + divider_spacing * 2;
		const float available_space = max((_orientation == Horizontal ? _width : _height) - divider_size, 0.0);
		
		if(_left_min + _right_min > available_space)
		{
			left  = available_space * (_left_min / (_left_min + _right_min));
			right = available_space - left;
		}
		
		if(_behaviour == FixedLeft)
		{
			left  = max(min(min(available_space, _position), _left_max), min(_left_min, available_space));
			right = available_space - left;
			return;
		}
		
		if(_behaviour == FixedRight)
		{
			right = max(min(min(available_space, _position), _right_max), min(_right_min, available_space));
			left  = available_space - right;
			return;
		}
		
		left  = max(min(available_space * _position, _left_max), _left_min);
		right = available_space - left;
	}
	
	protected void reposition()
	{
		float left, right;
		calculate_sizes(left, right);
		const bool is_horizontal = _orientation == Horizontal;
		const float size = max(_orientation == Horizontal ? _width : _height, 0.0);
		
		if(@_left != null)
		{
			_left.x = 0;
			_left.y = 0;
			_left.width = is_horizontal ? left : _width;
			_left.height = is_horizontal ? _height : left;
		}
		
		if(@_right != null)
		{
			_right.x = is_horizontal ? size - right : 0;
			_right.y = is_horizontal ? 0 : size - right;
			_right.width = is_horizontal ? right : _width;
			_right.height = is_horizontal ? _height : right;
		}
	}
	
	protected bool is_mouse_over_divider(const float left_size, const float right_size)
	{
		return _orientation == Horizontal
			? ui.mouse.x > x1 + left_size && ui.mouse.x < x2 - right_size && ui.mouse.y >= y1 && ui.mouse.y <= y2
			: ui.mouse.y > y1 + left_size && ui.mouse.y < y2 - right_size && ui.mouse.x >= x1 && ui.mouse.x <= x2;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_press(EventInfo@ event) override
	{
		if(event.button != ui.primary_button)
			return;
		
		float left, right;
		calculate_sizes(left, right);
		
		if(!is_mouse_over_divider(left, right))
			return;
		
		drag_offset = _orientation == Horizontal
			? ui.mouse.x - (x1 + left)
			: ui.mouse.y - (y1 + left);
		busy_dragging = true;
		ui._step_subscribe(this);
	}
	
}