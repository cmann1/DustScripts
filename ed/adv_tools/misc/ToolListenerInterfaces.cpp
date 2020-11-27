interface IToolEditorLoadListener
{
	
	void tool_editor_loaded(Tool@ tool);
	void tool_editor_unloaded(Tool@ tool);
	
}

interface IToolSelectListener
{
	
	void tool_select(Tool@ tool);
	
	void tool_deselect(Tool@ tool);
	
}

interface IToolStepListener
{
	
	void tool_step(Tool@ tool);
	
}

interface IToolDrawListener
{
	
	void tool_draw(Tool@ tool, const float sub_frame);
	
}