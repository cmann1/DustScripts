//#include '../common-old/PropGroupNames.cpp';
#include '../lib/props.cpp';
#include '../lib/std.cpp';
#include '../lib/drawing/common.cpp';
#include '../lib/camera.cpp';
//#include '../common-old/math.cpp';
//#include '../common-old/drawing_utils.cpp';
//#include '../common-old/utils.cpp';

enum State
{
	Idle,
	Select,
	PickOrigin
}

enum OutputType
{
	SpriteGroup,
	SpriteGroup2,
	SpriteBatch,
	Custom
}

class script
{
	
	scene@ g;
	bool in_game = false;
	
	controllable@ player = null;
	camera@ cam;
	bool print = true;
	
	State state = Idle;
	float selection_x1;
	float selection_y1;
	float selection_x2;
	float selection_y2;
	float selection_hud_x1;
	float selection_hud_y1;
	float selection_hud_x2;
	float selection_hud_y2;
	float origin_x;
	float origin_y;
	array<prop@> selected_props;
	int num_selected_props = 0;
	dictionary sprite_sets;
	
	textfield@ txt;
	
	array<bool> layer_vis(21);
	bool has_include = false;
	bool has_exclude = false;
	dictionary include_map;
	dictionary exclude_map;
	
	[option,0:SpriteGroup,1:SpriteGroup2,2:SpriteBatch,3:Custom]
	OutputType output_type = SpriteBatch;
	[text] string custom = 'PropStruct(0, %x;, %y;, %r;, %sx;, %sy;, %ps;, %pg;, %pi;, %p;, %l;, %sl;),';
	[text] float custom_ox = 0.5;
	[text] float custom_oy = 0.5;
	[text] bool layer0 = true;
	[text] bool layer1 = true;
	[text] bool layer2 = true;
	[text] bool layer3 = true;
	[text] bool layer4 = true;
	[text] bool layer5 = true;
	[text] bool layer6 = true;
	[text] bool layer7 = true;
	[text] bool layer8 = true;
	[text] bool layer9 = true;
	[text] bool layer10 = true;
	[text] bool layer11 = true;
	[text] bool layer12 = true;
	[text] bool layer13 = true;
	[text] bool layer14 = true;
	[text] bool layer15 = true;
	[text] bool layer16 = true;
	[text] bool layer17 = true;
	[text] bool layer18 = true;
	[text] bool layer19 = true;
	[text] bool layer20 = true;
	
	[text] array<string> includes;
	[text] array<string> excludes;
	
	script()
	{
		@g = get_scene();
		@cam = get_camera(0);
		@txt = create_textfield();
		txt.set_font('ProximaNovaReg', 42);
		txt.colour(0xFFFFFFFF);
		txt.align_horizontal(0);
		txt.align_vertical(-1);
		update_state(Idle);
	}
	
	void on_level_start()
	{
		has_include = includes.length() > 0;
		has_exclude = excludes.length() > 0;
		for(int i = int(includes.length()) - 1; i >=0; i--)
			include_map[includes[i]] = true;
		for(int i = int(excludes.length()) - 1; i >=0; i--)
			exclude_map[excludes[i]] = true;
		
		layer_vis[0 ] = layer0 ;
		layer_vis[1 ] = layer1 ;
		layer_vis[2 ] = layer2 ;
		layer_vis[3 ] = layer3 ;
		layer_vis[4 ] = layer4 ;
		layer_vis[5 ] = layer5 ;
		layer_vis[6 ] = layer6 ;
		layer_vis[7 ] = layer7 ;
		layer_vis[8 ] = layer8 ;
		layer_vis[9 ] = layer9 ;
		layer_vis[10] = layer10;
		layer_vis[11] = layer11;
		layer_vis[12] = layer12;
		layer_vis[13] = layer13;
		layer_vis[14] = layer14;
		layer_vis[15] = layer15;
		layer_vis[16] = layer16;
		layer_vis[17] = layer17;
		layer_vis[18] = layer18;
		layer_vis[19] = layer19;
		layer_vis[20] = layer20;
		
		in_game = true;
	}
	
	void update_state(State new_state)
	{
		state = new_state;
		
		if(state == Select)
		{
			txt.text('Press taunt to accept\nCancel: heavy');
		}
		else if(state == PickOrigin)
		{
			txt.text('Press taunt to select centre point\nCancel: heavy, Snap: Light');
		}
		else
		{
			txt.text('Press taunt to start selection');
			selected_props.resize(0);
				num_selected_props = 0;
		}
	}
	
	sprites@ get_sprites(string sprite_set)
	{
		sprites@ spr = cast<sprites>(sprite_sets[sprite_set]);
		if(spr is null)
		{
			@spr = create_sprites();
			spr.add_sprite_set(sprite_set);
			@sprite_sets[sprite_set] = spr;
		}
		
		return spr;
	}
	
	void print_sprite_group()
	{
		string output = 'sprite_group spr;\n';
		
		for(int i = 0; i < num_selected_props; i++)
		{
			prop@ p = selected_props[i];
			string sprite_set, sprite_name;
			sprite_from_prop(p, sprite_set, sprite_name);
			
			float scale_x = p.scale_x();
			float scale_y = p.scale_y();
			float rot = p.rotation();
			
			rectangle@ rect = get_sprites(sprite_set).get_sprite_rect(sprite_name, 0);
			float ox = (rect.left() + rect.get_width() * 0.5) * scale_x;
			float oy = (rect.top() + rect.get_height() * 0.5) * scale_y;
			rotate(ox, oy, rot * DEG2RAD, ox, oy);
			
			float off_x = p.x() - origin_x + ox;
			float off_y = p.y() - origin_y + oy;
			
			output += 'spr.add_sprite(\'' +  sprite_set + '\', \'' + sprite_name + '\'' +
						', 0.5, 0.5, ' + off_x + ', ' + off_y + ', ' + rot + ', ' + scale_x + ', ' + scale_y +
						', 0xFFFFFFFF, 0, ' + p.palette() + ', ' + p.layer() + ', ' + p.sub_layer() + ');\n';
		}
		
		puts(output);
	}
	
	void print_sprite_group2()
	{
		string sprite_set_name = '';
		string layer_sub_layer = '';
		string align_x_y = '';
		string off_x_y = '';
		string rotation = '';
		string scale_x_y = '';
		string colour = '';
		string frame = '';
		string palette = '';
		
		for(int i = 0; i < num_selected_props; i++)
		{
			prop@ p = selected_props[i];
			string sprite_set, sprite_name;
			sprite_from_prop(p, sprite_set, sprite_name);
			
			float scale_x = p.scale_x();
			float scale_y = p.scale_y();
			float rot = p.rotation();
			
			const float align_x = 0.5;
			const float align_y = 0.5;
			
			rectangle@ rect = get_sprites(sprite_set).get_sprite_rect(sprite_name, 0);
			float ox = (rect.left() + rect.get_width() * align_x) * scale_x;
			float oy = (rect.top() + rect.get_height() * align_y) * scale_y;
			rotate(ox, oy, rot * DEG2RAD, ox, oy);
			
			float off_x = p.x() - origin_x + ox;
			float off_y = p.y() - origin_y + oy;
			
			sprite_set_name += "'" + sprite_set + "','" + sprite_name + "',";
			layer_sub_layer += p.layer() + ',' + p.sub_layer() + ',';
			align_x_y += align_x + ',' + align_y + ',';
			off_x_y += off_x + ',' + off_y + ',';
			rotation += rot + ',';
			scale_x_y += scale_x + ',' + scale_y + ',';
			colour += '0xFFFFFFFF' + ',';
			frame += (0) + ',';
			palette += p.palette() + ',';
		}
		
		puts('========================================================================');
		puts('sprite_group spr(\n' +
			'array<string>={' + sprite_set_name + '},\n'
			'array<int>={' + layer_sub_layer + '},\n'
			'array<float>={' + align_x_y + '},\n'
			'array<float>={' + off_x_y + '},\n'
			'array<float>={' + rotation + '},\n'
			'array<float>={' + scale_x_y + '},\n'
			'array<uint>={' + colour + '},\n'
			'array<uint>={' + frame + '},\n'
			'array<uint>={' + palette + '});');
		puts('========================================================================');
	}
	
	void print_sprite_batch()
	{
		string sprite_set_name = '';
		string layer_sub_layer = '';
		string x_y = '';
		string scale = '';
		string rotation = '';
		string palette = '';
		
		for(int i = 0; i < num_selected_props; i++)
		{
			prop@ p = selected_props[i];
			string sprite_set, sprite_name;
			sprite_from_prop(p, sprite_set, sprite_name);
			
			sprite_set_name += "'" + sprite_set + "','" + sprite_name + "',";
			layer_sub_layer += p.layer() + ',' + p.sub_layer() + ',';
			x_y += (p.x() - origin_x) + ',' + (p.y() - origin_y) + ',';
			scale += p.scale_x() + ',' + p.scale_y() + ',';
			rotation += p.rotation() + ',';
			palette += p.palette() + ',';
		}
		
		puts('========================================================================');
		puts('SpriteBatch spr(\n' +
			'array<string>={' + sprite_set_name + '},\n'
			'array<int>={' + layer_sub_layer + '},\n'
			'array<float>={' + x_y + '},\n'
			'array<float>={' + scale + '},\n'
			'array<float>={' + rotation + '},\n'
			'array<uint>={' + palette + '});');
		puts('========================================================================');
	}
	
	void print_custom()
	{
		string output = '';
		uint custom_length = custom.length();
		
		for(int i = 0; i < num_selected_props; i++)
		{
			prop@ p = selected_props[i];
			string sprite_set, sprite_name;
			sprite_from_prop(p, sprite_set, sprite_name);
			
			float scale_x = p.scale_x();
			float scale_y = p.scale_y();
			float rot = p.rotation();
			
			rectangle@ rect = get_sprites(sprite_set).get_sprite_rect(sprite_name, 0);
			float ox = (rect.left() + rect.get_width() * custom_ox) * scale_x;
			float oy = (rect.top() + rect.get_height() * custom_oy) * scale_y;
			rotate(ox, oy, rot * DEG2RAD, ox, oy);
			
			float off_x = p.x() - origin_x + ox;
			float off_y = p.y() - origin_y + oy;
			
			uint j = 0;
			while(j < custom_length)
			{
				string chr = custom.substr(j++, 1);
				
				if(chr != '%'){
					output += chr;
					continue;
				}
				
				string var = '';
				while(j < custom_length)
				{
					chr = custom.substr(j++, 1);
					if(chr == ';') break;
					
					var += chr;
				}
				
				if(var == 'x')
					output += off_x + '';
				else if(var == 'y')
					output += off_y + '';
				
				else if(var == 'r')
					output += rot + '';
					
				else if(var == 'sx')
					output += scale_x + '';
				else if(var == 'sy')
					output += scale_y + '';
					
				else if(var == 'ps')
					output += p.prop_set() + '';
				else if(var == 'pg')
					output += p.prop_group() + '';
				else if(var == 'pi')
					output += p.prop_index() + '';
				
				else if(var == 'p')
					output += p.palette() + '';
				
				else if(var == 'l')
					output += p.layer() + '';
				else if(var == 'sl')
					output += p.sub_layer() + '';
			}
			
			output += '\n';
		}
		
		puts('=== ' + custom);
		puts('========================================================================');
		puts(output);
		puts('========================================================================');
	}
	
	void checkpoint_load()
	{
		@player = null;
		update_state(Idle);
		selected_props.resize(0);
		num_selected_props = 0;
	}
	
	void step(int entities)
	{
		if(!print) return;
		
		bool left_button = g.mouse_state(0) & 4 != 0;
		bool right_button = g.mouse_state(0) & 8 != 0;
		bool middle_button = g.mouse_state(0) & 16 != 0;
		
		if(player !is null)
		{
			if(state != Idle and player.heavy_intent() != 0)
			{
				update_state(Idle);
			}
			
			if(state == Select)
			{
				selection_x2 = g.mouse_x_world(0, 19);
				selection_y2 = g.mouse_y_world(0, 19);
				selection_hud_x2 = g.mouse_x_hud(0);
				selection_hud_y2 = g.mouse_y_hud(0);
				
				float x1 = min(selection_x1, selection_x2);
				float y1 = min(selection_y1, selection_y2);
				float x2 = max(selection_x1, selection_x2);
				float y2 = max(selection_y1, selection_y2);
				
				int prop_count = g.get_prop_collision(y1, y2, x1, x2);
				num_selected_props = 0;
				selected_props.resize(int(max(prop_count, selected_props.length())));
				for(int i = 0; i < prop_count; i++)
				{
					prop@ p = g.get_prop_collision_index(i);
					if(!layer_vis[p.layer()]) continue;
					
					const string prop_id = p.prop_set() + '.' + p.prop_group() + '.' + p.prop_index();
					if(has_include)
					{
						if(!include_map.exists(prop_id)) continue;
					}
					if(has_exclude)
					{
						if(exclude_map.exists(prop_id)) continue;
					}
					
					@selected_props[num_selected_props++] = p;
				}
				selected_props.resize(num_selected_props);
				
				if(player.taunt_intent() != 0)
				{
					update_state(PickOrigin);
					origin_x = selection_x2;
					origin_y = selection_y2;
				}
			}
			else if(state == Idle)
			{
				if(player.taunt_intent() != 0)
				{
					update_state(Select);
					selection_x1 = selection_x2 = g.mouse_x_world(0, 19);
					selection_y1 = selection_y2 = g.mouse_y_world(0, 19);
					selection_hud_x1 = selection_hud_x2 = g.mouse_x_hud(0);
					selection_hud_y1 = selection_hud_y2 = g.mouse_y_hud(0);
				}
			}
			else if(state == PickOrigin)
			{
				origin_x = g.mouse_x_world(0, 19);
				origin_y = g.mouse_y_world(0, 19);
				
				if(player.light_intent() != 0)
				{
					origin_x = floor(origin_x / 48.0) * 48.0;
					origin_y = floor(origin_y / 48.0) * 48.0;
				}
				
				if(player.taunt_intent() != 0)
				{
					switch(output_type)
					{
						case SpriteGroup: print_sprite_group(); break;
						case SpriteGroup2: print_sprite_group2(); break;
						case SpriteBatch: print_sprite_batch(); break;
						case Custom: print_custom(); break;
					}
					update_state(Idle);
				}
			}
		}
		else
		{
			entity@ e = controller_entity(0);
			@player = (@e != null ? e.as_controllable() : null);
		}
	}
	
	void draw(float sub_frame)
	{
		if(state == Select or state == PickOrigin)
		{
			float x1 = min(selection_x1, selection_x2);
			float y1 = min(selection_y1, selection_y2);
			float x2 = max(selection_x1, selection_x2);
			float y2 = max(selection_y1, selection_y2);
			
			world_to_screen(cam, x1, y1, x1, y1);
			world_to_screen(cam, x2, y2, x2, y2);
			
			for(int i = 0; i < num_selected_props; i++)
			{
				prop@ p = selected_props[i];
				string sprite_set, sprite_name;
				sprite_from_prop(p, sprite_set, sprite_name);
				
				const int layer = p.layer();
				const int sub_layer = p.sub_layer();
				const float x = p.x();
				const float y = p.y();
				const float rotation = p.rotation();
				const float scale_x = p.scale_x();
				const float scale_y = p.scale_y();
				get_sprites(sprite_set).draw_world(21, 21, sprite_name, 0, p.palette(), x, y, rotation, scale_x, scale_y, 0xAAFF00FF);
			}
			
			g.draw_rectangle_hud(21, 21, x1, y1, x2, y2, 0, 0x11FFFFFF);
			outline_rect_hud(g, x1, y1, x2, y2, 21, 21, 0.5, 0x66FFFFFF);
			
			if(state == PickOrigin)
			{
				float x, y;
				world_to_screen(cam, origin_x, origin_y, x, y);
				const float w = 0.5;
				const float s = 20;
				const uint c = 0x66FFFFFF;
				g.draw_line(21, 21, x - s, y, x + s, y, w, c);
				g.draw_line(21, 21, x, y - s, x, y + s, w, c);
			}
		}
		
		shadowed_text_hud(txt, 21, 22, 0, SCREEN_TOP + 10, ox:3, oy:3);
	}
	
}


class _PropTrigger : trigger_base
{
	
	scene@ g;
	scripttrigger @self;
	
	bool run_prev = false;
	
	bool has_include = false;
	bool has_exclude = false;
	dictionary include_map;
	dictionary exclude_map;
	
	[text] bool run = false;
	
	[text] array<string> includes;
	[text] array<string> excludes;
	
	_PropTrigger()
	{
		@g = get_scene();
	}
	
	void build_includes()
	{
		include_map.empty();
		exclude_map.empty();
		
		has_include = includes.length() > 0;
		has_exclude = excludes.length() > 0;
		for(int i = int(includes.length()) - 1; i >=0; i--)
		{
			include_map[includes[i]] = true;
		}
		for(int i = int(excludes.length()) - 1; i >=0; i--)
		{
			exclude_map[excludes[i]] = true;
		}
	}
	
	array<prop@>@ get_props()
	{
		build_includes();
		
		const float x = self.x();
		const float y = self.y();
		const float r = self.radius();
		int prop_count = g.get_prop_collision(y-r, y+r, x-r, x+r);
		
		array<prop@> props;
		
		for(int i = 0; i < prop_count; i++)
		{
			prop@ p = g.get_prop_collision_index(i);
			
			const string prop_id = p.prop_set() + '.' + p.prop_group() + '.' + p.prop_index();
			if(has_include)
			{
				if(!include_map.exists(prop_id)) continue;
			}
			if(has_exclude)
			{
				if(exclude_map.exists(prop_id)) continue;
			}
			
			props.insertLast(p);
		}
		
		return @props;
	}
	
	void _run() {}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		self.square(true);
	}
	
	void editor_step()
	{
		if(run_prev != run)
		{
			_run();
			run_prev = run;
		}
	}
	
	void activate(controllable @e) {}
	
}


class PropMover : _PropTrigger
{
	
	[text] int layer1 = -1;
	[text] int sub_layer1 = -1;
	[text] int layer2 = -1;
	[text] int sub_layer2 = -1;
	[text] int shift_layer = 0;
	[text] int shift_sub_layer = 0;
	
	void _run()
	{
		if((shift_layer == 0 and shift_sub_layer == 0) and (layer1 == layer2 and sub_layer1 == sub_layer2 or layer2 == -1 and sub_layer2 == -1))
			return;
		
		array<prop@>@ props = get_props();
		const uint prop_count = props.length();
		
		for(uint i = 0; i < prop_count; i++)
		{
			prop@ p = props[i];
			
			if(layer1 != -1 and int(p.layer()) != layer1 or sub_layer1 != -1 and int(p.sub_layer()) != sub_layer1)
				continue;
			
			if(shift_layer !=0 or shift_sub_layer != 0)
			{
				p.layer(p.layer() + shift_layer);
				p.sub_layer(p.sub_layer() + shift_sub_layer);
			}
			else
			{
				if(layer2 != -1)
					p.layer(layer2);
				if(sub_layer2 != -1)
					p.sub_layer(sub_layer2);
			}
		}
	}
	
}


//class PropScaler : _PropTrigger
//{
//	
//	[text] int layer1 = -1;
//	[text] int sub_layer1 = -1;
//	[text] int layer2 = -1;
//	[text] int sub_layer2 = -1;
//	[text] int shift_layer = 0;
//	[text] int shift_sub_layer = 0;
//	
//	void _run()
//	{
//		if((shift_layer == 0 and shift_sub_layer == 0) and (layer1 == layer2 and sub_layer1 == sub_layer2 or layer2 == -1 and sub_layer2 == -1))
//			return;
//		
//		array<prop@>@ props = get_props();
//		const uint prop_count = props.length();
//		
//		for(uint i = 0; i < prop_count; i++)
//		{
//			prop@ p = props[i];
//			
//			if(layer1 != -1 and int(p.layer()) != layer1 or sub_layer1 != -1 and int(p.sub_layer()) != sub_layer1)
//				continue;
//			
//			if(shift_layer !=0 or shift_sub_layer != 0)
//			{
//				p.layer(p.layer() + shift_layer);
//				p.sub_layer(p.sub_layer() + shift_sub_layer);
//			}
//			else
//			{
//				if(layer2 != -1)
//					p.layer(layer2);
//				if(sub_layer2 != -1)
//					p.sub_layer(sub_layer2);
//			}
//		}
//	}
//	
//}