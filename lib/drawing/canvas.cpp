namespace draw
{
	
	void shadowed_text(canvas@ c, textfield@ tf,
		const int layer, const int sub_layer,
		const float x, const float y,
		const float scale_x=1, const float scale_y=1,
		const float rotation=0, const uint shadow_colour=0x77000000,
		const float ox=5, const float oy=5)
	{
		const uint colour = tf.colour();
		
		tf.colour(shadow_colour);
		c.draw_text(tf, x + ox, y + oy, scale_x, scale_y, rotation);
		
		tf.colour(colour);
		c.draw_text(tf, x, y, scale_x, scale_y, rotation);
	}
	
	void outlined_text(canvas@ c, textfield@ tf,
		const int layer, const int sub_layer,
		float x, const float y,
		const float scale_x=1, const float scale_y=1,
		const float rotation=0, const uint shadow_colour=0x77000000,
		const float thickness=2)
	{
		const uint colour = tf.colour();
		
		tf.colour(shadow_colour);
		c.draw_text(tf, x - thickness, y, scale_x, scale_y, rotation);
		c.draw_text(tf, x + thickness, y, scale_x, scale_y, rotation);
		c.draw_text(tf, x, y - thickness, scale_x, scale_y, rotation);
		c.draw_text(tf, x, y + thickness, scale_x, scale_y, rotation);
		
		tf.colour(colour);
		c.draw_text(tf, x, y, scale_x, scale_y, rotation);
	}
	
}