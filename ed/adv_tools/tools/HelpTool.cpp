class HelpTool : Tool
{
	
	HelpTool(const string name)
	{
		super(name);
		
		selectable = false;
	}
	
	// //////////////////////////////////////////////////////////
	// Events
	// //////////////////////////////////////////////////////////
	
	bool on_before_select() override
	{
		group.script.editor.help_screen_vis(!group.script.editor.help_screen_vis());
		return false;
	}
	
}