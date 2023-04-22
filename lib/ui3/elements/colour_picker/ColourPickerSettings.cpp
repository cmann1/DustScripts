class ColourPickerSettings
{
	
	uint colour;
	float h, s, l;
	int a;
	EventCallback@ on_change_callback;
	EventCallback@ on_accept_callback;
	bool force_hsl;
	bool show_hsl = true;
	bool show_rgb = true;
	bool show_alpha = true;
	bool show_hex = true;
	bool close_on_click_outside = true;
	
	void set_hsl(const float h, const float s, const float l)
	{
		set_hsl(h, s, l, a);
	}
	
	void set_hsl(const float h, const float s, const float l, const int a)
	{
		this.h = h;
		this.s = s;
		this.l = l;
		this.a = a;
	}
	
}
