namespace string
{
	
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
			
			while(text[end_index] == 46 || text[end_index] == 44) // "." or ","
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
	
	string reversed(string str)
	{
		const int count = int(str.length()) - 1;
		for(int i = count; i >= 0; i--)
		{
			const uint chr = str[i];
			str[i] = str[count - i];
			str[count - i] = chr;
		}
		
		return str;
	}
	
	string pad_left(string str, const uint size, const string chr=' ')
	{
		while(str.length < size)
		{
			str = chr + str;
		}
		return str;
	}
	
	string pad_right(string str, const uint size, const string chr=' ')
	{
		while(str.length < size)
		{
			str += chr;
		}
		return str;
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
	
	int ord(const string &in s)
	{
		return s[0];
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
	
	uint hash(const string &in str)
	{
		uint hash = 7;
		
		for(int i = int(str.size()) - 1; i >= 0; i--)
		{
			hash = hash * 31 + str[i];
		}
		
		return hash;
	}
	
	string normalise_eol(const string &in text)
	{
		const int size = text.length;
		
		string buffer = '';
		buffer.resize(size);
		int buffer_size = size;
		int j = 0;
		int prev_chr = -1;
		
		for(int i = 0; i < size; i++)
		{
			const int chr = text[i];
			
			if(chr == 13)
			{
				buffer[j++] = 10;
			}
			else if(chr != 10 || chr == 10 && prev_chr != 13)
			{
				buffer[j++] = chr;
			}
			
			prev_chr = chr;
		}
		
		buffer.resize(j);
		return buffer;
	}
	
	string filter_input(string text)
	{
		const int size = text.length;
		int j = 0;
		
		for(int i = 0; i < size; i++)
		{
			const int chr = text[i];
			if(chr == 13)
			{
				text[j++] = 10;
			}
			else if(chr >= 32 && chr <= 126)
			{
				text[j++] = chr;
			}
		}
		
		text.resize(j);
		return text;
	}
	
}
