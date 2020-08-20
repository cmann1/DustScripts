enum ModifierKey
{
	
	None	= 0,
	Ctrl	= 1,
	Shift	= 2,
	Alt		= 4,
	/// Normally a key will only not match if an unselected modifier key is pressed.
	/// Of the "Only" flag is set, the key will only fire if one or more of the set modifiers are pressed
	Only	= 8,
	
	All		= Ctrl | Shift | Alt,
	
}