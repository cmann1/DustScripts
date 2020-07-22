#include '../std.cpp';
#include '../string.cpp';
#include '../fonts.cpp';
#include '../math/math.cpp';
#include '../utils/colour.cpp';
#include '../graphics/Graphics.cpp';
#include '../graphics/GraphicsUtils.cpp';
#include '../graphics/WorldGraphics.cpp';
#include '../graphics/HudGraphics.cpp';
#include 'UIMouse.cpp';
#include 'Style.cpp';
#include 'utils/ElementStack.cpp';
#include 'events/Event.cpp';
#include 'elements/Element.cpp';
#include 'elements/Button.cpp';
#include 'elements/Container.cpp';
#include 'elements/Image.cpp';
#include 'elements/Label.cpp';

class UI
{
	
	/*
	 * TODO:
	 *  Set ui layout
	 * UI scale
	 */
	
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
	private Graphics@ graphics;
	
	private bool _hud;
	private int _layer;
	private int _sub_layer;
	private int _player;
	
	private Container@ contents;
	// e.g. a drop down box that is open. There can only by one active element in a UI.
	private Element@ active_element;
	
	private EventInfo@ event_info = EventInfo();
	
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
		
		@event_info.mouse = mouse;
		
		if(hud)
			@graphics = HudGraphics(g, layer, sub_layer);
		else
			@graphics = WorldGraphics(g, layer, sub_layer);
		
		@style = Style();
		@style.graphics = graphics;
		
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
		graphics.layer = _layer;
		graphics.sub_layer = _sub_layer;
		
		for(int i = 0; i < draw_list_index; i++)
		{
			draw_list[i].draw(graphics, sub_frame);
		}
	}
	
	// TODO: Render elements outside view with dotted line
	void debug_draw()
	{
		Graphics::outline(graphics, contents.x1, contents.y1, contents.x2, contents.y2, -2, 0xaaffffff);
		
		element_stack.clear();
		contents._queue_children_for_layout(@element_stack);
		Element@ element = element_stack.pop();
		
		debug_text_field.align_horizontal(-1);
		debug_text_field.align_vertical(-1);
		const float id_scale = 0.5;
		const uint alpha = 0x55000000;
		
		while(@element != null)
		{
			if(element.visible && @element != @mouse_over_element)
			{
				const uint clr = get_element_id_colour(element);
				
				debug_text_field.text(element._id);
				debug_text_field.colour(scale_lightness(clr, 0.1) | 0xff000000);
				
				graphics.draw_rectangle(element.x1, element.y1, element.x2, element.y2,
					0, (element.hovered ? 0xff0000 : clr) | alpha);
				graphics.draw_text(debug_text_field,
					element.x1 + style.spacing, element.y1 + style.spacing,
					id_scale, id_scale, 0);
			}
			
			element._queue_children_for_layout(@element_stack);
			@element = element_stack.pop();
		}
		
		if(@mouse_over_element != null)
		{
			const uint clr = get_element_id_colour(mouse_over_element);
			
			graphics.draw_rectangle(mouse_over_element.x1, mouse_over_element.y1, mouse_over_element.x2, mouse_over_element.y2,
				0, 0x00ff00 | alpha);
			Graphics::outline(graphics, mouse_over_element.x1, mouse_over_element.y1, mouse_over_element.x2, mouse_over_element.y2, -1, 0xffffff | alpha);
			
			debug_text_field.text(mouse_over_element._id);
			debug_text_field.colour(scale_lightness(clr, 0.1) | 0xff000000);
			graphics.draw_text(debug_text_field,
				mouse_over_element.x1 + style.spacing, mouse_over_element.y1 + style.spacing,
				id_scale, id_scale, 0);
			
			// Debug print mouse stack
			if(@debug != null)
			{
				const int num_elements_mouse_over = int(elements_mouse_over.size());
				
				for(int i = num_elements_mouse_over - 1; i >= 0; i--)
				{
					string indent = '';
					
					for(int j = 0; j < i; j++) indent += '- ';
					
					@element = @elements_mouse_over[i];
					debug.print(indent + element._id, set_alpha(get_element_id_colour(element), 1), element._id, 1);
				}
			}
		}
	}
	
	// TODO: Do not registor the mouse, or draw elements outside of this region
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
	
	// Private
	// ---------------------------------------------------------
	
	private uint get_element_id_colour(Element@ element)
	{
		const float hash = float(string::hash(element._id));
		return hsl_to_rgb(
			sin(hash) * 0.5 + 0.5,
			map(sin(hash) * 0.5 + 0.5, 0, 1, 0.8, 0.9),
			map(sin(hash) * 0.5 + 0.5, 0, 1, 0.65, 0.75)
		);
	}
	
	private void update_layout()
	{
		element_stack.clear();
		contents.do_layout(0, 0);
		contents._queue_children_for_layout(@element_stack);
		
		draw_list_index = 0;
		
		const float view_x1 = contents.x1;
		const float view_y1 = contents.y1;
		const float view_x2 = contents.x2;
		const float view_y2 = contents.y2;
		const bool mouse_in_ui = mouse.x >= view_x1 && mouse.x <= view_x2 && mouse.y >= view_y1 && mouse.y <= view_y2;
		
		Element@ element = element_stack.pop();
		
		while(@element != null)
		{
			if(!element.visible)
				continue;
			
			element.do_layout(element.parent.x1, element.parent.y1);
			element._queue_children_for_layout(@element_stack);
			
			if(mouse_in_ui &&!element.disabled && element.mouse_enabled && element.parent.children_mouse_enabled)
			{
				if(element.overlaps_point(mouse.x, mouse.y))
				{
					@_mouse_over_element = element;
				}
			}
			
			if(element.x1 <= view_x2 && element.x2 >= view_x1 && element.y1 <= view_y2 && element.y2 >= view_y1)
			{
				if(draw_list_index >= draw_list_size)
				{
					draw_list_size = draw_list_index + 64;
				}
				
				@draw_list[draw_list_index++] = element;
			}
			
			@element = element_stack.pop();
		}
		
		if(@_mouse_over_element == @contents)
		{
			@_mouse_over_element = null;
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
		
		event_info.reset(EventType::MOUSE_EXIT, MouseButton::None, mouse.x, mouse.y);
		
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
				
				@event_info.target = element;
				element.mouse_exit.dispatch(event_info);
			}
			
			break;
		}
		
		/*
		 * Mouse enter
		 */
		
		if(is_mouse_over)
		{
			event_info.reset(EventType::MOUSE_ENTER, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = int(elements_mouse_enter.size()) - 1; i >= 0; i--)
			{
				Element@ element = @elements_mouse_enter[i];
				
				if(element.hovered)
					continue;
				
				element.hovered = true;
				elements_mouse_over.insertLast(element);
				
				@event_info.target = element;
				element.mouse_enter.dispatch(event_info);
			}
		}
		
		/*
		 * Mouse press
		 */
		
		if(is_mouse_over && mouse.left_press || mouse.middle_press || mouse.right_press)
		{
			event_info.reset(EventType::MOUSE_PRESS, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event_info.target = @elements_mouse_enter[i];
				
				if(mouse.left_press)
				{
					elements_left_pressed[element._id] = true;
					event_info.button = MouseButton::Left;
					element.mouse_press.dispatch(event_info);
				}
				
				if(mouse.right_press)
				{
					elements_right_pressed[element._id] = true;
					event_info.button = MouseButton::Right;
					element.mouse_press.dispatch(event_info);
				}
				
				if(mouse.middle_press)
				{
					elements_middle_pressed[element._id] = true;
					event_info.button = MouseButton::Middle;
					element.mouse_press.dispatch(event_info);
				}
			}
		}
		
		/*
		 * Mouse move
		 */
		
		if(is_mouse_over && mouse.moved)
		{
			event_info.reset(EventType::MOUSE_MOVE, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event_info.target = @elements_mouse_enter[i];
				element.mouse_move.dispatch(event_info);
			}
		}
		
		/*
		 * Mouse release and click
		 */
		
		if(is_mouse_over && mouse.left_release || mouse.middle_release || mouse.right_release)
		{
			// Release
			
			event_info.reset(EventType::MOUSE_RELEASE, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event_info.target = @elements_mouse_enter[i];
				
				if(mouse.left_release)
				{
					event_info.button = MouseButton::Left;
					element.mouse_release.dispatch(event_info);
				}
				
				if(mouse.right_release)
				{
					event_info.button = MouseButton::Right;
					element.mouse_release.dispatch(event_info);
				}
				
				if(mouse.middle_release)
				{
					event_info.button = MouseButton::Middle;
					element.mouse_release.dispatch(event_info);
				}
			}
			
			// Click
			
			event_info.reset(EventType::MOUSE_CLICK, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event_info.target = @elements_mouse_enter[i];
				
				if(mouse.left_release && elements_left_pressed.exists(element._id))
				{
					event_info.button = MouseButton::Left;
					element.mouse_click.dispatch(event_info);
				}
				
				if(mouse.right_release && elements_right_pressed.exists(element._id))
				{
					event_info.button = MouseButton::Right;
					element.mouse_click.dispatch(event_info);
				}
				
				if(mouse.middle_release && elements_middle_pressed.exists(element._id))
				{
					event_info.button = MouseButton::Middle;
					element.mouse_click.dispatch(event_info);
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
	
}