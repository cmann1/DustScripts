#include 'DebugTextLine.cpp';

class DebugTextLineList
{
	
	int max_lines = 100;
	textfield@ text_field;
	float spacing;
	float scale = 0.75;
	float text_height = 0;
	
	dictionary ids;
	DebugTextLine@ first = null;
	DebugTextLine@ last = null;
	int count;
	int id_count;
	
	private uint pool_size =  0;
	private uint pool_index =  0;
	private array<DebugTextLine@> pool(max_lines);
	
	void insert(DebugTextLine@ line, bool update_id_dict = true)
	{
//		puts('inserting: ' + line.text);
		
		if(max_lines == 0)
			return;
		
		if(@last == null)
		{
			@first = @last = line;
			
			if(line.key != '')
				id_count = 1;
			
			count = 1;
//			puts('  first');
		}
		else
		{
//			puts('  old last: ' + last.text);
//			puts('  new last: ' + line.text);
			@last.next = line;
			@line.prev = last;
			@last = line;
			
			count++;
			
			if(line.key != '')
				id_count++;
				
			if(count - id_count > max_lines)
			{
				DebugTextLine@ first_transient = first;
				
				while(@first_transient != null)
				{
					if(first_transient.key == '')
						break;
					
					@first_transient = first_transient.next;
				}
				
				if(@first_transient != null)
				{
//					puts('  removing first: ' + first_transient.text);
					remove(first_transient);
				}
			}
		}
		
		if(update_id_dict && line.key != '')
		{
			@ids[line.key] = @line;
		}
		
		text_field.text(line.text);
		line.height = text_field.text_height() * scale;
		text_height += line.height;
		
		if(count > 1)
		{
			text_height += spacing;
		}
		
//		debug_lines();
	}

	void remove(DebugTextLine@ line, bool release = true)
	{
//		puts('removing: ' + line.text);
		
		if(@line.prev != null)
		{
//			puts('  remove from prev: ' + line.prev.text);
			@line.prev.next = line.next;
		}
		else
		{
//			puts('  new first: ' + (@line.next != null ? line.next.text : 'null'));
			@first = line.next;
		}
		
		if(@line.next != null)
		{
//			puts('  remove from next: ' + line.next.text);
			@line.next.prev = line.prev;
		}
		else
		{
//			puts('  new last: ' + (@line.prev != null ? line.prev.text : 'null'));
			@last = line.prev;
		}
		
		@line.prev = null;
		@line.next = null;
		
		if(release)
		{
			if(line.key != '')
			{
				ids.delete(line.key);
			}
			
			release_line(line);
			
//			puts('  pool size: ' + pool_size);
		}
		
		text_height -= line.height;
		
		if(count > 1)
		{
			text_height -= spacing;
		}
		
		count--;
		
		if(line.key != '')
			id_count--;
		
//		debug_lines();
	}
	
	void calculate_height()
	{
		DebugTextLine@ line = first;
		text_height = 0;
		
		while(@line != null)
		{
			text_field.text(line.text);
			line.height = text_field.text_height() * scale;
			text_height += line.height;
			
			@line = line.next;
		}
		
		text_height += spacing * max(0, (count - 1));
	}
	
	void debug_lines()
	{
		DebugTextLine@ line = first;
		
		int a = 0;
		
		while(@line != null)
		{
			if(++a > count)
			{
				puts('Possible error in lines linked list');
				break;
			}
			
			string str = '         '+a+' - ';
			
			if(@line.prev != null)
				str += '(prev:' + line.prev.text + ') ';
			
			if(line.key != '')
				str += '[' + line.key + '] ';
			
			str += line.text;
			
			if(@line.next != null)
				str += ' (next:' + line.next.text + ')';
			
			puts(str);
			
			@line = line.next;
		}
	}
	
	DebugTextLine@ get(string text, uint colour, string key = '', int frames = 120)
	{
		if(pool_index == 0)
			return DebugTextLine(text, colour, key, frames);
		
		DebugTextLine@ line = pool[--pool_index];
		
		line.text = text;
		line.colour = colour;
		line.key = key;
		line.frames = frames;
		
		return line;
	}
	
	void release_line(DebugTextLine@ line)
	{
		if(pool_index == pool_size)
		{
			pool_size += 8;
			pool.resize(pool_size);
		}
		
		@pool[pool_index++] = line;
	}
	
}