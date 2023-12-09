#include '../../lib/triggers/EnterExitTrigger.cpp';

#include 'MessageBroadcasterBase.cpp';

/** Provides some options for broadcasting messages when the trigger is activated.
 * Using `MessageSystem` and `MessageSystemBroadcaster` is recommended because the built in broadcast system
 * does not clear broadcasters when a checkpoints is loaded.
 */
class MessageBroadcaster : trigger_base, callback_base, MessageBroadcasterBase, EnterExitTrigger
{
	
	/** Can players trigger this. */
	[text] bool players	= true;
	/** Can apples trigger this. */
	[text] bool apples	= true;
	/** Can enemies trigger this. */
	[text] bool enemies	= true;
	/** If true this trigger will remove itself after the first broadcast. */
	[text] bool once = false;
	/** If true the message won't broadcast when an entity enters if there is already another entity inside of the trigger area. */
	[text] bool group = false;
	/** If this is not zero, the message will be sent to the entity with this id instead of a global broadcast. */
	[entity] int entity_id;
	/** The id of the message that will be broadcast. */
	[text] string id;
	/** If not empty, an int will be set on the message when broadcast. */
	[text] string key;
	/** The int value when 'key' is not empty. */
	[text] int value;
	/** If not empty, the trigger will remove itself when it receives this event. */
	[text] string remove_event;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
	}
	
	void add_remove_event()
	{
		add_broadcast_receiver(remove_event, this, 'on_remove_event');
	}
	
	void remove_remove_event()
	{
		// No way to remove built in broadcast receivers.
	}
	
	void broadcast(const string &in id, message@ msg)
	{
		broadcast_message(id, msg);
	}
	
	void on_remove_event(string id, message@ msg)
	{
		do_remove(id, msg);
	}
	
}

