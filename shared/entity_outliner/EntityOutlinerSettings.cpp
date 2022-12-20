#include '../../lib/std.cpp';
#include '../../lib/math/math.cpp';
#include '../../lib/utils/colour.cpp';

class EntityOutlinerSettings
{
	
	[text] bool enabled = true;
	[colour,alpha] uint colour1 = 0xffffffff;
	[colour,alpha] uint colour2 = 0xffffffff;
	[text] bool draw_double = true;
	[text] bool outline_player = true;
	[text] bool outline_prisms = true;
	[text] bool scale_with_camera = false;
	
	[text] int player_layer1 = 18;
	[text] int player_sublayer1 = 9;
	[text|tooltip:'Settings for the second sprite when DrawDouble is on.\nLeave at -1 to use the same value as the first']
	int player_layer2 = -1;
	[text] int player_sublayer2 = -1;
	
	[text] int enemy_layer1 = 18;
	[text] int enemy_sublayer1 = 7;
	[text] int enemy_layer2 = -1;
	[text] int enemy_sublayer2 = -1;
	
	[text] int apple_layer1 = 18;
	[text] int apple_sublayer1 = 6;
	[text] int apple_layer2 = -1;
	[text] int apple_sublayer2 = -1;
	
	[text] float offset1_x = -2;
	[text] float offset1_y = 2;
	[text] float offset2_x = 2;
	[text] float offset2_y = -2;
	[text] float scale1 = 1;
	[text] float scale2 = 1;
	
	float transition;
	float t;
	
	void update_transition(EntityOutlinerSettings@ from, EntityOutlinerSettings@ target)
	{
		this.t = max(this.t - 1, 0.0);
		const float t = 1 - this.t / transition;
		
		enabled = lerp_bool(from.enabled, target.enabled, t);
		colour1 = colour::lerp(from.colour1, target.colour1, t);
		colour2 = colour::lerp(from.colour2, target.colour2, t);
		draw_double = lerp_bool(from.draw_double, target.draw_double, t);
		outline_player = lerp_bool(from.outline_player, target.outline_player, t);
		outline_prisms = lerp_bool(from.outline_prisms, target.outline_prisms, t);
		scale_with_camera = lerp_bool(from.scale_with_camera, target.scale_with_camera, t);
		
		player_layer1 = lerp_layer(from.player_layer1, target.player_layer1, t);
		player_sublayer1 = lerp_layer(from.player_sublayer1, target.player_sublayer1, t);
		enemy_layer1 = lerp_layer(from.enemy_layer1, target.enemy_layer1, t);
		enemy_sublayer1 = lerp_layer(from.enemy_sublayer1, target.enemy_sublayer1, t);
		apple_layer1 = lerp_layer(from.apple_layer1, target.apple_layer1, t);
		apple_sublayer1 = lerp_layer(from.apple_sublayer1, target.apple_sublayer1, t);
		
		if(this.t > 0)
		{
			player_layer2 = lerp_layer(
				from.player_layer2 >= 0 ? from.player_layer2 : from.player_layer1,
				target.player_layer2 >= 0 ? target.player_layer2 : target.player_layer1, t);
			player_sublayer2 = lerp_layer(
				from.player_sublayer2 >= 0 ? from.player_sublayer2 : from.player_sublayer1,
				target.player_sublayer2 >= 0 ? target.player_sublayer2 : target.player_sublayer1, t);
			enemy_layer2 = lerp_layer(
				from.enemy_layer2 >= 0 ? from.enemy_layer2 : from.enemy_layer1,
				target.enemy_layer2 >= 0 ? target.enemy_layer2 : target.enemy_layer1, t);
			enemy_sublayer2 = lerp_layer(
				from.enemy_sublayer2 >= 0 ? from.enemy_sublayer2 : from.enemy_sublayer1,
				target.enemy_sublayer2 >= 0 ? target.enemy_sublayer2 : target.enemy_sublayer1, t);
			apple_layer2 = lerp_layer(
				from.apple_layer2 >= 0 ? from.apple_layer2 : from.apple_layer1,
				target.apple_layer2 >= 0 ? target.apple_layer2 : target.apple_layer1, t);
			apple_sublayer2 = lerp_layer(
				from.apple_sublayer2 >= 0 ? from.apple_sublayer2 : from.apple_sublayer1,
				target.apple_sublayer2 >= 0 ? target.apple_sublayer2 : target.apple_sublayer1, t);
		}
		else
		{
			player_layer2 = target.player_layer2;
			player_sublayer2 = target.player_sublayer2;
			enemy_layer2 = target.enemy_layer2;
			enemy_sublayer2 = target.enemy_sublayer2;
			apple_layer2 = target.apple_layer2;
			apple_sublayer2 = target.apple_sublayer2;
		}
		
		offset1_x = lerp(from.offset1_x, target.offset1_x, t);
		offset1_y = lerp(from.offset1_y, target.offset1_y, t);
		offset2_x = lerp(from.offset2_x, target.offset2_x, t);
		offset2_y = lerp(from.offset2_y, target.offset2_y, t);
		scale1 = lerp(from.scale1, target.scale1, t);
		scale2 = lerp(from.scale2, target.scale2, t);
	}
	
	bool lerp_bool(const bool from, const bool to, const float t)
	{
		return (from ? 1 : 0) * (1.0 - t) + (to ? 1 : 0) * t >= 0.5;
	}
	
	int lerp_layer(const int from, const int to, const float t)
	{
		return int(round(from * (1.0 - t) + to * t));
	}
	
}
