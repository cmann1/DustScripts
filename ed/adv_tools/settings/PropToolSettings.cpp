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
	
	const float SelectRectLineWidth		= 1.5;
	
}