#include '../Layout.cpp';
#include 'FlowAlign.cpp';
#include 'FlowDirection.cpp';
#include 'FlowWrap.cpp';

class FlowLayout : Layout
{
	
	UI@ ui;
	
	FlowDirection direction;
	FlowAlign justify;
	FlowAlign align;
	FlowWrap wrap;
	float padding = NAN;
	float spacing = NAN;
	
	FlowLayout(UI@ ui,
		const FlowDirection direction, FlowAlign justify=FlowAlign::Start, FlowAlign align=FlowAlign::Start,
		FlowWrap wrap=FlowWrap::Wrap)
	{
		@this.ui		= ui;
		
		this.direction		= direction;
		this.justify		= justify;
		this.align			= align;
		this.wrap			= wrap;
	}
	
	void do_layout(UI@ ui, const array<Element@>@ elements,
		const float x1, const float y1, const float x2, const float y2,
		float &out out_x1, float &out out_y1, float &out out_x2, float &out out_y2) override
	{
		const int num_children = elements.size();
		
		if(num_children == 0)
			return;
		
		Element@ element = elements[0];
		bool first_element_placed = false;
		
		const float padding = is_nan(this.padding) ? ui.style.spacing : this.padding;
		const float spacing = is_nan(this.spacing) ? ui.style.spacing : this.spacing;
		
		const bool is_horizontal	= this.is_horizontal;
		const bool is_reversed		= direction == FlowDirection::RowReverse || direction == FlowDirection::ColumnReverse;
		
		const bool wrap = this.wrap == FlowWrap::Wrap || this.wrap == FlowWrap::WrapReversed;
		const bool wrap_reversed = this.wrap == FlowWrap::WrapReversed;
		
		const float main_axis_size		= (is_horizontal ? (x2 - x1) : (y2 - y1)) - padding * 2;
		const float main_axis_start		= (is_horizontal ? x1 : y1) + padding;
		const float main_axis_end		= main_axis_start + main_axis_size;
		const float cross_axis_size		= (is_horizontal ? (y2 - y1) : (x2 - x1)) - padding * 2;
		const float cross_axis_start	= (is_horizontal ? y1 : x1) + padding;
		const float cross_axis_end		= cross_axis_start + cross_axis_size;
		const float main_axis_mid		= (main_axis_start + main_axis_end) * 0.5;
		
		float cross_x = cross_axis_start;
		
		for(int i = 0; i < num_children; i++)
		{
			int main_end_index = i;
			int main_start_index = i;
			float cross_size = 0;
			float main_elements_size = 0;
			float main_x = main_axis_start;
			
			// -----------------------------------------------------------
			// Find the cross axis width and elements for this row
			
			for(int j = i; j < num_children; j++)
			{
				@element = elements[j];
				
				const float el_main_size  = is_horizontal ? element.width  : element.height;
				const float el_cross_size = is_horizontal ? element.height : element.width;
				
				if(wrap && main_x + el_main_size > main_axis_end)
				{
					if(main_elements_size == 0)
						main_elements_size = el_main_size;
					
					break;
				}
				
				main_elements_size += el_main_size;
				
				if(el_cross_size > cross_size)
				{
					cross_size = el_cross_size;
				}
				
				main_x += el_main_size + spacing;
				main_end_index = j;
			}
			
			const int main_num_elements = main_end_index - main_start_index + 1;
			const float main_size = main_elements_size + (main_num_elements - 1) * spacing;
			
			i = main_end_index;
			
			// -----------------------------------------------------------
			// Position elements in this row
			
			main_x = main_axis_start;
			
			switch(justify)
			{
				case FlowAlign::Start:
				case FlowAlign::Space:
					main_x = main_axis_start;
					break;
				case FlowAlign::Centre:
					main_x = main_axis_mid - main_size * 0.5;
					break;
				case FlowAlign::End:
					main_x = main_axis_end - main_size;
					break;
			}
			
			const float main_spacing = justify == FlowAlign::Space
				? (main_num_elements > 1
					? (main_axis_size - main_elements_size) / (main_num_elements - 1)
					: 0)
				: spacing;
			
			for(int j = main_start_index; j <= main_end_index; j++)
			{
				@element = elements[j];
				
				const float el_main_size  = is_horizontal ? element.width  : element.height;
				const float el_cross_size = is_horizontal ? element.height : element.width;
				const float main_x_final = is_reversed ? (main_axis_end - main_x + main_axis_start - el_main_size) : main_x;
				float cross_x_final = cross_x;
				
				switch(align)
				{
					case FlowAlign::Centre:
						cross_x_final += (cross_size - el_cross_size) * 0.5;
						break;
					case FlowAlign::End:
						cross_x_final += cross_size - el_cross_size;
						break;
					case FlowAlign::Stretch:
						if(is_horizontal)
							element.height = cross_size;
						else
							element.width = cross_size;
						break;
				}
				
				if(wrap_reversed)
				{
					cross_x_final = cross_axis_end - cross_x_final + cross_axis_start - cross_size;
				}
				
				if(is_horizontal)
				{
					element.x = main_x_final;
					element.y = cross_x_final;
				}
				else
				{
					element.y = main_x_final;
					element.x = cross_x_final;
				}
				
				if(!first_element_placed)
				{
					out_x1 = element.x;
					out_y1 = element.y;
					out_x2 = element.x + element.width;
					out_y2 = element.y + element.height;
					
					first_element_placed = true;
				}
				else
				{
					if(element.x < out_x1) out_x1 = element.x;
					if(element.y < out_y1) out_y1 = element.y;
					if(element.x + element.width  > out_x2) out_x2 = element.x + element.width;
					if(element.y + element.height > out_y2) out_y2 = element.y + element.height;
				}
				
				main_x += el_main_size + main_spacing;
			}
			
			cross_x += cross_size + spacing;
		}
		
		out_x1 -= padding;
		out_y1 -= padding;
		out_x2 += padding;
		out_y2 += padding;
	}
	
	bool is_horizontal { get const { return direction == FlowDirection::Row || direction == FlowDirection::RowReverse; } }
	
}