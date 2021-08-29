#include '../../math/math.cpp';
#include 'Layout.cpp';
#include 'flow/FlowAlign.cpp';
#include 'flow/FlowDirection.cpp';
#include 'flow/FlowFit.cpp';
#include 'flow/FlowWrap.cpp';

class GridLayout : Layout
{

	UI@ ui;
	
	/// How many columns. 0 means fit as many columns as possible - column_width must also be set in that case.
	int columns;
	/// If non-zero columns will all be this size. Otherwise each column will shrink/expand to fit the widest element
	float column_width;
	/// If non-zero rows will all be this size. Otherwise each row will shrink/expand to fit the tallest element
	float row_height;
	/// The direction
	FlowDirection direction;
	/// How to justify elements within a column if that element is smaller than the column
	FlowAlign justify;
	/// How to align elements within a row if that element is smaller than the row
	FlowAlign align;
	/// If the total grid is smaller than than the container size it will expand the columns and/or rows to fill all the available space
	FlowFit expand;
	/// The amount of space between columns. Leave as NAN to use the default style spacing.
	float column_spacing = NAN;
	/// The amount of space between rows. Leave as NAN to use the default style spacing.
	float row_spacing = NAN;
	
	GridLayout(UI@ ui,
		int columns, float column_width=0, float row_height=0,
		const FlowDirection direction=FlowDirection::Row, FlowAlign justify=FlowAlign::Start, FlowAlign align=FlowAlign::Start,
		FlowFit expand=FlowFit::None)
	{
		@this.ui		= ui;
		
		this.columns		= columns;
		this.column_width	= column_width;
		this.row_height		= row_height;
		this.direction		= direction;
		this.justify		= justify;
		this.align			= align;
		this.expand			= expand;
	}
	
	void do_layout(const array<Element@>@ elements,
		const float x1, const float y1, const float x2, const float y2,
		float &out out_x1, float &out out_y1, float &out out_x2, float &out out_y2) override
	{
		const bool is_horizontal	= this.is_horizontal;
		const int num_children = elements.size();
		
		const float padding_left	= is_nan(this.padding_left)		? ui.style.spacing : this.padding_left;
		const float padding_right	= is_nan(this.padding_right)	? ui.style.spacing : this.padding_right;
		const float padding_top		= is_nan(this.padding_top)		? ui.style.spacing : this.padding_top;
		const float padding_bottom	= is_nan(this.padding_bottom)	? ui.style.spacing : this.padding_bottom;
		const float padding_h		= padding_left + padding_right;
		const float padding_v		= padding_top + padding_bottom;
		
		int num_visible_children = 0;
		
		for(int i = 0; i < num_children; i++)
		{
			if(elements[i]._visible)
				num_visible_children++;
		}
		
		if(num_visible_children == 0)
		{
			out_x1 = x1;
			out_y1 = y1;
			out_x2 = x1 + padding_h;
			out_y2 = y1 + padding_v;
			return;
		}
		
		// //////////////////////////////////////////////////////
		// Step 1. Setup
		
		if(column_width < 0)
			column_width = 0;
		if(row_height < 0)
			row_height = 0;
		
		const bool dynamic_column_width = column_width == 0;
		const bool dynamic_row_height   = row_height == 0;
		
		if(columns <= 0 && dynamic_column_width)
			columns = 2;
		
		const bool is_reversed		= direction == FlowDirection::RowReverse || direction == FlowDirection::ColumnReverse;
		
		float main_axis_size	= is_horizontal ? (x2 - x1 - padding_h) : (y2 - y1 - padding_v);
		float main_axis_start	= is_horizontal ? (x1 + padding_left) : (y1 + padding_top);
		float main_axis_end		= main_axis_start + main_axis_size;
		float cross_axis_size	= is_horizontal ? (y2 - y1 - padding_v) : (x2 - x1 - padding_h);
		float cross_axis_start	= is_horizontal ? (y1 + padding_top) : (x1 + padding_left);
		float cross_axis_end	= cross_axis_start + cross_axis_size;
		
		const bool expand_columns = expand == FlowFit::MainAxis || expand == FlowFit::Both;
		const bool expand_rows = expand == FlowFit::CrossAxis || expand == FlowFit::Both;
		
		const float column_spacing	= max(0.0, is_nan(this.column_spacing) ? ui.style.spacing : this.column_spacing);
		const float row_spacing		= max(0.0, is_nan(this.row_spacing) ? ui.style.spacing : this.row_spacing);
		
		int num_columns = columns <= 0
			? floor_int(main_axis_size / column_width)
			: columns;
		
		// The number of columns above is calculated excluding spacing between columns
		// Recuce the number of columns until the columns and spacing fit
		// Don't know if there's a better way to do this
		if(columns <= 0)
		{
			while(num_columns * column_width + max(0.0, num_columns - 1) * column_spacing > max(0.0, main_axis_size))
				num_columns--;
			
			if(num_columns < 1)
				num_columns = 1;
		}
		
		int num_rows = ceil_int(float(num_visible_children) / num_columns);
		
		array<float>@ column_sizes = ui._float_array_x;
		int column_sizes_index = 0;
		array<float>@ row_sizes = ui._float_array_y;
		int row_sizes_index = 0;
		
		if(int(column_sizes.size()) < num_columns)
			column_sizes.resize(num_columns);
		if(int(row_sizes.size()) < num_rows)
			row_sizes.resize(num_rows);
		
		// //////////////////////////////////////////////////////
		// Step 2. Calculate the column and row sizes
		
		int column_start_index = -1;
		int row_index = 0;
		int column_index = 0;
		float column_size = 0;
		float row_size = 0;
		float total_column_size = 0;
		float total_row_size = 0;
		
		float final_column_width = column_width;
		float final_row_height = row_height;
		
		if(dynamic_column_width)
		{
			for(int i = 0; i < num_columns; i++)
			{
				column_sizes[i] = 0;
			}
		}
		
		if(dynamic_row_height)
		{
			for(int i = 0; i < num_rows; i++)
			{
				row_sizes[i] = 0;
			}
		}
		
		for(int i = 0; i < num_children; i++)
		{
			Element@ element = is_reversed
				? elements[column_start_index + num_columns - column_index - 1]
				: elements[i];
			
			if(!element._visible)
				continue;
			
			if(column_start_index == -1)
				column_start_index = i;
			
			const float el_column_size	= is_horizontal ? element._set_width  : element._set_height;
			const float el_row_size		= is_horizontal ? element._set_height : element._set_width;
			
			if(dynamic_column_width && column_sizes[column_index] < el_column_size)
				column_sizes[column_index] = el_column_size;
			
			if(dynamic_row_height && row_sizes[row_index] < el_row_size)
				row_sizes[row_index] = el_row_size;
			
			if(el_row_size > row_size)
				row_size = el_row_size;
			
			column_size += el_column_size;
			
			if(++column_index == num_columns)
			{
				column_start_index = i + 1;
				column_size = 0;
				column_index = 0;
				
				total_row_size += row_size;
				row_size = 0;
				row_index++;
			}
		}
		
		for(int i = 0; i < num_columns; i++)
		{
			total_column_size += column_sizes[i];
		}
		
		total_row_size += row_size;
		
		// //////////////////////////////////////////////////////
		// Step 3. Expand
		
		if(expand_columns)
		{
			const float fit_size = (main_axis_size - column_spacing * (num_columns - 1));
			
			if(columns <= 0)
			{
				final_column_width = fit_size / num_columns;
			}
			else if(dynamic_column_width)
			{
				if((total_column_size + column_spacing * (num_columns - 1)) < main_axis_size)
				{
					float new_total_size = 0;
					
					for(int i = 0; i < num_columns; i++)
					{
						new_total_size += (column_sizes[i] = (column_sizes[i] / total_column_size) * fit_size);
					}
					
					total_column_size = new_total_size;
				}
			}
			else if((column_width * num_columns + column_spacing * (num_columns - 1)) < main_axis_size)
			{
				final_column_width = fit_size / num_columns;
			}
		}
		
		if(expand_rows)
		{
			const float fit_size = (cross_axis_size - row_spacing * (num_rows - 1));
			
			if(dynamic_row_height)
			{
				if((total_row_size + column_spacing * (num_rows - 1)) < cross_axis_size)
				{
					for(int i = 0; i < num_rows ; i++)
					{
						row_sizes[i] = (row_sizes[i] / total_row_size) * fit_size;
					}
				}
			}
			else  if((row_height * num_rows + row_spacing * (num_rows - 1)) < cross_axis_size)
			{
				final_row_height = fit_size / num_rows;
			}
		}
		
		// //////////////////////////////////////////////////////
		// Step 4. Position
		
		row_index = 0;
		column_index = 0;
		column_size = dynamic_column_width ? column_sizes[0] : final_column_width;
		row_size = dynamic_row_height ? row_sizes[0] : final_row_height;
		
		if(!dynamic_column_width)
			total_column_size = column_size * num_columns;
		
		total_column_size += main_axis_start + column_spacing * (num_columns - 1);
		
		column_start_index = -1;
		float column_x = main_axis_start;
		float row_y = cross_axis_start;
		bool first_element_placed = false;
		
		for(int i = 0; i < num_children; i++)
		{
			Element@ element = is_reversed
				? elements[column_start_index + num_columns - column_index - 1]
				: elements[i];
			
			if(!element._visible)
				continue;
			
			if(column_start_index == -1)
				column_start_index = i;
			
			const float el_column_size	= is_horizontal ? element._set_width  : element._set_height;
			const float el_row_size		= is_horizontal ? element._set_height : element._set_width;
			
			float el_col_x;
			float el_row_y;
			
			if(dynamic_column_width)
				column_size = column_sizes[column_index];
			
			switch(justify)
			{
				case FlowAlign::Start:
					el_col_x = column_x;
					break;
				case FlowAlign::Centre:
					el_col_x = column_x + (column_size - el_column_size) * 0.5;
					break;
				case FlowAlign::End:
					el_col_x = column_x + (column_size - el_column_size);
					break;
				case FlowAlign::Stretch:
					el_col_x = column_x;
					
					if(is_horizontal)
						element._width  = column_size;
					else
						element._height = column_size;
					break;
			}
			
			switch(align)
			{
				case FlowAlign::Start:
					el_row_y = row_y;
					break;
				case FlowAlign::Centre:
					el_row_y = row_y + (row_size - el_row_size) * 0.5;
					break;
				case FlowAlign::End:
					el_row_y = row_y + (row_size - el_row_size);
					break;
				case FlowAlign::Stretch:
					el_row_y = row_y;
					
					if(is_horizontal)
						element._height = row_size;
					else
						element._width  = row_size;
					break;
			}
			
			if(is_horizontal)
			{
				element._x = el_col_x;
				element._y = el_row_y;
			}
			else
			{
				element._x = el_row_y;
				element._y = el_col_x;
			}
			
			element.validate_layout = true;
			
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
			
			column_x += column_size + column_spacing;
			
			if(++column_index == num_columns)
			{
				column_start_index = i + 1;
				column_x = main_axis_start;
				column_index = 0;
				
				row_y += row_size + row_spacing;
				row_index++;
				
				if(dynamic_row_height && row_index < num_rows)
					row_size = row_sizes[row_index];
			}
		}
		
		out_x1 -= padding_left;
		out_y1 -= padding_top;
		out_x2 += padding_right;
		out_y2 += padding_bottom;
	}
	
	bool is_horizontal { get const { return direction == FlowDirection::Row || direction == FlowDirection::RowReverse; } }
	
}
