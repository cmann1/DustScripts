#include 'Container.cpp';
#include '../events/Event.cpp';

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
	// Disabled this element. Only relevant for interactive elements
	bool disabled;
	
	float alpha = 1;
	
	float x, y;
	float width = 100, height = 100;
	
	// After do_layout, these will be the element's position in world/ui space
	float x1, y1;
	float x2, y2;
	
	Event mouse_enter;
	Event mouse_mouse;
	Event mouse_exit;
	Event mouse_press;
	Event mouse_move;
	Event mouse_release;
	Event mouse_click;
	
	Element(UI@ ui, const string &in type_identifier)
	{
		@this.ui = @ui;
		_id = type_identifier + (++ui.NEXT_ID);
	}
	
	void do_layout(const float parent_x, const float parent_y)
	{
		x1 = parent_x + x;
		y1 = parent_y + y;
		x2 = x1 + width;
		y2 = y1 + height;
	}
	
	void draw(Style@ style, const float sub_frame)
	{
		// Debug
		//style.draw_rectangle(x1, y1, x2, y2, 0, 0x55000000);
	}
	
	bool overlaps_point(const float px, const float py)
	{
		return px < x2 && px >= x1 && py < y2 && py >= y1;
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
	
}