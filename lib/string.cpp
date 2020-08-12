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
	
	string repeat(const string str, int times)
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
	
}