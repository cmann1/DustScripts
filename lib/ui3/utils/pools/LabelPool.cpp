#include '../../UI.cpp';
#include '../../Style.cpp';

class LabelPool
{
	
	private uint pool_index = 0;
	private uint pool_size = 8;
	private array<Label@> pool(pool_size);
	
	Label@ get(
		UI@ ui, const string text,
		const TextAlign align_h, const TextAlign align_v,
		const float scale, const uint colour,
		const string font, const uint size)
	{
		Label@ label = pool_index > 0 ? @pool[--pool_index] : Label(ui, text, font, size);
		
		label.text		= text;
		label.align_h	= align_h;
		label.align_v	= align_v;
		label.scale		= scale;
		label.colour	= colour;
		
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