class DebugItem
{
	
	int frames;
	uint layer;
	uint sub_layer;
	bool world;
	
	DebugItem@ prev;
	DebugItem@ next;
	
	void set(uint layer, uint sub_layer, int frames, bool world)
	{
		this.frames = frames;
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.world = world;
	}
	
	void draw(scene@ g) {}
	
}