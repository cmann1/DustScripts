#include 'EnterExitTrigger.cpp';

class MessageBroadcaster : trigger_base, EnterExitTrigger
{
	
	[text] bool players	= true;
	[text] bool apples	= true;
	[text] bool ememies	= true;
	[text] string id;
	[text] string key;
	[text] int value;
	
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
		
		if(!ememies && (
			type == 'enemy_tutorial_square' ||
			type == 'enemy_tutorial_hexagon' ||
			type.substr(0, 6) == 'enemy_'))
			return false;
		
		return true;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		message@ msg = create_message();
		
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
	
}