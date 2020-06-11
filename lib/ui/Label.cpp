#include 'UI.cpp';
#include 'Element.cpp';
#include 'Align.cpp';

class Label : Element
{
	
	string text;
	Align align_h;
	Align align_v;
	
	private float text_width;
	private float text_height;
	
	Label(UI@ ui, string text, Align align_h = Centre, Align align_v = Middle)
	{
		super(ui);
		
		update_text(text);
		this.align_h = align_h;
		this.align_v = align_v;
	}
	
	void update_text(string text)
	{
		this.text = text;
		ui.text_size(text, text_width, text_height);
		width = text_width + ui.padding * 2;
		height = text_height + ui.padding * 2;
	}
	
	void draw(scene@ g, Rect rect)
	{
		float x, y;
		
		if(align_h == Left)
			x = rect.x1;
		else if(align_h == Centre)
			x = rect.centre_x;
		else if(align_h == Right)
			x = rect.x2;
		
		if(align_v == Top)
			y = rect.y1;
		else if(align_v == Middle)
			y = rect.centre_y;
		else if(align_v == Bottom)
			y = rect.y2;
		
		ui.text(text, x, y, align_h, align_v);
	}
	
}