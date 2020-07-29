#include '../../../math/math.cpp';
#include '../Layout.cpp';
#include 'FlowAlign.cpp';
#include 'FlowDirection.cpp';
#include 'FlowFit.cpp';
#include 'FlowWrap.cpp';

class FlowLayout : Layout
{
	
	UI@ ui;
	
	FlowDirection direction;
	FlowAlign justify;
	FlowAlign align;
	FlowWrap wrap;
	FlowFit fit;
	float padding = NAN;
	float spacing = NAN;
	
	FlowLayout(UI@ ui,
		const FlowDirection direction, FlowAlign justify=FlowAlign::Start, FlowAlign align=FlowAlign::Start,
		FlowWrap wrap=FlowWrap::Wrap, FlowFit fit=FlowFit::None)
	{
		@this.ui		= ui;
		
		this.direction		= direction;
		this.justify		= justify;
		this.align			= align;
		this.wrap			= wrap;
		this.fit			= fit;
	}
	
	void do_layout(const array<Element@>@ elements,
		const float x1, const float y1, const float x2, const float y2,
		float &out out_x1, float &out out_y1, float &out out_x2, float &out out_y2,float px=0,float py=0) override
	{
		const int num_children = elements.size();
		const float padding = is_nan(this.padding) ? ui.style.spacing : this.padding;
		
		if(num_children == 0)
		{
			out_x1 = x1;
			out_y1 = y1;
			out_x2 = x1 + padding * 2;
			out_y2 = y1 + padding * 2;
			return;
		}
		
		const float spacing = max(0, is_nan(this.spacing) ? ui.style.spacing : this.spacing);
		
		const bool is_horizontal	= this.is_horizontal;
		const bool is_reversed		= direction == FlowDirection::RowReverse || direction == FlowDirection::ColumnReverse;
		
		const bool wrap = this.wrap == FlowWrap::Wrap || this.wrap == FlowWrap::WrapReversed;
		const bool wrap_reversed = this.wrap == FlowWrap::WrapReversed;
		
		float main_axis_size	= (is_horizontal ? (x2 - x1) : (y2 - y1)) - padding * 2;
		float main_axis_start	= (is_horizontal ? x1 : y1) + padding;
		float main_axis_end		= main_axis_start + main_axis_size;
		float cross_axis_size	= (is_horizontal ? (y2 - y1) : (x2 - x1)) - padding * 2;
		float cross_axis_start	= (is_horizontal ? y1 : x1) + padding;
		float cross_axis_end	= cross_axis_start + cross_axis_size;
		
		// //////////////////////////////////////////////////////
		// Step 1. Find the extents of each row and column
		
		// Stores the main and cross axis size for each row/column in pairs of two values
		array<float>@ axis_sizes = ui._float_array_x;
		int axis_sizes_stacksize = axis_sizes.size();
		int axis_sizes_index = 0;
		// Stores the index for the end of each main axis
		array<int>@ axis_end_indices = ui._int_array;
		int axis_end_indices_stacksize = axis_end_indices.size();
		int axis_end_indices_index = 0;
		
		// Exlcuding padding
		float main_axis_total_size = 0;
		// Exlcuding padding
		float cross_axis_total_size = 0;
		int num_cross_axis = 0;
		
		float main_x  = main_axis_start;
		float cross_x = cross_axis_start;
		float current_main_axis_size = 0;
		float current_cross_axis_size = 0;
		int num_axis_elements = 0;
		int num_visible_elements = 0;
		
		for(int i = 0; i < num_children; i++)
		{
			Element@ element = elements[i];
			
			if(!element.visible)
				continue;
			
			num_visible_elements++;
			
			const float el_main_size  = is_horizontal ? element._width  : element._height;
			const float el_cross_size = is_horizontal ? element._height : element._width;
			
			if(wrap && main_x + el_main_size > main_axis_end)
			{
				// Add spacing
				current_main_axis_size += (num_axis_elements - 1) * spacing;
				
				// New row/column
				if(current_main_axis_size > main_axis_total_size)
					main_axis_total_size = current_main_axis_size;
				
				cross_axis_total_size += current_cross_axis_size;
				
				// Store this row/column size
				if(axis_sizes_index >= axis_sizes_stacksize - 1)
					axis_sizes.resize(axis_sizes_stacksize += 16);
				axis_sizes[axis_sizes_index++] = current_main_axis_size;
				axis_sizes[axis_sizes_index++] = current_cross_axis_size;
				
				// Store the end index of this row/column
				if(axis_end_indices_index == axis_end_indices_stacksize)
					axis_end_indices.resize(axis_end_indices_stacksize += 16);
				axis_end_indices[axis_end_indices_index++] = i;

				// Reset
				main_x = main_axis_start + el_main_size + spacing;
				cross_x += current_cross_axis_size + spacing;
				current_main_axis_size = el_main_size;
				current_cross_axis_size = el_cross_size;
				num_axis_elements = 1;
				num_cross_axis++;
				
				continue;
			}
			
			current_main_axis_size += el_main_size;
			main_x += el_main_size + spacing;
			
			if(el_cross_size > current_cross_axis_size)
				current_cross_axis_size = el_cross_size;
			
			num_axis_elements++;
		}
		
		
		if(num_visible_elements == 0)
		{
			out_x1 = x1;
			out_y1 = y1;
			out_x2 = x1 + padding * 2;
			out_y2 = y1 + padding * 2;
			return;
		}
		
		// Only add the final row if it wasn't already added
		// This could happen if the last element is the start of a new row/column and it is wider than the main axis
		if(axis_end_indices_index == 0 || axis_end_indices[axis_end_indices_index - 1] < num_children)
		{
			// Add spacing
			current_main_axis_size += (num_axis_elements - 1) * spacing;
			
			// New row/column
			if(current_main_axis_size > main_axis_total_size)
				main_axis_total_size = current_main_axis_size;
			
			cross_axis_total_size += current_cross_axis_size;
			
			// Store this row/column size
			if(axis_sizes_index >= axis_sizes_stacksize - 1)
				axis_sizes.resize(axis_sizes_stacksize += 16);
			axis_sizes[axis_sizes_index++] = current_main_axis_size;
			axis_sizes[axis_sizes_index++] = current_cross_axis_size;
			
			// Store the end index of this row/column
			if(axis_end_indices_index == axis_end_indices_stacksize)
				axis_end_indices.resize(axis_end_indices_stacksize += 16);
			axis_end_indices[axis_end_indices_index++] = num_children;
			
			num_cross_axis++;
		}
		
		// Add spacing
		cross_axis_total_size += (num_cross_axis - 1) * spacing;
		
		// //////////////////////////////////////////////////////
		// Step 2. Position the elements
		
		int i = 0;
		int prev_axis_index = 0;
		int next_axis_index = 0;
		current_main_axis_size = 0;
		current_cross_axis_size = 0;
		axis_sizes_index = 0;
		axis_end_indices_index = 0;
		cross_x = cross_axis_start;
		
		if(fit == FlowFit::MainAxis || fit == FlowFit::Both)
		{
			main_axis_end = main_axis_start + main_axis_total_size;
			main_axis_size = main_axis_total_size;
		}
		
		if(fit == FlowFit::CrossAxis || fit == FlowFit::Both)
		{
			cross_axis_end = cross_axis_start + cross_axis_total_size;
			cross_axis_size = cross_axis_total_size;
		}
		
		float main_spacing;
		const float main_axis_mid = main_axis_start + main_axis_size * 0.5;
		bool first_element_placed = false;
		
		do
		{
			Element@ element = elements[i];
			
			while(!element.visible && i < num_children)
			{
				@element = elements[++i];
			}
			
			// Start a new row/column
			if(i == next_axis_index)
			{
				main_x = main_axis_start;
				cross_x += current_cross_axis_size;
				
				if(i > 0)
					cross_x += spacing;
				
				prev_axis_index = next_axis_index;
				
				while(i == next_axis_index)
				{
					next_axis_index = axis_end_indices[axis_end_indices_index++];
					current_main_axis_size = axis_sizes[axis_sizes_index++];
					current_cross_axis_size = axis_sizes[axis_sizes_index++];
				}
				
				num_axis_elements = next_axis_index - prev_axis_index;
				
				main_spacing = justify == FlowAlign::Space
					? (num_axis_elements > 1
						? (main_axis_size - current_main_axis_size + (num_axis_elements - 1) * spacing) / (num_axis_elements - 1)
						: 0)
					: spacing;
				
				switch(justify)
				{
					case FlowAlign::Centre:
						main_x = main_axis_mid - current_main_axis_size * 0.5;
						break;
					case FlowAlign::End:
						main_x = main_axis_end - current_main_axis_size;
						break;
				}
			}
			
			const float el_main_size  = is_horizontal ? element._width  : element._height;
			const float el_cross_size = is_horizontal ? element._height : element._width;
			const float main_x_final = is_reversed ? (main_axis_end - main_x + main_axis_start - el_main_size) : main_x;
			float cross_x_final = cross_x;
			
			switch(align)
			{
				case FlowAlign::Centre:
					cross_x_final += ((current_cross_axis_size - el_cross_size) * 0.5) * (wrap_reversed ? -1 : 1);
					break;
				case FlowAlign::End:
					cross_x_final += (current_cross_axis_size - el_cross_size) * (wrap_reversed ? -1 : 1);
					break;
				case FlowAlign::Stretch:
					if(is_horizontal)
						element._height = current_cross_axis_size;
					else
						element._width = current_cross_axis_size;
					break;
			}
			
			if(wrap_reversed)
			{
				cross_x_final = cross_axis_end - cross_x_final + cross_axis_start - current_cross_axis_size;
			}
			
			if(is_horizontal)
			{
				element._x = main_x_final;
				element._y = cross_x_final;
			}
			else
			{
				element._y = main_x_final;
				element._x = cross_x_final;
			}
			
			if(!first_element_placed)
			{
				out_x1 = element._x;
				out_y1 = element._y;
				out_x2 = out_x1 + element._width;
				out_y2 = out_y1 + element._height;
				first_element_placed = true;
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
			
			main_x += el_main_size + main_spacing;
			
			i++;
		}
		while(i < num_children);
		
		out_x1 -= padding;
		out_y1 -= padding;
		out_x2 += padding;
		out_y2 += padding;
	}
	
	bool is_horizontal { get const { return direction == FlowDirection::Row || direction == FlowDirection::RowReverse; } }
	
}