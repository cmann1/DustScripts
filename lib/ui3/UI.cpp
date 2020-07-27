#include '../std.cpp';
#include '../string.cpp';
#include '../fonts.cpp';
#include '../math/math.cpp';
#include '../utils/colour.cpp';
#include 'UIMouse.cpp';
#include 'Style.cpp';
#include 'utils/ClippingMode.cpp';
#include 'utils/ElementStack.cpp';
#include 'utils/LayoutContext.cpp';
#include 'utils/DrawingContext.cpp';
#include 'utils/pools/ImagePool.cpp';
#include 'utils/pools/LabelPool.cpp';
#include 'events/Event.cpp';
#include 'events/GenericEvent.cpp';
#include 'elements/Element.cpp';
#include 'elements/Container.cpp';
#include 'elements/Tooltip.cpp';
#include 'layouts/flow/FlowLayout.cpp';

class UI
{
	
	int NEXT_ID;
	int CUSTOM_TOOLTIP_ID;
	
	// Which mouse button is primarily used to interact with UI elements.
	// Left might be more problematic since it will also interact with the editor ui.
	MouseButton primary_button = MouseButton::Right;
	
	Style@ style;
	UIMouse@ mouse;
	bool is_mouse_over;
	
	// Uncomment and manually set during testing
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
	
	// Used for processing element layouts
	private ElementStack element_stack;
	int layout_context_pool_size = 16;
	int layout_context_pool_index = 0;
	private array<LayoutContext@> layout_context_pool(layout_context_pool_size);
	// The top most element the mouse is over
	private Element@ _mouse_over_element;
	// Elements entered on this frame
	private array<Element@> elements_mouse_enter();
	// The hierarchy of elements the mouse is over, from the outermost to the inner
	private array<Element@> elements_mouse_over();
	
	private Element@ mouse_over_overlays;
	
	private dictionary elements_left_pressed();
	private dictionary elements_right_pressed();
	private dictionary elements_middle_pressed();
	private array<Element@> elements_pressed_list();
	
	private dictionary tooltips;
	
	private textfield@ debug_text_field;
	
	private float x1 = 0;
	private float y1 = 0;
	private float x2 = 100;
	private float y2 = 100;
	
	private EventCallback@ on_tooltip_hide_delegate;
	
	// ///////////////////////////////////////////////////////////////
	// Common reusable things
	
	/*private*/ LabelPool _label_pool;
	/*private*/ ImagePool _image_pool;
	
	/*private*/ EventInfo@ _event_info = EventInfo();
	/*private*/ GenericEventInfo@ _generic_event_info = GenericEventInfo();
	/*private*/ FlowLayout@ _toolbar_flow_layout;
	
	/*private*/ array<float> _float_array(16);
	/*private*/ array<int> _int_array(16);
	/*private*/ array<Element@> _element_array(16);
	
	UI(bool hud=true, int layer=20, int sub_layer=19, int player=0)
	{
		@on_tooltip_hide_delegate = EventCallback(this.on_tooltip_hide);
		
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
			x1 = SCREEN_LEFT;
			y1 = SCREEN_TOP;
			x2 = SCREEN_RIGHT;
			y2 = SCREEN_BOTTOM;
		}
		
		@g = get_scene();
		@mouse = UIMouse(hud, layer, player);
		
		@_event_info.mouse = mouse;
		
		@style = Style(hud);
		
		@debug_text_field = create_textfield();
		debug_text_field.set_font(font::PROXIMANOVA_REG, 26);
		
		set_region(x1, y1, x2, y2);
		
		@_toolbar_flow_layout = FlowLayout(this, FlowDirection::Row, FlowAlign::Start, FlowAlign::Centre, FlowWrap::Wrap);
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
		set { mouse.hud = style._hud = _hud = value; }
	}
	
	uint layer
	{
		get { return _layer; }
		set { mouse.layer = style._layer = _layer = value; }
	}
	
	uint sub_layer
	{
		get { return _sub_layer; }
		set { style._sub_layer = _sub_layer = value; }
	}
	
	bool add_child(Element@ child)
	{
		return contents.add_child(child);
	}
	
	bool remove_child(Element@ child)
	{
		return contents.remove_child(child);
	}
	
	void move_to_front(Element@ child)
	{
		contents.move_to_front(child);
	}
	
	void move_to_back(Element@ child)
	{
		contents.move_to_back(child);
	}
	
	void move_up(Element@ child)
	{
		contents.move_up(child);
	}
	
	void move_down(Element@ child)
	{
		contents.move_down(child);
	}
	
	void clear()
	{
		contents.clear();
	}
	
	void step()
	{
		process_mouse_events(@_mouse_over_element == @mouse_over_overlays ? overlays : contents);
		
		mouse.step();
		
		switch(primary_button)
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
		
		const bool mouse_in_ui = mouse.x >= contents.x1 && mouse.x <= contents.x2 && mouse.y >= contents.y1 && mouse.y <= contents.y2;
		
		Element@ mouse_over_main = update_layout(contents, mouse_in_ui);
		@_mouse_over_element = mouse_over_main;
		
		@mouse_over_overlays = update_layout(overlays, mouse_in_ui);
		
		if(@mouse_over_overlays != null)
		{
			@_mouse_over_element = @mouse_over_overlays;
		}
		
		// Process the mouse events before the start of the next frame.
		// Changes made during an event callback might affect the layout so when draw is called next
		// some elements might not be positioned correctly.
		// Wait for the next frame so that these changes will be reflected when the next layout pass happens.
	}
	
	void draw(float sub_frame)
	{
		style._layer = _layer;
		style._sub_layer = _sub_layer;
		
		draw_root(contents);
		draw_root(overlays);
	}
	
	void debug_draw(bool just_outline=false, const float id_scale=0.4)
	{
		style.reset_drawing_context(null);
		style.outline(contents.x1, contents.y1, contents.x2, contents.y2, -2, 0xaaffffff);
		
		if(just_outline)
			return;
		
		debug_draw_root(contents, id_scale);
		debug_draw_root(overlays, id_scale);
	}
	
	void set_region(const float x1, const float y1, const float x2, const float y2)
	{
		this.x1 = x1;
		this.y1 = y1;
		this.x2 = x2;
		this.y2 = y2;
		
		contents.x = x1;
		contents.y = y1;
		contents.width  = x2 - x1;
		contents.height = y2 - y1;
		
		overlays.x = x1;
		overlays.y = y1;
		overlays.width  = x2 - x1;
		overlays.height = y2 - y1;
		
		style.set_default_clipping_region(x1, y1, x2, y2);
	}
	
	void get_region(float &out x1, float &out y1, float &out x2, float &out y2)
	{
		x1 = this.x1;
		y1 = this.y1;
		x2 = this.x2;
		y2 = this.y2;
	}
	
	float region_x1 { get const{ return x1; } }
	float region_y1 { get const{ return y1; } }
	float region_x2 { get const{ return x2; } }
	float region_y2 { get const{ return y2; } }
	float region_width  { get const{ return x2 - x1; } }
	float region_height { get const{ return y2 - y1; } }
	
	/**
	 * @brief Shows the tooltip for the given element if it has one.
	 * @param wait_for_mouse - If true and the tooltip hide type is MouseLeave, the tooltip will not close until the mouse enters it for the first time.
	 */
	void show_tooltip(Element@ element, bool wait_for_mouse=false)
	{
		if(@element == null || @element.tooltip == null)
			return;
		
		show_tooltip(element._id, element.tooltip, element, wait_for_mouse);
	}
	
	void hide_tooltip(Element@ element)
	{
		if(@element == null || @element.tooltip == null)
			return;
		
		hide_tooltip(element._id);
	}
	
	/**
	 * @param wait_for_mouse - If true and the tooltip hide type is MouseLeave, the tooltip will not close until the mouse enters it for the first time.
	 */
	void show_tooltip(TooltipOptions@ options, bool wait_for_mouse=false)
	{
		if(@options == null)
			return;
		
		if(options._id == '')
			options._id = '_tt_target' + (CUSTOM_TOOLTIP_ID++);
		
		show_tooltip(options._id, options, null, wait_for_mouse);
	}
	
	void hide_tooltip(TooltipOptions@ options)
	{
		if(@options == null)
			return;
		
		hide_tooltip(options._id);
	}
	
	void update_tooltip(Element@ element)
	{
		if(@element == null || @element.tooltip == null)
			return;
		
		update_tooltip(element._id, element.tooltip);
	}
	
	void update_tooltip(TooltipOptions@ options)
	{
		if(@options == null)
			return;
		
		update_tooltip(options._id, options);
	}
	
	// Private
	// ---------------------------------------------------------
	
	private Element@ update_layout(Element@ base, bool check_mouse_over)
	{
		Element@ mouse_over = null;
		
		ElementStack@ element_stack = @this.element_stack;
		element_stack.clear();
		
		base._queue_children_for_layout(@element_stack);
		
		array<LayoutContext@>@ layout_context_pool = @this.layout_context_pool;
		int layout_context_pool_size = this.layout_context_pool_size;
		int layout_context_pool_index = this.layout_context_pool_index;
		
		LayoutContext@ ctx = layout_context_pool_index > 0
			? @layout_context_pool[--layout_context_pool_index]
			: LayoutContext();
		@ctx.parent = null;
		@ctx.root = @base;
		ctx.num_children = element_stack.size + 1;
		ctx.mouse_active = check_mouse_over;
		ctx.clipping_mode = ClippingMode::Outside;
		
		ctx.x1 = base.x1;
		ctx.y1 = base.y1;
		ctx.x2 = base.x2;
		ctx.y2 = base.y2;
		ctx.subtree_x1 = ctx.x1;
		ctx.subtree_y1 = ctx.y1;
		ctx.subtree_x2 = ctx.x2;
		ctx.subtree_y2 = ctx.y2;
		base.subtree_x1 = ctx.x1;
		base.subtree_y1 = ctx.y1;
		base.subtree_x2 = ctx.x2;
		base.subtree_y2 = ctx.y2;
		
		base._do_layout(ctx);
		base.update_world_bounds();
		
		int stack_size = element_stack.size;
		const float mouse_x = mouse.x;
		const float mouse_y = mouse.y;
		
		while(stack_size > 0)
		{
			Element@ element = element_stack.pop();
			stack_size--;
			
			if(element.visible)
			{
				element._queue_children_for_layout(@element_stack);
				int num_children = element_stack.size - stack_size;
				stack_size += num_children;
				
				// Push a new context
				{
					LayoutContext@ new_ctx = layout_context_pool_index > 0
						? @layout_context_pool[--layout_context_pool_index]
						: LayoutContext();
					
					@new_ctx.parent = @ctx;
					@new_ctx.root = @element;
					new_ctx.num_children = num_children;
					new_ctx.mouse_active = ctx.mouse_active && element.mouse_enabled;
					new_ctx.clipping_mode = ctx.clipping_mode;
					new_ctx.x1 = ctx.x1;
					new_ctx.y1 = ctx.y1;
					new_ctx.x2 = ctx.x2;
					new_ctx.y2 = ctx.y2;
					
					@ctx = @new_ctx;
				}
				
				Element@ parent = element.parent;
				element._do_layout(ctx);
				
				if(element.clip_contents != ClippingMode::None)
				{
					ctx.clipping_mode = element.clip_contents;
					ctx.x1 = element.x1;
					ctx.y1 = element.y1;
					ctx.x2 = element.x2;
					ctx.y2 = element.y2;
				}
				
				if(@parent != null)
				{
					element.x1 = parent.x1 + element.x;
					element.y1 = parent.y1 + element.y;
					element.x2 = element.x1 + element.width;
					element.y2 = element.y1 + element.height;
					
					element.subtree_x1 = element.x1;
					element.subtree_y1 = element.y1;
					element.subtree_x2 = element.x2;
					element.subtree_y2 = element.y2;
					
					if(element.x1 < parent.subtree_x1)
						parent.subtree_x1 = element.x1;
					if(element.y1 < parent.subtree_y1)
						parent.subtree_y1 = element.y1;
					if(element.x2 > parent.subtree_x2)
						parent.subtree_x2 = element.x2;
					if(element.y2 > parent.subtree_y2)
						parent.subtree_y2 = element.y2;
				}
				else
				{
					element.x1 = element.x;
					element.y1 = element.y;
					element.x2 = element.x1 + element.width;
					element.y2 = element.y1 + element.height;
				}
				
				ctx.subtree_x1 = element.x1;
				ctx.subtree_y1 = element.y1;
				ctx.subtree_x2 = element.x2;
				ctx.subtree_y2 = element.y2;
				
				if(
					ctx.mouse_active && (
						ctx.clipping_mode == ClippingMode::None ||
						mouse_x >= ctx.x1 && mouse_x <= ctx.x2 &&
						mouse_y >= ctx.y1 && mouse_y <= ctx.y2 && (
							ctx.clipping_mode == ClippingMode::Outside ||
							element.x1 >= ctx.x1 && element.x2 <= ctx.x2 &&
							element.y1 >= ctx.y1 && element.y2 <= ctx.y2
						)
					)
				)
				{
					if(element.overlaps_point(mouse_x, mouse_y))
					{
						@mouse_over = element;
					}
				}
				
				if(element.disabled || !element.children_mouse_enabled)
				{
					ctx.mouse_active = false;
				}
			}
			
			while(ctx.num_children-- == 0)
			{
				if(layout_context_pool_index == layout_context_pool_size)
					layout_context_pool.resize(layout_context_pool_size += 16);
				
				LayoutContext@ parent = @ctx.parent;
				
				if(@parent != null)
				{
					if(ctx.subtree_x1 < parent.subtree_x1)
						parent.root.subtree_x1 = parent.subtree_x1 = ctx.subtree_x1;
					if(ctx.subtree_y1 < parent.subtree_y1)
						parent.root.subtree_y1 = parent.subtree_y1 = ctx.subtree_y1;
					if(ctx.subtree_x2 > parent.subtree_x2)
						parent.root.subtree_x2 = parent.subtree_x2 = ctx.subtree_x2;
					if(ctx.subtree_y2 > parent.subtree_y2)
						parent.root.subtree_y2 = parent.subtree_y2 = ctx.subtree_y2;
				}
				
				@layout_context_pool[layout_context_pool_index++] = @ctx;
				@ctx = @parent;
			}
		}
		
		if(@mouse_over == @base)
		{
			@mouse_over = null;
		}
		
		while(@ctx != null && @ctx.parent != null)
		{
			if(layout_context_pool_index == layout_context_pool_size)
				layout_context_pool.resize(layout_context_pool_size += 16);
			
			@layout_context_pool[layout_context_pool_index++] = @ctx;
			@ctx = @ctx.parent;
		}
		
		this.layout_context_pool_size = layout_context_pool_size;
		this.layout_context_pool_index = layout_context_pool_index;
		
		return mouse_over;
	}
	
	private void process_mouse_events(Element@ mouse_over_root)
	{
		is_mouse_over = @_mouse_over_element != null;
		
		// /////////////////////////////////////////////////
		// Build the list/stack of the elements the mouse is over, from the innermost child to the outermost parent
		// 
		
		elements_mouse_enter.resize(0);
		
		if(is_mouse_over)
		{
			Element@ mouse_over_traversal = _mouse_over_element;
			
			do
			{
				if(!mouse_over_traversal.disabled)
				{
					elements_mouse_enter.insertLast(mouse_over_traversal);
				}
				
				@mouse_over_traversal = mouse_over_traversal.parent;
			}
			while(@mouse_over_traversal != @mouse_over_root);
		}
		
		// /////////////////////////////////////////////////
		// Mouse exit
		// 
		
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
				element._mouse_exit();
				element.mouse_exit.dispatch(_event_info);
			}
			
			break;
		}
		
		// /////////////////////////////////////////////////
		// Mouse enter
		// 
		
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
				element._mouse_enter();
				element.mouse_enter.dispatch(_event_info);
			}
		}
		
		// /////////////////////////////////////////////////
		// Mouse press
		// 
		
		if(is_mouse_over && mouse.left_press || mouse.middle_press || mouse.right_press)
		{
			_event_info.reset(EventType::MOUSE_PRESS, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @_event_info.target = @elements_mouse_enter[i];
				
				if(mouse.left_press)
				{
					elements_left_pressed[element._id] = true;
					element._mouse_press(_event_info.button = MouseButton::Left);
					element.mouse_press.dispatch(_event_info);
				}
				
				if(mouse.right_press)
				{
					elements_right_pressed[element._id] = true;
					element._mouse_press(_event_info.button = MouseButton::Right);
					element.mouse_press.dispatch(_event_info);
				}
				
				if(mouse.middle_press)
				{
					elements_middle_pressed[element._id] = true;
					element._mouse_press(_event_info.button = MouseButton::Middle);
					element.mouse_press.dispatch(_event_info);
				}
				
				if(mouse.primary_press)
				{
					element.pressed = true;
					elements_pressed_list.insertLast(element);
					
					// Tooltip
					if(@element.tooltip != null && element.tooltip.trigger_type == TooltipTriggerType::MouseDown)
					{
						show_tooltip(_mouse_over_element);
					}
				}
			}
		}
		
		// /////////////////////////////////////////////////
		// Mouse move
		// 
		
		if(is_mouse_over && mouse.moved)
		{
			_event_info.reset(EventType::MOUSE_MOVE, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @_event_info.target = @elements_mouse_enter[i];
				element._mouse_move();
				element.mouse_move.dispatch(_event_info);
			}
		}
		
		// /////////////////////////////////////////////////
		// Mouse release and click
		// 
		
		if(is_mouse_over && mouse.left_release || mouse.middle_release || mouse.right_release)
		{
			// Release
			
			_event_info.reset(EventType::MOUSE_RELEASE, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @_event_info.target = @elements_mouse_enter[i];
				
				if(mouse.left_release)
				{
					element._mouse_release(_event_info.button = MouseButton::Left);
					element.mouse_release.dispatch(_event_info);
				}
				
				if(mouse.right_release)
				{
					element._mouse_release(_event_info.button = MouseButton::Right);
					element.mouse_release.dispatch(_event_info);
				}
				
				if(mouse.middle_release)
				{
					element._mouse_release(_event_info.button = MouseButton::Middle);
					element.mouse_release.dispatch(_event_info);
				}
			}
			
			// Click
			
			_event_info.reset(EventType::MOUSE_CLICK, MouseButton::None, mouse.x, mouse.y);
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @_event_info.target = @elements_mouse_enter[i];
				bool primary_clicked = false;
				
				if(mouse.left_release && elements_left_pressed.exists(element._id))
				{
					_event_info.button = MouseButton::Left;
					primary_clicked = primary_clicked || primary_button == _event_info.button;
					
					if(primary_clicked)
					{
						element._mouse_click();
						element.mouse_click.dispatch(_event_info);
					}
					
					element._mouse_button_click(MouseButton::Left);
					element.mouse_button_click.dispatch(_event_info);
				}
				
				if(mouse.right_release && elements_right_pressed.exists(element._id))
				{
					_event_info.button = MouseButton::Right;
					primary_clicked = primary_clicked || primary_button == _event_info.button;
					
					if(primary_clicked)
					{
						element._mouse_click();
						element.mouse_click.dispatch(_event_info);
					}
					
					element._mouse_button_click( MouseButton::Right);
					element.mouse_button_click.dispatch(_event_info);
				}
				
				if(mouse.middle_release && elements_middle_pressed.exists(element._id))
				{
					_event_info.button = MouseButton::Middle;
					primary_clicked = primary_clicked || primary_button == _event_info.button;
					
					if(primary_clicked)
					{
						element._mouse_click();
						element.mouse_click.dispatch(_event_info);
					}
					
					element._mouse_button_click(MouseButton::Middle);
					element.mouse_button_click.dispatch(_event_info);
				}
				
				// Tooltip
				if(primary_clicked)
				{
					if(@element.tooltip != null && element.tooltip.trigger_type == TooltipTriggerType::MouseClick)
					{
						show_tooltip(_mouse_over_element);
					}
				}
			}
		}
		
		// /////////////////////////////////////////////////
		// Finalise
		// 
		
		// Set mouse_over erray
		
		elements_mouse_over.resize(elements_mouse_enter.size());
		
		for(int i = int(elements_mouse_enter.size()) - 1, j = 0; i >= 0; i--, j++)
		{
			@elements_mouse_over[j] = @elements_mouse_enter[i];
		}
		
		// Clear pressed elements
		
		if(mouse.left_release)
		{
			elements_left_pressed.deleteAll();
			
			if(primary_button == MouseButton::Left)
				clear_press();
		}
		
		if(mouse.right_release)
		{
			elements_right_pressed.deleteAll();
			
			if(primary_button == MouseButton::Right)
				clear_press();
		}
		
		if(mouse.middle_release)
		{
			elements_middle_pressed.deleteAll();
			
			if(primary_button == MouseButton::Middle)
				clear_press();
		}
		
		// Hover tooltip
		
		if(
			@_mouse_over_element != null && !_mouse_over_element.disabled &&
			@_mouse_over_element.tooltip != null &&
			_mouse_over_element.tooltip.trigger_type == TooltipTriggerType::MouseOver)
		{
			show_tooltip(_mouse_over_element);
		}
	}
	
	private void clear_press()
	{
		for(int i = int(elements_pressed_list.length()) - 1; i >= 0; i--)
		{
			elements_pressed_list[i].pressed = false;
		}
		
		elements_pressed_list.resize(0);
	}
	
	private void draw_root(Element@ root)
	{
		Style@ style = @this.style;
		DrawingContext@ ctx = style.reset_drawing_context(root);
		
		const float disabled_alpha = style.disabled_alpha;
		
		ElementStack@ element_stack = @this.element_stack;
		element_stack.clear();
		
		element_stack.push(root);
		int stack_size = 1;
		
		do
		{
			Element@ element = element_stack.pop();
			stack_size--;
			
			if(
				element.visible && (
					ctx.clipping_mode == ClippingMode::None ||
					(
						ctx.clipping_mode == ClippingMode::Outside &&
						element.subtree_x1 <= ctx.x2 && element.subtree_x2 >= ctx.x1 &&
						element.subtree_y1 <= ctx.y2 && element.subtree_y2 >= ctx.y1) ||
					(
						ctx.clipping_mode == ClippingMode::Inside &&
						element.subtree_x1 >= ctx.x1 && element.subtree_x2 <= ctx.x2 &&
						element.subtree_y1 >= ctx.y1 && element.subtree_y2 <= ctx.y2)
				)
			)
			{
				element._queue_children_for_layout(@element_stack);
				int num_children = element_stack.size - stack_size;
				stack_size += num_children;
				
				@ctx = style.push_drawing_context(element, num_children);
				
				if(element.alpha != 1)
					ctx.alpha *= element.alpha;
				
				element._draw(@style, @ctx);
				
				if(element.clip_contents != ClippingMode::None)
				{
					ctx.clipping_mode = element.clip_contents;
					ctx.x1 = element.x1;
					ctx.y1 = element.y1;
					ctx.x2 = element.x2;
					ctx.y2 = element.y2;
				}
				
				if(element.disabled)
					ctx.alpha *= disabled_alpha;
			}
			
			if(stack_size > 0)
			{
				while(ctx.num_children-- == 0)
				{
					@ctx = style.pop_drawing_context();
				}
			}
		}
		while(stack_size > 0);
		
		while(@ctx != null && @ctx.parent != null)
		{
			@ctx = style.pop_drawing_context();
		}
	}
	
	private void debug_draw_root(Element@ root, const float id_scale=0.4)
	{
		Style@ style = @this.style;
		DrawingContext@ ctx = style.reset_drawing_context(root);
		
		ElementStack@ element_stack = @this.element_stack;
		element_stack.clear();
		
		element_stack.push(root);
		int stack_size = 1;
		
		debug_text_field.align_horizontal(-1);
		debug_text_field.align_vertical(-1);
		uint clr;
		
		do
		{
			Element@ element = element_stack.pop();
			stack_size--;
			
			if(element.visible)
			{
				const bool not_clipped = ctx.clipping_mode == ClippingMode::None ||
					(
						ctx.clipping_mode == ClippingMode::Outside &&
						element.subtree_x1 <= ctx.x2 && element.subtree_x2 >= ctx.x1 &&
						element.subtree_y1 <= ctx.y2 && element.subtree_y2 >= ctx.y1) ||
					(
						ctx.clipping_mode == ClippingMode::Inside &&
						element.subtree_x1 >= ctx.x1 && element.subtree_x2 <= ctx.x2 &&
						element.subtree_y1 >= ctx.y1 && element.subtree_y2 <= ctx.y2);
				
				element._queue_children_for_layout(@element_stack);
				int num_children = element_stack.size - stack_size;
				stack_size += num_children;
				
				@ctx = style.push_drawing_context(element, num_children);
				
				clr = get_element_id_colour(element, 0x00);
				const uint el_alpha = not_clipped ? 0x55000000 : 0x22000000;
				
				// DRAW STUFF HERE
				if(@element.parent != null)
				{
					if(element.x1 != element.subtree_x1 || element.x2 != element.subtree_x2 || element.y1 != element.subtree_y1 || element.y2 != element.subtree_y2)
					{
						style.outline_dotted(
							element.subtree_x1, element.subtree_y1, element.subtree_x2, element.subtree_y2,
							@element == @mouse_over_element ? -2 : 1,
							clr | el_alpha);
					}
					
					if(@element != @mouse_over_element)
					{
						if(not_clipped)
						{
							style.outline(
								element.x1, element.y1, element.x2, element.y2,
								1, (element.hovered ? 0xff0000 : clr) | el_alpha);
						}
						else
						{
							style.outline_dotted(element.x1, element.y1, element.x2, element.y2, 1, clr | el_alpha);
						}
						
						clr = scale_lightness(clr, 0.1) | el_alpha;
						
						style.draw_text(
							element._id,
							element.x1, element.y1,
							clr | (uint((el_alpha>>24) * 1.75)<<24),
							id_scale, id_scale);
					}
				}
				
				if(element.clip_contents != ClippingMode::None)
				{
					ctx.clipping_mode = element.clip_contents;
					ctx.x1 = element.x1;
					ctx.y1 = element.y1;
					ctx.x2 = element.x2;
					ctx.y2 = element.y2;
				}
			}
			
			if(stack_size > 0)
			{
				while(ctx.num_children-- == 0)
				{
					@ctx = style.pop_drawing_context();
				}
			}
		}
		while(stack_size > 0);
		
		while(@ctx != null && @ctx.parent != null)
		{
			@ctx = style.pop_drawing_context();
		}
		
		if(@mouse_over_element != null)
		{
			clr = get_element_id_colour(mouse_over_element, 0x99);
			
			style.outline(
				mouse_over_element.x1, mouse_over_element.y1, mouse_over_element.x2, mouse_over_element.y2,
				-2, clr);
			
			style.outline_text(
				mouse_over_element._id,
				mouse_over_element.x1, mouse_over_element.y1,
				0xffffffff, 0xff000000, 2 * id_scale,
				id_scale, id_scale);
			
			// Debug print mouse stack
			if(@debug != null)
			{
				const int num_elements_mouse_over = int(elements_mouse_over.size());
				
				for(int i = num_elements_mouse_over - 1; i >= 0; i--)
				{
					Element@ element = @elements_mouse_over[i];
					debug.print(string::repeat('- ', i) + element._id, set_alpha(get_element_id_colour(element), 1), element._id, 1);
				}
			}
		}
	}
	
	private void show_tooltip(const string id, TooltipOptions@ options, Element@ element, bool wait_for_mouse)
	{
		if(!options.enabled)
			return;
		
		if(!tooltips.exists(id))
		{
			Tooltip@ tooltip = Tooltip(this, options, element, wait_for_mouse);
			tooltip.hide.on(on_tooltip_hide_delegate);
			overlays.add_child(tooltip);
			@tooltips[id] = tooltip;
		}
		else
		{
			overlays.move_to_front(cast<Tooltip@>(tooltips[id]));
		}
	}
	
	private void hide_tooltip(const string id)
	{
		if(!tooltips.exists(id))
			return;
		
		Tooltip@ tooltip = cast<Tooltip@>(tooltips[id]);
		tooltip.force_hide();
	}
	
	private void update_tooltip(const string id, TooltipOptions@ options)
	{
		if(!tooltips.exists(id))
			return;
		
		Tooltip@ tooltip = cast<Tooltip@>(tooltips[id]);
		@tooltip.content = @options.get_contenet_element();
		tooltip.fit_to_contents();
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
	
	// Events
	// ---------------------------------------------------------
	
	private void on_tooltip_hide(EventInfo@ event)
	{
		Tooltip@ tooltip = cast<Tooltip@>(event.target);
		
		if(@tooltip == null)
			return;
		
		const string id = @tooltip.target != null ? tooltip.target._id : tooltip.options._id;
		
		if(!tooltips.exists(id))
			return;
		
		tooltip.hide.off(on_tooltip_hide_delegate);
		tooltips.delete(id);
		overlays.remove_child(tooltip);
	}
	
}