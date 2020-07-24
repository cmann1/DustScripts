#include '../std.cpp';
#include '../string.cpp';
#include '../fonts.cpp';
#include '../math/math.cpp';
#include '../utils/colour.cpp';
#include 'UIMouse.cpp';
#include 'Style.cpp';
#include 'utils/ElementStack.cpp';
#include 'events/Event.cpp';
#include 'elements/Element.cpp';
#include 'elements/Button.cpp';
#include 'elements/Container.cpp';
#include 'elements/Image.cpp';
#include 'elements/Label.cpp';
#include 'elements/SingleContainer.cpp';
#include 'elements/LockedContainer.cpp';
#include 'elements/Tooltip.cpp';

class UI
{
	
	int NEXT_ID;
	
	// Which mouse button is primarily used to interact with UI elements.
	// Left might be more problematic since it will also interact with the editor ui.
	MouseButton pimary_button = MouseButton::Right;
	
	Style@ style;
	UIMouse@ mouse;
	bool is_mouse_over;
	
	// Manually set during testing
	Debug@ debug;
	
	private scene@ g;
	
	private bool _hud;
	private int _layer;
	private int _sub_layer;
	private int _player;
	
	private Container@ contents;
	private Container@ overlays;
	// e.g. a drop down box that is open. There can only by one active element in a UI.
	private Element@ active_element;
	
	/*private*/ EventInfo@ _event_info = EventInfo();
	
	// Used for processing element layouts
	private ElementStack element_stack;
	// The top most element the mouse is over
	private Element@ _mouse_over_element;
	// Elements entered on this frame
	private array<Element@> elements_mouse_enter();
	// The hierarchy of elements the mouse is over, from the outermost to the inner
	private array<Element@> elements_mouse_over();
	
	private dictionary elements_left_pressed();
	private dictionary elements_right_pressed();
	private dictionary elements_middle_pressed();
	
	private int draw_list_size = 64;
	private array<Element@> draw_list(draw_list_size);
	private int draw_list_index;
	
	private textfield@ debug_text_field;
	
	UI(bool hud=true, int layer=20, int sub_layer=19, int player=0)
	{
		@contents = Container(this);
		@overlays = Container(this);
		contents._id = '_ROOT_';
		overlays._id = '_OVERLAYS_';
		
		_hud = hud;
		_layer = layer;
		_sub_layer = sub_layer;
		_player = player;
		
		if(hud)
		{
			contents.x = SCREEN_LEFT;
			contents.y = SCREEN_TOP;
			contents.width  = SCREEN_RIGHT - SCREEN_LEFT;
			contents.height = SCREEN_BOTTOM - SCREEN_TOP;
		}
		
		@g = get_scene();
		@mouse = UIMouse(hud, layer, player);
		
		@_event_info.mouse = mouse;
		
		@style = Style(hud);
		
		@debug_text_field = create_textfield();
		debug_text_field.set_font(font::PROXIMANOVA_REG, 26);
	}
	
	bool add_child(Element@ child)
	{
		return contents.add_child(child);
	}
	
	bool remove_child(Element@ child)
	{
		return contents.remove_child(child);
	}
	
	void clear()
	{
		contents.clear();
	}
	
	void step()
	{
		mouse.step();
		
		switch(pimary_button)
		{
			case MouseButton::Left:
				mouse.primary_down = mouse.left_down;
				mouse.primary_press = mouse.left_press;
				mouse.primary_release = mouse.left_release;
				break;
			case MouseButton::Middle:
				mouse.primary_down = mouse.middle_down;
				mouse.primary_press = mouse.middle_press;
				mouse.primary_release = mouse.middle_release;
				break;
			case MouseButton::Right:
				mouse.primary_down = mouse.right_down;
				mouse.primary_press = mouse.right_press;
				mouse.primary_release = mouse.right_release;
				break;
		}
		
		/*
		 * Update layout
		 */
		
		@_mouse_over_element = null;
		
		update_layout();
		process_mouse_events();
	}
	
	void draw(float sub_frame)
	{
		style._layer = _layer;
		style._sub_layer = _sub_layer;
		
		contents.draw(@style, sub_frame);
		style._reset_state();
		
		overlays.draw(@style, sub_frame);
		style._reset_state();
	}
	
	void debug_draw(bool just_outline=false, const float id_scale=0.4)
	{
		style.outline(contents.x1, contents.y1, contents.x2, contents.y2, -2, 0xaaffffff);
		
		if(just_outline)
			return;
		
		element_stack.clear();
		contents._queue_children_for_layout(@element_stack);
		Element@ element = element_stack.pop();
		
		debug_text_field.align_horizontal(-1);
		debug_text_field.align_vertical(-1);
		const uint alpha = 0x55000000;
		uint clr;
		
		const float view_x1 = contents.x1;
		const float view_y1 = contents.y1;
		const float view_x2 = contents.x2;
		const float view_y2 = contents.y2;
		
		while(@element != null)
		{
			if(element.visible && @element != @mouse_over_element)
			{
				clr = get_element_id_colour(element, 0x00);
				
				if(element.x1 <= view_x2 && element.x2 >= view_x1 && element.y1 <= view_y2 && element.y2 >= view_y1)
				{
					style.outline(
						element.x1, element.y1, element.x2, element.y2,
						1, (element.hovered ? 0xff0000 : clr) | alpha);
					
					clr = scale_lightness(clr | alpha, 0.1);
				}
				else
				{
					style.outline_dotted(element.x1, element.y1, element.x2, element.y2, 1, clr | alpha);
					clr = scale_lightness(clr, 0.1) | alpha;
				}
				
				style.draw_text(
					element._id,
					element.x1, element.y1,
					id_scale, id_scale, 0, clr | 0xff000000);
			}
			
			element._queue_children_for_layout(@element_stack);
			@element = element_stack.pop();
		}
		
		if(@mouse_over_element != null)
		{
			clr = get_element_id_colour(mouse_over_element);
			
			style.outline(mouse_over_element.x1, mouse_over_element.y1, mouse_over_element.x2, mouse_over_element.y2, -2, 0xffffffff);
			
			style.outline_text(
				mouse_over_element._id,
				mouse_over_element.x1, mouse_over_element.y1,
				id_scale, id_scale, 0, 0xffffffff, 0xff000000, 2 * id_scale);
			
			// Debug print mouse stack
			if(@debug != null)
			{
				const int num_elements_mouse_over = int(elements_mouse_over.size());
				
				for(int i = num_elements_mouse_over - 1; i >= 0; i--)
				{
					@element = @elements_mouse_over[i];
					debug.print(string::repeat('- ', i) + element._id, set_alpha(get_element_id_colour(element), 1), element._id, 1);
				}
			}
		}
	}
	
	void set_region(const float x1, const float y1, const float x2, const float y2)
	{
		contents.x = x1;
		contents.y = y1;
		contents.width  = x2 - x1;
		contents.height = y2 - y1;
	}
	
	// The top most element the mouse is over
	Element@ mouse_over_element { get { return @_mouse_over_element; } }
	
	/**
	 * @brief Returns mouse x relative to this element
	 */
	float mouse_x { get { return mouse.x - contents.x1; } }
	
	/**
	 * @brief Returns mouse y relative to this element
	 */
	float mouse_y { get { return mouse.y - contents.y1; } }
	
	bool hud
	{
		get { return _hud; }
		set { style._hud = _hud = value; }
	}
	
	uint layer
	{
		get { return _layer; }
		set { style._layer = _layer = value; }
	}
	
	uint sub_layer
	{
		get { return _sub_layer; }
		set { style._sub_layer = _sub_layer = value; }
	}
	
	// Private
	// ---------------------------------------------------------
	
	private void update_layout()
	{
		element_stack.clear();
		contents.do_layout(0, 0);
		contents._queue_children_for_layout(@element_stack);
		
		const float view_x1 = contents.x1;
		const float view_y1 = contents.y1;
		const float view_x2 = contents.x2;
		const float view_y2 = contents.y2;
		const bool mouse_in_ui = mouse.x >= view_x1 && mouse.x <= view_x2 && mouse.y >= view_y1 && mouse.y <= view_y2;
		
		Element@ element = element_stack.pop();
		
		while(@element != null)
		{
			if(element.visible)
			{
				element.do_layout(element.parent.x1, element.parent.y1);
				element._queue_children_for_layout(@element_stack);
				
				if(mouse_in_ui &&!element.disabled && element.mouse_enabled && element.parent.children_mouse_enabled)
				{
					if(element.overlaps_point(mouse.x, mouse.y))
					{
						@_mouse_over_element = element;
					}
				}
			}
			
			@element = element_stack.pop();
		}
		
		if(@_mouse_over_element == @contents)
		{
			@_mouse_over_element = null;
		}
		
		if(@_mouse_over_element != null && @_mouse_over_element.tooltip != null)
		{
			
		}
	}
	
	private void process_mouse_events()
	{
		is_mouse_over = @_mouse_over_element != null;
		
		/*
		 * Build the list/stack of the elements the mouse is over, from the innermost child to the outermost parent
		 */
		
		elements_mouse_enter.resize(0);
		
		if(is_mouse_over)
		{
			Element@ mouse_over_traversal = _mouse_over_element;
			
			do
			{
				elements_mouse_enter.insertLast(mouse_over_traversal);
				@mouse_over_traversal = mouse_over_traversal.parent;
			}
			while(@mouse_over_traversal != @contents);
		}
		
		/*
		 * Mouse exit
		 */
		
		_event_info.reset(EventType::MOUSE_EXIT, MouseButton::None, mouse.x, mouse.y);
		
		const int num_elements_mouse_over = int(elements_mouse_over.size());
		const int num_elements_mouse_enter = int(elements_mouse_enter.size());
		
		for(int i = 0, j = num_elements_mouse_enter - 1; i < num_elements_mouse_over; i++, j--)
		{
			// Compare the hierarchy of the element hierarchy the mouse was over on the last frame
			// with the hierarchy this frame
			
			Element@ prev_over_element = @elements_mouse_over[i];
			Element@ new_over_element  = j >= 0 ? @elements_mouse_enter[j] : null;
			
			if(@prev_over_element == @new_over_element)
				continue;
			
			// When they no longer match, trigger mouse exit events on the entire sub tree
			
			for(int k = num_elements_mouse_over - 1; k >= i; k--)
			{
				Element@ element = @elements_mouse_over[k];
				element.hovered = false;
				
				@_event_info.target = element;
				element.mouse_exit.dispatch(_event_info);
			}
			
			break;
		}
		
		/*
		 * Mouse enter
		 */
		
		if(is_mouse_over)
		{
			_event_info.reset(EventType::MOUSE_ENTER, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = int(elements_mouse_enter.size()) - 1; i >= 0; i--)
			{
				Element@ element = @elements_mouse_enter[i];
				
				if(element.hovered)
					continue;
				
				element.hovered = true;
				elements_mouse_over.insertLast(element);
				
				@_event_info.target = element;
				element.mouse_enter.dispatch(_event_info);
			}
		}
		
		/*
		 * Mouse press
		 */
		
		if(is_mouse_over && mouse.left_press || mouse.middle_press || mouse.right_press)
		{
			_event_info.reset(EventType::MOUSE_PRESS, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @_event_info.target = @elements_mouse_enter[i];
				
				if(mouse.left_press)
				{
					elements_left_pressed[element._id] = true;
					_event_info.button = MouseButton::Left;
					element.mouse_press.dispatch(_event_info);
				}
				
				if(mouse.right_press)
				{
					elements_right_pressed[element._id] = true;
					_event_info.button = MouseButton::Right;
					element.mouse_press.dispatch(_event_info);
				}
				
				if(mouse.middle_press)
				{
					elements_middle_pressed[element._id] = true;
					_event_info.button = MouseButton::Middle;
					element.mouse_press.dispatch(_event_info);
				}
			}
		}
		
		/*
		 * Mouse move
		 */
		
		if(is_mouse_over && mouse.moved)
		{
			_event_info.reset(EventType::MOUSE_MOVE, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @_event_info.target = @elements_mouse_enter[i];
				element.mouse_move.dispatch(_event_info);
			}
		}
		
		/*
		 * Mouse release and click
		 */
		
		if(is_mouse_over && mouse.left_release || mouse.middle_release || mouse.right_release)
		{
			// Release
			
			_event_info.reset(EventType::MOUSE_RELEASE, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @_event_info.target = @elements_mouse_enter[i];
				
				if(mouse.left_release)
				{
					_event_info.button = MouseButton::Left;
					element.mouse_release.dispatch(_event_info);
				}
				
				if(mouse.right_release)
				{
					_event_info.button = MouseButton::Right;
					element.mouse_release.dispatch(_event_info);
				}
				
				if(mouse.middle_release)
				{
					_event_info.button = MouseButton::Middle;
					element.mouse_release.dispatch(_event_info);
				}
			}
			
			// Click
			
			_event_info.reset(EventType::MOUSE_CLICK, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @_event_info.target = @elements_mouse_enter[i];
				
				if(mouse.left_release && elements_left_pressed.exists(element._id))
				{
					_event_info.button = MouseButton::Left;
					element.mouse_click.dispatch(_event_info);
				}
				
				if(mouse.right_release && elements_right_pressed.exists(element._id))
				{
					_event_info.button = MouseButton::Right;
					element.mouse_click.dispatch(_event_info);
				}
				
				if(mouse.middle_release && elements_middle_pressed.exists(element._id))
				{
					_event_info.button = MouseButton::Middle;
					element.mouse_click.dispatch(_event_info);
				}
			}
		}
		
		/*
		 * Finalise
		 */
		
		// Set mouse_over erray
		
		elements_mouse_over.resize(elements_mouse_enter.size());
		
		for(int i = int(elements_mouse_enter.size()) - 1, j = 0; i >= 0; i--, j++)
		{
			@elements_mouse_over[j] = @elements_mouse_enter[i];
		}
		
		// Clear pressed elements
		
		if(mouse.left_release)
			elements_left_pressed.deleteAll();
		if(mouse.right_release)
			elements_right_pressed.deleteAll();
		if(mouse.middle_release)
			elements_middle_pressed.deleteAll();
	}
	
	private uint get_element_id_colour(Element@ element, const uint alpha=0xff)
	{
		const float hash = float(string::hash(element._id));
		return hsl_to_rgb(
			sin(hash) * 0.5 + 0.5,
			map(sin(hash) * 0.5 + 0.5, 0, 1, 0.8, 0.9),
			map(sin(hash) * 0.5 + 0.5, 0, 1, 0.65, 0.75)
		) | (alpha << 24);
	}
	
}