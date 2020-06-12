class TooltipData
{
	
	string text;
	Rect rect;
	
	TooltipData()
	{
		
	}
	
	TooltipData(string text, const Rect &in rect)
	{
		this.text = text;
		this.rect = rect;
	}
	
}