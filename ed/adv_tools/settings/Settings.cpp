namespace Settings
{
	
	const float IconSize = 22;
	
	const float ToolbarIconSize = 33;
	const float UIFadeAlpha = 0.35;
	const float UIFadeSpeed = 4;
	
	const float SelectRectLineWidth		= 1.5;
	const uint SelectRectFillColour		= 0x1144ff44;
	const uint SelectRectLineColour		= 0x55aaffaa;
	
	const float	RotationHandleOffset		= 18;
	const float	RotateHandleSize			= 5;
	const uint RotateHandleColour			= 0xaaffffff;
	const uint RotateHandleHighlightColour	= 0xaaea9c3f;
	const float	ScaleHandleSize				= 4;
	
	/// 
	const array<float> ScaleHandleOffsets = {
		// Right
		 1,  0,
		 1,  0,
		// Bottom Right
		 1,  1,
		 1,  1,
		// Bottom
		 0,  1,
		 0,  1,
		// Bottom Left
		-1,  1,
		-1,  1,
		// Left
		-1,  0,
		-1,  0,
		// Top Left
		-1, -1,
		-1, -1,
		// Top
		 0, -1,
		 0, -1,
		// Top Right
		 1, -1,
		 1, -1,
	};
	
}

class Setting
{
	[text] string key;
}

class FloatSetting : Setting
{
	[text] float value;
}

class BoolSetting : Setting
{
	[text] bool value;
}

class StringSetting : Setting
{
	[text] string value;
}