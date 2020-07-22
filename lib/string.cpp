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
	
}