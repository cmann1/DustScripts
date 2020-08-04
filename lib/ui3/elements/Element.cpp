#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/ClippingMode.cpp';
#include '../utils/DrawingContext.cpp';
#include '../utils/LayoutContext.cpp';
#include '../utils/ElementStack.cpp';
#include '../popups/PopupOptions.cpp';
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
	
	PopupOptions@ tooltip = null;
	
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
	
	uint background_colour = 0;
	bool background_blur = false;
	uint border_colour = 0;
	float border_size = 0;
	
	int clip_contents = ClippingMode::None;
	float alpha = 1;
	
	float _x;
	float _y;
	float _width = 100;
	float _height = 100;
	/// Stores the width that has explicitly been set.
	/// Layouts can read _set_width and write _width allowing elements to grow and shrink back down.
	/// Elements must remember to also set this when manually setting _width
	float _set_width = _width;
	/// Same as _set_width
	float _set_height = _height;
	
	float _scroll_x;
	float _scroll_y;
	
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
	
	Element(UI@ ui)
	{
		@this.ui = @ui;
		_id = (++ui.NEXT_ID) + '';
	}
	
	string element_type { get const { return 'Element'; } }
	
	string id { get const { return element_type + _id; } }
	
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
			if(value < 0) value = 0;
			if(_width == value) return;
			_set_width = _width = value;
			if(@parent != null) parent._validate_layout = true;
		}
	}
	
	float height
	{
		get const { return _height; }
		set
		{
			if(value < 0) value = 0;
			if(_height == value) return;
			_set_height = _height = value;
			if(@parent != null) parent._validate_layout = true; 
		}
	}
	
	float scroll_x
	{
		get const { return _scroll_x; }
		set
		{
			if(_scroll_x == value) return;
			_scroll_x = value;
		}
	}
	
	float scroll_y
	{
		get const { return _scroll_y; }
		set
		{
			if(_scroll_y == value) return;
			_scroll_y = value;
		}
	}
	
	float _get_max_width(const float max_height=-1)
	{
		return _set_width;
	}
	
	float _get_max_height(const float max_width=-1)
	{
		return _set_height;
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
		bool has_border = border_colour != 0 && border_size != 0;
		
		if(background_blur)
		{
			style.draw_glass(x1 + 1, y1 + 1, x2 - 1, y2 - 1, 0);
		}
		
		if(background_colour != 0)
		{
			const float inset = has_border ? max(0, border_size) : 0;
			
			style.draw_rectangle(
				x1 + inset, y1 + inset, x2 - inset, y2 - inset,
				0, background_colour);
		}
		
		if(has_border)
		{
			style.outline(x1, y1, x2, y2, border_size, border_colour);
		}
	}
	
	void update_world_bounds(Element@ parent)
	{
		x1 = parent.x1 + _x;
		y1 = parent.y1 + _y;
		x2 = x1 + _width;
		y2 = y1 + _height;
	}
	
	void update_world_bounds()
	{
		x1 = _x;
		y1 = _y;
		x2 = _x + _width;
		y2 = _y + _height;
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