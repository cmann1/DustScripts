class DebugTextLine
{
	
	string text;
	uint colour;
	string key;
	int frames;
	float height;
	
	DebugTextLine@ prev;
	DebugTextLine@ next;
	
	DebugTextLine(string text, uint colour, string key, int frames)
	{
		this.text = text;
		this.colour = colour;
		this.key = key;
		this.frames = frames;
	}
	
}