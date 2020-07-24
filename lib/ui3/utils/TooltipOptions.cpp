class TooltipOptions
{
	
	Element@ content;
	bool interactable;
	TooltipPosition position;
	TooltipTriggerType trigger_type;
	TooltipHideType hide_type;
	
	private float _fade_max;
	private float _offset_max;
	private float _spacing;
	
	TooltipOptions(
		Element@ content, bool interactable=false, TooltipPosition position=TooltipPosition::Above,
		TooltipTriggerType trigger_type=TooltipTriggerType::MouseOver,
		TooltipHideType hide_type=TooltipHideType::MouseLeave)
	{
		@this.content		= content;
		this.position		= position;
		this.interactable	= interactable;
		this.trigger_type	= trigger_type;
		this.hide_type		= hide_type;
		
		fade_max 	= content.ui.style.tooltip_fade_frames;
		offset_max 	= content.ui.style.tooltip_fade_offset;
		spacing 	= content.ui.style.tooltip_default_spacing;
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
	
}