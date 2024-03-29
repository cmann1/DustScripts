#include 'PopupPosition.cpp';
#include 'PopupTriggerType.cpp';
#include 'PopupHideType.cpp';
#include '../TextAlign.cpp';
#include '../events/Event.cpp';
#include '../events/IGenericEventTarget.cpp';

class PopupOptions : IGenericEventTarget
{
	
	string _id = '';
	
	bool enabled = true;
	PopupPosition position;
	/// If false the popup will instead be added to the normal UI layer along with other elements,
	/// instead of the overlay layer displayed above the normal UI.
	bool as_overlay = true;
	/// If true, the popup will try to stretch to fit the target
	bool stretch;
	PopupTriggerType trigger_type;
	PopupHideType hide_type;
	bool keep_open_while_pressed;
	/// Will force the popup to stay open for a single frame
	bool force_open;
	/// Will force the popup to stay open while this is true
	bool locked;
	/// Normally only the top most element element will trigger tooltips.
	/// If this is set to true, this tooltip will trigger when the mouse is over
	/// this element or any of its descendants
	bool trigger_when_hovered;
	/// If true and hide_type == MouseLeave, the popup will hide if the target becomes invisible or is removed from the hierarchy
	bool hide_when_target_removed = true;
	/// If true and the hide type is MouseLeave, the tooltip will not close until after the mouse enters it for the first time.
	bool wait_for_mouse;
	bool interactable;
	/// Controls the mouse_self option for this popup. See Element::mouse_self
	bool mouse_self = true;
	bool follow_mouse;
	TextAlign text_align_h = TextAlign::Left;
	/// Only relevant for PopupHideType::MouseDownOutside. If true any mouse button being pressed will close the tooltip
	bool any_mouse_down_button = true;
	bool allow_target_overlap = true;
	
	/// The amount of space around the outside. Leave as NAN to use the default style spacing.
	float padding_left = NAN;
	/// Same as padding_left.
	float padding_right = NAN;
	/// Same as padding_left.
	float padding_top = NAN;
	/// Same as padding_left.
	float padding_bottom = NAN;
	
	Event show;
	Event hide_start;
	Event hide;
	
	protected UI@ ui;
	
	// Content can be any element
	protected Element@ _content;
	protected Popup@ _popup;
	// Content can be set to a string and this will be used instead of "content".
	// Behind the scenes a Label with some default settings element will be created if need
	// and used to display the string
	protected string _content_str;
	protected bool has_content_element;
	/*protected*/ Label@ content_str_label;
	
	protected float _fade_max;
	protected float _offset_max;
	protected float _spacing;
	
	/*protected*/ uint _background_colour = 0;
	/*protected*/ bool _background_blur = false;
	/*protected*/ uint _border_colour = 0;
	/*protected*/ float _border_size = 0;
	/*protected*/ uint _shadow_colour = 0;
	/*protected*/ float _blur_inset = 0;
	/*protected*/ bool _has_background_colour = false;
	/*protected*/ bool _has_background_blur = false;
	/*protected*/ bool _has_border_colour = false;
	/*protected*/ bool _has_border_size = false;
	/*protected*/ bool _has_shadow_colour = false;
	/*protected*/ bool _has_blur_inset = false;
	
	/*protected*/ bool _invalidated = false;
	
	protected bool _popup_visible;
	
	PopupOptions(
		UI@ ui, Element@ content, bool interactable=false, PopupPosition position=PopupPosition::Above,
		PopupTriggerType trigger_type=PopupTriggerType::MouseOver,
		PopupHideType hide_type=PopupHideType::MouseLeave,
		bool follow_mouse=false)
	{
		init(
			ui, interactable, position,
			trigger_type, hide_type, follow_mouse);
		
		@this._content = content;
		has_content_element = true;
	}
	
	PopupOptions(
		UI@ ui, const string content, bool interactable=false, PopupPosition position=PopupPosition::Above,
		PopupTriggerType trigger_type=PopupTriggerType::MouseOver,
		PopupHideType hide_type=PopupHideType::MouseLeave,
		bool follow_mouse=false)
	{
		init(
			ui, interactable, position,
			trigger_type, hide_type, follow_mouse);
		
		this._content_str = content;
		has_content_element = false;
	}
	
	protected void init(
		UI@ ui, bool interactable, PopupPosition position,
		PopupTriggerType trigger_type,
		PopupHideType hide_type,
		bool follow_mouse)
	{
		@this.ui			= ui;
		this.position		= position;
		this.interactable	= interactable;
		this.trigger_type	= trigger_type;
		this.hide_type		= hide_type;
		this.follow_mouse	= follow_mouse;
		
		fade_max 	= ui.style.tooltip_fade_frames;
		offset_max 	= ui.style.tooltip_fade_offset;
		spacing 	= ui.style.tooltip_default_spacing;
	}
	
	string name { get const { return _id; } }
	
	Popup@ popup
	{
		get { return _popup; }
	}
	
	float fade_max
	{
		get { return _fade_max; }
		set { _fade_max = max(0.0, value); }
	}
	
	float offset_max
	{
		get { return _offset_max; }
		set { _offset_max = max(0.0, value); }
	}
	
	float spacing
	{
		get { return _spacing; }
		set { _spacing = value; }
	}
	
	float padding
	{
		set { padding_left = padding_right = padding_top = padding_bottom = value; }
	}
	
	void padding(const float padding)
	{
		padding_left = padding_right = padding_top = padding_bottom = padding;
	}
	
	void padding(const float padding_left_right, const float padding_top_bottom)
	{
		padding_left	= padding_left_right;
		padding_right	= padding_left_right;
		padding_top		= padding_top_bottom;
		padding_bottom	= padding_top_bottom;
	}
	
	void padding(const float padding_left, const float padding_right, const float padding_top, const float padding_bottom)
	{
		this.padding_left	= padding_left;
		this.padding_right	= padding_right;
		this.padding_top	= padding_top;
		this.padding_bottom	= padding_bottom;
	}
	
	uint background_colour
	{
		get const { return _background_colour; }
		set { _background_colour = value; _has_background_colour = true; }
	}
	bool background_blur
	{
		get const { return _background_blur; }
		set { _background_blur = value; _has_background_blur = true; }
	}
	uint border_colour
	{
		get const { return _border_colour; }
		set { _border_colour = value; _has_border_colour = true; }
	}
	float border_size
	{
		get const { return _border_size; }
		set { _border_size = value; _has_border_size = true; }
	}
	uint shadow_colour
	{
		get const { return _shadow_colour; }
		set { _shadow_colour = value; _has_shadow_colour = true; }
	}
	float blur_inset
	{
		get const { return _blur_inset; }
		set { _blur_inset = value; _has_blur_inset = true; }
	}
	
	bool has_background_colour
	{
		get const { return _has_background_colour; }
		set { _has_background_colour = value; }
	}
	bool has_background_blur
	{
		get const { return _has_background_blur; }
		set { _has_background_blur = value; }
	}
	bool has_border_colour
	{
		get const { return _has_border_colour; }
		set { _has_border_colour = value; }
	}
	bool has_border_size
	{
		get const { return _has_border_size; }
		set { _has_border_size = value; }
	}
	bool has_shadow_colour
	{
		get const { return _has_shadow_colour; }
		set { _has_shadow_colour = value; }
	}
	bool has_blur_inset
	{
		get const { return _has_blur_inset; }
		set { _has_blur_inset = value; }
	}
	
	bool popup_visible
	{
		get const { return _popup_visible; }
	}
	
	void set_content(Element@ content)
	{
		if(@content_str_label != null)
		{
			ui._label_pool.release(@content_str_label);
			@content_str_label = null;
		}
		
		@_content = content;
		has_content_element = true;
		invalidate();
	}
	
	void set_content(string content)
	{
		_content_str = content;
		has_content_element = false;
		
		if(@content_str_label != null)
		{
			content_str_label.text = _content_str;
		}
		
		invalidate();
	}
	
	Element@ content_element
	{
		get const { return _content; }
		set { set_content(value); }
	}
	
	string content_string
	{
		get const { return _content_str; }
		set { set_content(value); }
	}
	
	Element@ get_content_element()
	{
		if(has_content_element)
			return _content;
		
		if(@content_str_label == null)
		{
			@content_str_label = ui._label_pool.get(
				ui, _content_str, true,
				text_align_h,
				GraphicAlign::Left, GraphicAlign::Top,
				ui.style.tooltip_text_scale, ui.style.tooltip_has_text_colour, ui.style.tooltip_text_colour,
				ui.style.tooltip_font, ui.style.tooltip_text_size);
		}
		
		return content_str_label;
	}
	
	/// Clears all background, border, and shadow options.
	void clear_drawing()
	{
		background_colour = 0;
		background_blur = false;
		border_colour = 0;
		border_size = 0;
		shadow_colour = 0;
	}
	
	void _on_popup_show(Popup@ popup)
	{
		@_popup = popup;
		_popup_visible = true;
		ui._queue_event(@show, EventType::SHOW, @popup, @this);
	}
	
	void _on_popup_start_hide(Popup@ popup)
	{
		_popup_visible = false;
		ui._queue_event(@hide_start, EventType::SHOW, @popup, @this);
	}
	
	void _on_popup_hide(Popup@ popup)
	{
		if(@content_str_label != null)
		{
			ui._label_pool.release(@content_str_label);
			@content_str_label = null;
		}
		
		ui._queue_event(@hide, EventType::HIDE, @popup, @this);
		@_popup = null;
	}
	
	private void invalidate()
	{
		_invalidated = true;
	}
	
}
