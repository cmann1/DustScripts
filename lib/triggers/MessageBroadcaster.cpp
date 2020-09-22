#include 'EnterExitTrigger.cpp';

class MessageBroadcaster : trigger_base, callback_base, EnterExitTrigger
{
	
	/// Can players trigger this.
	[text] bool players	= true;
	/// Can apples trigger this.
	[text] bool apples	= true;
	/// Can enemies trigger this.
	[text] bool enemies	= true;
	/// The id of the message that will be broadcast.
	[text] string id;
	/// If not empty, an int will be set on the message when broadcast.
	[text] string key;
	/// The int value when 'key' is not empty.
	[text] int value;
	/// If not empty, the trigger will remove itself when it receives this event.
	[text] string remove_event;
	
	scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		
		if(remove_event != '')
		{
			add_broadcast_receiver(remove_event, this, 'on_remove_event');
		}
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
		if(c.player_index() != -1 && !players)
			return false;
		
		const string type = c.type_name();
		
		if(!apples && type == 'hittable_apple')
			return false;
		
		if(!enemies && (
			type == 'enemy_tutorial_square' ||
			type == 'enemy_tutorial_hexagon' ||
			type.substr(0, 6) == 'enemy_'))
			return false;
		
		return true;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		message@ msg = create_message();
		msg.set_entity('broadcaster', self.as_entity());
		
		if(key != '')
		{
			msg.set_int(key, value);
		}
		
		broadcast_message(id, msg);
	}
	
	/*void on_trigger_exit(controllable@ c)
	{
		puts('on_trigger_exit ' + (c.player_index() != -1 ? -c.player_index() : c.id()));
	}*/
	
	void on_remove_event(string, message@)
	{
		get_scene().remove_entity(self.as_entity());
	}
	
}