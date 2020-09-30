#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/ClippingMode.cpp';
#include '../utils/DrawingContext.cpp';
#include '../utils/LayoutContext.cpp';
#include '../utils/ElementStack.cpp';
#include '../popups/PopupOptions.cpp';
#include '../events/Event.cpp';
#include 'Container.cpp';

/// The base class for all display objects in a UI
abstract class Element
{
	
	UI@ ui;
	Container@ parent;
	
	/// Changing this MIGHT cause problems, especially if it conflicts with another element. Rather use name if you need to identify an element.
	string _id;
	/// A way to identify this element, e.g. during even callbacks - can be anything.
	string name = '';
	
	PopupOptions@ tooltip = null;
	
	bool visible = true;
	/// Can this element and all descendants interact with the mouse
	bool mouse_enabled = true;
	/// Prevents this element from recieving/capturing mouse events, but stills allows descendants
	bool mouse_self = true;
	/// Can this element's children interact with the mouse
	bool children_mouse_enabled = true;
	/// Is the mouse within/over this element, or any of its descendants
	bool hovered;
	/// The mouse is down and was pressed over this element
	bool pressed;
	/// Disabled this element. Only relevant for interactive elements
	bool disabled;
	
	uint background_colour = 0;
	bool background_blur = false;
	float blur_inset = 1;
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
	bool _scroll_children = true;
	
	/// After do_layout, these will be the element's position in world/ui space
	float x1, y1;
	float x2, y2;
	/// This element bounds including children
	float subtree_x1, subtree_y1;
	float subtree_x2, subtree_y2;
	
	Event mouse_enter;
	Event mouse_exit;
	Event mouse_press;
	Event mouse_move;
	Event mouse_release;
	/// Triggered when the primary ui button is clicked
	Event mouse_click;
	/// Called when any mouse button is clicked
	Event mouse_button_click;
	Event mouse_scroll;
	
	/// Internal
	bool validate_layout = true;
	
	Element(UI@ ui)
	{
		@this.ui = @ui;
		_id = (++ui.NEXT_ID) + '';
	}
	
	string element_type { get const { return 'Element'; } }
	
	string id { get const { return element_type + _id; } }
	
	/// Returns true if the given point is within this element
	bool overlaps_point(const float x, const float y)
	{
		return x < x2 && x >= x1 && y < y2 && y >= y1;
	}
	
	/// Returns true if the mouse is within this element
	bool check_mouse()
	{
		return ui.mouse.x < x2 && ui.mouse.x >= x1 && ui.mouse.y < y2 && ui.mouse.y >= y1;
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
	
	/// Forces calculation of this element's global bounding box. Useful for example if an element is moved
	/// on the same frame a tooltip is shown to prevent it from popping up in the element's previous location for a single frame.
	void force_calculate_bounds()
	{
		to_global(_x, _y, x1, y1);
		x2 = x1 + _width;
		y2 = y1 + _height;
	}
	
	Element@ find_closest(const string type_name)
	{
		Element@ element = @this;
		
		do
		{
			if(element.element_type == type_name)
				return @element;
			
			@element = element.parent;
		}
		while(@element != null);
		
		return null;
	}
	
	/// Shows the tooltip for this element if it has one.
	void show_tooltip()
	{
		if(@tooltip == null)
			return;
		
		ui.show_tooltip(this);
	}
	
	void hide_tooltip()
	{
		ui.hide_tooltip(this);
	}
	
	/// Returns mouse x relative to this element
	float mouse_x { get { return ui.mouse.x - x1; } }
	
	/// Returns mouse y relative to this element
	float mouse_y { get { return ui.mouse.y - y1; } }
	
	float x
	{
		get const { return _x; }
		set
		{
			if(_x == value)
				return;
			
			_x = value;
			
			if(@parent != null)
				parent.validate_layout = true;
		}
	}
	
	float y
	{
		get const { return _y; }
		set
		{
			if(_y == value)
				return;
			
			_y = value;
			
			if(@parent != null)
				parent.validate_layout = true;
		}
	}
	
	float width
	{
		get const { return _width; }
		set
		{
			if(value < 0)
				value = 0;
			if(_width == value)
				return;
			
			_set_width = _width = value;
			validate_layout = true;
			
			if(@parent != null)
				parent.validate_layout = true;
		}
	}
	
	float height
	{
		get const { return _height; }
		set
		{
			if(value < 0)
				value = 0;
			if(_height == value)
				return;
			
			_set_height = _height = value;
			validate_layout = true;
			
			if(@parent != null)
				parent.validate_layout = true; 
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
	
	void invalidate_layout()
	{
		validate_layout = true;
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
	
	/// Internal. Append all children of this element to the stack. Must be in reverse order
	void _queue_children_for_layout(ElementStack@ stack)
	{
		
	}
	
	void _do_layout(LayoutContext@ ctx)
	{
		
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const bool has_border = border_colour != 0 && border_size != 0;
		
		if(background_blur)
		{
			style.draw_glass(x1 + blur_inset, y1 + blur_inset, x2 - blur_inset, y2 - blur_inset, 0);
		}
		
		if(background_colour != 0)
		{
			const float inset = has_border ? max(0.0, border_size) : 0;
			
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
	
	float _get_max_width(const float max_height=-1)
	{
		return _set_width;
	}
	
	float _get_max_height(const float max_width=-1)
	{
		return _set_height;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	/// When the mouse enters this element
	void _mouse_enter(EventInfo@ event) { }
	
	/// When the mouse exits this element
	void _mouse_exit(EventInfo@ event) { }
	
	/// This element is pressed with any mouse button
	void _mouse_press(EventInfo@ event) { }
	
	/// The mouse moves within this element
	void _mouse_move(EventInfo@ event) { }
	
	/// The mouse is released within this element. Will also trigger for the active element
	/// even when the mouse is outside
	void _mouse_release(EventInfo@ event) { }
	
	/// This element is clicked with any mouse button
	void _mouse_button_click(EventInfo@ event) { }
	
	/// This element is clicked with the primary mouse button
	void _mouse_click(EventInfo@ event) { }
	
	/// The mouse wheel is scroll while over this element
	void _mouse_scroll(EventInfo@ event) { }
	
}