#include '../../lib/enums/ColType.cpp';
#include '../../lib/enums/EntityState.cpp';
#include '../../lib/enums/AttackType.cpp';
#include '../../lib/math/math.cpp';

/** A stripped down version of the entity outliner. */
class EntityOutlinerBasic
{
	
	[text] bool enabled = true;
	[colour,alpha] uint colour = 0xffffffff;
	[text] bool draw_double = true;
	[text] bool outline_player = true;
	[text] bool outline_prisms = true;
	[text] bool scale_with_camera = false;
	
	[text] int player_layer = 18;
	[text] int player_sublayer = 9;
	
	[text] int enemy_layer = 18;
	[text] int enemy_sublayer = 7;
	
	[text] int apple_layer = 18;
	[text] int apple_sublayer = 6;
	
	[text] float offset_x = -2;
	[text] float offset_y = 2;
	
	private scene@ g;
	private camera@ cam;
	
	private int entity_list_size = 32;
	private array<controllable@> entity_list(entity_list_size);
	private array<sprites@> sprites_list(entity_list_size);
	private array<int> layers_list(entity_list_size * 2);
	private dictionary sprites_dict;
	private int num_entities;
	private float _cam_scale = 1;
	
	float cam_scale { get const { return _cam_scale; } }
	
	EntityOutlinerBasic()
	{
		@g = get_scene();
	}
	
	void checkpoint_load(camera@ cam)
	{
		// Add all entities on screen because step is not run for a few frames after a checkpoint is loaded
		get_entities_on_screen(cam);
	}
	
	void step(camera@ cam, const bool disable_draw=false)
	{
		_cam_scale = scale_with_camera ? cam.screen_height() / 1080 : 1;
		
		if(disable_draw || !enabled)
		{
			num_entities = 0;
			return;
		}
		
		@this.cam = cam;
		get_entities_on_screen(cam);
	}
	
	void editor_step(camera@ cam)
	{
		step(cam);
		_cam_scale = scale_with_camera ? 1 / cam.editor_zoom() : 1;
	}
	
	void draw(float sub_frame)
	{
		if(!enabled)
			return;
		
		for(int i = 0; i < num_entities; i++)
		{
			controllable@ c = entity_list[i];
			sprites@ spr = c.get_sprites();
			
			string sprite_name;
			uint frame;
			float face;
			float draw_offset_x = c.draw_offset_x();
			float draw_offset_y = c.draw_offset_y();
			float rotation = c.rotation();
			
			const string type_name = c.type_name();
			
			if(c.attack_state() != AttackType::Idle && type_name != EntityOutlinerBasic::StrEnemyStoneboss)
			{
				sprite_name = c.attack_sprite_index();
				frame = uint(max(c.attack_timer(), 0.0));
				face = c.attack_face();
				
				if(c.state() == EntityState::WallRun)
				{
					draw_offset_x = draw_offset_y = 0;
				}
			}
			else
			{
				sprite_name = c.sprite_index();
				frame = uint(max(c.state_timer(), 0.0));
				face = c.face();
			}
			
			const int length = spr.get_animation_length(sprite_name);
			if(length > 0)
			{
				frame = frame % length;
			}
			
			const float x = lerp(c.prev_x(), c.x(), sub_frame) + draw_offset_x;
			const float y = lerp(c.prev_y(), c.y(), sub_frame) + draw_offset_y;
			
			if(type_name == EntityOutlinerBasic::StrEnemyFlag)
			{
				rotation = 0;
				// TODO: Fix stun offset for flags (I think it requires being able to get draw offset1 and 2 separately)
				// draw_offset_x = draw_offset_y = 0;
			}
			
			int layer, sublayer;
			
			if(type_name == EntityOutlinerBasic::StrHittableApple)
			{
				layer = apple_layer;
				sublayer = apple_sublayer;
			}
			else if(type_name.substr(0, 5) == EntityOutlinerBasic::StrDust_)
			{
				if(!outline_player)
					continue;
				
				layer = player_layer;
				sublayer = player_sublayer;
			}
			else
			{
				layer = enemy_layer;
				sublayer = enemy_sublayer;
			}
			
			const float scale = c.scale();
			spr.draw_world(
				layer, sublayer,
				sprite_name, frame, 0,
				x + offset_x * _cam_scale, y + offset_y * _cam_scale,
				rotation, scale * face, scale, colour);
			
			if(draw_double)
			{
				spr.draw_world(
					layer, sublayer,
					sprite_name, frame, 0,
					x - offset_x * _cam_scale, y - offset_y * _cam_scale,
					rotation, scale * face, scale, colour);
			}
		}
	}
	
	private void get_entities_on_screen(camera@ cam)
	{
		float view1_x, view1_y, view1_w, view1_h;
		float view2_x, view2_y, view2_w, view2_h;
		cam.get_layer_draw_rect(0, 19, view1_x, view1_y, view1_w, view1_h);
		cam.get_layer_draw_rect(1, 19, view2_x, view2_y, view2_w, view2_h);
		const float padding = 96;
		view1_x -= padding; view1_y -= padding;
		view2_x -= padding; view2_y -= padding;
		view1_w += padding * 2; view1_h += padding * 2;
		view2_w += padding * 2; view2_h += padding * 2;
		
		const float view_x1 = min(view1_x, view2_x);
		const float view_y1 = min(view1_y, view2_y);
		const float view_x2 = max(view1_x + view1_w, view2_x + view2_w);
		const float view_y2 = max(view1_y + view1_h, view2_y + view2_h);
		
		num_entities = 0;
		
		add_entities_type(view_y1, view_y2, view_x1, view_x2, ColType::Enemy);
		add_entities_type(view_y1, view_y2, view_x1, view_x2, ColType::Player);
	}
	
	private void add_entities_type(const float view_y1, const float view_y2, const float view_x1, const float view_x2, const ColType type)
	{
		const int count = g.get_entity_collision(view_y1, view_y2, view_x1, view_x2, type);
		
		while(entity_list_size < num_entities + count + 4)
		{
			entity_list_size *= 2;
			entity_list.resize(entity_list_size);
			sprites_list.resize(entity_list_size);
			layers_list.resize(entity_list_size * 2);
		}
		
		for(int i = 0; i < count; i++)
		{
			controllable@ c = g.get_controllable_collision_index(i);
			if(@c == null)
				continue;
			if(c.life() <= 0)
				continue;
			if(c.destroyed())
				continue;
			if(!outline_prisms && c.type_name().substr(0, 14) == EntityOutlinerBasic::StrEnemyTutorial)
				continue;
			
			@entity_list[num_entities++] = c;
		}
	}
	
}

namespace EntityOutlinerBasic
{
	const string StrEnemyStoneboss = 'enemy_stoneboss';
	const string StrEnemyFlag = 'enemy_flag';
	const string StrHittableApple = 'hittable_apple';
	const string StrDust_ = 'dust_';
	const string StrEnemyTutorial = 'enemy_tutorial';
}
