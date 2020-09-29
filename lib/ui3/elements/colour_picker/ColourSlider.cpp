#include '../../../utils/colour.cpp';
#include 'ColourSliderType.cpp';

namespace ColourSlider { const string TYPE_NAME = 'ColourSlider'; }

class ColourSlider : Element, IStepHandler
{
	
	float value;
	float value_multiplier = 1;
	float hue;
	uint colour = 0xffffffff;
	ColourSliderType type = S;
	
	Event change;
	
	private bool dragging;
	
	ColourSlider(UI@ ui)
	{
		super(ui);
		
		width = 255;
		height = 34;
	}
	
	string element_type { get const override { return ColourSlider::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	// ///////////////////////////////////////////////////////////////////
	// Methods
	// ///////////////////////////////////////////////////////////////////
	
	bool ui_step()
	{
		if(!dragging)
			return false;
		
		if(!ui.mouse.moved)
			return true;
		
		update_value();
		
		return true;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		uint clr1, clr2;
		
		switch(type)
		{
			case H:
			{
				draw_hue_section(style, 0, 0xff0000, 0xffff00);
				draw_hue_section(style, 1, 0xffff00, 0x00ff00);
				draw_hue_section(style, 2, 0x00ff00, 0x00ffff);
				draw_hue_section(style, 3, 0x00ffff, 0x0000ff);
				draw_hue_section(style, 4, 0x0000ff, 0xff00ff);
				draw_hue_section(style, 5, 0xff00ff, 0xff0000);
				clr1 = 0x000000;
				clr2 = 0x000000;
			}
				break;
			case S:
			{
				clr1 = 0xffffffff;
				clr2 = hsv_to_rgb(hue, 1, 1) | 0xff000000;
				
				style.draw_quad(false,
					x1, y1, x2, y1, x2, y2, x1, y2,
					clr1, clr2, clr2, clr1);
			}
				break;
			case L:
			{
				clr1 = 0xff000000;
				clr2 = hsv_to_rgb(hue, 1, 1) | 0xff000000;
				
				style.draw_quad(false,
					x1, y1, x2, y1, x2, y2, x1, y2,
					clr1, clr2, clr2, clr1);
			}
				break;
			case R:
			case G:
			case B:
			{
				const uint mask = type == R ? 0xff0000 : type == G ? 0x00ff00 : 0x0000ff;
				clr1 = (colour & ~mask) | 0xff000000;
				clr2 = (colour |  mask) | 0xff000000;
				
				style.draw_quad(false,
					x1, y1, x2, y1, x2, y2, x1, y2,
					clr1, clr2, clr2, clr1);
			}
				break;
			case A:
			{
				const float repeat = 12;
				style.draw_rectangle(x1, y1, x2, y2, 0, 0xffffffff);
				int y = 0;
				
				for(int i = 0; i < repeat; i++)
				{
					style.draw_rectangle(
						x1 + _width * (i / repeat),
						y1 + (y2 - y1) * (y / 2.0),
						x1 + _width * ((i + 1) / repeat),
						y1 + (y2 - y1) * ((y + 1) / 2.0), 0, 0xffaaaaaa);
					
					y = (y + 1) % 2;
				}
				
				style.draw_quad(false,
					x1, y1, x2, y1, x2, y2, x1, y2,
					colour & 0x00ffffff,
					colour | 0xff000000, colour | 0xff000000,
					colour & 0x00ffffff);
				
				clr1 = 0xffffffff;
				clr2 = colour;
			}
				break;
			default:
				style.draw_rectangle(x1, y1, x2, y2, 0, colour);
				break;
		}
		
		const float x = x1 + 1 + (_width - 3) * value;
		
		int r1, g1, b1, a1;
		int r2, g2, b2, a2;
		int_to_rgba(clr1, r1, g1, b1, a1);
		int_to_rgba(clr2, r2, g2, b2, a2);
		
		const float offset = 0.7;
		r1 = round_int(lerp(r1, r2, value) * offset);
		g1 = round_int(lerp(g1, g2, value) * offset);
		b1 = round_int(lerp(b1, b2, value) * offset);
		
		style.draw_rectangle(
			x - 3, this.y1,
			x + 4, this.y2,
			0, 0x11000000);
		style.draw_rectangle(
			x - 2, this.y1,
			x + 3, this.y2,
			0, 0x33000000);
		style.draw_rectangle(
			x - 1, this.y1,
			x + 2, this.y2,
			0, 0xffffffff);
	}
	
	private void draw_hue_section(Style@ style, const float i, const uint clr1, const uint clr2)
	{
		const float t1 = (i) / 6;
		const float t2 = (i + 1) / 6;
		
		style.draw_quad(false,
			x1 + _width * t1, y1,
			x1 + _width * t2, y1,
			x1 + _width * t2, y2,
			x1 + _width * t1, y2,
			clr1 | 0xff000000, clr2 | 0xff000000,
			clr2 | 0xff000000, clr1 | 0xff000000);
	}
	
	private void update_value()
	{
		value = clamp01(mouse_x / _width);
		ui._dispatch_event(@change, EventType::CHANGE, this);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_press(EventInfo@ event) override
	{
		if(event.button != ui.primary_button)
			return;
		
		update_value();
		dragging = ui._step_subscribe(this, dragging);
	}
	
	void _mouse_release(EventInfo@ event) override
	{
		dragging = false;
	}
	
}