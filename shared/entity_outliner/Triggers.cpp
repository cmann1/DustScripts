#include '../../lib/triggers/EnterExitTrigger.cpp';

class EntityOutlinerReset : trigger_base, EnterExitTrigger
{
	
	scripttrigger@ self;
	[text] bool only_once = false;
	[text|tooltip:'Will smoothly transition from the current settings\nto these settings over the given number of frames'] float transition = 0;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void activate(controllable@ c)
	{
		activate_enter_exit(c);
	}
	
	void step()
	{
		step_enter_exit();
	}
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		return c.player_index() != -1;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		message@ msg = create_message();
		msg.set_entity('trigger', self.as_entity());
		msg.set_int('player', c.player_index());
		broadcast_message('EntityOutliner', msg);
		
		if(only_once)
		{
			get_scene().remove_entity(self.as_entity());
		}
	}
	
}

class EntityOutlinerTrigger : EntityOutlinerReset
{
	
	[text] EntityOutlinerSettings outliner;
	message@ msg;
	
	void editor_step()
	{
		if(self.editor_selected())
		{
			if(@msg == null)
			{
				@msg = create_message();
				msg.set_entity('trigger', self.as_entity());
				msg.set_string('event', 'trigger_select');
				broadcast_message('EntityOutliner.editor', msg);
			}
		}
		else if(@msg != null)
		{
			msg.set_string('event', 'trigger_deselect');
			broadcast_message('EntityOutliner.editor', msg);
			@msg = null;
		}
	}
	
	void editor_var_changed(var_info@ info)
	{
		if(@msg != null)
		{
			msg.set_string('event', 'trigger_update');
			broadcast_message('EntityOutliner.editor', msg);
		}
	}
	
}
