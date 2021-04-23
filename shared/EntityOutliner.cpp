#include 'entity_outliner/EntityOutliner.cpp';
#include 'entity_outliner/Triggers.cpp';

class script : callback_base
{
	
	[text|label:Outliner] EntityOutlinerSettings settings;
	
	private camera@ cam;
	private EntityOutliner entity_outliner;
	
	script()
	{
		@cam = get_active_camera();
		entity_outliner.init_settings(@settings);
	}
	
	void checkpoint_load()
	{
		@cam = get_active_camera();
		entity_outliner.checkpoint_load(cam);
	}
	
	void checkpoint_save()
	{
		entity_outliner.checkpoint_save();
	}
	
	void editor_var_changed(var_info@ info)
	{
		entity_outliner.editor_var_changed(info, @settings);
	}
	
	void step(int num_entities)
	{
		@cam = get_active_camera();
		entity_outliner.step(cam);
	}
	
	void editor_step()
	{
		entity_outliner.editor_step(cam);
	}
	
	void draw(float sub_frame)
	{
		entity_outliner.draw(sub_frame);
	}
	
	void editor_draw(float sub_frame)
	{
		entity_outliner.draw(sub_frame);
	}
	
}
