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
	
	string nice(string str)
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
	
}