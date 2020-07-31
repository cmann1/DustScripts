#include '../math/math.cpp';
#include '../fonts.cpp';
#include '../utils/colour.cpp';
#include 'TextAlign.cpp';
#include 'elements/Element.cpp';
#include 'utils/Orientation.cpp';
#include 'utils/DrawingContext.cpp';

class Style
{
	
	uint text_clr						= 0xffffffff;
	uint normal_bg_clr					= 0xd9050505;
	uint normal_border_clr				= 0x33ffffff;
	uint highlight_bg_clr				= 0xd9521c17;
	uint highlight_border_clr			= 0xd9933c34;
	uint selected_bg_clr				= 0xd9702d26;
	uint selected_border_clr			= 0xffb16860;
	uint selected_highlight_bg_clr		= 0xd982352c;
	uint selected_highlight_border_clr	= 0xffb16860;
	uint disabled_bg_clr				= 0xa6000000;
	uint disabled_border_clr			= 0x26ffffff;
	uint secondary_bg_clr				= 0x33ffffff;
	
	uint popup_bg_clr					= normal_bg_clr;
	uint popup_border_clr				= 0;
	uint popup_shadow_clr				= 0x30000000;
	bool blur_popup_bg					= true;
	
	uint dialog_bg_clr					= normal_bg_clr;
	uint dialog_border_clr				= 0;
	uint dialog_shadow_clr				= 0x40000000;
	bool blur_dialog_bg					= true;
	
	float shadow_offset_x	= 3;
	float shadow_offset_y	= 3;
	float disabled_alpha	= 0.35;
	
	float border_size = 1;
	float selected_border_size = 2;
	float spacing = 4;
	float button_pressed_icon_offset = 1;
	
	float gripper_thickness = 2;
	float gripper_margin = 1;
	
	float divider_thickness = 1;
	float divider_margin = 1;
	
	float snap_distance  = 6;
	float snap_threshold = 3;
	
	// The default font used when creating labels, etc.
	string default_font = font::PROXIMANOVA_REG;
	// The default text size used when creating labels, etc.
	uint default_text_size = 26;
	// The default scaling for text - should be set before creating any UI. Changing it after may not reflect correctly everywhere.
	float default_text_scale = 0.75;
	
	string tooltip_font = default_font;
	uint tooltip_text_size = default_text_size;
	float tooltip_text_scale = default_text_scale;
	uint tooltip_text_colour = text_clr;
	float tooltip_padding = spacing * 2;
	float tooltip_blur_inset = 0;
	
	int tooltip_fade_frames = 6;
	float tooltip_fade_offset = 5;
	float tooltip_default_spacing = spacing * 1.5;
	
	float default_scrollbar_size = 8;
	float scrollbar_fixed_size = 20;
	
	// Text measurements don't seem to line up exactly always. Use these global values to offset
	float text_offset_x = -1;
	float text_offset_y = -1;
	
	private string current_font = default_font;
	private uint current_text_size = default_text_size;
	private TextAlign current_align_v = TextAlign::Left;
	private TextAlign current_align_h = TextAlign::Top;
	private uint current_text_colour = text_clr;
	
	private textfield@ text_field;
	private dictionary sprite_sets;
	
	private DrawingContext@ default_ctx = DrawingContext();
	private DrawingContext@ ctx = @default_ctx;
	
	private int drawing_context_pool_size = 16;
	private int drawing_context_pool_index = 0;
	private array<DrawingContext@> drawing_context_pool(drawing_context_pool_size);
	
	private scene@ g;
	
	uint _layer;
	uint _sub_layer;
	bool _hud;
	
	Style(){}
	
	Style(bool hud)
	{
		_hud = hud;
		
		@g = get_scene();
		
		@text_field = create_textfield();
		text_field.set_font(current_font, current_text_size);
		text_field.align_horizontal(current_align_h);
		text_field.align_vertical(current_align_v);
	}
	
	void measure_text(const string text, string font, uint size, float scale_x, float scale_y, float &out width, float &out height)
	{
		if(is_nan(scale_x))
			scale_x = default_text_scale;
		if(is_nan(scale_y))
			scale_y = default_text_scale;
		
		if(font == '')
			font = default_font;
		if(size == 0)
			size = default_text_size;
		
		if(current_font != font || current_text_size != size)
			text_field.set_font(current_font = font, current_text_size = size);
		
		text_field.text(text);
		width  = text_field.text_width() * scale_x;
		height = text_field.text_height() * scale_y;
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
	
	void set_default_clipping_region(const float x1, const float y1, const float x2, const float y2)
	{
		default_ctx.x1 = x1;
		default_ctx.y1 = y1;
		default_ctx.x2 = x2;
		default_ctx.y2 = y2;
	}
	
	DrawingContext@ reset_drawing_context(Element@ root)
	{
		@ctx = @default_ctx;
		@ctx.root = @root;
		ctx.clipping_mode = ClippingMode::Outside;
		ctx.num_children = 0;
		
		return @default_ctx;
	}
	
	DrawingContext@ push_drawing_context(Element@ new_root, const int num_children)
	{
		DrawingContext@ new_ctx = drawing_context_pool_index > 0
			? @drawing_context_pool[--drawing_context_pool_index]
			: DrawingContext();
		
		@new_ctx.parent = @ctx;
		@new_ctx.root = @new_root;
		new_ctx.num_children = num_children;
		new_ctx.alpha = ctx.alpha;
		
		new_ctx.clipping_mode = ctx.clipping_mode;
		new_ctx.x1 = ctx.x1;
		new_ctx.y1 = ctx.y1;
		new_ctx.x2 = ctx.x2;
		new_ctx.y2 = ctx.y2;
		
		@ctx = @new_ctx;
		
		return @ctx;
	}
	
	DrawingContext@ pop_drawing_context()
	{
		if(drawing_context_pool_index == drawing_context_pool_size)
			drawing_context_pool.resize(drawing_context_pool_size += 16);
		
		@drawing_context_pool[drawing_context_pool_index++] = @ctx;
		
		@ctx = @ctx.parent;
		return @ctx;
	}
	
	float gripper_required_space { get { return gripper_thickness + gripper_margin * 2; } }
	
	// -----------------------------------------------------------------
	// Basic drawing methods
	// -----------------------------------------------------------------
		
	void draw_rectangle(
		const float x1, const float y1, const float x2, const float y2,
		const float rotation, uint colour) const
	{
		if(ctx.alpha != 1)
			colour = set_alpha(colour);
		
		if(_hud)
			g.draw_rectangle_hud(_layer, _sub_layer, x1, y1, x2, y2, rotation, colour);
		else
			g.draw_rectangle_world(_layer, _sub_layer, x1, y1, x2, y2, rotation, colour);
	}
	
	void draw_glass(
		const float x1, const float y1, const float x2, const float y2,
		const float rotation, uint colour=0x00000000) const
	{
		if(ctx.alpha < 1)
			return;
		
		if(_hud)
			g.draw_glass_hud  (_layer, _sub_layer, x1, y1, x2, y2, rotation, colour);
		else
			g.draw_glass_world(_layer, _sub_layer, x1, y1, x2, y2, rotation, colour);
	}

	void draw_gradient(
		const float x1, const float y1, const float x2, const float y2,
		uint c00, uint c10, uint c11, uint c01) const
	{
		if(ctx.alpha != 1)
		{
			c00 = set_alpha(c00);
			c10 = set_alpha(c10);
			c11 = set_alpha(c11);
			c01 = set_alpha(c01);
		}
		
		if(_hud)
			g.draw_gradient_hud(_layer, _sub_layer, x1, y1, x2, y2, c00, c10, c11, c01);
		else
			g.draw_gradient_world(_layer, _sub_layer, x1, y1, x2, y2, c00, c10, c11, c01);
	}

	void draw_line(
		const float x1, const float y1, const float x2, const float y2,
		const float width, uint colour) const
	{
		if(ctx.alpha != 1)
			colour = set_alpha(colour);
		
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		const float length = sqrt(dx * dx + dy * dy);
		
		const float mx = (x1 + x2) * 0.5;
		const float my = (y1 + y2) * 0.5;
		
		if(_hud)
			g.draw_rectangle_hud(_layer, _sub_layer,
				mx - width, my - length * 0.5,
				mx + width, my + length * 0.5, atan2(-dx, dy) * RAD2DEG, colour);
		else
			g.draw_rectangle_world(_layer, _sub_layer,
				mx - width, my - length * 0.5,
				mx + width, my + length * 0.5, atan2(-dx, dy) * RAD2DEG, colour);
	}

	void draw_quad(
		const bool is_glass,
		const float x1, const float y1, const float x2, const float y2,
		const float x3, const float y3, const float x4, const float y4,
		uint c1, uint c2, uint c3, uint c4) const
	{
		if(ctx.alpha != 1)
		{
			c1 = set_alpha(c1);
			c2 = set_alpha(c2);
			c3 = set_alpha(c3);
			c4 = set_alpha(c4);
		}
		
		if(_hud)
			g.draw_quad_hud(_layer, _sub_layer, is_glass, x1, y1, x2, y2, x3, y3, x4, y4, c1, c2, c3, c4);
		else
			g.draw_quad_world(_layer, _sub_layer, is_glass, x1, y1, x2, y2, x3, y3, x4, y4, c1, c2, c3, c4);
	}
	
	void draw_sprite(
		sprites@ sprite,
		const string sprite_name, const uint frame, const uint palette,
		const float x, const float y, const float rotation,
		const float scale_x, const float scale_y,
		uint colour) const
	{
		if(ctx.alpha != 1)
			colour = set_alpha(colour);
		
		if(_hud)
			sprite.draw_hud  (_layer, _sub_layer, sprite_name, frame, palette, x, y, rotation, scale_x, scale_y, colour);
		else
			sprite.draw_world(_layer, _sub_layer, sprite_name, frame, palette, x, y, rotation, scale_x, scale_y, colour);
	}
	
	void draw_text(
		const string text,
		float x, float y,
		uint colour,
		float scale_x=NAN, float scale_y=NAN,
		const float rotation=0,
		const TextAlign align_h=TextAlign::Left, const TextAlign align_v=TextAlign::Top,
		string font='', uint size=0)
	{
		if(is_nan(scale_x))
			scale_x = default_text_scale;
		if(is_nan(scale_y))
			scale_y = default_text_scale;
		
		if(font == '')
			font = default_font;
		if(size == 0)
			size = default_text_size;
		
		if(current_font != font || current_text_size != size)
			text_field.set_font(current_font = font, current_text_size = size);
		
		if(current_align_h != align_h)
			text_field.align_horizontal(current_align_h = align_h);
		
		if(current_align_v != align_v)
			text_field.align_vertical(current_align_v = align_v);
		
		if(ctx.alpha != 1)
			colour = set_alpha(colour);
		
		if(current_text_colour != colour)
		{
			current_text_colour = colour;
			text_field.colour(colour);
		}
		
		text_field.text(text);
		
		x += text_offset_x * scale_x;
		y += text_offset_y * scale_y;
		
		if(_hud)
			text_field.draw_hud(_layer, _sub_layer, x, y, scale_x, scale_y, rotation);
		else
			text_field.draw_world(_layer, _sub_layer, x, y, scale_x, scale_y, rotation);
	}
	
	uint set_alpha(uint colour)
	{
		if(ctx.alpha == 0)
			return colour & 0x00ffffff;
		
		return (colour & 0x00ffffff) | (uint(ctx.alpha * ((colour >> 24) & 0xff)) << 24);
	}
	
	uint get_interactive_element_background_colour(const bool highlighted, const bool selected, const bool active, const bool disabled, bool draw_background=true)
	{
		return !draw_background ? 0 : disabled ? disabled_bg_clr
				: (highlighted && selected ? selected_highlight_bg_clr
					: selected ? selected_bg_clr : (highlighted ? highlight_bg_clr : normal_bg_clr));
	}
	
	uint get_interactive_element_border_colour(const bool highlighted, const bool selected, const bool active, const bool disabled, bool draw_border=true)
	{
		return !draw_border ? 0 : disabled ? disabled_border_clr
			: (highlighted && (selected || active) ? selected_highlight_border_clr
				: (selected || active) ? selected_border_clr : (highlighted ? highlight_border_clr : normal_border_clr));
	}
	
	// -----------------------------------------------------------------
	// Advanced drawing methods
	// -----------------------------------------------------------------
	
	void draw_interactive_element(
		const Element@ element,
		const bool highlighted, const bool selected, const bool active, const bool disabled,
		bool draw_background=true, bool draw_border=true)
	{
		const uint border_clr = get_interactive_element_border_colour(highlighted, selected, active, disabled, draw_border);
		
		const float border_size = selected ? selected_border_size : this.border_size;
		
		// Fill/bg
		
		if(draw_background)
		{
			const uint bg_clr = get_interactive_element_background_colour(highlighted, selected, active, disabled, draw_background);
			
			const float inset = border_clr != 0 ? max(0, border_size) : 0;
			
			draw_rectangle(
				element.x1 + inset, element.y1 + inset, element.x2 - inset, element.y2 - inset,
				0, bg_clr);
		}
		
		// Border
		if(border_clr != 0 && border_size > 0)
		{
			outline(element.x1, element.y1, element.x2, element.y2, border_size, border_clr);
		}
	}
	
	void draw_popup_element(const Element@ element, const float blur_inset=1)
	{
		draw_element(element, popup_shadow_clr, popup_bg_clr, popup_border_clr, blur_popup_bg, blur_inset);
	}
	
	void draw_dialog_element(const Element@ element, const float blur_inset=1)
	{
		draw_element(element, dialog_shadow_clr, dialog_bg_clr, dialog_border_clr, blur_dialog_bg, blur_inset);
	}
	
	void draw_element(const Element@ element, const uint shadow_clr, const uint bg_clr, const uint border_clr, const bool blur, const float blur_inset)
	{
		// Shadow
		if(shadow_clr != 0)
		{
			draw_rectangle(
				element.x1 + shadow_offset_x, element.y1 + shadow_offset_y,
				element.x2 + shadow_offset_x, element.y2 + shadow_offset_y,
				0, shadow_clr);
		}
		
		if(blur)
		{
			draw_glass(element.x1 + blur_inset, element.y1 + blur_inset, element.x2 - blur_inset, element.y2 - blur_inset, 0);
		}
		
		const float inset = border_clr != 0 ? max(0, border_size) : 0;
		
		// Fill/bg
		draw_rectangle(element.x1 + inset, element.y1 + inset, element.x2 - inset, element.y2 - inset, 0, bg_clr);
		
		// Border
		if(border_clr != 0)
		{
			outline(element.x1, element.y1, element.x2, element.y2, border_size, border_clr);
		}
	}
	
	void draw_gripper(const Orientation orientation, const float x, float start, float end)
	{
		// Calculate how many whole dots will fit between start and end
		const int num_dots = floor_int((end - start) / (gripper_thickness * 2));
		
		if(num_dots <= 1)
		{
			if(orientation == Orientation::Horizontal)
				draw_rectangle(x + gripper_margin, start, x + gripper_margin + gripper_thickness, end, 0, normal_border_clr);
			else
				draw_rectangle(start, x + gripper_margin, end, x + gripper_margin + gripper_thickness, 0, normal_border_clr);
		}
		else
		{
			// Centre the dots lengthwise
			const float dots_length = gripper_thickness * (num_dots * 2 - 1);
			float y = start + ((end - start) - dots_length) * 0.5;
			
			for(int i = 0; i < num_dots; i++)
			{
				if(orientation == Orientation::Horizontal)
					draw_rectangle(x + gripper_margin, y, x + gripper_margin + gripper_thickness, y + gripper_thickness, 0, normal_border_clr);
				else
					draw_rectangle(y, x + gripper_margin, y + gripper_thickness, x + gripper_margin + gripper_thickness, 0, normal_border_clr);
				
				y += gripper_thickness * 2;
			}
		}
	}
	
	void shadowed_text(
		const string text,
		const float x, const float y,
		uint colour, uint shadow_colour, float ox=3, float oy=3,
		const float scale_x=-1, const float scale_y=-1, const float rotation=0,
		const TextAlign align_h=TextAlign::Left, const TextAlign align_v=TextAlign::Top,
		string font='', uint size=0)
	{
		draw_text(text, x + ox, y + oy, shadow_colour, scale_x, scale_y, rotation, align_h, align_v, font, size);
		draw_text(text, x,      y,      colour,        scale_x, scale_y, rotation, align_h, align_v, font, size);
		
	}
	
	void outline_text(
		const string text,
		const float x, const float y,
		uint colour, uint outline_colour, float width=3,
		const float scale_x=-1, const float scale_y=-1, const float rotation=0,
		const TextAlign align_h=TextAlign::Left, const TextAlign align_v=TextAlign::Top,
		string font='', uint size=0)
	{
		draw_text(text, x + width, y        , outline_colour, scale_x, scale_y, rotation, align_h, align_v, font, size);
		draw_text(text, x - width, y        , outline_colour, scale_x, scale_y, rotation, align_h, align_v, font, size);
		draw_text(text, x        , y + width, outline_colour, scale_x, scale_y, rotation, align_h, align_v, font, size);
		draw_text(text, x        , y - width, outline_colour, scale_x, scale_y, rotation, align_h, align_v, font, size);
		
		draw_text(text, x, y, colour, scale_x, scale_y, rotation, align_h, align_v, font, size);
	}
	
	/**
	 * @brief Outlines the given rect. The outline is on the inside if thickness is positive, and on the outside if negative.
	 */
	void outline(
		const float x1, const float y1, const float x2, const float y2,
		const float thickness, const uint colour) const
	{
		//Left
		draw_rectangle(
			x1,
			y1 + thickness,
			x1 + thickness,
			y2 - thickness, 0, colour);
		// Right
		draw_rectangle(
			x2 - thickness,
			y1 + thickness,
			x2,
			y2 - thickness, 0, colour);
		// Top
		draw_rectangle(
			x1,
			y1,
			x2,
			y1 + thickness, 0, colour);
		// Bottom
		draw_rectangle(
			x1,
			y2 - thickness,
			x2,
			y2, 0, colour);
	}
	
	/**
	 * @brief Outlines the given rect. The outline is on the inside if thickness is positive, and on the outside if negative.
	 */
	void outline_dotted(
		const float x1, const float y1, const float x2, const float y2,
		const float thickness, const uint colour, const float dash_size=4, const float space_size=4) const
	{
		//Left
		//
		float x = y1 + thickness;
		float end_x = y2 - thickness;
		while(x < end_x)
		{
			draw_rectangle(
				x1, x,
				x1 + thickness, min(x + dash_size, end_x), 0, colour);
			x += dash_size + space_size;
		}
		
		// Right
		//
		x = y1 + thickness;
		end_x = y2 - thickness;
		while(x < end_x)
		{
			draw_rectangle(
				x2 - thickness, x,
				x2, min(x + dash_size, end_x), 0, colour);
			x += dash_size + space_size;
		}
		
		// Top
		//
		x = x1;
		end_x = x2;
		while(x < end_x)
		{
			draw_rectangle(
				x, y1,
				min(x + dash_size, end_x), y1 + thickness, 0, colour);
			x += dash_size + space_size;
		}
		// Bottom
		//
		x = x1;
		end_x = x2;
		while(x < end_x)
		{
			draw_rectangle(
				x, y2 - thickness,
				min(x + dash_size, end_x), y2, 0, colour);
			x += dash_size + space_size;
		}
	}

}