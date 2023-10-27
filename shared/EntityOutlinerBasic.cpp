#include 'entity_outliner/EntityOutlinerBasic.cpp';

class script : callback_base
{
	
	[option,-1:Ignore,1:Low,Medium,High] int min_script_fx_level = -1;
	
	private camera@ cam;
	[persist] private EntityOutlinerBasic entity_outliner;
	
	private bool is_active = true;
	
	script()
	{
		@cam = get_active_camera();
		
		on_video_settings_change();
	}
	
	void on_editor_start()
	{
		on_video_settings_change();
	}
	
	void on_video_settings_change()
	{
		const int level = scene.script_fx_level;
		is_active = min_script_fx_level < 0 || level > 0 && level >= min_script_fx_level;
	}
	
	void on_level_start()
	{
		on_video_settings_change();
	}
	
	void checkpoint_load()
	{
		@cam = get_active_camera();
		entity_outliner.checkpoint_load(cam);
	}
	
	void editor_var_changed(var_info@ info)
	{
		if(info.name == 'min_script_fx_level')
		{
			on_video_settings_change();
			return;
		}
	}
	
	void step(int num_entities)
	{
		if(is_active)
		{
			@cam = get_active_camera();
			entity_outliner.step(cam);
		}
	}
	
	void editor_step()
	{
		if(is_active)
		{
			entity_outliner.editor_step(cam);
		}
	}
	
	void draw(float sub_frame)
	{
		if(is_active)
		{
			entity_outliner.draw(sub_frame);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(is_active)
		{
			entity_outliner.draw(sub_frame);
		}
	}
	
}

