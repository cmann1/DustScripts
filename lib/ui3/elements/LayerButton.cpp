#include '../../editor/common.cpp';
#include '../events/Event.cpp';
#include '../popups/PopupPosition.cpp';
#include '../utils/Orientation.cpp';
#include '../popups/PopupOptions.cpp';
#include 'LockedContainer.cpp';

class LayerButton : LockedContainer
{
	
	/// Triggered when layer is selected or deselected.
	/// If auto_close is true and multi select is false, this is the same as the select event.
	Event change;
	/// Triggered when the layer selection popup closes. The event type will be SELECT if the selection is accepted, or CANCEL
	/// if it is cancel by pressing the escape key
	Event select;
	
	protected LayerSelectorType _type = LayerSelectorType::Layers;
	protected Orientation _orientation;
	// TODO: Set to true
	protected bool _auto_close = false;
	protected PopupPosition _position = PopupPosition::Below;
	
	protected bool _open;
	protected bool canceled;
	protected bool has_layer_select;
	protected LayerSelector@ _layer_select;
	protected PopupOptions@ popup_options;
	
	protected EventCallback@ layer_select_delegate;
	protected EventCallback@ sub_layer_select_delegate;
	protected EventCallback@ layer_selector_close_delegate;
	
	protected array<bool> selected_layers;
	protected array<bool> selected_sub_layers;
	
	LayerButton(UI@ ui, const Orientation orientation=Orientation::Horizontal)
	{
		super(ui);
		
		this.orientation = orientation;
		
		@layer_select_delegate = EventCallback(on_layer_select);
		@sub_layer_select_delegate = EventCallback(on_sub_layer_select);
		@layer_selector_close_delegate = EventCallback(on_layer_selector_close);
		
		_set_width  = _width  = orientation == Orientation::Horizontal ? 70 : 34;
		_set_height = _height = orientation == Orientation::Horizontal ? 34 : 70;
	}
	
	string element_type { get const { return 'LayerButton'; } }
	
	/// Show layers, sublayers, or both
	LayerSelectorType type
	{
		get const { return _type; }
		set
		{
			if(_type == value)
				return;
			
			_type = value;
			layer_select.type = _type;
			
			if(_open)
			{
				ui.update_layer_picker();
			}
		}
	}
	
	/// The layer and sublayer numbers can be displayed in a row or column
	Orientation orientation
	{
		get const { return _orientation; }
		set
		{
			if(_orientation == value)
				return;
			
			_orientation = value;
			
			const float width_t = _width;
			width = height;
			height = width_t;
		}
	}
	
	/// Auto close the layer selection popup when a layer is picked
	bool auto_close
	{
		get const { return _auto_close; }
		set
		{
			if(_auto_close == value)
				return;
			
			_auto_close = value;
		}
	}
	
	/// The position the tooltip will be displayed
	PopupPosition position
	{
		get const { return _position; }
		set
		{
			_position = value;
			LayerSelector@ l = layer_select;
			popup_options.position = _position;
		}
	}
	
	LayerSelector@ layer_select
	{
		get
		{
			if(@_layer_select != null)
				return @_layer_select;
			
			@_layer_select = ui._create_layer_selector_for_popup(
				_type, _position,
				layer_select_delegate, sub_layer_select_delegate, layer_selector_close_delegate,
				@popup_options);
			
			ui._initialise_layer_selector_for_popup(
				_layer_select, popup_options,
				_type, _position);
			
			return _layer_select;
		}
	}
	
	/// Is the layer selection popup currently open.
	bool open { get const { return _open; } }
	
	/// Opens the layer selection popup.
	void show()
	{
		if(_open)
			return;
		
		update_layers_selection();
		
		layer_select.fit_to_contents(true);
		ui.show_tooltip(popup_options, this);
		
		_open = true;
	}
	
	/// Hides the layer selection popup if it is open
	void hide()
	{
		if(!_open)
			return;
		
		ui.hide_tooltip(popup_options);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(_open && ui._has_editor && editor_api::consume_gvb_press(ui._editor, GVB::Escape))
		{
			canceled = true;
			hide();
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_interactive_element(
			x1, y1, x2, y2,
			hovered || pressed,
			_open, pressed, disabled);
	}
	
	protected void update_layers_selection()
	{
		if(_type == LayerSelectorType::Layers || _type == LayerSelectorType::Both)
		{
			layer_select.get_layers_selected(@selected_layers);
		}
		
		if(_type == LayerSelectorType::SubLayers || _type == LayerSelectorType::Both)
		{
			layer_select.get_layers_selected(@selected_sub_layers);
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_click()
	{
		if(!_open)
		{
			show();
		}
		else
		{
			hide();
		}
	}
	
	private void on_layer_select(EventInfo@ event)
	{
		puts(id + '.on_layer_select');
		ui._dispatch_event(@change, EventType::CHANGE, this);
		
		auto_hide_layer_selector();
	}
	
	private void on_sub_layer_select(EventInfo@ event)
	{
		puts(id + '.on_sub_layer_select');
		ui._dispatch_event(@change, EventType::CHANGE, this);
		
		auto_hide_layer_selector();
	}
	
	private void on_layer_selector_close(EventInfo@ event)
	{
		puts(id + '.on_layer_selector_close');
		_open = false;
		
		if(!canceled)
		{
			update_layers_selection();
			puts('  accept');
			ui._dispatch_event(@select, EventType::SELECT, this);
		}
		else
		{
			if(_type == LayerSelectorType::Layers || _type == LayerSelectorType::Both)
			{
				layer_select.set_layers_selected(@selected_layers, false, true);
			}
			
			if(_type == LayerSelectorType::SubLayers || _type == LayerSelectorType::Both)
			{
				layer_select.set_sub_layers_selected(@selected_sub_layers, false, true);
			}
			
			canceled = false;
			puts('  cancel');
			ui._dispatch_event(@select, EventType::CANCEL, this);
		}
	}
	
	private void auto_hide_layer_selector()
	{
		if(!_auto_close)
			return;
		
		if(_type == LayerSelectorType::Both)
		{
			if(_layer_select.num_layers_selected() > 0 && _layer_select.num_sub_layers_selected() > 0)
			{
				hide();
			}
		}
		else
		{
			hide();
		}
	}
	
}