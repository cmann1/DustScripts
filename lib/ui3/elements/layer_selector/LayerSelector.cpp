#include '../../../fonts.cpp';
#include '../../../enums/GVB.cpp';
#include '../../../math/math.cpp';
#include '../../utils/GraphicAlign.cpp';
#include '../../events/Event.cpp';
#include '../Element.cpp';
#include '../Checkbox.cpp';
#include '../Label.cpp';
#include '../LockedContainer.cpp';
#include 'LayerSelectorSet.cpp';
#include 'LayerSelectorType.cpp';
#include 'ILayerSelectorControl.cpp';

namespace LayerSelector { const string TYPE_NAME = 'LayerSelector'; }

class LayerSelector : LockedContainer
{
	
	/// Internal. Used by the LayerButton to stay in sync with its LayerSelector
	ILayerSelectorControl@ _control;
	
	Event layer_select;
	Event sub_layer_select;
	
	// Properties
	
	protected LayerSelectorType _type = LayerSelectorType::Uninitialised;
	protected bool _layers_multi_select = true;
	protected bool _sub_layers_multi_select = true;
	protected uint _layers_min_select = 0;
	protected uint _sub_layers_min_select = 0;
	protected bool _drag_select = true;
	protected float _align_v = GraphicAlign::Top;
	protected bool  _labels_first = true;
	protected float _label_spacing = NAN;
	protected float _layer_spacing = NAN;
	protected float _padding = NAN;
	protected bool _toggle_on_press = true;
	protected int _select_layer_group_modifier = GVB::Control;
	protected int _select_range_modifier = GVB::Shift;
	
	protected bool _show_all_layers_toggle = false;
	protected bool _show_all_sub_layers_toggle = false;
	protected bool _toggle_all_top = true;
	
	protected string _font = font::ENVY_BOLD;
	protected uint _font_size = 20;
	
	protected bool _individual_backgrounds = true;
	protected uint _shadow_colour;
	
	//
	
	protected LayerSelectorSet@ layers;
	protected LayerSelectorSet@ sub_layers;
	protected bool has_layers;
	protected bool has_sub_layers;
	
	LayerSelector(UI@ ui, const LayerSelectorType type=LayerSelectorType::Layers)
	{
		super(ui);
		
		this.type = type;
		
		mouse_self = !_individual_backgrounds;
		
		validate_layout = true;
	}
	
	string element_type { get const override { return LayerSelector::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Background
	// ///////////////////////////////////////////////////////////////////
	
	uint background_colour
	{
		get { return _individual_backgrounds ? (has_layers ? layers.background_colour : sub_layers.background_colour) : cast<Element@>(this).background_colour; }
		set
		{
			if(_individual_backgrounds)
			{
				if(has_layers)
					layers.background_colour = value;
				if(has_sub_layers)
					sub_layers.background_colour = value;
			}
			else
			{
				cast<Element@>(this).background_colour = value;
			}
		}
	}
	
	bool background_blur
	{
		get { return _individual_backgrounds ? (has_layers ? layers.background_blur : sub_layers.background_blur) : cast<Element@>(this).background_blur; }
		set
		{
			if(_individual_backgrounds)
			{
				if(has_layers)
					layers.background_blur = value;
				if(has_sub_layers)
					sub_layers.background_blur = value;
			}
			else
			{
				cast<Element@>(this).background_blur = value;
			}
		}
	}
	
	float blur_inset
	{
		get { return _individual_backgrounds ? (has_layers ? layers.blur_inset : sub_layers.blur_inset) : cast<Element@>(this).blur_inset; }
		set
		{
			if(_individual_backgrounds)
			{
				if(has_layers)
					layers.blur_inset = value;
				if(has_sub_layers)
					sub_layers.blur_inset = value;
			}
			else
			{
				cast<Element@>(this).blur_inset = value;
			}
		}
	}
	
	uint border_colour
	{
		get { return _individual_backgrounds ? (has_layers ? layers.border_colour : sub_layers.border_colour) : cast<Element@>(this).border_colour; }
		set
		{
			if(_individual_backgrounds)
			{
				if(has_layers)
					layers.border_colour = value;
				if(has_sub_layers)
					sub_layers.border_colour = value;
			}
			else
			{
				cast<Element@>(this).border_colour = value;
			}
		}
	}
	
	float border_size
	{
		get { return _individual_backgrounds ? (has_layers ? layers.border_size : sub_layers.border_size) : cast<Element@>(this).border_size; }
		set
		{
			if(_individual_backgrounds)
			{
				if(has_layers)
					layers.border_size = value;
				if(has_sub_layers)
					sub_layers.border_size = value;
			}
			else
			{
				cast<Element@>(this).border_size = value;
			}
		}
	}
	
	uint shadow_colour
	{
		get { return _individual_backgrounds ? (has_layers ? layers.shadow_colour : sub_layers.shadow_colour) : _shadow_colour; }
		set
		{
			if(_individual_backgrounds)
			{
				if(has_layers)
					layers.shadow_colour = value;
				if(has_sub_layers)
					sub_layers.shadow_colour = value;
			}
			else
			{
				_shadow_colour = value;
			}
		}
	}
	
	/// Controls whether the background, border, and shadow are drawn around the entire LayerSelector,
	/// or separately around the layers and sublayers.
	bool individual_backgrounds
	{
		get const { return _individual_backgrounds; }
		set
		{
			if(_individual_backgrounds == value)
				return;
			
			_individual_backgrounds = value;
			mouse_self = !_individual_backgrounds;
			
			Element@ base = cast<Element@>(this);
			
			if(_individual_backgrounds)
			{
				if(has_layers)
				{
					layers.background_colour = base.background_colour;
					layers.background_blur = base.background_blur;
					layers.border_colour = base.border_colour;
					layers.border_size = base.border_size;
					layers.shadow_colour = _shadow_colour;
				}
				
				if(has_sub_layers)
				{
					sub_layers.background_colour = base.background_colour;
					sub_layers.background_blur = base.background_blur;
					sub_layers.border_colour = base.border_colour;
					sub_layers.border_size = base.border_size;
					sub_layers.shadow_colour = _shadow_colour;
				}
				
				base.background_colour = 0;
				base.background_blur = false;
				base.border_colour = 0;
				base.border_size = 0;
				_shadow_colour = 0;
			}
			else
			{
				background_colour	= has_layers ? layers.background_colour : sub_layers.background_colour;
				background_blur		= has_layers ? layers.background_blur : sub_layers.background_blur;
				border_colour		= has_layers ? layers.border_colour : sub_layers.border_colour;
				border_size			= has_layers ? layers.border_size : sub_layers.border_size;
				_shadow_colour		= has_layers ? layers.shadow_colour : sub_layers.shadow_colour;
				
				if(has_layers)
					layers.clear_drawing_options();
				
				if(has_sub_layers)
					sub_layers.clear_drawing_options();
			}
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	/// Whether the layer list, the sub layer list, or both are shown.
	LayerSelectorType type
	{
		get const { return _type; }
		set
		{
			if(value == LayerSelectorType::Uninitialised || _type == value)
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
			
			if(@_control != null)
			{
				_control.on_layer_selector_type_change();
			}
		}
	}
	
	/// Sets whether or not multiple layers and sublayers can be selected at once
	/// If mutliple layers are selected and this is disabled, only the highest selected layer will remain selected.
	bool multi_select
	{
		get const
		{
			return _layers_multi_select && _sub_layers_multi_select;
		}
		set
		{
			if(_layers_multi_select == value && _sub_layers_multi_select == value)
				return;
			
			_layers_multi_select = value;
			_sub_layers_multi_select = value;
			
			if(has_layers)
				layers.update_multi_select(_layers_multi_select);
			if(has_sub_layers)
				sub_layers.update_multi_select(_sub_layers_multi_select);
		}
	}
	
	/// Sets whether or not multiple layers can be selected at once
	/// If mutliple layers are selected and this is disabled, only the highest selected layer will remain selected.
	bool multi_select_layers
	{
		get const { return _layers_multi_select; }
		set
		{
			if(_layers_multi_select == value)
				return;
			
			_layers_multi_select = value;
			
			if(has_layers)
				layers.update_multi_select(_layers_multi_select);
		}
	}
	
	/// Sets whether or not multiple sublayers can be selected at once
	/// If mutliple layers are selected and this is disabled, only the highest selected layer will remain selected.
	bool multi_select_sub_layers
	{
		get const { return _sub_layers_multi_select; }
		set
		{
			if(_sub_layers_multi_select == value)
				return;
			
			_sub_layers_multi_select = value;
			
			if(has_layers)
				sub_layers.update_multi_select(_sub_layers_multi_select);
		}
	}
	
	/// The minimum number of selected layers. Set to zero to allow layers to be deselected, 0xffffffff to never allow layers to be deselected,
	/// or any other number to prevent less than that to from being selected at any given time.
	uint min_select
	{
		get const { return _layers_min_select; }
		set
		{
			if(_layers_min_select == value && _sub_layers_min_select == value)
				return;
			
			_layers_min_select = value;
			_sub_layers_min_select = value;
			
			if(has_layers)
				layers.min_select = _layers_min_select;
			if(has_sub_layers)
				sub_layers.min_select = _sub_layers_min_select;
		}
	}
	
	/// The minimum number of selected layers. Set to zero to allow layers to be deselected, 0xffffffff to never allow layers to be deselected,
	/// or any other number to prevent less than that to from being selected at any given time.
	uint min_select_layers
	{
		get const { return _layers_min_select; }
		set
		{
			if(_layers_min_select == value)
				return;
			
			_layers_min_select = value;
			
			if(has_layers)
				layers.min_select = _layers_min_select;
		}
	}
	
	/// The minimum number of selected sublayers. Set to zero to allow layers to be deselected, 0xffffffff to never allow layers to be deselected,
	/// or any other number to prevent less than that to from being selected at any given time.
	uint min_select_sub_layers
	{
		get const { return _sub_layers_min_select; }
		set
		{
			if(_sub_layers_min_select == value)
				return;
			
			_sub_layers_min_select = value;
			
			if(has_sub_layers)
				sub_layers.min_select = _sub_layers_min_select;
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
			
			_drag_select = value;
			
			if(has_layers)
				layers.drag_select = _drag_select;
			if(has_sub_layers)
				sub_layers.drag_select = _drag_select;
		}
	}
	
	/// Only relevant when both the layer and sublayer lists are shown.
	/// Controls how they are aligned vertically.
	float align_v
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
			
			if(@layers != null)
				layers.update_toggle_on_press(_toggle_on_press);
			if(@sub_layers != null)
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
	
	/// Sets the GVB key that can be used to select a range of layers.
	/// Set to -1 to disable.
	int select_range_modifier
	{
		get const { return _select_range_modifier; }
		set
		{
			if(_select_range_modifier == value)
				return;
			
			_select_range_modifier = value;
			
			if(has_layers)
				layers.select_range_modifier = _select_range_modifier;
			if(has_sub_layers)
				sub_layers.select_range_modifier = _select_range_modifier;
		}
	}
	
	/// Controls whether the labels or checkboxes are displayed first on the left side
	bool labels_first
	{
		get const { return _labels_first; }
		set
		{
			if(_labels_first == value)
				return;
			
			_labels_first = value;
			
			if(has_layers)
				layers.update_labels_first(_labels_first);
			if(has_sub_layers)
				sub_layers.update_labels_first(_labels_first);
		}
	}
	
	/// The spacing between labels and checkboxes
	float label_spacing
	{
		get const { return _label_spacing; }
		set
		{
			if(_label_spacing == value)
				return;
			
			_label_spacing = value;
			
			if(@layers != null)
				layers.update_label_spacing(_label_spacing);
			if(@sub_layers != null)
				sub_layers.update_label_spacing(_label_spacing);
		}
	}
	
	/// The spacing between layers
	float layer_spacing
	{
		get const { return _layer_spacing; }
		set
		{
			if(_layer_spacing == value)
				return;
			
			_layer_spacing = value;
			
			if(@layers != null)
				layers.update_layer_spacing(_layer_spacing);
			if(@sub_layers != null)
				sub_layers.update_layer_spacing(_layer_spacing);
		}
	}
	
	/// The spacing around layers and sublayers
	float padding
	{
		get const { return _padding; }
		set
		{
			if(_padding == value)
				return;
			
			_padding = value;
			
			if(has_layers)
				layers.padding = _padding;
			if(has_sub_layers)
				sub_layers.padding = _padding;
			
			invalidate();
		}
	}
	
	/// Whether not to show the toggle all layers checkbox
	bool show_all_layers_toggle
	{
		get const { return _show_all_layers_toggle; }
		set
		{
			if(_show_all_layers_toggle == value)
				return;
			
			_show_all_layers_toggle = value;
			
			if(has_layers && layers.update_toggle_all_visibility(_show_all_layers_toggle))
			{
				rebuild_layers();
			}
		}
	}
	
	/// Whether not to show the toggle all sublayers checkbox
	bool show_all_sub_layers_toggle
	{
		get const { return _show_all_sub_layers_toggle; }
		set
		{
			if(_show_all_sub_layers_toggle == value)
				return;
			
			_show_all_sub_layers_toggle = value;
			
			if(has_sub_layers && sub_layers.update_toggle_all_visibility(_show_all_sub_layers_toggle))
			{
				rebuild_sub_layers();
			}
		}
	}
	
	/// The toggle all checkboxes can be displayed at the bottom or the top
	bool toggle_all_top
	{
		get const { return _toggle_all_top; }
		set
		{
			if(_toggle_all_top == value)
				return;
			
			_toggle_all_top = value;
			
			if(has_layers)
				layers.toggle_all_top = _toggle_all_top;
			if(has_sub_layers)
				sub_layers.toggle_all_top = _toggle_all_top;
			
			invalidate();
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
	
	// ///////////////////////////////////////////////////////////////////
	// Colours
	// ///////////////////////////////////////////////////////////////////
	
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
	
	/// Sets all layer labels to the default colours
	void reset_default_layer_colour()
	{
		if(@layers != null)
		{
			layers.reset_default_colours(true);
		}
	}
	
	/// Sets all sublayer labels to the default colours
	void reset_default_sub_layer_colour()
	{
		if(@sub_layers != null)
		{
			sub_layers.reset_default_colours(false);
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Layer visiblity
	// ///////////////////////////////////////////////////////////////////
	
	// Get
	
	/// Returns true if the given layer is visible
	bool get_layer_visibility(const int layer)
	{
		return layers.get_visible_count(layer, layer) > 0;
	}
	
	/// Returns true if the given sub layer is visible
	bool get_sub_layer_visibility(const int layer)
	{
		return sub_layers.get_visible_count(layer, layer) > 0;
	}
	
	/// Returns the number of visible layers in the given range
	int get_layer_visibility(const int start_layer, const int end_layer)
	{
		return layers.get_visible_count(start_layer, end_layer);
	}
	
	/// Returns the number of visible sublayers in the given range
	int get_sub_layer_visibility(const int start_layer, const int end_layer)
	{
		return sub_layers.get_visible_count(start_layer, end_layer);
	}
	
	// Set
	
	/// Sets the visibility of the given layer. Returns true if anything changed.
	bool set_layer_visibility(const int layer, const bool visible)
	{
		return update_layers_visiblity(layers, layer, layer, visible) != 0;
	}
	
	/// Sets the visibility of the given layer. Returns true if anything changed.
	bool set_sub_layer_visibility(const int sub_layer, const bool visible)
	{
		return update_layers_visiblity(sub_layers, sub_layer, sub_layer, visible) != 0;
	}
	
	/// Sets the visibility of the layers between start_layer and end_layer inclusive.
	/// Returns the number of changed layers.
	int set_layer_visibility(const int start_layer, const int end_layer, const bool visible)
	{
		return update_layers_visiblity(layers, start_layer, end_layer, visible);
	}
	
	/// Sets the visibility of the sublayers between start_layer and end_layer inclusive.
	/// Returns the number of changed layers.
	int set_sub_layer_visibility(const int start_layer, const int end_layer, const bool visible)
	{
		return update_layers_visiblity(sub_layers, start_layer, end_layer, visible);
	}
	
	// Groups get
	
	/// Get the number of visible backdrop layers checkboxes
 	int get_backdrop_layers_visibility()	{ return layers.get_visible_count(0, 5 ); }
	/// Get the number of visible parallax layers checkboxes
	int get_parallax_layers_visibility()	{ return layers.get_visible_count(6, 11); }
	/// Get the number of visible background layers checkboxes
	int get_background_layers_visibility()	{ return layers.get_visible_count(12, 17); }
	/// Get the number of visible entity layers checkboxes
	int get_entities_layer_visibility()		{ return layers.get_visible_count(18, 18); }
	/// Get the number of visible collision layers checkboxes
	int get_collision_layer_visibility()	{ return layers.get_visible_count(19, 19); }
	/// Get the number of visible foreground layers checkboxes
	int get_foreground_layer_visibility()	{ return layers.get_visible_count(20, 20); }
	/// Get the number of visible ui layers checkboxes
	int get_ui_layers_visibility()			{ return layers.get_visible_count(21, 22); }
	
	// Groups set
	
	/// Set the visibility of all bakground layer checkboxes
 	bool set_backdrop_layers_visibility(const bool visible)		{ return update_layers_visiblity(layers, 0, 5,   visible) > 0; }
	/// Set the visibility of all parallax layer checkboxes
	bool set_parallax_layers_visibility(const bool visible)		{ return update_layers_visiblity(layers, 6, 11,  visible) > 0; }
	/// Set the visibility of all background layer checkboxes
	bool set_background_layers_visibility(const bool visible)	{ return update_layers_visiblity(layers, 12, 17, visible) > 0; }
	/// Set the visibility of all entity layer checkboxes
	bool set_entities_layer_visibility(const bool visible)		{ return update_layers_visiblity(layers, 18, 18, visible) > 0; }
	/// Set the visibility of all collision layer checkboxes
	bool set_collision_layer_visibility(const bool visible)		{ return update_layers_visiblity(layers, 19, 19, visible) > 0; }
	/// Set the visibility of all foreground layer checkboxes
	bool set_foreground_layer_visibility(const bool visible)	{ return update_layers_visiblity(layers, 20, 20, visible) > 0; }
	/// Set the visibility of all ui layer checkboxes
	bool set_ui_layers_visibility(const bool visible)			{ return update_layers_visiblity(layers, 21, 22, visible) > 0; }
	
	// ///////////////////////////////////////////////////////////////////
	// Selection
	// ///////////////////////////////////////////////////////////////////
	
	/// Deselects all layers and returns the number that were changed.
	int select_layers_none(const bool trigger_events=true, const bool ignore_min_select=false)
	{
		return @layers != null ? layers.select_none(trigger_events, ignore_min_select) : 0;
	}
	
	/// Selects all layers and returns the number that were changed.
	int select_layers_all(const bool trigger_events=true)
	{
		return @layers != null && layers.multi_select ? layers.select_all(trigger_events) : 0;
	}
	
	/// Deselects all sub layers and returns the number that were changed.
	int select_sub_layers_none(const bool trigger_events=true, const bool ignore_min_select=false)
	{
		return @sub_layers != null ? sub_layers.select_none(trigger_events, ignore_min_select) : 0;
	}
	
	/// Selects all sub layers and returns the number that were changed.
	int select_sub_layers_all(const bool trigger_events=true)
	{
		return @sub_layers != null && sub_layers.multi_select ? sub_layers.select_all(trigger_events) : 0;
	}
	
	// Set range
	
	/// Sets the selected state of the given layer and returns true if it was actually changed.
	bool set_layer_selected(const int layer, const bool selected=true, const bool trigger_event=true, const bool ignore_min_select=false)
	{
		return @layers != null ? layers.initialise_states(layer, layer, selected, -1, trigger_event, ignore_min_select) == 1 : false;
	}
	
	/// Sets the selected state of all layers in the range and returns how many were actually changed.
	int set_layers_selected(const int start_layer, const int end_layer, const bool selected=true, const bool trigger_event=true, const bool ignore_min_select=false)
	{
		return @layers != null ? layers.initialise_states(start_layer, end_layer, selected, -1, trigger_event, ignore_min_select) : 0;
	}
	
	/// Sets the selected state of all layers from the given array and returns how many were actually changed.
	/// Hidden layer checkboxes are not skipped.
	int set_layers_selected(const array<bool>@ selected, const bool trigger_event=true, const bool ignore_min_select=false)
	{
		return @layers != null ? layers.initialise_states(@selected, trigger_event, ignore_min_select) : 0;
	}
	
	/// Sets the selected state of the given sublayer and returns true if it was actually changed.
	bool set_sub_layer_selected(const int layer, const bool selected=true, const bool trigger_event=true, const bool ignore_min_select=false)
	{
		return @sub_layers != null ? sub_layers.initialise_states(layer, layer, selected, -1, trigger_event, ignore_min_select) == 1 : false;
	}
	
	/// Sets the selected state of all sublayers in the range and returns how many were actually changed.
	int set_sub_layers_selected(const int start_layer, const int end_layer, const bool selected=true, const bool trigger_event=true, const bool ignore_min_select=false)
	{
		return @sub_layers != null ? sub_layers.initialise_states(start_layer, end_layer, selected, -1, trigger_event, ignore_min_select) : 0;
	}
	
	/// Sets the selected state of all sublayers from the given array and returns how many were actually changed.
	/// Hidden layer checkboxes are not skipped.
	int set_sub_layers_selected(const array<bool>@ selected, const bool trigger_event=true, const bool ignore_min_select=false)
	{
		return @sub_layers != null ? sub_layers.initialise_states(@selected, trigger_event, ignore_min_select) : 0;
	}
	
	// Get range
	
	/// Gets the selected state of all layers. Hidden layer checkboxes are not skipped.
	void get_layers_selected(array<bool>@ selected)
	{
		if(@layers != null)
		{
			layers.get_selected(@selected);
		}
	}
	
	/// Gets the selected state of all sublayers. Hidden layer checkboxes are not skipped.
	void get_sub_layers_selected(array<bool>@ selected)
	{
		if(@sub_layers != null)
		{
			sub_layers.get_selected(@selected);
		}
	}
	
	/// Returns the total number of selected layers.
	int num_layers_selected()
	{
		return @layers != null ? layers.count_selected() : 0;
	}
	
	/// Returns the number of selected layers in the given range.
	int num_layers_selected(const int start_layer, const int end_layer)
	{
		return @layers != null ? layers.count_selected(start_layer, end_layer) : 0;
	}
	
	/// Returns true if the given layer is selected
	bool is_layer_selected(const int layer)
	{
		return @layers != null ? layers.count_selected(layer, layer) == 1 : false;
	}
	
	/// Returns the total number of selected sublayers.
	int num_sub_layers_selected()
	{
		return @sub_layers != null ? sub_layers.count_selected() : 0;
	}
	
	/// Returns the number of selected sublayers in the given range.
	int num_sub_layers_selected(const int start_layer, const int end_layer)
	{
		return @sub_layers != null ? sub_layers.count_selected(start_layer, end_layer) : 0;
	}
	
	/// Returns true if the given sublayer is selected
	bool is_sub_layer_selected(const int layer)
	{
		return @sub_layers != null ? sub_layers.count_selected(layer, layer) == 1 : false;
	}
	
	// Layer groups set
	
	/// Set all backdrop layers selected state
	int set_backdrop_layers_selected(const bool selected, const bool trigger_event=true, const bool ignore_min_select=false)
		{ return @layers != null ? layers.initialise_states(0, 5,   selected, -1, trigger_event, ignore_min_select) : 0; }
	/// Set all parallax layers selected state
	int set_parallax_layers_selected(const bool selected, const bool trigger_event=true, const bool ignore_min_select=false)
		{ return @layers != null ? layers.initialise_states(6, 11,  selected, -1, trigger_event, ignore_min_select) : 0; }
	/// Set all background layers selected state
	int set_background_layers_selected(const bool selected, const bool trigger_event=true, const bool ignore_min_select=false)
		{ return @layers != null ? layers.initialise_states(12, 17, selected, -1, trigger_event, ignore_min_select) : 0; }
	/// Set all entities layers selected state
	int set_entities_layer_selected(const bool selected, const bool trigger_event=true, const bool ignore_min_select=false)
		{ return @layers != null ? layers.initialise_states(18, 18, selected, -1, trigger_event, ignore_min_select) : 0; }
	/// Set all collision layers selected state
	int set_collision_layer_selected(const bool selected, const bool trigger_event=true, const bool ignore_min_select=false)
		{ return @layers != null ? layers.initialise_states(19, 19, selected, -1, trigger_event, ignore_min_select) : 0; }
	/// Set all foreground layers selected state
	int set_foreground_layer_selected(const bool selected, const bool trigger_event=true, const bool ignore_min_select=false)
		{ return @layers != null ? layers.initialise_states(20, 20, selected, -1, trigger_event, ignore_min_select) : 0; }
	/// Set all ui layers selected state
	int set_ui_layers_selected(const bool selected, const bool trigger_event=true, const bool ignore_min_select=false)
		{ return @layers != null ? layers.initialise_states(21, 22, selected, -1, trigger_event, ignore_min_select) : 0; }
	
	// Layer groups get
	
	/// Get the number of selected backdrop layers
	int num_backdrop_layers_selected()
		{ return @layers != null ? layers.count_selected(0, 5) : 0; }
	/// Get the number of selected parallax layers
	int num_parallax_layers_selected()
		{ return @layers != null ? layers.count_selected(6, 11) : 0; }
	/// Get the number of selected background layers
	int num_background_layers_selected()
		{ return @layers != null ? layers.count_selected(12, 17) : 0; }
	/// Get the number of selected entity layers
	int num_entities_layer_selected()
		{ return @layers != null ? layers.count_selected(18, 18) : 0; }
	/// Get the number of selected collision layers
	int num_collision_layer_selected()
		{ return @layers != null ? layers.count_selected(19, 19) : 0; }
	/// Get the number of selected foreground layers
	int num_foreground_layer_selected()
		{ return @layers != null ? layers.count_selected(20, 20) : 0; }
	/// Get the number of selected ui layers
	int num_ui_layers_selected()
		{ return @layers != null ? layers.count_selected(21, 22) : 0; }
	
	// Get/set selected
	
	/// Sets a single layer as the only selected layer. Returns true if something changed.
	/// If layer < 0 will select none.
	bool set_selected_layer(const int layer, const bool trigger_event=true, const bool ignore_min_select=false)
	{
		return @layers != null ? layers.set_selected(layer, trigger_event, ignore_min_select) : false;
	}
	
	/// Sets a single sublayer as the only selected layer. Returns true if something changed.
	/// If layer < 0 will select none.
	bool set_selected_sub_layer(const int layer, const bool trigger_event=true, const bool ignore_min_select=false)
	{
		return @sub_layers != null ? sub_layers.set_selected(layer, trigger_event, ignore_min_select) : false;
	}
	
	/// Returns the selected layer index or -1 if none are selected. If multiple are selected returns the highest index.
	int get_selected_layer()
	{
		return @layers != null ? layers.get_selected() : 0;
	}
	
	/// Returns the selected sublayer index or -1 if none are selected. If multiple are selected returns the highest index.
	int get_selected_sub_layer()
	{
		return @sub_layers != null ? sub_layers.get_selected() : 0;
	}
	
	/// Returns the number of selected layers and populates results with the indices of all selected layers in order.
	/// results will be expanded as needed.
	int get_selected_layers(array<int>@ results)
	{
		return @layers != null ? layers.get_selected(@results) : 0;
	}
	
	/// Returns the number of selected sublayers and populates results with the indices of all selected sublayers in order.
	/// results will be expanded as needed.
	int get_selected_sub_layers(array<int>@ results)
	{
		return @sub_layers != null ? sub_layers.get_selected(@results) : 0;
	}
	
	/// Returns the number of selected layers in the given range and populates results with the indices in order.
	/// results will be expanded as needed.
	int get_selected_layers(array<int>@ results, const int start_layer, const int end_layer)
	{
		return @layers != null ? layers.get_selected(@results, start_layer, end_layer) : 0;
	}
	
	/// Returns the number of selected sublayers in the given range and populates results with the indices in order.
	/// results will be expanded as needed.
	int get_selected_sub_layers(array<int>@ results, const int start_layer, const int end_layer)
	{
		return @sub_layers != null ? sub_layers.get_selected(@results, start_layer, end_layer) : 0;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Misc
	// ///////////////////////////////////////////////////////////////////
	
	/// Resets most basic properties to their default values
	void reset(const bool reset_background_properties=true, const bool reset_label_colours=true)
	{
		multi_select_layers = true;
		multi_select_sub_layers = true;
		min_select_layers = 0;
		min_select_sub_layers = 0;
		drag_select = true;
		align_v = GraphicAlign::Top;
		labels_first = true;
		label_spacing = NAN;
		layer_spacing = NAN;
		padding = NAN;
		toggle_on_press = true;
		select_layer_group_modifier = GVB::Control;
		select_range_modifier = GVB::Shift;
		
		show_all_layers_toggle = true;
		show_all_sub_layers_toggle = true;
		toggle_all_top = true;
		
		set_font(font::ENVY_BOLD, 20);
		
		if(reset_background_properties)
		{
			individual_backgrounds = true;
			shadow_colour = 0;
			background_colour = 0;
			background_blur = false;
			border_colour = 0;
			border_size = 0;
		}
		
		if(reset_label_colours)
		{
			reset_default_layer_colour();
			reset_default_sub_layer_colour();
		}
		
		if(@layers != null)
			layers.previous_select_index = -1;
		if(@sub_layers != null)
			sub_layers.previous_select_index = -1;
		
		validate_layout = true;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Protected/Internal
	// ///////////////////////////////////////////////////////////////////
	
	protected int update_layers_visiblity(LayerSelectorSet@ layers, const int start_layer, const int end_layer, const bool visible)
	{
		if(@layers == null)
			return 0;
		
		const int result = layers.update_visibility(start_layer, end_layer, visible);
		
		if(result > 0)
		{
			if(@layers == @this.layers)
				rebuild_layers();
			else
				rebuild_sub_layers();
		}
		
		return result;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		float width = 0;
		float height = 0;
		
		if(has_layers)
		{
//			if(layers.validate_layout)
//			{
//				layers.do_layout();
//			}
			
			height = max(layers._height, height);
			layers.x = 0;
			width += layers._width;
		}
		
		if(has_sub_layers)
		{
//			if(sub_layers.validate_layout)
//			{
//				sub_layers.do_layout();
//			}
			
			height = max(sub_layers._height, height);
			sub_layers.x = width;
			width += sub_layers._width;
		}
		
		if(has_layers)
		{
			layers.y = 0;//(height - layers._height) * align_v;
		}
		
		if(has_sub_layers)
		{
			sub_layers.y = 0;//(height - sub_layers._height) * align_v;
		}
		
		this.width = ceil(width);
		this.height = ceil(height);
		
		validate_layout = false;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		if(_shadow_colour != 0)
		{
			style.draw_rectangle(
				x1 + ui.style.shadow_offset_x, y1 + ui.style.shadow_offset_y,
				x2 + ui.style.shadow_offset_x, y2 + ui.style.shadow_offset_y,
				0, _shadow_colour);
		}
		
		Element::_draw(style, ctx);
	}
	
	protected void invalidate()
	{
		validate_layout = true;
		
		if(has_layers)
			layers.validate_layout = true;
		if(has_sub_layers)
			sub_layers.validate_layout = true;
	}
	
	protected void initialise_layers_set()
	{
		if(@layers == null)
		{
			// 24 = 21 layers (0-20) + 2 ui layers + toggle all
			@layers = LayerSelectorSet(ui, this, 24, @layer_select, EventType::LAYER_SELECT);
		}
		
		layers.initialise_layer_values(0, 5,   0, true);
		layers.initialise_layer_values(6, 11,  1, true);
		layers.initialise_layer_values(12, 17, 2, true);
		layers.initialise_layer_values(18, 18, 3, true);
		layers.initialise_layer_values(19, 19, 4, true);
		layers.initialise_layer_values(20, 20, 5, true);
		layers.initialise_layer_values(21, 22, 6, true);
		layers.initialise_layer_values(23, 23, 7, _show_all_layers_toggle && layers.multi_select);
		
		layers.min_select	= _layers_min_select;
		layers.multi_select	= _layers_multi_select;
		
		initialise_layers_set_generic(layers);
		rebuild_layers();
	}
	
	protected void initialise_sub_layers_set()
	{
		if(@sub_layers == null)
		{
			// 26 = 25 sub layers (0-24) + toggle all
			@sub_layers = LayerSelectorSet(ui, this, 26, @sub_layer_select, EventType::SUB_LAYER_SELECT);
		}
		
		sub_layers.initialise_layer_values(0, 24, 0, true);
		sub_layers.initialise_layer_values(25, 25, 1, _show_all_sub_layers_toggle && sub_layers.multi_select);
		
		sub_layers.min_select	= _sub_layers_min_select;
		sub_layers.multi_select	= _sub_layers_multi_select;
		
		initialise_layers_set_generic(sub_layers);
		rebuild_sub_layers();
	}
	
	protected void initialise_layers_set_generic(LayerSelectorSet@ layers)
	{
		layers.drag_select					= _drag_select;
		layers.toggle_on_press				= _toggle_on_press;
		layers.select_layer_group_modifier	= _select_layer_group_modifier;
		layers.select_range_modifier		= _select_range_modifier;
		
		layers.labels_first		= _labels_first;
		layers.label_spacing	= _label_spacing;
		layers.layer_spacing	= _layer_spacing;
		layers.padding			= _padding;
		
		layers.toggle_all_top	= _toggle_all_top;
		
		layers.font			= _font;
		layers.font_size	= _font_size;
		
		Container::add_child(layers);
	}
	
	protected void rebuild_layers()
	{
		layers.rebuild();
		
		layers.rebuild_checkboxes(true);
		
		layers.rebuild_complete();
		validate_layout = true;
	}
	
	protected void rebuild_sub_layers()
	{
		sub_layers.rebuild();
		
		sub_layers.rebuild_checkboxes(false);
		
		sub_layers.rebuild_complete();
		validate_layout = true;
	}
	
}
