#include '../fonts.cpp';

class DebugTextState
{
	
	textfield@ text_field;
	string font_name = font::PROXIMANOVA_REG;
	uint size = 36;
	int align_x = -1;
	int align_y = -1;
	uint colour = 0xffffffff;
	
	DebugTextState()
	{
		@text_field = create_textfield();
		text_field.set_font(font_name, size);
		text_field.align_horizontal(align_x);
		text_field.align_vertical(align_y);
		text_field.colour(colour);
	}
	
	void set(string font_name, uint size, int align_x, int align_y, uint colour)
	{
		if(this.font_name != font_name || this.size != size)
		{
			this.font_name = font_name;
			this.size = size;
			text_field.set_font(font_name, size);
		}
		
		if(this.align_x != align_x)
		{
			this.align_x = align_x;
			text_field.align_horizontal(align_x);
		}
		
		if(this.align_y != align_y)
		{
			this.align_y = align_y;
			text_field.align_vertical(align_y);
		}
		
		if(this.colour != colour)
		{
			this.colour = colour;
			text_field.colour(colour);
		}
	}
	
}