#include '../std.cpp';
#include '../string.cpp';
#include '../fonts.cpp';
#include '../editor/common.cpp';
#include '../enums/GVB.cpp';
#include '../input/Keyboard.cpp';
#include '../input/IKeyboardFocusListener.cpp';
#include '../math/math.cpp';
#include '../utils/colour.cpp';
#include 'UIMouse.cpp';
#include 'Style.cpp';
#include 'utils/ClippingMode.cpp';
#include 'utils/ElementStack.cpp';
#include 'utils/ITextEditable.cpp';
#include 'utils/IStepHandler.cpp';
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
#include 'elements/TextBox.cpp';
#include 'layouts/flow/FlowLayout.cpp';

class UI : IKeyboardFocusListener
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
	Keyboard@ keyboard;
	bool block_editor_input = true;
	
	/// Only relevant when hud = true. When true, certain drawing operations
	/// will attempt to snap to whole pixels to give cleaner lines
	bool pixel_perfect = true;
	
	Event mouse_press;
	Event mouse_move;
	Event mouse_release;
	
	/// Triggered when the UI size changes.
	/// This could be when set_region is called explicitly, or when the game window is resized and auto_fit_screen and hud is enabled.
	Event screen_resize;
	/// Triggered at the end of the frame after layout, but before drawing
	Event after_layout;
	
	float _auto_fit_padding_left;
	float _auto_fit_padding_right;
	float _auto_fit_padding_top;
	float _auto_fit_padding_bottom;
	float _screen_width;
	float _screen_height;
	bool _even_screen_width;
	bool _even_screen_height;
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
	
	private Element@ _focused_element;
	private string _clipboard;
	
	private Element@ debug_mouse_over_element;
	private bool debug_draw_active;
	private DrawingContext debug_mouse_over_clipping_ctx;
	
	private Element@ mouse_over_overlays;
	
	private dictionary elements_left_pressed();
	private dictionary elements_right_pressed();
	private dictionary elements_middle_pressed();
	private array<Element@> elements_left_pressed_list();
	private array<Element@> elements_right_pressed_list();
	private array<Element@> elements_middle_pressed_list();
	
	private int num_queued_events;
	private int queued_events_size = 16;
	private array<Event@> queued_events(queued_events_size);
	private array<EventInfo@> queued_event_infos(queued_events_size);
	
	private int step_listeners_size = 16;
	private array<IStepHandler@> step_listeners(step_listeners_size);
	private int num_step_listeners;
	
	private dictionary tooltips;
	
	private textfield@ debug_text_field;
	
	private float x1 = 0;
	private float y1 = 0;
	private float x2 = 100;
	private float y2 = 100;
	
	private EventCallback@ on_tooltip_hide_delegate;
	
	// Layer selector
	
	private LayerSelector@ layer_selector;
	private PopupOptions@ layer_selector_popup;
	private EventCallback@ on_layer_select_callback;
	private EventCallback@ on_sub_layer_select_callback;
	private EventCallback@ on_layer_select_close_callback;
	private bool close_layer_selector_on_select;
	
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
	
	uint _frame;
	
	private int _first_valid_char_index = 32;
	private int _last_valid_char_index  = 126;
	private dictionary font_metrics;
	
	private TextBox@ _text_box;
	private EventCallback@ _text_box_accept_delegate;
	private ITextEditable@ text_editable;
	private bool text_editable_accept_on_blur;
	
	camera@ _camera;
	editor_api@ _editor;
	bool _has_editor;
	
	UI(bool hud=true, int layer=15, int sub_layer=0, int player=0)
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
		
		if(_has_editor)
		{
			@keyboard = Keyboard(this);
		}
		
		@_camera = get_active_camera();
		
		this.hud = hud;
		this.layer = layer;
		this.sub_layer = sub_layer;
		_player = player;
		
		_screen_width  = g.hud_screen_width(false);
		_screen_height = g.hud_screen_height(false);
		
		if(_hud && _auto_fit_screen)
		{
			fit_to_screen_internal();
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
	
	/// Draw this UI in the world, or hud
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
				fit_to_screen_internal();
			}
		}
	}
	
	/// The layer this UI is drawn on
	uint layer
	{
		get { return _layer; }
		set { style.layer = mouse.layer = _layer = value; }
	}
	
	/// The sublayer this UI is drawn on
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
				fit_to_screen_internal();
			}
		}
	}
	
	/// Mostly for testing. Adds padding around the screen when auto_fit_screen is true
	float auto_fit_padding_left
	{
		get const { return _auto_fit_padding_left; }
		set { _auto_fit_padding_left = update_auto_fit_padding(_auto_fit_padding_left, value); }
	}
	
	/// Same as auto_fit_padding_left
	float auto_fit_padding_right
	{
		get const { return _auto_fit_padding_right; }
		set { _auto_fit_padding_right = update_auto_fit_padding(_auto_fit_padding_right, value); }
	}
	
	/// Same as auto_fit_padding_left
	float auto_fit_padding_top
	{
		get const { return _auto_fit_padding_top; }
		set { _auto_fit_padding_top = update_auto_fit_padding(_auto_fit_padding_top, value); }
	}
	
	/// Same as auto_fit_padding_left
	float auto_fit_padding_bottom
	{
		get const { return _auto_fit_padding_bottom; }
		set { _auto_fit_padding_bottom = update_auto_fit_padding(_auto_fit_padding_bottom, value); }
	}
	
	IKeyboardFocus@ focus
	{
		get { return @keyboard != null ? @keyboard.focus : null; }
		set
		{
			if(@keyboard == null)
				return;
			
			@keyboard.focus = value;
		}
	}
	
	Element@ focused_element
	{
		get { return @_focused_element; }
		set
		{
			if(@keyboard == null)
				return;
			
			IKeyboardFocus@ keyboard_focus = cast<IKeyboardFocus@>(@value);
			
			if(@keyboard_focus != null)
			{
				@keyboard.focus = keyboard_focus;
			}
		}
	}
	
	string clipboard
	{
		get const { return _clipboard; }
		set { _clipboard = value; }
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
		_frame++;
		
		/*
		 * Auto fit hud to screen
		 */
		
		if(_hud)
		{
			const float new_screen_width  = g.hud_screen_width(false);
			const float new_screen_height = g.hud_screen_height(false);
			
			if(!approximately(_screen_width, new_screen_width) || !approximately(_screen_height, new_screen_height))
			{
				_screen_width  = new_screen_width;
				_screen_height = new_screen_height;
				
				if(_auto_fit_screen)
				{
					fit_to_screen_internal();
				}
			}
		}
		
		/*
		 * Mouse events are instead processed at the start of the next frame to allow any changes
		 * made during the event phase to take affect during the next layout before draw
		 */
		
		process_mouse_events(@_mouse_over_element == @mouse_over_overlays ? overlays : contents);
		
		if(@_active_mouse_element != null)
		{
			if(!mouse.primary_down || !contents.contains(@_active_mouse_element))
			{
				@_active_mouse_element = null;
			}
			else if(active_mouse_element_processed)
			{
				@_mouse_over_element = null;
			}
			else
			{
				active_mouse_element_processed = true;
			}
		}
		
		/*
		 * Update mouse and keyboard
		 */
		
		const bool block_mouse = _has_editor && (!_hud && _editor.mouse_in_gui() || _editor.key_check_gvb(GVB::Space));
		mouse.step(block_mouse);
		
		switch(primary_button)
		{
			case MouseButton::Left:
				mouse.primary_down = mouse.left_down;
				mouse.primary_press = mouse.left_press;
				mouse.primary_release = mouse.left_release;
				mouse.primary_double_click = mouse.left_double_click;
				break;
			case MouseButton::Middle:
				mouse.primary_down = mouse.middle_down;
				mouse.primary_press = mouse.middle_press;
				mouse.primary_release = mouse.middle_release;
				mouse.primary_double_click = mouse.middle_double_click;
				break;
			case MouseButton::Right:
				mouse.primary_down = mouse.right_down;
				mouse.primary_press = mouse.right_press;
				mouse.primary_release = mouse.right_release;
				mouse.primary_double_click = mouse.right_double_click;
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
		
		if(@keyboard != null)
		{
			if(@_focused_element != null)
			{
				if(
					!_focused_element.visible ||
					((mouse.left_press || mouse.middle_press || mouse.right_press) && !_focused_element.check_mouse()) ||
					(!contents.contains(@_focused_element) && !overlays.contains(@_focused_element))
				)
				{
					@keyboard.focus = null;
					@_focused_element = null;
				}
			}
			
			keyboard.step();
		}
		
		/*
		 * Step and process queueed events
		 */

		for(int i = num_step_listeners - 1; i >= 0; i--)
		{
			if(!step_listeners[i].ui_step())
			{
				@step_listeners[i] = step_listeners[--num_step_listeners];
			}
		}
		
		if(num_queued_events > 0)
		{
			process_queued_events();
		}
		
		/*
		 * Update layout
		 */
		
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
			editor_api::block_all_mouse(_editor);
		}
		
		_event_info.reset(EventType::AFTER_LAYOUT);
		after_layout.dispatch(_event_info);
		
		if(num_queued_events > 0)
		{
			process_queued_events();
		}
	}
	
	void draw()
	{
		draw_root(contents);
		draw_root(overlays);
	}
	
	void set_region(float x1, float y1, float x2, float y2)
	{
		x1 = round(x1);
		y1 = round(y1);
		x2 = round(x2);
		y2 = round(y2);
		
		if(this.x1 == x1 && this.y1 == y1 && this.x2 == x2 && this.y2 == y2)
			return;
		
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
		
		_event_info.reset(EventType::SCREEN_RESIZE);
		screen_resize.dispatch(_event_info);
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
	
	float screen_width { get const{ return _screen_width; } }
	
	float screen_height { get const{ return _screen_height; } }
	
	void fit_to_screen()
	{
		fit_to_screen_internal();
	}
	
	/// Shows the tooltip for the given element if it has one.
	void show_tooltip(Element@ element)
	{
		if(@element == null || @element.tooltip == null)
			return;
		
		show_tooltip(element._id, element.tooltip, element);
	}
	
	void hide_tooltip(Element@ element)
	{
		if(@element == null || @element.tooltip == null)
			return;
		
		hide_tooltip(element._id);
	}
	
	void show_tooltip(PopupOptions@ options, Element@ target = null)
	{
		if(@options == null)
			return;
		
		if(options._id == '')
			options._id = '_tt_target' + (CUSTOM_TOOLTIP_ID++);
		
		show_tooltip(options._id, options, target);
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
	
	/// Shows a layer selection popup. If target is null, will be displayed at the current mouse position.
	/// The returned LayerSelector can be used to adjust the layer selector properties.
	LayerSelector@ pick_layer(Element@ target, EventCallback@ callback, EventCallback@ close_callback=null,
		const PopupPosition position=PopupPosition::Below, const bool auto_close=true)
	{
		close_layer_selector_on_select = auto_close;
		@on_layer_select_callback = @callback;
		@on_layer_select_close_callback = @close_callback;
		
		return show_layer_selector(
			LayerSelectorType::Layers,
			target, position);
	}
	
	/// Shows a sub layer selection popup. If target is null, will be displayed at the current mouse position.
	/// The returned LayerSelector can be used to adjust the layer selector properties.
	LayerSelector@ pick_sub_layer(Element@ target, EventCallback@ callback, EventCallback@ close_callback=null,
		const PopupPosition position=PopupPosition::Below, const bool auto_close=true)
	{
		close_layer_selector_on_select = auto_close;
		@on_sub_layer_select_callback = @callback;
		@on_layer_select_close_callback = @close_callback;
		
		return show_layer_selector(
			LayerSelectorType::SubLayers,
			target, position);
	}
	
	/// Shows a layer and sub layer selection popup. If target is null, will be displayed at the current mouse position.
	/// The returned LayerSelector can be used to adjust the layer selector properties.
	/// If auto_close is true, the popup will only be closed once a layer and sub layer have been selected.
	/// Otherwise hide_layer_selector will need to be called manually.
	LayerSelector@ pick_layer_and_sub_layer(Element@ target,
		EventCallback@ layer_callback, EventCallback@ sub_layer_callback, EventCallback@ close_callback=null,
		const PopupPosition position=PopupPosition::Below, const bool auto_close=false)
	{
		close_layer_selector_on_select = auto_close;
		@on_layer_select_callback = @layer_callback;
		@on_sub_layer_select_callback = @sub_layer_callback;
		@on_layer_select_close_callback = @close_callback;
		
		return show_layer_selector(
			LayerSelectorType::Both,
			target, position);
	}
	
	/// Hides the layer selector if it is currently visible.
	void hide_layer_selector()
	{
		hide_tooltip(layer_selector_popup);
		
		if(@on_layer_select_close_callback != null)
		{
			_event_info.reset(EventType::CLOSE, layer_selector);
			on_layer_select_close_callback(_event_info);
			@on_layer_select_close_callback = null;
		}
	}
	
	/// Forces an update to the layer picker popup if it is open.
	void update_layer_picker()
	{
		update_tooltip(layer_selector_popup);
	}
	
	void update_layer_picker_auto_close(bool auto_close)
	{
		close_layer_selector_on_select = auto_close;
	}
	
	/// Make sure to setup a Debug instance before calling this
	void debug_draw(bool just_outline=false, bool show_ids=false, bool show_element_data=true, const float id_scale=1)
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
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	bool _step_subscribe(IStepHandler@ listener, const bool already_subscribed=false)
	{
		if(already_subscribed)
			return true;
		
		if(num_step_listeners == step_listeners_size)
		{
			step_listeners.resize(step_listeners_size += 16);
		}
		
		@step_listeners[num_step_listeners++] = @listener;
		return true;
	}
	
	float _pixel_floor(const float value) { return _hud || true && pixel_perfect ? floor(value) : value; }
	
	float _pixel_round(const float value) { return _hud || true && pixel_perfect ? round(value) : value; }
	
	float _pixel_ceil(const float value) { return _hud || true && pixel_perfect ? ceil(value) : value; }
	
	void _dispatch_event(Event@ event, const string type, Element@ target, const string value='')
	{
		_event_info.reset(type, target, value);
		event.dispatch(_event_info);
	}
	
	void _dispatch_event(Event@ event, const string type, IGenericEventTarget@ generic_target, const string value='')
	{
		_event_info.reset(type, null, generic_target, value);
		event.dispatch(_event_info);
	}
	
	void _dispatch_event(Event@ event, const string type, Element@ target, IGenericEventTarget@ generic_target, const string value='')
	{
		_event_info.reset(type, target, generic_target, value);
		event.dispatch(_event_info);
	}
	
	void _queue_event(Event@ event, EventInfo@ event_info)
	{
		if(num_queued_events == queued_events_size)
		{
			queued_events_size += 16;
			queued_events.resize(queued_events_size);
			queued_event_infos.resize(queued_events_size);
		}
		
		@event_info.mouse = mouse;
		@queued_events[num_queued_events]			= @event;
		@queued_event_infos[num_queued_events++]	= event_info;
	}
	
	void _queue_event(Event@ event, const string type, Element@ target, const string value='')
	{
		EventInfo@ event_info = _event_info_pool.get();
		event_info.reset(type, target, value);
		@event_info.mouse = mouse;
		_queue_event(@event, @event_info);
	}
	
	void _queue_event(Event@ event, const string type, IGenericEventTarget@ generic_target, const string value='')
	{
		EventInfo@ event_info = _event_info_pool.get();
		event_info.reset(type, null, generic_target, value);
		@event_info.mouse = mouse;
		_queue_event(@event, @event_info);
	}
	
	void _queue_event(Event@ event, const string type, Element@ target, IGenericEventTarget@ generic_target, const string value='')
	{
		EventInfo@ event_info = _event_info_pool.get();
		event_info.reset(type, target, generic_target, value);
		@event_info.mouse = mouse;
		_queue_event(@event, @event_info);
	}
	
	LayerSelector@ _create_layer_selector_for_popup(
		const LayerSelectorType type, const PopupPosition position,
		EventCallback@ layer_select, EventCallback@ sub_layer_select, EventCallback@ layer_selector_hide,
		PopupOptions@ &out popup_options)
	{
		LayerSelector@ layer_selector = LayerSelector(this, type);
		layer_selector.layer_select.on(layer_select);
		layer_selector.sub_layer_select.on(sub_layer_select);
		
		@popup_options = PopupOptions(this, layer_selector, true, position, PopupTriggerType::Manual, PopupHideType::MouseDownOutside, false);
		popup_options.clear_drawing();
		popup_options.mouse_self = false;
		popup_options.padding_left = 0;
		popup_options.padding_right = 0;
		popup_options.padding_top = 0;
		popup_options.padding_bottom = 0;
		popup_options.hide_start.on(layer_selector_hide);
		
		return layer_selector;
	}
	
	void _initialise_layer_selector_for_popup(
		LayerSelector@ layer_selector, PopupOptions@ popup_options,
		const LayerSelectorType type, const PopupPosition position)
	{
		popup_options.position = position;
		
		layer_selector.reset(false);
		layer_selector.type = type;
		layer_selector.multi_select = false;
		layer_selector.individual_backgrounds = true;
		layer_selector.background_colour = style.normal_bg_clr;
		layer_selector.background_blur = true;
		layer_selector.blur_inset = 0;
		layer_selector.border_colour = 0;
		layer_selector.border_size = 0;
		layer_selector.shadow_colour = style.dialog_shadow_clr;
		layer_selector.show_all_layers_toggle = false;
		layer_selector.show_all_sub_layers_toggle = false;
	}
	
	int first_valid_char_index
	{
		get const { return _first_valid_char_index; }
	}
	
	int last_valid_char_index 
	{
		get const { return _last_valid_char_index;  }
	}
	
	void get_font_metrics(string font, uint size, array<float>@ &out widths, float &out line_height)
	{
		style.get_real_font(font, size, font, size);
		const string key = font + '_' + size;
		const string height_key = key + '_line_height';
		
		if(font_metrics.exists(key))
		{
			@widths = cast<array<float>@>(font_metrics[key]);
			line_height = float((font_metrics[height_key]));
			return;
		}
		
		string s = ' ';
		@widths = array<float>(_last_valid_char_index - _first_valid_char_index + 1);
		line_height = 0;
		
		for(int i = _first_valid_char_index; i <= _last_valid_char_index; i++)
		{
			s[0] = i;
			float width, height;
			style.measure_text(s, font, size, 1, 1, width, height);
			widths[i - _first_valid_char_index] = width;
			
			if(height > line_height)
			{
				line_height = height;
			}
		}
		
		@font_metrics[key] = @widths;
		font_metrics[height_key] = line_height;
	}
	
	/// The caller will be responsible for adding the TextBox to a container, and initialising all properties as
	/// the text box is reset each time this is called.
	/// @param accept_on_blur Will the event type triggered by CANCEL or ACCEPT when the text box loses focus
	void _start_editing(ITextEditable@ editable, const bool accept_on_blur=true)
	{
		if(@_text_box == null)
		{
			@_text_box = TextBox(this);
			@_text_box_accept_delegate = EventCallback(on_text_box_accept);
		}
		
		_stop_editing(_text_box.accept_on_blur ? EventType::ACCEPT : EventType::CANCEL);
		
		_text_box.text = '';
		_text_box.multi_line = false;
		_text_box.allowed_characters = '';
		_text_box.character_validation = None;
		_text_box.select_none();
		_text_box.width  = 140;
		_text_box.height = 34;
		_text_box.accept.on(_text_box_accept_delegate);
		
		@text_editable = editable;
		text_editable_accept_on_blur = accept_on_blur;
		
		editable.text_editable_start(_text_box);
	}
	
	void _stop_editing(const string event_type=EventType::ACCEPT)
	{
		if(@text_editable == null)
			return;
		
		text_editable.text_editable_stop(@_text_box, event_type);
		_text_box.accept.off(_text_box_accept_delegate);
		@text_editable = null;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Layout and drawing
	// ///////////////////////////////////////////////////////////////////
	
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
				
				if(element.validate_layout)
				{
					element._do_layout(ctx);
					element.validate_layout = false;
				}
				
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
					element.mouse_self && ctx.mouse_active && (
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
				
				if(@ctx.root == @element && element._scroll_children)
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
		
		EventInfo@ event = _event_info_pool.get();
		event.reset(EventType::MOUSE_EXIT, MouseButton::None, mouse.x, mouse.y);
		@event.mouse = mouse;
		@event.src = @_mouse_over_element;
		
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
				
				@event.target = element;
				element._mouse_exit(event);
				element.mouse_exit.dispatch(event);
			}
			
			break;
		}
		
		// /////////////////////////////////////////////////
		// Mouse enter
		// 
		
		bool mouse_over_element_entered = false;
		
		if(is_mouse_over)
		{
			event.reset(EventType::MOUSE_ENTER, MouseButton::None, mouse.x, mouse.y);
			@event.src = @_mouse_over_element;
			
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
				
				if(@element == @_mouse_over_element)
				{
					mouse_over_element_entered = true;
				}
				
				@event.target = element;
				element._mouse_enter(event);
				element.mouse_enter.dispatch(event);
			}
		}
		
		// /////////////////////////////////////////////////
		// Mouse press
		// 
		
		if(is_mouse_over && mouse.left_press || mouse.middle_press || mouse.right_press)
		{
			event.reset(EventType::MOUSE_PRESS, MouseButton::None, mouse.x, mouse.y);
			@event.src = @_mouse_over_element;
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event.target = @elements_mouse_enter[i];
				
				if(mouse.left_press)
				{
					elements_left_pressed[element._id] = true;
					elements_left_pressed_list.insertLast(element);
					
					event.button = MouseButton::Left;
					element._mouse_press(event);
					element.mouse_press.dispatch(event);
				}
				
				if(mouse.right_press)
				{
					elements_right_pressed[element._id] = true;
					elements_right_pressed_list.insertLast(element);
					
					event.button = MouseButton::Right;
					element._mouse_press(event);
					element.mouse_press.dispatch(event);
				}
				
				if(mouse.middle_press)
				{
					elements_middle_pressed[element._id] = true;
					elements_middle_pressed_list.insertLast(element);
					
					event.button = MouseButton::Middle;
					element._mouse_press(event);
					element.mouse_press.dispatch(event);
				}
				
				if(mouse.primary_press)
				{
					element.pressed = true;
					
					// Tooltip
					if(@element.tooltip != null && element.tooltip.trigger_type == PopupTriggerType::MouseDown && @element == @_mouse_over_element)
					{
						show_tooltip(element);
					}
				}
			}
			
			dispatch_ui_mouse_button_events(@mouse_press, @event, mouse.left_press, mouse.right_press, mouse.middle_press);
		}
		
		// /////////////////////////////////////////////////
		// Mouse move
		// 
		
		if(is_mouse_over && mouse.moved)
		{
			event.reset(EventType::MOUSE_MOVE, MouseButton::None, mouse.x, mouse.y);
			@event.src = @_mouse_over_element;
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event.target = @elements_mouse_enter[i];
				element._mouse_move(event);
				element.mouse_move.dispatch(event);
			}
			
			@event.target = null;
			mouse_move.dispatch(event);
		}
		
		// /////////////////////////////////////////////////
		// Mouse scroll
		// 
		
		int scroll_dir;
		
		if(is_mouse_over && mouse.scrolled(scroll_dir))
		{
			event.reset(EventType::MOUSE_SCROLL, MouseButton::None, mouse.x, mouse.y);
			@event.src = @_mouse_over_element;
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event.target = @elements_mouse_enter[i];
				element._mouse_scroll(event);
				element.mouse_scroll.dispatch(event);
			}
		}
		
		// /////////////////////////////////////////////////
		// Mouse release and click
		// 
		
		if(is_mouse_over && mouse.left_release || mouse.middle_release || mouse.right_release)
		{
			// Release
			
			event.reset(EventType::MOUSE_RELEASE, MouseButton::None, mouse.x, mouse.y);
			@event.src = @_mouse_over_element;
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event.target = @elements_mouse_enter[i];
				
				if(mouse.left_release)
				{
					event.button = MouseButton::Left;
					element._mouse_release(event);
					element.mouse_release.dispatch(event);
				}
				
				if(mouse.right_release)
				{
					event.button = MouseButton::Right;
					element._mouse_release(event);
					element.mouse_release.dispatch(event);
				}
				
				if(mouse.middle_release)
				{
					event.button = MouseButton::Middle;
					element._mouse_release(event);
					element.mouse_release.dispatch(event);
				}
			}
			
			dispatch_ui_mouse_button_events(@mouse_release, @event, mouse.left_release, mouse.right_release, mouse.middle_release);
			
			// Click
			
			event.reset(EventType::MOUSE_CLICK, MouseButton::None, mouse.x, mouse.y);
			@event.src = @_mouse_over_element;
			
			for(int i = 0; i < num_elements_mouse_enter; i++)
			{
				Element@ element = @event.target = @elements_mouse_enter[i];
				bool primary_clicked = false;
				
				if(mouse.left_release && elements_left_pressed.exists(element._id))
				{
					event.button = MouseButton::Left;
					primary_clicked = primary_clicked || primary_button == event.button;
					
					element._mouse_button_click(event);
					element.mouse_button_click.dispatch(event);
					
					if(primary_clicked)
					{
						element._mouse_click(event);
						element.mouse_click.dispatch(event);
					}
				}
				
				if(mouse.right_release && elements_right_pressed.exists(element._id))
				{
					event.button = MouseButton::Right;
					primary_clicked = primary_clicked || primary_button == event.button;
					
					element._mouse_button_click(event);
					element.mouse_button_click.dispatch(event);
					
					if(primary_clicked)
					{
						element._mouse_click(event);
						element.mouse_click.dispatch(event);
					}
				}
				
				if(mouse.middle_release && elements_middle_pressed.exists(element._id))
				{
					event.button = MouseButton::Middle;
					primary_clicked = primary_clicked || primary_button == event.button;
					
					element._mouse_button_click(event);
					element.mouse_button_click.dispatch(event);
					
					if(primary_clicked)
					{
						element._mouse_click(event);
						element.mouse_click.dispatch(event);
					}
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
		
		_event_info_pool.release(@event);
		
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
			clear_press(MouseButton::Left, @elements_left_pressed_list);
		}
		
		if(mouse.right_release)
		{
			elements_right_pressed.deleteAll();
			clear_press(MouseButton::Right, @elements_right_pressed_list);
		}
		
		if(mouse.middle_release)
		{
			elements_middle_pressed.deleteAll();
			clear_press(MouseButton::Middle, @elements_middle_pressed_list);
		}
		
		// Hover tooltip
		if(
			mouse_over_element_entered &&
			@_mouse_over_element != null && !_mouse_over_element.disabled &&
			@_mouse_over_element.tooltip != null &&
			_mouse_over_element.tooltip.trigger_type == PopupTriggerType::MouseOver)
		{
			show_tooltip(_mouse_over_element);
		}
	}
	
	private void dispatch_ui_mouse_button_events(Event@ event, EventInfo@ event_info, const bool left, const bool right, const bool middle)
	{
		@event_info.target = _mouse_over_element;
		
		if(left)
		{
			event_info.button = MouseButton::Left;
			event.dispatch(event_info);
		}
		
		if(right)
		{
			event_info.button = MouseButton::Right;
			event.dispatch(event_info);
		}
		
		if(middle)
		{
			event_info.button = MouseButton::Middle;
			event.dispatch(event_info);
		}
	}
	
	private void clear_press(const MouseButton button, array<Element@>@ elements_pressed_list)
	{
		EventInfo@ event = _event_info_pool.get();
		event.reset(EventType::MOUSE_RELEASE, button, mouse.x, mouse.y);
		@event.mouse = mouse;
		@event.src = @_mouse_over_element;
		
		for(int i = int(elements_pressed_list.length()) - 1; i >= 0; i--)
		{
			Element@ element = @event.target = @elements_pressed_list[i];
			
			if(!element.hovered)
			{
				element._mouse_release(event);
			}
			
			element.pressed = false;
		}
		
		elements_pressed_list.resize(0);
		_event_info_pool.release(@event);
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
		
		const float zoom = _hud ? 1.0 : 1.0 / _camera.editor_zoom();
		
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
							(@element == @mouse_over_element ? -2 : 1) * zoom,
							clr | el_alpha);
					}
					
					if(@element != @mouse_over_element)
					{
						if(not_clipped)
						{
							style.outline(
								element.x1, element.y1, element.x2, element.y2,
								1 * zoom, (element.hovered ? 0xff0000 : clr) | el_alpha);
						}
						else
						{
							style.outline_dotted(element.x1, element.y1, element.x2, element.y2, 1 * zoom, clr | el_alpha);
						}
						
						clr = scale_lightness(clr, 0.1) | el_alpha;
						
						if(show_ids)
						{
							style.draw_text(
								element.name != '' ? element.name : element.id,
								element.x1, element.y1,
								clr | (uint((el_alpha>>24) * 1.75) << 24),
								id_scale * zoom, id_scale * zoom);
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
				-2 * zoom, clr);
			
			style.outline_text(
				mouse_over_element.name != '' ? mouse_over_element.name : mouse_over_element.id,
				mouse_over_element.x1, mouse_over_element.y1 - 5 * zoom,
				0xffffffff, 0xff000000, 2 * id_scale * zoom,
				id_scale * zoom, id_scale * zoom, 0, TextAlign::Left, TextAlign::Bottom);
			
			if(debug_mouse_over_clipping_ctx.clipping_mode != ClippingMode::None)
			{
				style.outline_dotted(
					debug_mouse_over_clipping_ctx.x1, debug_mouse_over_clipping_ctx.y1,
					debug_mouse_over_clipping_ctx.x2, debug_mouse_over_clipping_ctx.y2,
					-2 * zoom, 0x99ffffff);
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
			if(gr.debug_is_transposed)
				debug.print(indent + '  transposed', txt_clr, print_id + id++, 0);
			debug.print(indent + '  draw_scale: ' + gr.debug_draw_scale_x + ', ' + gr.debug_draw_scale_y, txt_clr, print_id + id++, 0);
			debug.print(indent + '  draw_pos:   ' + gr.debug_draw_x + ', ' + gr.debug_draw_y, txt_clr, print_id + id++, 0);
			if(gr.padding_left != 0 || gr.padding_right != 0 || gr.padding_top != 0 || gr.padding_bottom != 0)
				debug.print(indent + '  padding:    ' + gr.padding_left + ' ' + gr.padding_right + ' ' + gr.padding_top + ' ' + gr.padding_bottom, txt_clr, print_id + id++, 0);
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
			debug.print(indent +     'content rect: ' +
				container.scroll_min_x + ', ' + container.scroll_min_y + '  ' +
				container.scroll_max_x + ', ' + container.scroll_max_y, txt_clr, print_id + id++, 0);
		}
		
		if(debug_el._scroll_x != 0 || debug_el._scroll_y != 0)
		{
			debug.print(indent +     'scroll: ' +
				debug_el._scroll_x + ', ' + debug_el._scroll_y, txt_clr, print_id + id++, 0);
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
	
	// ///////////////////////////////////////////////////////////////////
	// Private
	// ///////////////////////////////////////////////////////////////////
	
	private void fit_to_screen_internal()
	{
		if(!_hud)
			return;
		
		const float new_screen_width  = g.hud_screen_width(false);
		const float new_screen_height = g.hud_screen_height(false);
		 
		_screen_width  = new_screen_width;
		_screen_height = new_screen_height;
		_even_screen_width  = (_screen_width % 2) == 0;
		_even_screen_height = (_screen_height % 2) == 0;
		
		set_region(
			-new_screen_width * 0.5 + auto_fit_padding_left,  -new_screen_height * 0.5 + auto_fit_padding_top,
			 new_screen_width * 0.5 - auto_fit_padding_right,  new_screen_height * 0.5 - auto_fit_padding_bottom);
	}
	
	private float update_auto_fit_padding(const float current, const float new_value)
	{
		if(current == new_value)
			return current;
		
		if(_hud && _auto_fit_screen)
		{
			fit_to_screen_internal();
		}
		
		return new_value;
	}
	
	private void show_tooltip(const string id, PopupOptions@ options, Element@ element)
	{
		if(!options.enabled)
			return;
		
		if(!tooltips.exists(id))
		{
			Popup@ tooltip = Popup(this, options, element);
			tooltip.hide.on(on_tooltip_hide_delegate);
			overlays.add_child(tooltip);
			@tooltips[id] = tooltip;
		}
		else
		{
			Popup@ tooltip = cast<Popup@>(tooltips[id]);
			tooltip.update(options, element);
			overlays.move_to_front(tooltip);
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
	
	private LayerSelector@ show_layer_selector(const LayerSelectorType type, Element@ target, const PopupPosition position)
	{
		if(@layer_selector == null)
		{
			@layer_selector = _create_layer_selector_for_popup(
				type, position,
				EventCallback(on_layer_select), EventCallback(on_sub_layer_select), EventCallback(on_layer_selector_hide),
				layer_selector_popup);
		}
		
		_initialise_layer_selector_for_popup(
			layer_selector, layer_selector_popup,
			type, position);
		
		layer_selector.select_layers_none(false);
		layer_selector.select_sub_layers_none(false);
		
		layer_selector.fit_to_contents(true);
		show_tooltip(layer_selector_popup, target);
		
		return layer_selector;
	}
	
	private void auto_hide_layer_selector()
	{
		if(layer_selector.type == LayerSelectorType::Both)
		{
			if(layer_selector.num_layers_selected() > 0 && layer_selector.num_sub_layers_selected() > 0)
			{
				hide_layer_selector();
			}
		}
		else
		{
			hide_layer_selector();
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
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
	
	private void on_layer_select(EventInfo@ event)
	{
		if(@on_layer_select_callback != null)
		{
			on_layer_select_callback(event);
		}
		
		if(close_layer_selector_on_select)
		{
			auto_hide_layer_selector();
		}
	}
	
	private void on_sub_layer_select(EventInfo@ event)
	{
		if(@on_sub_layer_select_callback != null)
		{
			on_sub_layer_select_callback(event);
		}
		
		if(close_layer_selector_on_select)
		{
			auto_hide_layer_selector();
		}
	}
	
	private void on_layer_selector_hide(EventInfo@ event)
	{
		if(@on_layer_select_close_callback != null)
		{
			_event_info.reset(EventType::CLOSE, layer_selector);
			on_layer_select_close_callback(_event_info);
			@on_layer_select_close_callback = null;
		}
	}
	
	void on_keyboard_focus_change(IKeyboardFocus@ focus)
	{
		@_focused_element = @keyboard._focus != null ? cast<Element@>(@focus) : null;
		
		if(@_focused_element != null && @_focused_element.parent != null)
		{
			@_focused_element.parent.scroll_into_view = @_focused_element;
		}
	}
	
	private void on_text_box_accept(EventInfo@ event)
	{
		_stop_editing(event.type);
	}
	
}