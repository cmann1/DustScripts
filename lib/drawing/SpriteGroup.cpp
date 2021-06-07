#include '../math/math.cpp';

class SpriteGroup
{
	
	protected dictionary sprite_sprites_map;
	
	array<SpriteGroupSprite> sprite_list;
	
	SpriteGroup(){}
	
	SpriteGroup(
		const array<string> &in sprite_set_name,
		const array<int> &in layer_sub_layer,
		const array<float> &in align_offset_rotation_scale,
		const array<uint> &in colour_frame_palette
	)
	{
		add(sprite_set_name, layer_sub_layer, align_offset_rotation_scale, colour_frame_palette);
	}
	
	sprites@ get_sprites(string sprite_set)
	{
		sprites@ spr = cast<sprites>(sprite_sprites_map[sprite_set]);
		if(spr is null)
		{
			@sprite_sprites_map[sprite_set] = @spr = create_sprites();
			spr.add_sprite_set(sprite_set);
		}
		
		return spr;
	}
	
	void add(
		const array<string> &in sprite_set_name,
		const array<int> &in layer_sub_layer,
		const array<float> &in align_offset_rotation_scale,
		const array<uint> &in frame_palette_colour)
	{
		sprites@ spr = null;
		rectangle@ spr_rect = null;
		string previous_sprite_set = '';
		string previous_sprite_name = '';
		float spr_x, spr_y;
		float spr_width, spr_height;
		
		const int num_sprites = int(sprite_set_name.size());
		for(int i = 0, j = 0, k = 0; i < num_sprites; i += 2, j += 7, k += 3)
		{
			const string sprite_set = sprite_set_name[i];
			const string sprite_name = sprite_set_name[i + 1];
			const int layer = layer_sub_layer[i];
			const int sub_layer = layer_sub_layer[i + 1];
			const float align_x = align_offset_rotation_scale[j];
			const float align_y = align_offset_rotation_scale[j + 1];
			const float offset_x = align_offset_rotation_scale[j + 2];
			const float offset_y = align_offset_rotation_scale[j + 3];
			const float rotation = align_offset_rotation_scale[j + 4];
			const float scale_x = align_offset_rotation_scale[j + 5];
			const float scale_y = align_offset_rotation_scale[j + 6];
			const uint frame = frame_palette_colour[k];
			const uint palette = frame_palette_colour[k + 1];
			const uint colour = frame_palette_colour[k + 2];
			
			if(previous_sprite_set != sprite_set || spr is null)
			{
				@spr = cast<sprites>(sprite_sprites_map[sprite_set]);
				if(spr is null)
				{
					@sprite_sprites_map[sprite_set] = @spr = create_sprites();
					spr.add_sprite_set(sprite_set);
				}
				@spr_rect = null;
			}
			if(previous_sprite_name != sprite_name || spr_rect is null)
			{
				@spr_rect = spr.get_sprite_rect(sprite_name, frame);
			}
			
			const float rx = (spr_rect.left() + spr_rect.get_width() * align_x) * scale_x;
			const float ry = (spr_rect.top() + spr_rect.get_height() * align_y) * scale_y;
			const float cs = cos(rotation * DEG2RAD);
			const float sn = sin(rotation * DEG2RAD);
			
			sprite_list.insertLast(SpriteGroupSprite(
				spr, sprite_set, sprite_name,
				layer, sub_layer,
				offset_x - (rx * cs - ry * sn),
				offset_y - (ry * cs + rx * sn),
				rotation,
				scale_x, scale_y,
				colour, frame, palette
			));
		}
	}
	
	void add(
		string sprite_set, string sprite_name,
		int layer, int sub_layer,
		float align_x=0.5, float align_y=0.5,
		float offset_x=0, float offset_y=0, float rotation=0,
		float scale_x=1, float scale_y=1,
		uint colour=0xFFFFFFFF, uint frame=0, uint palette=0)
	{ 
		sprites@ spr = get_sprites(sprite_set);
		rectangle@ spr_rect = spr.get_sprite_rect(sprite_name, frame);

		const float rx = (spr_rect.left() + spr_rect.get_width() * align_x) * scale_x;
		const float ry = (spr_rect.top() + spr_rect.get_height() * align_y) * scale_y;
		const float cs = cos(rotation * DEG2RAD);
		const float sn = sin(rotation * DEG2RAD);
		
		sprite_list.insertLast(SpriteGroupSprite(
			spr, sprite_set, sprite_name,
			layer, sub_layer,
			offset_x - (rx * cs - ry * sn),
			offset_y - (ry * cs + rx * sn),
			rotation,
			scale_x, scale_y,
			colour, frame, palette
		));
	}
	
	void set_colour_all(uint colour)
	{
		for(int i = int(sprite_list.size()) - 1; i >= 0; i--)
		{
			sprite_list[i].colour = colour;
		}
	}
	
	void add_layer_all(int layer, int sub_layer=0)
	{
		for(int i = int(sprite_list.size()) - 1; i >= 0; i--)
		{
			SpriteGroupSprite@ spr_group = @sprite_list[i];
			spr_group.layer += layer;
			spr_group.sub_layer += sub_layer;
		}
	}
	
	void clear()
	{
		sprite_list.resize(0);
	}
	
	void draw(
		float x, float y, float rotation, float scale,
		int layer = 0, int sub_layer = 0, float alpha=1
	)
	{
		const float cs = cos(rotation * DEG2RAD);
		const float sn = sin(rotation * DEG2RAD);
		
		if(alpha <= 0)
			return;
		
		const bool has_alpha = alpha < 1;
		uint alpha_colour;
		if(has_alpha)
			alpha_colour = uint(alpha * 255) << 24;
		
		for(int i = 0, count = int(sprite_list.size()); i < count; i++)
		{
			SpriteGroupSprite@ spr_group = @sprite_list[i];

			const float px = spr_group.x * cs - spr_group.y * sn;
			const float py = spr_group.y * cs + spr_group.x * sn;
			spr_group.spr.draw_world(
				layer + spr_group.layer, sub_layer + spr_group.sub_layer,
				spr_group.sprite_name, spr_group.frame, spr_group.palette,
				x + px * scale, y + py * scale,
				rotation + spr_group.rotation,
				spr_group.scale_x * scale,
				spr_group.scale_y * scale,
				has_alpha ? ((spr_group.colour & 0xFFFFFF) | alpha_colour) : spr_group.colour);
		}
	}
	
	void draw_hud(
		float x, float y, float rotation, float scale,
		int layer = 0, int sub_layer = 0, float alpha=1
	)
	{
		const float cs = cos(rotation * DEG2RAD);
		const float sn = sin(rotation * DEG2RAD);
		
		const bool has_alpha = alpha < 1;
		uint alpha_colour;
		if(has_alpha)
			alpha_colour = uint(alpha * 255) << 24;
		
		for(int i = 0, count = int(sprite_list.size()); i < count; i++)
		{
			SpriteGroupSprite@ spr_group = @sprite_list[i];

			const float px = spr_group.x * cs - spr_group.y * sn;
			const float py = spr_group.y * cs + spr_group.x * sn;
			spr_group.spr.draw_hud(
				layer + spr_group.layer, sub_layer + spr_group.sub_layer,
				spr_group.sprite_name, spr_group.frame, spr_group.palette,
				x + px * scale, y + py * scale,
				rotation + spr_group.rotation,
				spr_group.scale_x * scale,
				spr_group.scale_y * scale,
				has_alpha ? ((spr_group.colour & 0xFFFFFF) | alpha_colour) : spr_group.colour);
		}
	}
	
	SpriteRectangle get_rectangle(float rotation, float scale)
	{
		const float cs = cos(rotation * DEG2RAD);
		const float sn = sin(rotation * DEG2RAD);
		const int count = int(sprite_list.size());
		
		if(count == 0)
		{
			return SpriteRectangle(0, 0, 0, 0);
		}
		
		float left = 0, right = 0, top = 0, bottom = 0;

		for(int i = 0; i < count; i++)
		{
			SpriteGroupSprite@ spr_group = @sprite_list[i];

			const float px = spr_group.x * cs - spr_group.y * sn;
			const float py = spr_group.y * cs + spr_group.x * sn;
			rectangle@ src = spr_group.spr.get_sprite_rect(spr_group.sprite_name, spr_group.frame);

			float scs = cos((rotation + spr_group.rotation) * DEG2RAD);
			float ssn = sin((rotation + spr_group.rotation) * DEG2RAD);
			
			const array<float> xs = {src.left(), src.right()};
			const array<float> ys = {src.top(), src.bottom()};
			for(int j = 0; j < 2; j++)
			{
				for(int k = 0; k < 2; k++)
				{
					float cx = (px + (xs[j] * scs - ys[k] * ssn) * spr_group.scale_x) * scale;
					float cy = (py + (ys[k] * scs + xs[j] * ssn) * spr_group.scale_y) * scale;

					left = min(left, cx);
					right = max(right, cx);
					top = min(top, cy);
					bottom = max(bottom, cy);
				}
			}
		}
		
		return SpriteRectangle(top, bottom, left, right);
	}
	
}

class SpriteGroupSprite
{
	
	sprites@ spr;
	string sprite_set;
	string sprite_name;
	
	float x, y;
	float rotation;
	float scale_x, scale_y;
	uint colour;
	uint frame;
	uint palette;
	int layer;
	int sub_layer;
	
	SpriteGroupSprite(){}
	
	SpriteGroupSprite(sprites@ spr, const string &in sprite_set, const string &in sprite_name,
		int layer, int sub_layer, float x, float y, float rotation, float scale_x, float scale_y, uint colour=0xFFFFFFFF, uint frame=0, uint palette=0)
	{
		@this.spr = spr;
		this.sprite_set = sprite_set;
		this.sprite_name = sprite_name;
		
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.x = x;
		this.y = y;
		this.rotation = rotation;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.colour = colour;
		this.frame = frame;
		this.palette = palette;
	}
	
}

class SpriteRectangle
{
	
	float top;
	float bottom;
	float left;
	float right;

	SpriteRectangle()
	{
		top = bottom = left = right = 0;
	}
	
	SpriteRectangle(float top, float bottom, float left, float right)
	{
		this.top = top;
		this.bottom = bottom;
		this.left = left;
		this.right = right;
	}
	
}
