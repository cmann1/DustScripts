namespace PropToolSettings
{
	
	const float SmallPropRadius = 6;
	
	/// Custom pick radius for specific props. Can provide a larger default radius for smaller
	/// props to make them easier to pick
	const dictionary PropRadii = {
		// Vines
		{'1.5.4', SmallPropRadius},
		{'1.5.5', SmallPropRadius},
		// Mushrooms
		{'2.5.20', SmallPropRadius},
		{'2.5.21', SmallPropRadius},
		{'2.5.24', SmallPropRadius},
		// Wires  
		{'3.27.5', SmallPropRadius},
		{'3.27.6', SmallPropRadius},
		{'3.27.7', SmallPropRadius},
		{'3.27.8', SmallPropRadius}
	};
	
	const uint HighlightOverlayColour	= 0x5500eeff;
	const uint HighlightOutlineColour	= 0xff00eeff;
	const float HighlightOutlineWidth	= 1;
	
	const uint SelectOverlayColour		= 0x3344eeff;
	const uint SelectedOutlineColour	= 0xaa44eeff;
	const float SelectedOutlineWidth	= 0.75;
	
	const uint PendingAddOverlayColour		= 0x3344ff44;
	const uint PendingAddOutlineColour		= 0xaa44ff44;
	const float PendingAddOutlineWidth		= 1;
	
	const uint PendingRemoveOverlayColour	= 0x33ff4444;
	const uint PendingRemoveOutlineColour	= 0xaaff4444;
	const float PendingRemoveOutlineWidth	= 1;
	
	const float BoundingBoxLineWidth	= 1.5;
	const uint BoundingBoxColour		= 0x55ffffff;
	
	const float SelectRectLineWidth		= 1.5;
	const uint SelectRectFillColour		= 0x1144ff44;
	const uint SelectRectLineColour		= 0x5544ff44;
	
}