void build_sprite(message@ msg, string name, int ox=0, int oy=0, string var_prefix="spr_")
{
	msg.set_string(name, var_prefix + name);
	if(ox != 0) msg.set_int(name + "|offsetx", ox);
	if(oy != 0) msg.set_int(name + "|offsety", oy);
}

void build_sound(message@ msg, string name, float loop_seconds = 0, string var_prefix="snd_")
{
	msg.set_string(name, var_prefix + name);
	if(loop_seconds > 0) msg.set_int(name + "|loop", int(44100 * loop_seconds)); // 2 seconds in
}