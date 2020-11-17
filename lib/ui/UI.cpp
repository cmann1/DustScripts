#include '../std.cpp';
#include '../input/Mouse.cpp';
#include '../math/Vec2.cpp';
#include '../math/Rect.cpp';
#include 'Align.cpp';
#include 'Side.cpp';
#include 'TooltipData.cpp';

class UI
{
	
	float text_scale = 0.85;
	
	float padding = 5;
	uint bg_colour = 0xDD090909;
	uint hover_bg_colour = 0xDD2b1b1b;
	uint border_colour = 0x99555555;
	uint highlight_colour = 0xCCba483c;
	uint highlight_border_colour = 0xCCb16860;
	uint highlight_light_colour = 0x55000000 | (highlight_colour & 0x00FFFFFF);
	
	float scrollbar_width = 10;
	float scrollbar_thumb_size = 20;
	uint scrollbar_thumb_colour = 0xDD4c3636;
	uint scrollbar_thumb_hover_colour = 0xDD966c6c;
	uint scrollbar_track_colour = 0x66000000;
	
	float tooltip_spacing = 8;
	Vec2 tooltip_shadow_offset(4, 4);
	uint tooltip_bg_colour = 0xDD090909;
	uint tooltip_shadow_colour = 0x22000000;
	uint tooltip_border_colour = 0xDD1f1f1f;
	
	Mouse@ mouse = Mouse(true);
	
	private scene@ g;
	private textfield@ text_field;
	
	private float mouse_scale = 1;
	
	private array<TooltipData> tooltips;
	
	UI()
	{
		@g = get_scene();
    mouse.scale_hud = true;
		
		@text_field = create_textfield();
		text_field.set_font('ProximaNovaReg', 26);
	}
	
	void step()
	{
		mouse.step();
	}
	
	void post_draw()
	{
		if(tooltips.size() > 0)
		{
			for(uint i = 0; i < tooltips.size(); i++)
			{
				TooltipData tooltip = tooltips[i];
				
				g.draw_rectangle_hud(22, 22,
					tooltip.rect.x1 + tooltip_shadow_offset.x,
					tooltip.rect.y1 + tooltip_shadow_offset.y,
					tooltip.rect.x2 + tooltip_shadow_offset.x,
					tooltip.rect.y2 + tooltip_shadow_offset.y,
					0, tooltip_shadow_colour);
				g.draw_rectangle_hud(22, 22, tooltip.rect.x1, tooltip.rect.y1, tooltip.rect.x2, tooltip.rect.y2, 0, tooltip_bg_colour);
				outline_outside(tooltip.rect, 1, tooltip_border_colour);
				text(tooltip.text, tooltip.rect.centre_x, tooltip.rect.centre_y, Align::Centre, Align::Middle);
			}
			
			tooltips.resize(0);
		}
	}
	
	// Mouse
	// -------------------------------
	
	bool mouse_intersects(const Rect &in rect)
	{
		return mouse.x >= rect.x1 && mouse.x <= rect.x2 && mouse.y >= rect.y1 && mouse.y <= rect.y2;
	}
	
	// Drawing
	// -------------------------------
	
	void outline_outside(const Rect &in rect, float width, uint colour)
	{
		const float hwidth = width * 0.5;
		
		// Top
		g.draw_rectangle_hud(22, 22,
			rect.x1 - hwidth, rect.y1 - width,
			rect.x2 + hwidth, rect.y1,
			0, colour);
		// Bottom
		g.draw_rectangle_hud(22, 22,
			rect.x1 - hwidth, rect.y2,
			rect.x2 + hwidth, rect.y2 + width,
			0, colour);
		// Left
		g.draw_rectangle_hud(22, 22,
			rect.x1 - width, rect.y1 - hwidth,
			rect.x1, rect.y2 + hwidth,
			0, colour);
		// Right
		g.draw_rectangle_hud(22, 22,
			rect.x2, rect.y1 - hwidth,
			rect.x2 + width, rect.y2 + hwidth,
			0, colour);
	}
	
	void border(const Rect &in rect, bool highlight = false)
	{
		outline_outside(rect, 1, highlight ? highlight_border_colour : border_colour);
	}
	
	void highlight(const Rect &in rect)
	{
		outline_outside(rect, 2, highlight_colour);
	}
	
	void highlight_light(const Rect &in rect)
	{
		outline_outside(rect, 2, highlight_light_colour);
	}

	void text(string text, float x, float y, int align_h = -1, int align_v = 1, float rotation = 0)
	{
		text_field.align_horizontal(align_h);
		text_field.align_vertical(align_v);
		text_field.text(text);
		text_field.draw_hud(22, 22, x, y, text_scale, text_scale, rotation);
	}
	
	void text_size(string text, float &out width, float &out height)
	{
		text_field.text(text);
		width = text_field.text_width();
		height = text_field.text_height();
	}
	
	void tooltip(string text, const Rect &in rect, Side side)
	{
		text_field.text(text);
		const float width = text_field.text_width() * text_scale + padding * 4;
		const float height = text_field.text_height() * text_scale + padding * 4;
		
		Rect tooltipRect;
		
		while(true)
		{
			if(side == Side::Left || side == Side::Right)
			{
				if(side == Side::Left)
				{
					tooltipRect.x2 = rect.x1 - tooltip_spacing;
					tooltipRect.x1 = tooltipRect.x2 - width;
					
					if(tooltipRect.x1 < SCREEN_LEFT && rect.x1 - SCREEN_LEFT < SCREEN_RIGHT - rect.x2)
					{
						side = Side::Right;
						continue;
					}
				}
				else
				{
					tooltipRect.x1 = rect.x2 + tooltip_spacing;
					tooltipRect.x2 = tooltipRect.x1 + width;
					
					if(tooltipRect.x2 > SCREEN_RIGHT && rect.x1 - SCREEN_LEFT > SCREEN_RIGHT - rect.x2)
					{
						side = Side::Left;
						continue;
					}
				}
				
				tooltipRect.y1 = (rect.y1 + rect.y2) * 0.5 - height * 0.5;
				tooltipRect.y2 = tooltipRect.y1 + height;
			}
			else
			{
				if(side == Side::Top)
				{
					tooltipRect.y2 = rect.y1 - tooltip_spacing;
					tooltipRect.y1 = tooltipRect.y2 - height;
					
					if(tooltipRect.y1 < SCREEN_TOP && rect.y1 - SCREEN_TOP < SCREEN_BOTTOM - rect.y2)
					{
						side = Side::Bottom;
						continue;
					}
				}
				else
				{
					tooltipRect.y1 = rect.y2 + tooltip_spacing;
					tooltipRect.y2 = tooltipRect.y1 + height;
					
					if(tooltipRect.y2 > SCREEN_BOTTOM && rect.y1 - SCREEN_TOP > SCREEN_BOTTOM - rect.y2)
					{
						side = Side::Top;
						continue;
					}
				}
				
				tooltipRect.x1 = (rect.x1 + rect.x2) * 0.5 - width * 0.5;
				tooltipRect.x2 = tooltipRect.x1 + width;
			}
			
			break;
		}
		
		tooltips.insertLast(TooltipData(text, tooltipRect));
	}
	
}

