#include 'PopupPosition.cpp';
#include 'PopupHideType.cpp';
#include 'TooltipTriggerType.cpp';

class TooltipOptions
{
	
	string _id = '';
	
	private UI@ ui;
	
	// Content can be any element
	private Element@ _content;
	// Content can be set to a string and this will be used instead of "content".
	// Behind the scenes a Label with some default settings element will be created if need
	// and used to display the string
	private string _content_str;
	
	TextAlign text_align_h = TextAlign::Left;
	
	bool enabled = true;
	/// Normally only the top most element element will trigger tooltips.
	/// If this is set to true, this tooltip will trigger when the mouse is over
	/// this element or any of its descendants
	bool trigger_when_hovered = false;
	bool interactable;
	PopupPosition position;
	TooltipTriggerType trigger_type;
	PopupHideType hide_type;
	bool follow_mouse;
	
	private float _fade_max;
	private float _offset_max;
	private float _spacing;
	
	private bool has_content_element;
	private Label@ content_str_label;
	
	TooltipOptions(
		UI@ ui, Element@ content, bool interactable=false, PopupPosition position=PopupPosition::Above,
		TooltipTriggerType trigger_type=TooltipTriggerType::MouseOver,
		PopupHideType hide_type=PopupHideType::MouseLeave,
		bool follow_mouse=false)
	{
		init(
			ui, interactable, position,
			trigger_type, hide_type, follow_mouse);
		
		@this._content = content;
		has_content_element = true;
	}
	
	TooltipOptions(
		UI@ ui, const string content, bool interactable=false, PopupPosition position=PopupPosition::Above,
		TooltipTriggerType trigger_type=TooltipTriggerType::MouseOver,
		PopupHideType hide_type=PopupHideType::MouseLeave,
		bool follow_mouse=false)
	{
		init(
			ui, interactable, position,
			trigger_type, hide_type, follow_mouse);
		
		this._content_str = content;
		has_content_element = false;
	}
	
	private void init(
		UI@ ui, bool interactable, PopupPosition position,
		TooltipTriggerType trigger_type,
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
	
	float fade_max
	{
		get { return _fade_max; }
		set { _fade_max = max(0, value); }
	}
	
	float offset_max
	{
		get { return _offset_max; }
		set { _offset_max = max(0, value); }
	}
	
	float spacing
	{
		get { return _spacing; }
		set { _spacing = value; }
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
	}
	
	void set_content(string content)
	{
		_content_str = content;
		has_content_element = false;
		
		if(@content_str_label != null)
		{
			content_str_label.text = _content_str;
		}
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
	
	Element@ get_contenet_element()
	{
		if(has_content_element)
			return _content;
		
		if(@content_str_label == null)
		{
			@content_str_label = ui._label_pool.get(
				ui, _content_str, true,
				text_align_h,
				GraphicAlign::Left, GraphicAlign::Top,
				ui.style.tooltip_text_scale, ui.style.tooltip_text_colour,
				ui.style.tooltip_font, ui.style.tooltip_text_size);
		}
		
		return content_str_label;
	}
	
	void _on_tooltip_hide()
	{
		if(@content_str_label != null)
		{
			ui._label_pool.release(@content_str_label);
			@content_str_label = null;
		}
	}
	
}