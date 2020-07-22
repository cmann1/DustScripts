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
	
	private string current_font = font::PROXIMANOVA_REG;
	private uint current_text_size = 26;
	private string new_font = font::PROXIMANOVA_REG;
	private uint new_text_size = 26;
	
	private textfield@ text_field;
	private dictionary sprite_sets;
	
	Style()
	{
		@text_field = create_textfield();
		
		text_field.set_font(current_font, current_text_size);
	}
	
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
	
}