/**
 * @brief Extra drawing methods for th Graphics class
 */
namespace Graphics
{
	
	/**
	 * @brief Outlines the given rect. The outline is on the inside if thickness is positive, and on the outside if negative.
	 */
	void outline(
		Graphics@ graphics,
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in thickness, const uint &in colour)
	{
		//Left
		graphics.draw_rectangle(
			x1,
			y1 + thickness,
			x1 + thickness,
			y2 - thickness, 0, colour);
		// Right
		graphics.draw_rectangle(
			x2 - thickness,
			y1 + thickness,
			x2,
			y2 - thickness, 0, colour);
		// Top
		graphics.draw_rectangle(
			x1,
			y1,
			x2,
			y1 + thickness, 0, colour);
		// Bottom
		graphics.draw_rectangle(
			x1,
			y2 - thickness,
			x2,
			y2, 0, colour);
	}
	
}