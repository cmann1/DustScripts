#include '../fonts.cpp';
#include '../graphics/Graphics.cpp';
#include '../graphics/GraphicsUtils.cpp';

class Style
{
	
	Graphics@ graphics;
	
	uint text_clr						= 0xffffffff;
	uint normal_bg_clr					= 0xd9000000;
	uint normal_border_clr				= 0x33ffffff;
	uint highlight_bg_clr				= 0xd9622a24;
	uint highlight_border_clr			= 0xd9933c34;
	uint selected_bg_clr				= 0xd9000000;
	uint selected_border_clr			= 0xffb16860;
	uint selected_highlight_bg_clr		= 0xd9622a24;
	uint selected_highlight_border_clr	= 0xffb16860;
	uint disabled_bg_clr				= 0xd9000000;
	uint disabled_border_clr			= 0x26ffffff;
	uint disabled_alpha					= 0x40000000;
	
	float border_size = 1;
	float spacing = 4;
	float button_pressed_icon_offset = 1;
	
	// The default font used when creating labels, etc.
	string default_font = font::PROXIMANOVA_REG;
	// The default text size used when creating labels, etc.
	uint default_text_size = 26;
	// The default scaling for text - should be set before creating any UI. Changing it after may not reflect correctly everywhere.
	float default_text_scale = 0.75;
	
	// Text measurements don't seem to line up exactly always. Use these global values to offset
	float text_offset_x = -1;
	float text_offset_y = -1;
	
	private string current_font = default_font;
	private uint current_text_size = default_text_size;
	private TextAlign current_align_v = TextAlign::Left;
	private TextAlign current_align_h = TextAlign::Top;
	
	private textfield@ text_field;
	private dictionary sprite_sets;
	
	Style()
	{
		@text_field = create_textfield();
		text_field.set_font(current_font, current_text_size);
		text_field.align_horizontal(current_align_h);
		text_field.align_vertical(current_align_v);
	}
	
	void measure_text(const string text, const string font, const uint size, const float scale, float &out width, float &out height)
	{
		if(current_font != font || current_text_size != size)
			text_field.set_font(current_font = font, current_text_size = size);
		
		text_field.text(text);
		width  = text_field.text_width() * scale;
		height = text_field.text_height() * scale;
	}
	
	sprites@ get_sprite_for_set(const string &in sprite_set)
	{
		if(sprite_sets.exists(sprite_set))
		{
			return cast<sprites@>(sprite_sets[sprite_set]);
		}
		
		sprites@ sprite = create_sprites();
		sprite.add_sprite_set(sprite_set);
		@sprite_sets[sprite_set] = sprite;
		
		return sprite;
	}
	
	/*
	 * Drawing methods
	 */
	//{
	
	void draw_interactive_element(const Element@ &in element, const bool &in highlighted, const bool &in selected, const bool &in disabled)
	{
		// Fill/bg
		
		const uint bg_clr = disabled ? disabled_bg_clr
			: (highlighted && selected ? selected_highlight_bg_clr
				: selected ? selected_bg_clr : (highlighted ? highlight_bg_clr : normal_bg_clr));
		
		graphics.draw_rectangle(
			element.x1, element.y1, element.x2, element.y2,
			0, bg_clr);
		
		// Border
		
		const uint border_clr = disabled ? disabled_border_clr
			: (highlighted && selected ? selected_highlight_border_clr
				: selected ? selected_border_clr : (highlighted ? highlight_border_clr : normal_border_clr));
		
		Graphics::outline(graphics, element.x1, element.y1, element.x2, element.y2, border_size, border_clr);
	}
	
	void draw_text(const string text, const float x, const float y, const TextAlign align_h, const TextAlign align_v, float scale=-1, string font='', uint size=0)
	{
		if(scale <= 0)
		{
			scale = default_text_scale;
		}
		
		if(font == '')
		{
			font = default_font;
		}
		
		if(size == 0)
		{
			size = default_text_size;
		}
		
		if(current_font != font || current_text_size != size)
			text_field.set_font(current_font = font, current_text_size = size);
		
		if(current_align_h != align_h)
			text_field.align_horizontal(current_align_h = align_h);
		
		if(current_align_v != align_v)
			text_field.align_vertical(current_align_v = align_v);
		
		text_field.text(text);
		
		graphics.draw_text(text_field,
			x + text_offset_x * scale, y + text_offset_y * scale,
			scale, scale, 0);
	}
	
	//}
	
}