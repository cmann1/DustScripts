#include '../../../lib/tiles/get_tile_quad.cpp'
#include '../../../lib/tiles/get_tile_edge_points.cpp'
#include 'Graphic.cpp'

class TileShapeGraphic : Graphic
{
	int shape;

	TileShapeGraphic(UI@ ui, int shape)
	{
		super(ui);

		this.shape = shape;

		_origin_x = 0;
		_origin_y = 0;
		_sizing = ImageSize::None;
		_width  = _set_width  = 48;
		_height = _set_height = 48;
		_graphic_width  = 48;
		_graphic_height = 48;
	}

	void _draw(Style@ style, DrawingContext@ ctx)
	{
		const float x = x1 + draw_x;
		const float y = y1 + draw_y;

		const uint fill = 0xB23B3072;
		const uint outline = 0xFF7F82B6;

		float x1, y1, x2, y2, x3, y3, x4, y4;
		get_tile_quad(shape, x1, y1, x2, y2, x3, y3, x4, y4, x, y);
		style.draw_quad(false, x1, y1, x2, y2, x3, y3, x4, y4, fill, fill, fill, fill);

		for(int edge=0; edge<4; ++edge)
		{
			if(get_edge_points(shape, edge, x1, y1, x2, y2, x, y))
			{
				style.draw_line(x1, y1, x2, y2, 1, outline);
			}
		}
	}
}
