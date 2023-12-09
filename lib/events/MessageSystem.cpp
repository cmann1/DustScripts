funcdef void MessageHandler(const string &in, message@);

interface IMessageSystemSource
{
	
	MessageSystem@ get_message_system();
	
}

/** Custom global message system. Needed because the built in messaging system
 * doesn't clear broadcast receivers when loading checkpoints.
 * This means that adding listeners e.g. in trigger `init` will cause a memory leak
 * and script triggers that are no longer in the scene will continue to recieve signals.
 * 
 * To use add a field to your script: `MessageSystem mesages` and call `clear()` in `checkpoint_load`.
 * To listen for events, use `add_listener` in the `on_add` entity callback, and `remove_listener` in the `on_remove` callback.
 * - Make sure to store a reference to the `MessageHandler` and use the same one for both adding and removing listeners. */
class MessageSystem
{
	
	private dictionary message_handlers;
	private int iter_i, iter_count;
	
	void clear()
	{
		message_handlers.deleteAll();
	}
	
	void add_listener(const string &in id, MessageHandler@ handler)
	{
		array<MessageHandler@>@ handlers;
		
		if(!message_handlers.exists(id))
		{
			@handlers = array<MessageHandler@>();
			@message_handlers[id] = handlers;
		}
		else
		{
			@handlers = cast<array<MessageHandler@>@>(message_handlers[id]);
		}
		
		if(handlers.findByRef(handler) == -1)
		{
			handlers.insertLast(handler);
		}
	}
	
	void remove_listener(const string &in id, MessageHandler@ handler)
	{
		if(!message_handlers.exists(id))
			return;
		
		array<MessageHandler@>@ handlers = cast<array<MessageHandler@>@>(message_handlers[id]);
		const int index = handlers.findByRef(handler);
		
		if(index != -1)
		{
			handlers.removeAt(index);
			
			if(iter_i > -1)
			{
				if(iter_i >= index)
				{
					iter_i--;
				}
				
				iter_count--;
			}
		}
	}
	
	void broadcast(const string &in id, message@ msg = null)
	{
		if(!message_handlers.exists(id))
			return;
		
		array<MessageHandler@>@ handlers = cast<array<MessageHandler@>@>(message_handlers[id]);
		iter_count = int(handlers.length);
		
		for(iter_i = 0; iter_i < iter_count; iter_i++)
		{
			handlers[iter_i](id, msg);
		}
		
		iter_i = -1;
	}
	
}
