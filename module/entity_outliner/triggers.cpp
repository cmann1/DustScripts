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
		msg.set_entity(EntityOutliner::StrTrigger, self.as_entity());
		msg.set_int(EntityOutliner::StrPlayer, c.player_index());
		broadcast_message(EntityOutliner::StrEntityOutliner, msg);
		
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
	
	[persist|tooltip:'The inner radius/distance at which the trigger\'s\neffect will be at 100%.']
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
		
		self.editor_colour_circle(0x66beb382);
		self.editor_colour_inactive(0xaabeb382);
		self.editor_colour_active(0xfff5ebba);
	}
	
	void activate(controllable@ c)
	{
		activate_enter_exit(c);
	}
	
	void step()
	{
		step_enter_exit();
		
		const float radius = get_source_radius();
		const float x = get_source_centre_x();
		const float y = get_source_centre_y();
		
		for(int i = entities_enter_exit_list_count - 1; i >= 0 ; i--)
		{
			controllable@ c = entities_enter_exit_list[i];
			float mx, my;
			c.centre(mx, my);
			const float t = 1 - clamp01((distance(mx, my, x, y) - min_radius) / abs(radius - min_radius));
			const float offset = lerp(offset_min, offset_max, t);
			const float angle = atan2(y - my, x - mx);
			
			msg.set_int(EntityOutliner::StrId, c.id());
			msg.set_int(EntityOutliner::StrSubLayer, sub_layer);
			msg.set_int(EntityOutliner::StrColour, colour);
			msg.set_float(EntityOutliner::StrOffsetX, cos(angle) * offset);
			msg.set_float(EntityOutliner::StrOffsetY, sin(angle) * offset);
			msg.set_float(EntityOutliner::StrScale, scale);
			msg.set_int(EntityOutliner::StrClosest, 1);
			msg.set_int(EntityOutliner::StrFadeGlobal, fade_global ? 1 : 0);
			msg.set_float(EntityOutliner::StrT, t);
			broadcast_message(EntityOutliner::StrEntityOutlinerSource, msg);
		}
	}
	
	protected float get_source_radius()
	{
		return self.radius();
	}
	
	protected float get_source_centre_x()
	{
		return self.x();
	}
	
	protected float get_source_centre_y()
	{
		return self.y();
	}
	
}

class EntityOutlinerSourcePos : EntityOutlinerSource
{
	
	[position,mode:world,layer:19,y:src_y] float src_x;
	[hidden] float src_y;
	
	private scene@ g;
	
	private float real_radius;
	
	void init(script@ s, scripttrigger@ self)
	{
		EntityOutlinerSource::init(s, self);
		
		const float dx = self.x() - src_x;
		const float dy = self.y() - src_y;
		const float dist = sqrt(dx * dx + dy * dy);
		real_radius = self.radius() + dist;
		
		@g = get_scene();
	}
	
	protected float get_source_radius() override
	{
		return real_radius;
	}
	
	protected float get_source_centre_x() override
	{
		return src_x;
	}
	
	protected float get_source_centre_y() override
	{
		return src_y;
	}
	
	void editor_draw(float)
	{
		g.draw_line_world(22, 1, self.x(), self.y(), src_x, src_y, 3, 0x88ff5522);
		g.draw_rectangle_world(22, 22, src_x - 5, src_y - 5, src_x + 5, src_y + 5, 45, 0xffff5522);
	}
	
}
