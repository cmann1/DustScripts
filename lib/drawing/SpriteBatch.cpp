#include '../props/common.cpp';
#include 'SpriteGroup.cpp';

/*
 * Simpler than SpriteGroup
 */
class SpriteBatch
{
	
	dictionary sprite_map;
	array<SpriteBatchSprite> sprite_list;
	int num_sprites = 0;
	
	SpriteBatch()
	{
	}
	
	SpriteBatch(
		const array<string> &in sprite_set_name, const array<int> &in layer_sub_layer,
		const array<float> &in x_y, const array<float> &in scale, const array<float> &in rotation, const array<uint> &in palette)
	{
		num_sprites = int(palette.length());
		sprite_list.resize(num_sprites);
		int i2 = 0;
		for(int i = 0; i < num_sprites; i++, i2 += 2)
		{
			const string sprite_set = sprite_set_name[i2];
			sprites@ spr = cast<sprites>(sprite_map[sprite_set]);
			if(spr is null)
			{
				@sprite_map[sprite_set] = @spr = create_sprites();
				spr.add_sprite_set(sprite_set);
			}
			
			const int i3 = i2 + 1;
			SpriteBatchSprite@ batch_spr = @sprite_list[i];
			
			@batch_spr.spr = spr;
			batch_spr.sprite_set = sprite_set;
			batch_spr.sprite_name = sprite_set_name[i3];
			batch_spr.layer = layer_sub_layer[i2];
			batch_spr.sub_layer = layer_sub_layer[i3];
			batch_spr.x = x_y[i2];
			batch_spr.y = x_y[i3];
			batch_spr.scale_x = scale[i2];
			batch_spr.scale_y = scale[i3];
			batch_spr.rotation = rotation[i];
			batch_spr.palette = palette[i];
		}
	}
	
	void add(
		const string &in sprite_set, const string &in sprite_name, int layer, int sub_layer,
		float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint palette=0)
	{
		sprites@ spr = get_sprites(sprite_set);
		sprite_list.insertLast(
			SpriteBatchSprite(spr, sprite_set, sprite_name, layer, sub_layer, x, y, scale_x, scale_y, rotation, palette)
		);
		num_sprites++;
	}
	
	void add(prop@ p)
	{
		string sprite_set, sprite_name;
		sprite_from_prop(p, sprite_set, sprite_name);
		
		sprites@ spr = get_sprites(sprite_set);
		sprite_list.insertLast(
			SpriteBatchSprite(spr, sprite_set, sprite_name, p.layer(), p.sub_layer(), p.x(), p.y(), p.scale_x(), p.scale_y(), p.rotation(), p.palette())
		);
		num_sprites++;
	}
	
	sprites@ get_sprites(string sprite_set)
	{
		sprites@ spr = cast<sprites>(sprite_map[sprite_set]);
		if(spr is null)
		{
			@sprite_map[sprite_set] = @spr = create_sprites();
			spr.add_sprite_set(sprite_set);
		}
		
		return spr;
	}
	
	void draw(float x, float y, uint colour=0xffffffff)
	{
		for(int i = 0; i < num_sprites; i++)
		{
			SpriteBatchSprite@ s = sprite_list[i];
			s.spr.draw_world(s.layer, s.sub_layer, s.sprite_name, 0, s.palette, x + s.x, y + s.y, s.rotation, s.scale_x, s.scale_y, colour);
		}
	}
	
	SpriteRectangle get_rectangle(const float x, const float y)
	{
		if(num_sprites == 0)
		{
			return SpriteRectangle(x, y, x, y);
		}
		
		float left = 0, right = 0, top = 0, bottom = 0;

		for(int i = 0; i < num_sprites; i++)
		{
			SpriteBatchSprite@ spr = @sprite_list[i];

			rectangle@ src = spr.spr.get_sprite_rect(spr.sprite_name, 0);

			float scs = cos(spr.rotation * DEG2RAD);
			float ssn = sin(spr.rotation * DEG2RAD);
			
			const array<float> xs = {src.left(), src.right()};
			const array<float> ys = {src.top(), src.bottom()};
			for(int j = 0; j < 2; j++)
			{
				for(int k = 0; k < 2; k++)
				{
					float cx = (spr.x + (xs[j] * scs - ys[k] * ssn) * spr.scale_x);
					float cy = (spr.y + (ys[k] * scs + xs[j] * ssn) * spr.scale_y);

					left = min(left, cx);
					right = max(right, cx);
					top = min(top, cy);
					bottom = max(bottom, cy);
				}
			}
		}
		
		return SpriteRectangle(y + top, y + bottom, x + left, x + right);
	}
	
	void place_props(scene@ g, const float x, const float y)
	{
		for(uint i = 0; i < sprite_list.length; i++)
		{
			SpriteBatchSprite@ s = sprite_list[i];
			uint prop_set, prop_group, prop_index;
			prop_from_sprite(s.sprite_set, s.sprite_name, prop_set, prop_group, prop_index);
			prop@ p = create_prop(prop_set, prop_group, prop_index, x + s.x, y + s.y, s.layer, s.sub_layer, s.rotation);
			p.palette(s.palette);
			p.scale_x(s.scale_x);
			p.scale_y(s.scale_y);
			g.add_prop(p);
		}
	}
	
}

class SpriteBatchSprite
{
	sprites@ spr;
	string sprite_set, sprite_name;
	int layer, sub_layer;
	float x, y;
	float scale_x, scale_y;
	float rotation;
	uint palette;
	
	SpriteBatchSprite(){}
	
	SpriteBatchSprite(
		sprites@ spr, const string &in sprite_set, const string &in  sprite_name, int layer, int sub_layer,
		float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint palette=0)
	{
		@this.spr = spr;
		this.sprite_set = sprite_set;
		this.sprite_name = sprite_name;
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.x = x;
		this.y = y;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.rotation = rotation;
		this.palette = palette;
	}
	
}
