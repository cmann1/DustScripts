#include '../Element.cpp';
#include '../Side.cpp';
#include '../../drawing/Sprite.cpp';

class PropIcon : Element
{
	
	PropSelector@ propSelector;
	const PropIndex@ prop_index;
	int palette;
	
	bool selected;
	private Sprite sprite;
	
	PropIcon(UI@ ui, PropSelector@ propSelector, const PropIndex@ prop_index, int palette = -1)
	{
		super(ui);
		
		@this.propSelector = propSelector;
		@this.prop_index = prop_index;
		this.palette = palette;
		
		width = height = 64;
		
		string sprite_set = prop_index.sprite_set_name();
		sprite.set(prop_index.sprite_set_name(), prop_index.sprite_name());
	}
	
	void draw(scene@ g, Rect rect) override
	{
		const bool is_mouse_over = ui.mouse_intersects(rect);
		
		if(is_mouse_over)
		{
			if(ui.mouse.right_down)
			{
				@propSelector.focused_prop = this;
			}
			
			ui.tooltip(prop_index.name, rect, Side::Bottom);
		}
		
		g.draw_rectangle_hud(22, 22, rect.x1, rect.y1, rect.x2, rect.y1 + height, 0, is_mouse_over ? ui.hover_bg_colour : ui.bg_colour);
	
		if(@propSelector.focused_prop == @this || @propSelector.focused_prop == null)
		{
			const float padding = ui.padding * 2;
			const float scale = min(
				(min(width, sprite.sprite_width) - padding) / sprite.sprite_width,
				(min(height, sprite.sprite_height) - padding) / sprite.sprite_height);// + (is_mouse_over ? 0.075 : 0);
			
			sprite.draw_hud(22, 22, 0, palette == -1 ? 0 : palette, rect.centre_x, rect.centre_y, 0, scale, scale, 0xFFFFFFFF);
		}
		
		if(selected)
		{
			ui.highlight(rect);
		}
		else if(is_mouse_over)
		{
			ui.highlight_light(rect);
		}
		
		if(is_mouse_over && ui.mouse.left_press)
		{
//			puts('CLICK ME ' + prop_index.name + ' > ' + rect.to_string() + '' + str(ui.mouse_x, ui.mouse_y));
			propSelector.select_prop(this);
		}
	}
	
}