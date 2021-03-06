#include '../../UI.cpp';
#include '../../Style.cpp';

class LabelPool
{
	
	private uint pool_index = 0;
	private uint pool_size = 8;
	private array<Label@> pool(pool_size);
	
	Label@ get(
		UI@ ui, const string text, const bool auto_size,
		const TextAlign text_align_h, const float align_h, const float align_v,
		const float scale, const bool has_colour, const uint colour,
		const string font, const uint size)
	{
		Label@ label = pool_index > 0 ? @pool[--pool_index] : Label(ui, text, auto_size, font, size);
		
		label.text			= text;
		label.text_align_h	= text_align_h;
		label.align_h		= align_h;
		label.align_v		= align_v;
		label.scale			= scale;
		label.colour		= colour;
		label.has_colour	= has_colour;
		
		return label;
	}
	
	void release(Label@ obj)
	{
		if(pool_index == pool_size)
		{
			pool_size += 8;
			pool.resize(pool_size);
		}
		
		@pool[pool_index++] = @obj;
	}
	
}
