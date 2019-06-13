void world_to_screen(camera@ cam, const float x, const float y, float &out out_x, float &out out_y)
{
	const float zoom = cam.screen_height() / 1080;
	out_x = (x - cam.x()) / (960 * zoom) * 800;
	out_y = (y - cam.y()) / (540 * zoom) * 450;
}