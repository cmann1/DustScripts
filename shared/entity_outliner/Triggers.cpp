#include '../../lib/triggers/EnterExitTrigger.cpp';
#include '../../lib/math/math.cpp';

class EntityOutlinerReset : trigger_base, EnterExitTrigger
{
	
	scripttrigger@ self;
	[persist] bool only_once = false;
	[persist|tooltip:'Will smoothly transition from the current settings\nto these settings over the given number of frames.']
	float transition = 0;
	
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

class EntityOutlinerSource : trigger_base, EnterExitTrigger
{
	
	scripttrigger@ self;
	[persist|tooltip:'The inner radius/distance at which the trigger\'s effect will be at 100%.']
	float min_radius = 48;
	[persist|tooltip:'If set, this will render an additional outline on this sublayer,\notherwise the global outline will be overriden.']
	int sub_layer = -1;
	[colour,alpha] uint colour = 0xffffffff;
	[persist|tooltip:'The outline offset at the edges of the trigger.\nCan be negative.']
	float offset_min = 2;
	[persist|tooltip:'The outline offset at the centre of the trigger.']
	float offset_max = 3;
	[persist]
	float scale = 1;
	[persist|tooltip:'Toggles whether the global outline should be faded out\nas this light source outline is faded in.']
	bool fade_global = true;
	
	message@ msg = create_message();
	
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
		
		const float radius = self.radius();
		const float x = self.x();
		const float y = self.y();
		
		for(int i = entities_enter_exit_list_count - 1; i >= 0 ; i--)
		{
			controllable@ c = entities_enter_exit_list[i];
			rectangle@ r = c.base_rectangle();
			const float mx = c.x() + r.left() + r.width * 0.5;
			const float my = c.y() + r.top() + r.height * 0.5;
			const float t = 1 - clamp01((distance(mx, my, x, y) - min_radius) / (radius - min_radius));
			const float offset = lerp(offset_min, offset_max, t);
			const float angle = atan2(y - my, x - mx);
			
			msg.set_int('id', c.id());
			msg.set_int('sub_layer', sub_layer);
			msg.set_int('colour', colour);
			msg.set_float('offset_x', cos(angle) * offset);
			msg.set_float('offset_y', sin(angle) * offset);
			msg.set_float('scale', scale);
			msg.set_int('closest', 1);
			msg.set_int('fade_global', fade_global ? 1 : 0);
			msg.set_float('t', t);
			broadcast_message('EntityOutlinerSource', msg);
		}
	}
	
}
