#include '../../../fonts.cpp';
#include '../../../enums/GlobalVirtualButton.cpp';
#include '../../../math/math.cpp';
#include '../../utils/GraphicAlign.cpp';
#include '../../events/Event.cpp';
#include '../Checkbox.cpp';
#include '../Label.cpp';
#include '../LockedContainer.cpp';
#include 'LayerSelectorSet.cpp';
#include 'LayerSelectorType.cpp';

class LayerSelector : LockedContainer
{
	
	Event layer_select;
	Event sub_layer_select;
	
	// Properties
	
	protected LayerSelectorType _type = LayerSelectorType(-1);
	// TODO: Turn this off by default
	protected bool _multi_select = true;
	/*DONE*/ protected bool _allow_deselect = true;
	// TODO: Turn this off by default
	/*DONE*/ protected bool _drag_select = true;
	protected int _align_v = GraphicAlign::Top;
	/*DONE*/ protected bool  _labels_first = true;
	/*DONE*/ protected float _label_spacing = NAN;
	/*DONE*/ protected float _layer_spacing = NAN;
	/*DONE*/ protected bool _toggle_on_press = true;
	/*DONE*/ protected int _select_layer_group_modifier = GlobalVirtualButton::Shift;
	
	protected bool _show_backdrop_layers = true;
	protected bool _show_parallax_layers = true;
	protected bool _show_background_layers = true;
	protected bool _show_entities_layer = true;
	protected bool _show_collision_layer = true;
	protected bool _show_foreground_layer = true;
	protected bool _show_ui_layers = true;
	
	protected int _min_sublayer = 0;
	protected int _max_sublayer = 24;
	
	// TODO: Turn these off by default
	protected bool _show_all_layers_toggle = true;
	protected bool _show_all_sub_layers_toggle = true;
	
	/*DONE*/ protected string _font = font::ENVY_BOLD;
	/*DONE*/ protected uint _font_size = 20;
	
	//
	
	protected bool validate_layout;
	
	protected LayerSelectorSet@ layers;
	protected LayerSelectorSet@ sub_layers;
	protected bool has_layers;
	protected bool has_sub_layers;
	
	LayerSelector(UI@ ui, const LayerSelectorType type=LayerSelectorType::Layers)
	{
		super(ui);
		
		background_colour = ui.style.normal_bg_clr;
		background_blur = true;
		
		this.type = type;
		
		validate_layout = true;
	}
	
	string element_type { get const override { return 'LayerSelector'; } }
	
	/// Whether the layer list, the sub layer list, or both are shown.
	LayerSelectorType type
	{
		get const { return _type; }
		set
		{
			if(_type == value)
				return;
			
			_type = value;
			
			has_layers = false;
			has_sub_layers = false;
			validate_layout = true;
			
			if(_type == LayerSelectorType::Layers || _type == LayerSelectorType::Both)
			{
				has_layers = true;
				initialise_layers_set();
				layers.visible = true;
			}
			else if(@layers != null)
			{
				layers.visible = false;
			}
			
			if(_type == LayerSelectorType::SubLayers || _type == LayerSelectorType::Both)
			{
				has_sub_layers = true;
				initialise_sub_layers_set();
				sub_layers.visible = true;
			}
			else if(@sub_layers != null)
			{
				sub_layers.visible = false;
			}
		}
	}
	
	/// If this and multi_select is set, multiple layers can be selected or deselected by clicking and dragging the mouse.
	bool drag_select
	{
		get const { return _drag_select; }
		set
		{
			if(_drag_select == value)
				return;
			
			if(has_layers)
				layers.drag_select = _drag_select;
			if(has_sub_layers)
				sub_layers.drag_select = _drag_select;
		}
	}
	
	/// Only relevant when both the layer and sublayer lists are shown.
	/// Controls how they are aligned vertically.
	int align_v
	{
		get const { return _align_v; }
		set
		{
			if(_align_v == value)
				return;
			
			_align_v = value;
			validate_layout = true;
		}
	}
	
	/// If true, layers will be toggled when pressed instead of when clicked
	bool toggle_on_press
	{
		get const { return _toggle_on_press; }
		set
		{
			if(_toggle_on_press == value)
				return;
			
			_toggle_on_press = value;
			
			if(has_layers)
				layers.update_toggle_on_press(_toggle_on_press);
			if(has_sub_layers)
				sub_layers.update_toggle_on_press(_toggle_on_press);
		}
	}
	
	/// Sets the GVB key that can be used to toggle entire layer groups.
	/// Set to -1 to disable.
	int select_layer_group_modifier
	{
		get const { return _select_layer_group_modifier; }
		set
		{
			if(_select_layer_group_modifier == value)
				return;
			
			_select_layer_group_modifier = value;
			
			if(has_layers)
				layers.select_layer_group_modifier = _select_layer_group_modifier;
			if(has_sub_layers)
				sub_layers.select_layer_group_modifier = _select_layer_group_modifier;
		}
	}
	
	/// If false, will prevent layers from being selected.
	/// Only has an effect when multi select is off.
	bool allow_deselect
	{
		get const { return _allow_deselect; }
		set
		{
			_allow_deselect = value;
			
			if(has_layers)
				layers.allow_deselect = _allow_deselect;
			if(has_sub_layers)
				sub_layers.allow_deselect = _allow_deselect;
		}
	}
	
	/// Controls whether the labels or checkboxes are displayed first on the left side
	bool labels_first
	{
		get const { return _labels_first; }
		set
		{
			_labels_first = value;
			
			if(has_layers)
				layers.update_labels_first(_labels_first);
			if(has_sub_layers)
				sub_layers.update_labels_first(_labels_first);
		}
	}
	
	/// Controls the spacing between labels and checkboxes
	float label_spacing
	{
		get const { return _label_spacing; }
		set
		{
			if(_label_spacing == value)
				return;
			
			_label_spacing = value;
			
			if(has_layers)
				layers.update_label_spacing(_label_spacing);
			if(has_sub_layers)
				sub_layers.update_label_spacing(_label_spacing);
		}
	}
	
	/// Controls the spacing between labels and checkboxes
	float layer_spacing
	{
		get const { return _layer_spacing; }
		set
		{
			if(_layer_spacing == value)
				return;
			
			_layer_spacing = value;
			
			if(has_layers)
				layers.update_layer_spacing(_layer_spacing);
			if(has_sub_layers)
				sub_layers.update_layer_spacing(_layer_spacing);
		}
	}
	
	/// Sets the font used for all layer labels
	void set_font(const string font, const uint size)
	{
		if(_font == font && _font_size == size)
			return;
		
		_font = font;
		_font_size = size;
		
		if(has_layers)
			layers.set_font(font, size);
		if(has_sub_layers)
			sub_layers.set_font(font, size);
		
		invalidate();
	}
	
	/// Returns true and sets colour if the specified layer has custom colour set
	bool get_layer_colour(const int layer, uint &out colour)
	{
		return layers.get_layer_colour(layer, colour);
	}
	
	/// Sets a custom colour for the specified layer. If end_layer is not -1 will set all layers between layer and end_layer (inclusive)
	/// Returns true if the layer colours was succesfully set.
	bool set_layer_colour(const int layer, const uint colour, const int end_layer=-1)
	{
		return layers.set_layer_colour(layer, colour, end_layer);
	}
	
	/// Clears the custom colour for the specified layer if it has any. If end_layer is not -1 will set all layers between layer and end_layer (inclusive)
	/// Returns true if the layer colours was succesfully cleared.
	bool clear_layer_colour(const int layer, const int end_layer=-1)
	{
		return layers.clear_layer_colour(layer, end_layer);
	}
	
	/// Returns true and sets colour if the specified sub layer has custom colour set
	bool get_sub_layer_colour(const int sub_layer, uint &out colour)
	{
		return layers.get_layer_colour(sub_layer, colour);
	}
	
	/// Sets a custom colour for the specified sub layer. If end_sub_layer is not -1 will set all layers between sub_layer and end_sub_layer (inclusive)
	/// Returns true if the sub layer colours was succesfully set.
	bool set_sub_layer_colour(const int sub_layer, const uint colour, const int end_sub_layer=-1)
	{
		return layers.set_layer_colour(sub_layer, colour, end_sub_layer);
	}
	
	/// Clears the custom colour for the specified sub layer if it has any. If end_sub_layer is not -1 will set all layers between sub_layer and end_sub_layer (inclusive)
	/// Returns true if the sub layer colours was succesfully cleared.
	bool clear_sub_layer_colour(const int sub_layer, const int end_sub_layer=-1)
	{
		return layers.clear_layer_colour(sub_layer, end_sub_layer);
	}
	
	/// Deselects all layers and returns the number that were changed
	int select_layers_none()
	{
		if(!has_layers)
			return 0;
		
		return layers.select_all();
	}
	
	/// Selects all layers and returns the number that were changed
	int select_layers_all()
	{
		if(!_multi_select || !has_layers)
			return 0;
		
		return layers.select_all();
	}
	
	// TODO: select all/none sublayers
	// TODO: Get number of selected layers and sub layers
	
	void set_selectable_sub_layers(int min, int max)
	{
		if(min < 0)
			min = 0;
		if(max > 24)
			max = 24;
		
		if(max < min)
		{
			const int tmin = min;
			min = max;
			max = tmin;
		}
		
		if(min == _min_sublayer && max == _max_sublayer)
			return;
		
		_min_sublayer = min;
		_max_sublayer = max;
		
		if(has_sub_layers)
		{
			rebuild_sub_layers();
		}
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(validate_layout)
		{
			float width = 0;
			float height = 0;
			
			if(has_layers && layers.validate_layout)
			{
				layers.do_layout();
				width += layers._width;
				height = max(layers._height, height);
				layers.x = ui.style.spacing;
				layers.y = ui.style.spacing;
			}
			
			if(has_sub_layers && sub_layers.validate_layout)
			{
				sub_layers.do_layout();
				width += sub_layers._width;
				height = max(sub_layers._height, height);
				sub_layers.x = ui.style.spacing;
				sub_layers.y = ui.style.spacing;
			}
			
			this.width = width + ui.style.spacing * 2;
			this.height = height + ui.style.spacing * 2;
			
			validate_layout = false;
		}
	}
	
	protected void invalidate()
	{
		validate_layout = true;
		
		if(has_layers)
			layers.validate_layout = true;
		if(has_sub_layers)
			layers.validate_layout = true;
	}
	
	protected void initialise_layers_set()
	{
		if(@layers != null)
			return;
		
		// 24 = 21 layers (0-20) + 2 ui layers + toggle all
		@layers = LayerSelectorSet(ui, this, 24, @layer_select, EventType::LAYER_SELECT);
		
		initialise_layers_set_generic(layers);
		rebuild_layers();
		
		Container::add_child(layers);
	}
	
	protected void initialise_sub_layers_set()
	{
		if(@sub_layers != null)
			return;
		
		// 26 = 25 sub layers (0-24) + toggle all
		@sub_layers = LayerSelectorSet(ui, this, 26, @sub_layer_select, EventType::SUB_LAYER_SELECT);
		
		initialise_layers_set_generic(sub_layers);
		rebuild_sub_layers();
		
		Container::add_child(sub_layers);
	}
	
	protected void initialise_layers_set_generic(LayerSelectorSet@ layers)
	{
		layers.allow_deselect				= _allow_deselect;
		layers.multi_select					= _multi_select;
		layers.drag_select					= _drag_select;
		layers.toggle_on_press				= _toggle_on_press;
		layers.select_layer_group_modifier	= _select_layer_group_modifier;
		
		layers.labels_first		= _labels_first;
		layers.label_spacing	= _label_spacing;
		layers.layer_spacing	= _layer_spacing;
		
		layers.font			= _font;
		layers.font_size	= _font_size;
	}
	
	protected void rebuild_layers()
	{
		layers.rebuild();
		
		layers.rebuild_checkboxes(0, 0, 5,   _show_backdrop_layers, true, 0xffe7e6a7);
		layers.rebuild_checkboxes(1, 6, 11,  _show_parallax_layers, true, 0xffbea7e7);
		layers.rebuild_checkboxes(2, 12, 17, _show_background_layers);
		layers.rebuild_checkboxes(3, 18, 18, _show_entities_layer, true, 0xff7bc4d9);
		layers.rebuild_checkboxes(4, 19, 19, _show_collision_layer, true, 0xff7bd98f);
		layers.rebuild_checkboxes(5, 20, 20, _show_foreground_layer);
		layers.rebuild_checkboxes(6, 21, 22, _show_ui_layers, true, 0xff818181);
		layers.rebuild_checkboxes(7, 23, 23, _show_all_layers_toggle && _multi_select);
		
		layers.rebuild_complete();
		validate_layout = true;
	}
	
	protected void rebuild_sub_layers()
	{
		sub_layers.rebuild();
		
		layers.rebuild_checkboxes(0, _min_sublayer, _max_sublayer, true);
		sub_layers.rebuild_hide_other(_min_sublayer, _max_sublayer);
		layers.rebuild_checkboxes(1, 24, 24, _show_all_sub_layers_toggle && _multi_select);
		
		sub_layers.rebuild_complete();
		validate_layout = true;
	}
	
}