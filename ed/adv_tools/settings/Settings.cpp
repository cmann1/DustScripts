namespace Settings
{
	
	const float ToolbarIconSize = 33;
	const float UIFadeAlpha = 0.35;
	const float UIFadeSpeed = 4;
	
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