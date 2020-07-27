#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/ClippingMode.cpp';
#include '../utils/DrawingContext.cpp';
#include '../utils/LayoutContext.cpp';
#include '../utils/ElementStack.cpp';
#include '../utils/TooltipOptions.cpp';
#include '../events/Event.cpp';
#include 'Container.cpp';

abstract class Element
{
	/**
	 * @class Element
	 * @brief The base class for all display objects in a UI
	 */
	
	UI@ ui;
	Container@ parent;
	
	// Changing this MIGHT cause problems, especially if it conflicts with another element. Rather use name if you need to identify an element.
	string _id;
	// A way to identify this element, e.g. during even callbacks - can be anything.
	string name = '';
	
	TooltipOptions@ tooltip = null;
	
	bool visible = true;
	// Can this element interact with the mouse
	bool mouse_enabled = true;
	// Can this element's children interact with the mouse
	bool children_mouse_enabled = true;
	// Is the mouse within/over this element, or any of its descendants
	bool hovered;
	// The mouse is down and was pressed over this element
	bool pressed;
	// Disabled this element. Only relevant for interactive elements
	bool disabled;
	
	int clip_contents = ClippingMode::None;
	float alpha = 1;
	
	float _x;
	float _y;
	float _width = 100;
	float _height = 100;
	
	// After do_layout, these will be the element's position in world/ui space
	float x1, y1;
	float x2, y2;
	// This element bounds including children
	float subtree_x1, subtree_y1;
	float subtree_x2, subtree_y2;
	
	Event mouse_enter;
	Event mouse_exit;
	Event mouse_press;
	Event mouse_move;
	Event mouse_release;
	// Triggered when the primary ui button is clicked
	Event mouse_click;
	// Called when any mouse button is clicked
	Event mouse_button_click;
	
	Element(UI@ ui, const string &in type_identifier)
	{
		@this.ui = @ui;
		_id = type_identifier + (++ui.NEXT_ID);
	}
	
	bool overlaps_point(const float x, const float y)
	{
		return x < x2 && x >= x1 && y < y2 && y >= y1;
	}
	
	void to_local(const float x, const float y, float &out out_x, float &out out_y)
	{
		out_x = x;
		out_y = y;
		
		Container@ p = @parent;
		
		while(@p != null)
		{
			out_x -= parent.x;
			out_y -= parent.y;
			@p = p.parent;
		}
	}
	
	void to_global(const float x, const float y, float &out out_x, float &out out_y)
	{
		out_x = x;
		out_y = y;
		
		Container@ p = @parent;
		
		while(@p != null)
		{
			out_x += parent.x;
			out_y += parent.y;
			@p = p.parent;
		}
	}
	
	/**
	 * @brief Shows the tooltip for this element if it has one.
	 * @param wait_for_mouse - If true and the tooltip hide type is MouseLeave, the tooltip will not close until the mouse enters it for the first time.
	 */
	void show_tooltip(bool wait_for_mouse=true)
	{
		if(@tooltip == null)
			return;
		
		ui.show_tooltip(this, wait_for_mouse);
	}
	
	void hide_tooltip()
	{
		ui.hide_tooltip(this);
	}
	
	/**
	 * @brief Returns mouse x relative to this element
	 */
	float mouse_x { get { return ui.mouse.x - x1; } }
	
	/**
	 * @brief Returns mouse y relative to this element
	 */
	float mouse_y { get { return ui.mouse.y - y1; } }
	
	float x
	{
		get const { return _x; }
		set
		{
			if(_x == value) return;
			_x = value;
			if(@parent != null) parent._validate_layout = true;
		}
	}
	
	float y
	{
		get const { return _y; }
		set
		{
			if(_y == value) return;
			_y = value;
			if(@parent != null) parent._validate_layout = true;
		}
	}
	
	float width
	{
		get const { return _width; }
		set
		{
			if(_width == value) return;
			_width = value;
			if(@parent != null) parent._validate_layout = true;
		}
	}
	
	float height
	{
		get const { return _height; }
		set
		{
			if(_height == value) return;
			_height = value;
			if(@parent != null) parent._validate_layout = true;
		}
	}
	
	// ------------------------------------------------
	// Internal
	// ------------------------------------------------
	
	void _added()
	{
		
	}
	
	void _removed()
	{
		hovered = false;
	}
	
	/**
	 * @brief Internal. Append all children of this element to the stack. Must be in reverse order
	 */
	void _queue_children_for_layout(ElementStack@ stack)
	{
		
	}
	
	void _do_layout(LayoutContext@ ctx)
	{
		
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		// Debug
		//style.draw_rectangle(x1, y1, x2, y2, 0, 0x55000000);
	}
	
	void update_world_bounds(Element@ parent)
	{
		x1 = parent.x1 + x;
		y1 = parent.y1 + y;
		x2 = x1 + width;
		y2 = y1 + height;
	}
	
	void update_world_bounds()
	{
		x1 = x;
		y1 = y;
		x2 = x + width;
		y2 = y + height;
	}
	
	// ------------------------------------------------
	// Mouse Events
	// ------------------------------------------------
	
	void _mouse_enter() { }
	void _mouse_exit() { }
	void _mouse_press(const MouseButton button) { }
	void _mouse_move() { }
	void _mouse_release(const MouseButton button) { }
	void _mouse_click() { }
	void _mouse_button_click(const MouseButton button) { }
	
}