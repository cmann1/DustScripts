#include '../enums/AttackType.cpp';

/** Returns the correct sprite name, frame, and facing based on the entity's state and attack state. */
void get_entity_sprite_info(
	controllable@ e, string &out sprite_name, uint &out frame, int &out face)
{
	sprites@ spr = e.get_sprites();
	
	if(e.attack_state() == AttackType::Idle)
	{
		sprite_name = e.sprite_index();
		frame = uint(e.state_timer());
		face = e.face();
	}
	else
	{
		sprite_name = e.attack_sprite_index();
		frame = uint(e.attack_timer());
		face = e.attack_face();
	}
	
	// No clue why, but get_animation_length can return 0?
	// Unable to reproduce
	// if(spr.get_animation_length(sprite_name) == 0)
	// {
	// 	puts('-----------------------------------------------------');
	// 	puts('--- spr.get_animation_length(sprite_name) == 0');
	// 	puts(' sprite_name: ' + sprite_name);
	// 	puts(' frame: ' + frame);
	// }
	
	frame = spr.get_animation_length(sprite_name) != 0
		? frame % spr.get_animation_length(sprite_name)
		: 0;
}
