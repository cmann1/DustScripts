namespace Settings
{
	
	const float IconSize = 22;
	
	const float ToolbarIconSize = 33;
	const float UIFadeAlpha = 0.35;
	const float UIFadeSpeed = 4;
	
	const float SelectRectLineWidth		= 1.5;
	const uint SelectRectFillColour		= 0x1144ff44;
	const uint SelectRectLineColour		= 0x55aaffaa;
	
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