#include '../../editor/common.cpp';
#include '../events/Event.cpp';
#include '../popups/PopupPosition.cpp';
#include '../utils/Orientation.cpp';
#include '../popups/PopupOptions.cpp';
#include 'LockedContainer.cpp';
#include 'Label.cpp';

namespace LayerButton { const string TYPE_NAME = 'LayerButton'; }

class LayerButton : LockedContainer, ILayerSelectorControl, IStepHandler
{
	
	/// Triggered when layer is selected or deselected.
	/// If auto_close is true and multi select is false, this is the same as the select event.
	Event change;
	/// Triggered when the layer selection popup closes. The event type will be SELECT if the selection is accepted, or CANCEL
	/// if it is cancel by pressing the escape key
	Event select;
	
	protected Orientation _orientation;
	protected bool _auto_close = true;
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
	
	protected Label@ layer_label;
	protected Label@ sub_layer_label;
	
	protected bool has_selected_layer;
	protected bool has_selected_sublayer;
	
	LayerButton(UI@ ui, const Orientation orientation=Orientation::Horizontal)
	{
		super(ui);
		
		this.orientation = orientation;
		
		@layer_select_delegate = EventCallback(on_layer_select);
		@sub_layer_select_delegate = EventCallback(on_sub_layer_select);
		@layer_selector_close_delegate = EventCallback(on_layer_selector_close);
		
		_set_width  = _width  = orientation == Orientation::Horizontal ? 70 : 34;
		_set_height = _height = orientation == Orientation::Horizontal ? 34 : 70;
		
		children_mouse_enabled = false;
		
		const LayerSelectorType type = LayerSelectorType::Both;
		
		@_layer_select = ui._create_layer_selector_for_popup(
			type, _position,
			layer_select_delegate, sub_layer_select_delegate, layer_selector_close_delegate,
			@popup_options);
		
		ui._initialise_layer_selector_for_popup(
			_layer_select, popup_options,
			type, _position);
		
		@_layer_select._control = @this;
		on_layer_selector_type_change();
	}
	
	string element_type { get const { return LayerButton::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
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
			
			validate_layout = true;
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
	
	/// The LayerSelector used by this LayerButton.
	LayerSelector@ layer_select { get { return _layer_select; } }
	
	/// Is the layer selection popup currently open.
	bool open { get const { return _open; } }
	
	/// Opens the layer selection popup.
	void show()
	{
		if(_open)
			return;
		
		accept_layers_selection();
		
		@_layer_select._control = null;
		layer_select.fit_to_contents(true);
		ui.show_tooltip(popup_options, this);
		
		_open = ui._step_subscribe(this, _open);
		
		has_selected_sublayer = false;
		has_selected_layer = false;
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
	
	bool ui_step() override
	{
		if(_open && ui._has_editor && editor_api::consume_gvb_press(ui._editor, GVB::Escape))
		{
			canceled = true;
			hide();
		}
		
		return _open;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(_layer_select.type == LayerSelectorType::Both)
		{
			if(_orientation == Orientation::Horizontal)
			{
				const float mid_x = _width * 0.5;
				
				layer_label._x = ui.style.spacing;
				layer_label._y = ui.style.spacing;
				layer_label._width = mid_x - ui.style.spacing - layer_label._x;
				layer_label._height = _height - ui.style.spacing * 2;
				sub_layer_label._x = mid_x + ui.style.spacing;
				sub_layer_label._y = ui.style.spacing;
				sub_layer_label._width = _width - ui.style.spacing - sub_layer_label._x;
				sub_layer_label._height = _height - ui.style.spacing * 2;
			}
			else
			{
				const float mid_y = _height * 0.5;
				
				layer_label._x = ui.style.spacing;
				layer_label._y = ui.style.spacing;
				layer_label._width = _width - ui.style.spacing * 2;
				layer_label._height = mid_y - ui.style.spacing - layer_label._y;
				sub_layer_label._x = ui.style.spacing;
				sub_layer_label._y = mid_y + ui.style.spacing;
				sub_layer_label._width = _width - ui.style.spacing * 2;
				sub_layer_label._height = _height - ui.style.spacing - sub_layer_label._y;
			}
		}
		else if(_layer_select.type == LayerSelectorType::Layers)
		{
			layer_label.x = ui.style.spacing;
			layer_label.y = ui.style.spacing;
			layer_label.width = _width - ui.style.spacing * 2;
			layer_label.height = _height - ui.style.spacing * 2;
		}
		else
		{
			sub_layer_label._x = ui.style.spacing;
			sub_layer_label._y = ui.style.spacing;
			sub_layer_label._width = _width - ui.style.spacing * 2;
			sub_layer_label._height = _height - ui.style.spacing * 2;
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		const bool is_horizontal = _orientation == Orientation::Horizontal;
		
		style.draw_interactive_element(
			x1, y1, x2, y2,
			hovered || pressed,
			_open, pressed, disabled);
		
		// Divider
		
		if(_layer_select.type == LayerSelectorType::Both)
		{
			if(is_horizontal)
			{
				const float mid_x = (x1 + x2) * 0.5;
				style.draw_rectangle(
					mid_x - ui.style.border_size * 0.5, y1 + ui.style.spacing,
					mid_x + ui.style.border_size * 0.5, y2 - ui.style.spacing,
					0, ui.style.normal_border_clr);
			}
			else
			{
				const float mid_y = (y1 + y2) * 0.5;
				style.draw_rectangle(
					x1 + ui.style.spacing, mid_y - ui.style.border_size * 0.5,
					x2 - ui.style.spacing, mid_y + ui.style.border_size * 0.5,
					0, ui.style.normal_border_clr);
			}
		}
	}
	
	protected void update_labels()
	{
		if(@layer_label != null && layer_label.visible)
		{
			update_label(@layer_label, @selected_layers);
		}
		
		if(@sub_layer_label != null && sub_layer_label.visible)
		{
			update_label(@sub_layer_label, @selected_sub_layers);
		}
	}
	
	protected void update_label(Label@ label, array<bool>@ values)
	{
		int selected_count = 0;
		int min_layer = -1;
		int max_layer = -1;
		
		for(int i = 0, count = int(values.length()); i < count; i++)
		{
			if(values[i])
			{
				if(min_layer == -1)
				{
					min_layer = i;
				}
				
				max_layer = i;
				selected_count++;
			}
		}
		
		if(max_layer == -1)
		{
			label.text = '-';
		}
		else if(selected_count == 1)
		{
			label.text = max_layer + '';
		}
		else
		{
			label.text = min_layer + '\n' + max_layer;
		}
	}
	
	protected Label@ create_label()
	{
		Label@ label = Label(ui, '-');
		label.set_font(font::ENVY_BOLD, 20);
		label.text_align_h = TextAlign::Centre;
		label.sizing = ImageSize::ConstrainInside;
		label.align_h = GraphicAlign::Centre;
		label.align_v = GraphicAlign::Middle;
		
		Container::add_child(label);
		
		return label;
	}
	
	protected void accept_layers_selection()
	{
		if(_layer_select.type == LayerSelectorType::Layers || _layer_select.type == LayerSelectorType::Both)
		{
			layer_select.get_layers_selected(@selected_layers);
		}
		
		if(_layer_select.type == LayerSelectorType::SubLayers || _layer_select.type == LayerSelectorType::Both)
		{
			layer_select.get_sub_layers_selected(@selected_sub_layers);
		}
		
		update_labels();
	}
	
	// ///////////////////////////////////////////////////////////////////
	// ILayerSelectorControl
	// ///////////////////////////////////////////////////////////////////
	
	void on_layer_selector_type_change()
	{
		if(_open)
		{
			ui.update_layer_picker();
		}
		
		if(_layer_select.type == LayerSelectorType::Layers || _layer_select.type == LayerSelectorType::Both)
		{
			if(@layer_label == null)
			{
				@layer_label = create_label();
			}
			
			layer_label.visible = true;
		}
		else if(@sub_layer_label != null)
		{
			layer_label.visible = false;
		}
		
		if(_layer_select.type == LayerSelectorType::SubLayers || _layer_select.type == LayerSelectorType::Both)
		{
			if(@sub_layer_label == null)
			{
				@sub_layer_label = create_label();
			}
			
			sub_layer_label.visible = true;
		}
		else if(@sub_layer_label != null)
		{
			sub_layer_label.visible = false;
		}
		
		validate_layout = true;
	}
	
	void on_layer_selector_selection_change()
	{
		// Assume that if the popup isn't open, the layers were explicitly set via script
		// and should be accepted
		if(!_open)
		{
			accept_layers_selection();
		}
		else
		{
			update_labels();
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_press(EventInfo@ event) override
	{
		if(event.button != ui.primary_button)
			return;
		
		@ui._active_mouse_element = @this;
	}
	
	void _mouse_click(EventInfo@ event) override
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
		if(!_open)
			return;
		
		has_selected_layer = true;
		ui._dispatch_event(@change, EventType::CHANGE, this);
		
		auto_hide_layer_selector();
	}
	
	private void on_sub_layer_select(EventInfo@ event)
	{
		if(!_open)
			return;
		
		has_selected_sublayer = true;
		ui._dispatch_event(@change, EventType::CHANGE, this);
		
		auto_hide_layer_selector();
	}
	
	private void on_layer_selector_close(EventInfo@ event)
	{
		_open = false;
		string event_type;
		
		if(!canceled)
		{
			accept_layers_selection();
			event_type = EventType::SELECT;
		}
		else
		{
			if(_layer_select.type == LayerSelectorType::Layers || _layer_select.type == LayerSelectorType::Both)
			{
				layer_select.set_layers_selected(@selected_layers, true, true);
			}
			
			if(_layer_select.type == LayerSelectorType::SubLayers || _layer_select.type == LayerSelectorType::Both)
			{
				layer_select.set_sub_layers_selected(@selected_sub_layers, true, true);
			}
			
			canceled = false;
			event_type = EventType::CANCEL;
			update_labels();
		}
		
		@_layer_select._control = @this;
		ui._dispatch_event(@select, event_type, this);
	}
	
	private void auto_hide_layer_selector()
	{
		if(!_auto_close)
			return;
		
		if(_layer_select.type == LayerSelectorType::Both)
		{
			if(!_layer_select.multi_select)
			{
				if(has_selected_layer && has_selected_sublayer)
				{
					hide();
				}
			}
			else if(_layer_select.num_layers_selected() > 0 && _layer_select.num_sub_layers_selected() > 0)
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