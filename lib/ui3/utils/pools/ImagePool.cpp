#include '../../UI.cpp';
#include '../../Style.cpp';

class ImagePool
{
	
	private uint pool_index = 0;
	private uint pool_size = 8;
	private array<Image@> pool(pool_size);
	
	Image@ get(
		UI@ ui, const string sprite_text, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=-0.5, const float offset_y=-0.5)
	{
		Image@ image = pool_index > 0 ? @pool[--pool_index] : Image(ui, sprite_text, sprite_name, width, height, offset_x, offset_y);
		
		image.set_sprite(sprite_text, sprite_name, width, height, offset_x, offset_y);
		
		return image;
	}
	
	void release(Image@ obj)
	{
		if(pool_index == pool_size)
		{
			pool_size += 8;
			pool.resize(pool_size);
		}
		
		@pool[pool_index++] = @obj;
	}
	
}