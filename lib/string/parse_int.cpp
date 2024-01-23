namespace string
{
	
	int try_parse_int(string text, const bool force_hex=false)
	{
		uint base = string::parse_hex_prefix(text, text, force_hex) || force_hex ? 16 : 10;
		return parseInt(text, base);
	}
	
	uint try_parse_uint(string text, const bool force_hex=false)
	{
		uint base = string::parse_hex_prefix(text, text, force_hex) || force_hex ? 16 : 10;
		return parseUInt(text, base);
	}
	
	uint try_parse_rgb(string text, const bool auto_fill_alpha=false, const bool force_hex=false)
	{
		text = string::trim(text);
		string::parse_hex_prefix(text, text, force_hex);
		uint alpha = 0;
		
		if(text.length() == 3)
		{
			text =
				text.substr(0, 1) + text.substr(0, 1) +
				text.substr(1, 1) + text.substr(1, 1) +
				text.substr(2, 1) + text.substr(2, 1);
			
			if(auto_fill_alpha) alpha = 0xff000000;
		}
		else if(text.length() == 4)
		{
			text =
				text.substr(0, 1) + text.substr(0, 1) +
				text.substr(1, 1) + text.substr(1, 1) +
				text.substr(2, 1) + text.substr(2, 1) +
				text.substr(3, 1) + text.substr(3, 1);
		}
		else if(text.length() == 6)
		{
			if(auto_fill_alpha) alpha = 0xff000000;
		}
		
		return parseInt(text, 16) | alpha;
	}
	
	bool parse_hex_prefix(string text, string &out output, const bool force_hex=false)
	{
		text = string::trim(text);
		bool is_hex = false;
		
		// 35 = #
		if(text.length() > 1 && text[0] == 35)
		{
			output = text.substr(1);
			is_hex = true;
		}
		// 48 = 0, 88 = X, 120 = x
		else if(text.length() > 2 && text[0] == 48 && (text[1] == 88 || text[1] == 120))
		{
			output = text.substr(2);
			is_hex = true;
		}
		else
		{
			output = text;
		}
		
		return is_hex || force_hex;
	}
	
	string try_convert_int(const int value, const bool force_hex=false)
	{
		return force_hex
			? formatInt(value, 'H')
			: value + '';
	}
	
	string try_convert_uint(const uint value, const bool force_hex=false)
	{
		return force_hex
			? formatInt(value, 'H')
			: value + '';
	}
	
}
