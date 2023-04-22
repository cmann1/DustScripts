class AnchorLayout : Layout
{
	
	/* Allow shrinking beyond explcitly set element sizes. */
	bool allow_shrink = false;
	
	private int size_pass2 = 8;
	private int num_pass2;
	private array<Element@> pass2(size_pass2);
	
	AnchorLayout(UI@ ui)
	{
		super(ui);
	}
	
	void do_layout(const array<Element@>@ elements,
		const float x1, const float y1, const float x2, const float y2,
		float &out out_x1, float &out out_y1, float &out out_x2, float &out out_y2) override
	{
		const float padding_left	= is_nan(this.padding_left)		? style.spacing : this.padding_left;
		const float padding_right	= is_nan(this.padding_right)	? style.spacing : this.padding_right;
		const float padding_top		= is_nan(this.padding_top)		? style.spacing : this.padding_top;
		const float padding_bottom	= is_nan(this.padding_bottom)	? style.spacing : this.padding_bottom;
		const float padding_h		= padding_left + padding_right;
		const float padding_v		= padding_top + padding_bottom;
		
		const float spacing = max(0.0, style.spacing);
		
		int num_elements = 0;
		
		const float c_w = max((x2 - x1) - padding_h, 0.0);
		const float c_h = max((y2 - y1) - padding_v, 0.0);
		const float c_x1 = x1 + padding_left;
		const float c_y1 = y1 + padding_top;
		const float c_x2 = c_x1 + c_w;
		const float c_y2 = c_y1 + c_h;
		
		while(int(elements.length) > size_pass2)
		{
			pass2.resize(size_pass2 *= 2);
		}
		num_pass2 = 0;
		
		for(int j = 0; j < 2; j++)
		{
			const bool is_pass1 = j == 0;
			int i = is_pass1 ? elements.length : num_pass2;
			while(--i >= 0)
			{
				Element@ element = elements[i];
				if(!element._visible)
					continue;
				
				float el_x1 = element._x;
				float el_y1 = element._y;
				float el_x2 = el_x1 + (allow_shrink ? element._width : element._set_width);
				float el_y2 = el_y1 + (allow_shrink ? element._height : element._set_height);
				
				if(element.anchor_left.type != None)
				{
					float nx;
					if(!element.anchor_left.calculate(element, c_x1, c_w, is_pass1, nx))
					{
						@pass2[num_pass2++] = element;
						continue;
					}
					
					if(element.anchor_right.type == None)
					{
						el_x2 += nx - el_x1;
					}
					
					el_x1 = nx;
				}
				else if(is_pass1 && element.anchor_right.type == None && el_x1 < padding_left)
				{
					el_x1 = padding_left;
				}
				
				if(element.anchor_right.type != None)
				{
					float nx;
					if(!element.anchor_right.calculate(element, c_x2, c_w, is_pass1, nx))
					{
						@pass2[num_pass2++] = element;
						continue;
					}
					
					if(element.anchor_left.type == None)
					{
						el_x1 += nx - el_x2;
					}
					
					el_x2 = nx;
				}
				
				if(element.anchor_top.type != None)
				{
					float ny;
					if(!element.anchor_top.calculate(element, c_y1, c_h, is_pass1, ny))
					{
						@pass2[num_pass2++] = element;
						continue;
					}
					
					if(element.anchor_bottom.type == None)
					{
						el_y2 += ny - el_y1;
					}
					
					el_y1 = ny;
				}
				else if(is_pass1 && element.anchor_bottom.type == None && el_y1 < padding_top)
				{
					el_y1 = padding_top;
				}
				
				if(element.anchor_bottom.type != None)
				{
					float ny;
					if(!element.anchor_bottom.calculate(element, c_y2, c_h, is_pass1, ny))
					{
						@pass2[num_pass2++] = element;
						continue;
					}
					
					if(element.anchor_top.type == None)
					{
						el_y1 += ny - el_y2;
					}
					
					el_y2 = ny;
				}
				
				if(el_x2 < el_x1)
					el_x2 = el_x1;
				if(el_y2 < el_y1)
					el_y2 = el_y1;
				
				if(!allow_shrink)
				{
					if(el_x2 - el_x1 < element._set_width)
					{
						el_x2 = el_x1 + element._set_width;
					}
					if(el_y2 - el_y1 < element._set_height)
					{
						el_y2 = el_y1 + element._set_height;
					}
				}
				
				if(
					element._x != el_x1 || element._y != el_y1 ||
					element._x + element._width != el_x2 || element._y + element._height != el_y2)
				{
					element._x = el_x1;
					element._y = el_y1;
					element._width = el_x2 - el_x1;
					element._height = el_y2 - el_y1;
					element.validate_layout = true;
				}
				
				if(num_elements == 0)
				{
					out_x1 = element._x;
					out_y1 = element._y;
					out_x2 = out_x1 + element._width;
					out_y2 = out_y1 + element._height;
				}
				else
				{
					if(element._x < out_x1)
						out_x1 = element._x;
					if(element._y < out_y1)
						out_y1 = element._y;
					if(element._x + element._width > out_x2)
						out_x2 = element._x + element._width;
					if(element._y + element._height > out_y2)
						out_y2 = element._y + element._height;
				}
				
				num_elements++;
			}
			
			if(num_pass2 == 0)
				break;
			
			@elements = @pass2;
		}
		
		if(num_elements == 0)
		{
			out_x1 = x1;
			out_y1 = y1;
			out_x2 = x1 + padding_h;
			out_y2 = y1 + padding_v;
		}
		else
		{
			out_x2 += padding_right;
			out_y2 += padding_bottom;
		}
	}
	
}
