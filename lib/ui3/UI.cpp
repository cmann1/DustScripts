#include '../std.cpp';
#include '../string.cpp';
#include '../fonts.cpp';
#include '../enums/GlobalVirtualButton.cpp';
#include '../math/math.cpp';
#include '../utils/colour.cpp';
#include 'UIMouse.cpp';
#include 'Style.cpp';
#include 'utils/ClippingMode.cpp';
#include 'utils/ElementStack.cpp';
#include 'utils/LayoutContext.cpp';
#include 'utils/DrawingContext.cpp';
#include 'utils/pools/EventInfoPool.cpp';
#include 'utils/pools/ImagePool.cpp';
#include 'utils/pools/LabelPool.cpp';
#include 'events/Event.cpp';
#include 'elements/Element.cpp';
#include 'elements/Container.cpp';
#include 'elements/Graphic.cpp';
#include 'elements/Popup.cpp';
#include 'layouts/flow/FlowLayout.cpp';

class UI
{
	
	int NEXT_ID;
	int CUSTOM_TOOLTIP_ID;
	
	/// Which mouse button is primarily used to interact with UI elements.
	/// (Left might be more problematic since it will also interact with the editor ui.)
	/// Obsolete since the introduction of the editor api and the ability to consume mouse events
	MouseButton primary_button = MouseButton::Left;
	MouseButton secondary_button = MouseButton::Right;
	
	Style@ style;
	UIMouse@ mouse;
	bool block_editor_input = true;
	
	/// Only relevant when hud = true. When true, certain drawing operations
	/// will attempt to snap to whole pixels to give cleaner lines
	bool pixel_perfect = true;
	float screen_width;
	float screen_height;
	bool _even_screen_width;
	bool _even_screen_height;
	/// Mostly for testing. Adds padding around the screen when auto_fit_screen is true
	float auto_fit_padding_x;
	float auto_fit_padding_y;
	
	Event screen_resize;
	
	/// Debug only
	bool _disable_clipping = false;
	
	// Uncomment and manually set during testing
	Debug@ debug;
	
	private scene@ g;
	
	private bool _hud;
	private int _layer;
	private int _sub_layer;
	private int _player;
	
	/// Only relevant when hud = true. Will automatically set the hud's region to match the screen
	private bool _auto_fit_screen = true;
	
	private Container@ contents;
	private Container@ overlays;
	/// e.g. a drop down box that is open. There can only by one active element in a UI.
	private Element@ active_element;
	/// Prevents all elements from interacting with the mouse. Must be set every frame by the active element
	/*private*/ Element@ _active_mouse_element;
	private bool active_mouse_element_processed;
	
	private bool is_mouse_over;
	
	/// Used for processing element layouts
	private ElementStack element_stack;
	private int layout_context_pool_size = 16;
	private int layout_context_pool_index = 0;
	private array<LayoutContext@> layout_context_pool(layout_context_pool_size);
	/// The top most element the mouse is over
	private Element@ _mouse_over_element;
	/// Elements entered on this frame
	private array<Element@> elements_mouse_enter();
	/// The hierarchy of elements the mouse is over, from the outermost to the inner
	private array<Element@> elements_mouse_over();
	
	private Element@ debug_mouse_over_element;
	private bool debug_draw_active;
	private PopupOptions@ debug_tooltip_options;
	private DrawingContext debug_mouse_over_clipping_ctx;
	
	private Element@ mouse_over_overlays;
	
	private dictionary elements_left_pressed();
	private dictionary elements_right_pressed();
	private dictionary elements_middle_pressed();
	private array<Element@> elements_pressed_list();
	
	private int num_queued_events;
	private int queued_events_size = 16;
	private array<Event@> queued_events(queued_events_size);
	private array<EventInfo@> queued_event_infos(queued_events_size);
	
	private dictionary tooltips;
	
	private textfield@ debug_text_field;
	
	private float x1 = 0;
	private float y1 = 0;
	private float x2 = 100;
	private float y2 = 100;
	
	private EventCallback@ on_tooltip_hide_delegate;
	
	// ///////////////////////////////////////////////////////////////
	// Common reusable things
	
	/*private*/ EventInfoPool _event_info_pool;
	/*private*/ LabelPool _label_pool;
	/*private*/ ImagePool _image_pool;
	
	/*private*/ EventInfo@ _event_info = EventInfo();
	/*private*/ FlowLayout@ _toolbar_flow_layout;
	
	/*private*/ array<float> _float_array_x(16);
	/*private*/ array<float> _float_array_y(16);
	/*private*/ array<int> _int_array(16);
	/*private*/ array<Element@> _element_array(16);
	
	editor_api@ _editor;
	bool _has_editor;
	
	UI(bool hud=true, int layer=20, int sub_layer=10, int player=0)
	{
		@on_tooltip_hide_delegate = EventCallback(this.on_tooltip_hide);
		
		@g = get_scene();
		@mouse = UIMouse(hud, layer, player);
		
		@contents = Container(this);
		@overlays = Container(this);
		contents.name = '_ROOT_';
		overlays.name = '_OVERLAYS_';
		
		if(hud)
		{
			x1 = SCREEN_LEFT;
			y1 = SCREEN_TOP;
			x2 = SCREEN_RIGHT;
			y2 = SCREEN_BOTTOM;
		}
		
		@_event_info.mouse = mouse;
		
		@style = Style(hud);
		
		@debug_text_field = create_textfield();
		debug_text_field.set_font(font::PROXIMANOVA_REG, 26);
		
		set_region(x1, y1, x2, y2);
		
		@_toolbar_flow_layout = FlowLayout(this, FlowDirection::Row, FlowAlign::Start, FlowAlign::Centre, FlowWrap::Wrap);
		
		@_editor = get_editor_api();
		_has_editor = @_editor != null;
		
		this.hud = hud;
		this.layer = layer;
		this.sub_layer = sub_layer;
		_player = player;
		
		screen_width  = g.hud_screen_width(false);
		screen_height = g.hud_screen_height(false);
		
		if(_hud && _auto_fit_screen)
		{
			fit_to_screen();
		}
	}
	
	/// The top most element the mouse is over
	Element@ mouse_over_element { get { return @_mouse_over_element; } }
	
	/// The top most element the mouse is over
	bool is_mouse_over_ui { get { return @_mouse_over_element != null; } }
	
	/// Returns mouse x relative to the UI
	float mouse_x { get { return mouse.x - contents.x1; } }
	
	/// Returns mouse y relative to the UI
	float mouse_y { get { return mouse.y - contents.y1; } }
	
	bool hud
	{
		get { return _hud; }
		set
		{
			if(_hud == value)
				return;
			
			style.hud = mouse.hud = _hud = value;
			
			if(_hud && _auto_fit_screen)
			{
				fit_to_screen(true);
			}
		}
	}
	
	uint layer
	{
		get { return _layer; }
		set { style.layer = mouse.layer = _layer = value; }
	}
	
	uint sub_layer
	{
		get { return _sub_layer; }
		set { style.sub_layer = _sub_layer = value; }
	}
	
	/// Only relevant when hud = true. Will automatically set the hud's region to match the screen
	bool auto_fit_screen
	{
		get const { return _auto_fit_screen; }
		set
		{
			if(_auto_fit_screen == value)
				return;
			
			_auto_fit_screen = value;
			
			if(_hud && _auto_fit_screen)
			{
				fit_to_screen(true);
			}
		}
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
		if(_hud)
		{
			const float new_screen_width  = g.hud_screen_width(false);
			const float new_screen_height = g.hud_screen_height(false);
			
			if(!approximately(screen_width, new_screen_width) || !approximately(screen_height, new_screen_height))
			{
				screen_width  = new_screen_width;
				screen_height = new_screen_height;
				
				if(_auto_fit_screen)
				{
					fit_to_screen();
				}
				
				_event_info.reset(EventType::SCREEN_RESIZE);
				screen_resize.dispatch(_event_info);
			}
		}
		
		// Don't clear on the firt frame so that pressed elements will still update once
		if(@_active_mouse_element != null && mouse.primary_down)
		{
			if(active_mouse_element_processed)
				@_mouse_over_element = null;
			else
				active_mouse_element_processed = true;
		}
		else
		{
			active_mouse_element_processed = false;
		}
		
		process_mouse_events(@_mouse_over_element == @mouse_over_overlays ? overlays : contents);
		
		if(num_queued_events > 0)
		{
			process_queued_events();
		}
		
		mouse.step(_has_editor && _editor.key_check_gvb(GlobalVirtualButton::Space));
		
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
		
		switch(secondary_button)
		{
			case MouseButton::Left:
				mouse.secondary_down = mouse.left_down;
				mouse.secondary_press = mouse.left_press;
				mouse.secondary_release = mouse.left_release;
				break;
			case MouseButton::Middle:
				mouse.secondary_down = mouse.middle_down;
				mouse.secondary_press = mouse.middle_press;
				mouse.secondary_release = mouse.middle_release;
				break;
			case MouseButton::Right:
				mouse.secondary_down = mouse.right_down;
				mouse.secondary_press = mouse.right_press;
				mouse.secondary_release = mouse.right_release;
				break;
		}
		
		/*
		 * Update layout
		 */
		
		@_active_mouse_element = null;
		// Don't clear _mouse_over_element
		// @_mouse_over_element = null;
		
		if(debug_draw_active)
		{
			@debug_mouse_over_element = null;
		}
		
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
		
		debug_draw_active = false;
		
		if(block_editor_input && _hud && _has_editor && @_mouse_over_element != null)
		{
			_editor.key_clear_gvb(GlobalVirtualButton::LeftClick);
			_editor.key_clear_gvb(GlobalVirtualButton::RightClick);
			_editor.key_clear_gvb(GlobalVirtualButton::MiddleClick);
			_editor.key_clear_gvb(GlobalVirtualButton::WheelDown);
			_editor.key_clear_gvb(GlobalVirtualButton::WheelUp);
			_editor.key_clear_gvb(GlobalVirtualButton::Space);
		}
	}
	
	void draw()
	{
		draw_root(contents);
		draw_root(overlays);
	}
	
	/// Make sure to setup a Debug instance before calling this
	void debug_draw(bool just_outline=false, bool show_ids=false, bool show_element_data=true, const float id_scale=0.4)
	{
		style.reset_drawing_context(null);
		style.outline(contents.x1, contents.y1, contents.x2, contents.y2, -2, 0xaaffffff);
		
		if(just_outline)
			return;
		
		debug_mouse_over_clipping_ctx.clipping_mode = ClippingMode::None;
		
		debug_draw_root(contents, show_ids, id_scale);
		debug_draw_root(overlays, show_ids, id_scale);
		
		if(show_element_data)
		{
			debug_print_mouse_stack();
			debug_draw_element_data();
		}
		
		debug_draw_active = true;
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
	
	void fit_to_screen(bool update_screen_values=false)
	{
		if(!_hud)
			return;
		
		const float new_screen_width  = g.hud_screen_width(false);
		const float new_screen_height = g.hud_screen_height(false);
		
		set_region(
			-new_screen_width * 0.5 + auto_fit_padding_x, -new_screen_height * 0.5 + auto_fit_padding_y,
			 new_screen_width * 0.5 - auto_fit_padding_x,  new_screen_height * 0.5 - auto_fit_padding_y);
		 
		 if(update_screen_values)
		 {
			screen_width  = new_screen_width;
			screen_height = new_screen_height;
			_even_screen_width = (screen_width % 2) == 0;
			_even_screen_height = (screen_height % 2) == 0;
		 }
	}
	
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
	void show_tooltip(PopupOptions@ options, Element@ target = null, bool wait_for_mouse=false)
	{
		if(@options == null)
			return;
		
		if(options._id == '')
			options._id = '_tt_target' + (CUSTOM_TOOLTIP_ID++);
		
		show_tooltip(options._id, options, target, wait_for_mouse);
	}
	
	void hide_tooltip(PopupOptions@ options)
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
	
	void update_tooltip(PopupOptions@ options)
	{
		if(@options == null)
			return;
		
		update_tooltip(options._id, options);
	}
	
	uint get_element_id_colour(Element@ element, const uint alpha=0xff)
	{
		const float hash = float(string::hash(element._id));
		return hsl_to_rgb(
			sin(hash) * 0.5 + 0.5,
			map(sin(hash) * 0.5 + 0.5, 0, 1, 0.8, 0.9),
			map(sin(hash) * 0.5 + 0.5, 0, 1, 0.65, 0.75)
		) | (alpha << 24);
	}
	
	uint get_element_id_colour(const string id, const uint alpha=0xff)
	{
		const float hash = float(string::hash(id));
		return hsl_to_rgb(
			sin(hash) * 0.5 + 0.5,
			map(sin(hash) * 0.5 + 0.5, 0, 1, 0.8, 0.9),
			map(sin(hash) * 0.5 + 0.5, 0, 1, 0.65, 0.75)
		) | (alpha << 24);
	}
	
	bool contains(Element@ element)
	{
		return contents.contains(element);
	}
	
	bool contains_overlay(Element@ element)
	{
		return overlays.contains(element);
	}
	
	/* internal */ float pixel_floor(const float value) { return _hud && pixel_perfect ? floor(value) : value; }
	/* internal */ float pixel_round(const float value) { return _hud && pixel_perfect ? round(value) : value; }
	/* internal */ float pixel_ceil(const float value) { return _hud && pixel_perfect ? ceil(value) : value; }
	
	// Private
	// ---------------------------------------------------------
	
	private Element@ update_layout(Element@ base, bool check_mouse_over)
	{
		const bool debug_draw_active = this.debug_draw_active;
		Element@ mouse_over = null;
		Element@ debug_mouse_over = null;
		
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
		ctx.x1 = base._x;
		ctx.y1 = base._y;
		ctx.x2 = base.x + base._width;
		ctx.y2 = base.y + base._height;
		ctx.subtree_x1 = ctx.x1;
		ctx.subtree_y1 = ctx.y1;
		ctx.subtree_x2 = ctx.x2;
		ctx.subtree_y2 = ctx.y2;
		base.subtree_x1 = ctx.x1;
		base.subtree_y1 = ctx.y1;
		base.subtree_x2 = ctx.x2;
		base.subtree_y2 = ctx.y2;
		ctx.scroll_x = base._scroll_x;
		ctx.scroll_y = base._scroll_y;
		
		base._do_layout(ctx);
		base.update_world_bounds();
		
		int stack_size = element_stack.size;
		const float mouse_x = mouse.x;
		const float mouse_y = mouse.y;
		int element_index = 0;
		int mouse_over_index = -1;
		int debug_mouse_over_index = -1;
		
		while(stack_size > 0)
		{
			Element@ element = element_stack.pop();
			stack_size--;
			element_index++;
			
			if(element.visible)
			{
				element._queue_children_for_layout(@element_stack);
				int num_children = element_stack.size - stack_size;
				stack_size += num_children;
				
				// Push a new context
				//{
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
					new_ctx.scroll_x = ctx.scroll_x;
					new_ctx.scroll_y = ctx.scroll_y;
					
					@ctx = @new_ctx;
				//}
				
				Element@ parent = element.parent;
				element._do_layout(ctx);
				
				if(@parent != null)
				{
					element.x1 = ctx.scroll_x + parent.x1 + element.x;
					element.y1 = ctx.scroll_y + parent.y1 + element.y;
					element.x2 = element.x1 + element._width;
					element.y2 = element.y1 + element._height;
					
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
					element.x1 = ctx.scroll_x + element.x;
					element.y1 = ctx.scroll_y + element.y;
					element.x2 = element.x1 + element._width;
					element.y2 = element.y1 + element._height;
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
						@mouse_over = @element;
						mouse_over_index = element_index;
					}
				}
				else if(debug_draw_active && element.overlaps_point(mouse_x, mouse_y))
				{
					@debug_mouse_over = @element;
					debug_mouse_over_index = element_index;
				}
				
				if(element.clip_contents != ClippingMode::None)
				{
					ctx.clipping_mode = element.clip_contents;
					ctx.x1 = max(ctx.x1, element.x1);
					ctx.y1 = max(ctx.y1, element.y1);
					ctx.x2 = min(ctx.x2, element.x2);
					ctx.y2 = min(ctx.y2, element.y2);
				}
				
				if(element.disabled || !element.children_mouse_enabled)
				{
					ctx.mouse_active = false;
				}
				
				if(@ctx.root == @element)
				{
					ctx.scroll_x = element._scroll_x;
					ctx.scroll_y = element._scroll_y;
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
			@mouse_over = null;
		
		if(debug_draw_active)
		{
			if(@mouse_over != null && mouse_over_index >= debug_mouse_over_index)
				@debug_mouse_over = @mouse_over;
			
			if(@debug_mouse_over != null)
				@debug_mouse_over_element = @debug_mouse_over;
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
				
				if(
					!element.disabled && @element.tooltip != null && element.tooltip.trigger_when_hovered &&
					element.tooltip.trigger_type == PopupTriggerType::MouseOver)
				{
					show_tooltip(element);
				}
				
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
					if(@element.tooltip != null && element.tooltip.trigger_type == PopupTriggerType::MouseDown && @element == @_mouse_over_element)
					{
						show_tooltip(element);
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
					if(@element.tooltip != null && element.tooltip.trigger_type == PopupTriggerType::MouseClick && @element == @_mouse_over_element)
					{
						show_tooltip(element);
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
			_mouse_over_element.tooltip.trigger_type == PopupTriggerType::MouseOver)
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
	
	private void process_queued_events()
	{
		for(int i = 0; i < num_queued_events; i++)
		{
			EventInfo@ event_info = @queued_event_infos[i];
			queued_events[i].dispatch(event_info);
			_event_info_pool.release(@event_info);
		}
		
		num_queued_events = 0;
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
						element.subtree_y1 >= ctx.y1 && element.subtree_y2 <= ctx.y2) ||
					_disable_clipping
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
					ctx.x1 = max(ctx.x1, element.x1);
					ctx.y1 = max(ctx.y1, element.y1);
					ctx.x2 = min(ctx.x2, element.x2);
					ctx.y2 = min(ctx.y2, element.y2);
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
	
	private void debug_draw_root(Element@ root, bool show_ids, const float id_scale=0.4)
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
		
		bool mouse_over_clipped = false;
		float mouse_over_clipping_x1;
		float mouse_over_clipping_y1;
		float mouse_over_clipping_x2;
		float mouse_over_clipping_y2;
		
		Element@ mouse_over_element = @debug_mouse_over_element != null ? @debug_mouse_over_element : @_mouse_over_element;
		
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
				
				// Instead only remeber clipping roots
				DrawingContext@ prev_ctx = @ctx;
				@ctx = style.push_drawing_context(element, num_children);
				@ctx.root = @prev_ctx.root;
				
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
						
						if(show_ids)
						{
							style.draw_text(
								element.name != '' ? element.name : element.id,
								element.x1, element.y1,
								clr | (uint((el_alpha>>24) * 1.75)<<24),
								id_scale, id_scale);
						}
					}
					else
					{
						@debug_mouse_over_clipping_ctx.root = @ctx.root;
						debug_mouse_over_clipping_ctx.clipping_mode = ctx.clipping_mode;
						debug_mouse_over_clipping_ctx.x1 = ctx.x1;
						debug_mouse_over_clipping_ctx.y1 = ctx.y1;
						debug_mouse_over_clipping_ctx.x2 = ctx.x2;
						debug_mouse_over_clipping_ctx.y2 = ctx.y2;
					}
				}
				
				if(element.clip_contents != ClippingMode::None)
				{
					@ctx.root = @element;
					ctx.clipping_mode = element.clip_contents;
					ctx.x1 = max(ctx.x1, element.x1);
					ctx.y1 = max(ctx.y1, element.y1);
					ctx.x2 = min(ctx.x2, element.x2);
					ctx.y2 = min(ctx.y2, element.y2);
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
				mouse_over_element.name != '' ? mouse_over_element.name : mouse_over_element.id,
				mouse_over_element.x1, mouse_over_element.y1,
				0xffffffff, 0xff000000, 2 * id_scale,
				id_scale, id_scale);
			
			if(debug_mouse_over_clipping_ctx.clipping_mode != ClippingMode::None)
			{
				style.outline_dotted(
					debug_mouse_over_clipping_ctx.x1, debug_mouse_over_clipping_ctx.y1,
					debug_mouse_over_clipping_ctx.x2, debug_mouse_over_clipping_ctx.y2,
					-2, 0x99ffffff);
			}
		}
	}
	
	private void debug_draw_element_data()
	{
		if(@debug == null)
			return;
		
		Element@ debug_el = @debug_mouse_over_element != null ? @debug_mouse_over_element : @_mouse_over_element;
		
		if(@debug_el == null && contents.overlaps_point(mouse.x, mouse.y))
			@debug_el = @contents;
		
		if(@debug_el == null)
			return;
		
		const uint id_clr = get_element_id_colour(debug_el, 0xff);
		const uint txt_clr = 0xffffffff; // scale_lightness(id_clr, 0.25);
		const string print_id = debug_el._id + '_db_';
		const string indent = '  ';
		int id = 0;
		
		Graphic@ gr = cast<Graphic@>(debug_el);
		
		if(@gr != null)
		{
			debug.print(indent + '  draw_scale: ' + gr.debug_draw_scale_x + ', ' + gr.debug_draw_scale_y, txt_clr, print_id + id++, 0);
			debug.print(indent + '  draw_pos:   ' + gr.debug_draw_x + ', ' + gr.debug_draw_y, txt_clr, print_id + id++, 0);
			if(gr.padding != 0)
				debug.print(indent + '  padding:    ' + gr.padding, txt_clr, print_id + id++, 0);
			if(gr.graphic_offset_x != 0 || gr.graphic_offset_y != 0)
				debug.print(indent + '  offset:     ' + gr.graphic_offset_x + ', ' + gr.graphic_offset_y, txt_clr, print_id + id++, 0);
			if(gr.origin_x != 0 || gr.origin_y != 0)
				debug.print(indent + '  origin:     ' + gr.origin_x + ', ' + gr.origin_y, txt_clr, print_id + id++, 0);
			debug.print(indent + '  align:      ' + gr.align_h + ', ' + gr.align_v, txt_clr, print_id + id++, 0);
			debug.print(indent + '  scale:      ' + gr.scale_x + ', ' + gr.scale_y, txt_clr, print_id + id++, 0);
			debug.print(indent + '  size:       ' + gr.graphic_width + ' x ' + gr.graphic_height, txt_clr, print_id + id++, 0);
			debug.print(indent + '[graphic]', txt_clr, print_id + id++, 0);
		}
		
		Image@ img = cast<Image@>(debug_el);
		
		if(@img != null)
		{
			debug.print(indent + 'src:    ' + img._sprite_set + '/' + img._sprite_name, txt_clr, print_id + id++, 0);
		}
		
		if(debug_el.alpha != 1)
		{
			debug.print(indent + 'alpha:    ' + debug_el.alpha, txt_clr, print_id + id++, 0);
		}
		
		if(
			debug_mouse_over_clipping_ctx.clipping_mode != ClippingMode::None &&
			@debug_mouse_over_clipping_ctx.root != @contents &&
			@debug_mouse_over_clipping_ctx.root != @overlays)
		{
			debug.print(
				indent + '      <' + debug_mouse_over_clipping_ctx.x1 + ', ' + debug_mouse_over_clipping_ctx.y1 +
					'> <' + debug_mouse_over_clipping_ctx.x2 + ', ' + debug_mouse_over_clipping_ctx.y2 + '>',
				txt_clr, print_id + id++, 0);
			const string type = debug_mouse_over_clipping_ctx.clipping_mode == ClippingMode::Inside ? 'inside' : 'outside';
			debug.print(
				indent + 'clip: ' + debug_mouse_over_clipping_ctx.root._id + '.' + type, txt_clr, print_id + id++, 0);
		}
		
		string size_data = indent +     'size: ' + debug_el._width + ' x ' + debug_el._height;
		if(debug_el._width != debug_el._set_width || debug_el._height != debug_el._set_height)
			size_data += ' (' + debug_el._set_width + ' x ' + debug_el._set_height + ')';
		debug.print(size_data, txt_clr, print_id + id++, 0);
		
		Container@ container = cast<Container@>(debug_el);
		
		if(@container != null)
		{
			debug.print(indent +     'scroll: ' +
				container.scroll_min_x + ', ' + container.scroll_min_y + '  ' +
				container.scroll_max_x + ', ' + container.scroll_max_y, txt_clr, print_id + id++, 0);
		}
		
		if(debug_el.x1 != debug_el._x || debug_el.y1 != debug_el._y)
			debug.print(indent +     'global: ' + debug_el.x1 + ', ' + debug_el.y1, txt_clr, print_id + id++, 0);
			debug.print(indent +     'local:  ' + debug_el._x + ', ' + debug_el._y, txt_clr, print_id + id++, 0);
		
		debug.print('[' + debug_el.id + (debug_el.name != '' ? '.' + debug_el.name : '') + ']', id_clr, print_id + id++, 0);
	}
	
	private void debug_print_mouse_stack()
	{
		if(@debug == null)
			return;
		
		Element@ debug_el = @debug_mouse_over_element != null ? @debug_mouse_over_element : @_mouse_over_element;
		
		if(@debug_el == null)
			return;
		
		array<Element@>@ stack = @_element_array;
		int stack_size = int(stack.length());
		int index = 0;
		
		Element@ element = @debug_el;
		int element_count = 0;
		
		while(@element != null)
		{
			element_count++;
			@element = @element.parent;
		}
		
		@element = @debug_el;
		
		while(@element != null)
		{
			string data = element.name != '' ? element.name : element.id;
			
			if(!element.hovered)
				data =  '{' + data + '}';
			
			debug.print(string::repeat('-', (element_count - index++)) + ' ' + data, set_alpha(get_element_id_colour(element), element.hovered ? 1.0 : 0.6), element._id + 'a', 0);
			@element = @element.parent;
		}
		
		debug.print('[mouse path]', 0x99ffffff, '[mouse path]', 0);
	}
	
	/* internal */ void _queue_event(Event@ event, EventInfo@ event_info)
	{
		if(num_queued_events == queued_events_size)
		{
			queued_events_size += 16;
			queued_events.resize(queued_events_size);
			queued_event_infos.resize(queued_events_size);
		}
		
		@queued_events[num_queued_events]			= @event;
		@queued_event_infos[num_queued_events++]	= event_info;
	}
	
	private void show_tooltip(const string id, PopupOptions@ options, Element@ element, bool wait_for_mouse)
	{
		if(!options.enabled)
			return;
		
		if(!tooltips.exists(id))
		{
			Popup@ tooltip = Popup(this, options, element, wait_for_mouse);
			tooltip.hide.on(on_tooltip_hide_delegate);
			overlays.add_child(tooltip);
			@tooltips[id] = tooltip;
		}
		else
		{
			overlays.move_to_front(cast<Popup@>(tooltips[id]));
		}
	}
	
	private void hide_tooltip(const string id)
	{
		if(!tooltips.exists(id))
			return;
		
		Popup@ tooltip = cast<Popup@>(tooltips[id]);
		tooltip.force_hide();
	}
	
	private void update_tooltip(const string id, PopupOptions@ options)
	{
		if(!tooltips.exists(id))
			return;
		
		Popup@ tooltip = cast<Popup@>(tooltips[id]);
		@tooltip.content = @options.get_contenet_element();
		tooltip.fit_to_contents();
	}
	
	// Events
	// ---------------------------------------------------------
	
	private void on_tooltip_hide(EventInfo@ event)
	{
		Popup@ tooltip = cast<Popup@>(event.target);
		
		if(@tooltip == null)
			return;
		
		string id = tooltip._options._id;
		
		if(!tooltips.exists(id))
		{
			if(@tooltip._target == null)
				return;
			
			id = tooltip._target._id;
			
			if(!tooltips.exists(id))
				return;
		}
		
		tooltip.hide.off(on_tooltip_hide_delegate);
		tooltips.delete(id);
		overlays.remove_child(tooltip);
	}
	
}