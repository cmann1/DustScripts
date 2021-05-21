namespace string
{
	
	uint hash(const string &in str)
	{
		uint hash = 7;
		
		for(int i = int(str.size()) - 1; i >= 0; i--)
		{
			hash = hash * 31 + str[i];
		}
		
		return hash;
	}
	
	string repeat(const string &in str, int times)
	{
		if(times < 1)
			return '';
		
		string output = '';
		
		for(int i = 0; i < times; i ++)
		{
			output += str;
		}
		
		return output;
	}
	
	string uppercase(string str)
	{
		for(int i = int(str.length()) - 1; i >= 0; i--)
		{
			const uint chr = str[i];
			
			if(chr >= 97 && chr <= 122)
			{
				str[i] = chr - 32;
			}
		}
		
		return str;
	}
	
	string lowercase(string str)
	{
		for(int i = int(str.length()) - 1; i >= 0; i--)
		{
			const uint chr = str[i];
			
			if(chr >= 65 && chr <= 92)
			{
				str[i] = chr + 32;
			}
		}
		
		return str;
	}
	
	string title_case(string str)
	{
		bool prev_space = true;
		
		for(int i = 0, count = int(str.length()); i < count; i++)
		{
			const uint chr = str[i];
			
			if(prev_space && chr >= 97 && chr <= 122)
			{
				str[i] = chr - 32;
			}
			
			prev_space = chr == 32;
		}
		
		return str;
	}
	
	string nice(const string str)
	{
		bool pending_space = false;
		bool prev_space = true;
		string output = '';
		int output_size = 0;
		output.resize(str.length());
		
		for(int i = 0, count = int(str.length()); i < count; i++)
		{
			const uint chr = str[i];
			
			if(chr == 32 || chr == 45 || chr == 95)
			{
				prev_space = true;
			}
			else
			{
				//if(chr >= 48 && chr <= 57 || chr >= 65 && chr <= 92 || chr >= 97 && chr <= 122)
				
				if(pending_space)
				{
					output[output_size++] = 32;
					pending_space = false;
				}
				
				if(prev_space && chr >= 97 && chr <= 122)
				{
					output[output_size++] = chr - 32;
				}
				else
				{
					output[output_size++] = chr;
				}
				
				prev_space = false;
			}
			
			pending_space = prev_space || pending_space;
		}
		
		output.resize(output_size);
		
		return output;
	}
	
	string nice_float(const float value, const uint precision=2, const bool trim_trailing_zeros=true)
	{
		string text = formatFloat(value, '', 0, precision);
		
		if(trim_trailing_zeros && precision > 0)
		{
			const int length = int(text.length());
			int end_index = length - 1;
			
			while(text[end_index] == 48) // "0"
				end_index--;
			
			while(text[end_index] == 46) // "."
				end_index--;
			
			if(end_index == -1)
			{
				text = '';
			}
			else if(end_index < length - 1)
			{
				text = text.substr(0, end_index + 1);
			}
		}
		
		return text;
	}
	
	string join(const array<int>@ arr, const string delimiter, int start=0, int count=-1)
	{
		const int arr_length = int(arr.length());
		
		if(start < 0)
			start = 0;
		else if(start >= arr_length)
			start = arr_length - 1;
		
		count = start + (count < 0 ? arr_length : count);
		
		if(count > arr_length)
			count = arr_length;
		
		if(count - start == 0)
			return '';
		
		string output = arr[start] + '';
		
		for(int i = start + 1; i < count; i++)
		{
			output += delimiter + arr[i];
		}
		
		return output;
	}
	
	string join(const array<uint>@ arr, const string delimiter, int start=0, int count=-1)
	{
		const int arr_length = int(arr.length());
		
		if(start < 0)
			start = 0;
		else if(start >= arr_length)
			start = arr_length - 1;
		
		count = start + (count < 0 ? arr_length : count);
		
		if(count > arr_length)
			count = arr_length;
		
		if(count - start == 0)
			return '';
		
		string output = arr[start] + '';
		
		for(int i = start + 1; i < count; i++)
		{
			output += delimiter + arr[i];
		}
		
		return output;
	}
	
	string join(const array<float>@ arr, const string delimiter, int start=0, int count=-1)
	{
		const int arr_length = int(arr.length());
		
		if(start < 0)
			start = 0;
		else if(start >= arr_length)
			start = arr_length - 1;
		
		count = start + (count < 0 ? arr_length : count);
		
		if(count > arr_length)
			count = arr_length;
		
		if(count - start == 0)
			return '';
		
		string output = arr[start] + '';
		
		for(int i = start + 1; i < count; i++)
		{
			output += delimiter + arr[i];
		}
		
		return output;
	}
	
	string join(const array<bool>@ arr, const string delimiter, int start=0, int count=-1)
	{
		const int arr_length = int(arr.length());
		
		if(start < 0)
			start = 0;
		else if(start >= arr_length)
			start = arr_length - 1;
		
		count = start + (count < 0 ? arr_length : count);
		
		if(count > arr_length)
			count = arr_length;
		
		if(count - start == 0)
			return '';
		
		string output = arr[start] + '';
		
		for(int i = start + 1; i < count; i++)
		{
			output += delimiter + arr[i];
		}
		
		return output;
	}
	
	string replace(const string str, const string search, const string replacement)
	{
		string output = '';
		int start_index = 0;
		int index;
		const int str_size = int(str.length());
		const int search_term_size = int(search.length());
		
		while(start_index < str_size && (index = str.findFirst(search, start_index)) >= 0)
		{
			output += str.substr(start_index, index - start_index) + replacement;
			
			start_index = index + search_term_size;
		}
		
		if(start_index < str_size)
		{
			output += str.substr(start_index);
		}
		
		return output;
	}
	
	bool is_whitespace(const int chr)
	{
		return chr >= 9 && chr <= 13 || chr == 32;
	}
	
	bool is_punctuation(const int chr)
	{
		return
			(chr >= 33  && chr <= 47) ||
			(chr >= 58  && chr <= 64) ||
			(chr >= 91  && chr <= 96) ||
			(chr >= 123 && chr <= 126);
	}
	
	bool is_digit(const int chr)
	{
		return chr >= 48  && chr <= 57;
	}
	
	bool is_alpha(const int chr)
	{
		return
			(chr >= 65  && chr <= 90) ||
			(chr >= 97  && chr <= 122);
	}
	
	bool is_lowercase(const int chr)
	{
		return chr >= 97  && chr <= 122;
	}
	
	bool is_uppercase(const int chr)
	{
		return chr >= 65  && chr <= 90;
	}
	
	bool is_alphanumeric(const int chr)
	{
		return
			(chr >= 48  && chr <= 57) ||
			(chr >= 65  && chr <= 90) ||
			(chr >= 97  && chr <= 122);
	}
	
	string chr(const int x)
	{
		string s = ' ';
		s[0] = x;
		return s;
	}
	
	string trim(const string str)
	{
		const int first = str.findFirstNotOf('\n\r \t');
		
		if(first == -1)
			return '';
		
		const int last = str.findLastNotOf('\n\r \t');
		
		return last != -1
			? str.substr(first, last - first)
			: str.substr(first);
	}
	
	// Parsing
	
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
