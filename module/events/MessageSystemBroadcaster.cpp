#include '../../lib/triggers/EnterExitTrigger.cpp';
#include '../../lib/events/MessageSystem.cpp';

#include 'MessageBroadcasterBase.cpp';

/** Provides some options for broadcasting messages when the trigger is activated.
 * But using the custom `MessageSystem` to avoid using the buggy built in message mechanisms.
 * See `MessageSystem` for details.
 * 
 * Assumes `script` has a ppublic field named `messages` of type `MessageSystem`. */
class MessageBroadcaster : trigger_base, MessageBroadcasterBase, EnterExitTrigger
{
	
	/** Can players trigger this. */
	[persist] bool players	= true;
	/** Can apples trigger this. */
	[persist] bool apples	= true;
	/** Can enemies trigger this. */
	[persist] bool enemies	= true;
	/** If true this trigger will remove itself after the first broadcast. */
	[persist] bool once	= false;
	/** If true the message won't broadcast when an entity enters if there is already another entity inside of the trigger area. */
	[persist] bool group = false;
	/** If this is not zero, the message will be sent to the entity with this id instead of a global broadcast. */
	[entity] int entity_id;
	/** The id of the message that will be broadcast. */
	[persist] string id;
	/** If not empty, an int will be set on the message when broadcast. */
	[persist] string key;
	/** The int value when 'key' is not empty. */
	[persist] int value;
	/** If not empty, the trigger will remove itself when it receives this event. */
	[persist] string remove_event;
	
	MessageHandler@ remove_callback;
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
	}
	
	void add_remove_event()
	{
		if(@remove_callback == null)
		{
			@remove_callback = MessageHandler(on_remove_event);
		}
		
		script.messages.add_listener(remove_event, remove_callback);
	}
	
	void remove_remove_event()
	{
		if(@remove_callback != null)
		{
			script.messages.remove_listener(remove_event, remove_callback);
			@remove_callback = null;
		}
	}
	
	void broadcast(const string &in id, message@ msg)
	{
		script.messages.broadcast(id, msg);
	}
	
	void on_remove_event(const string &in id, message@ msg)
	{
		do_remove(id, msg);
	}
	
}
