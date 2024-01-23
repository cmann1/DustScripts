#include 'common.cpp';

namespace string
{
	
	/** Parses a list of integers. Each entry can be separated by one or more spaces or commas.
	 * An entry can be a single number, or define a range in the form `START-END`.
	 * Entries can be prefixed with a `!` or `+` to indicate an exclusion or inclusion. In this case the appropriate flags bit will be set.
	 * Flag bits:
	 * 		- 1 : Exclusion.
	 * 		- 2 : Inclusion.
	 * @param str The string to parse.
	 * @param results
	 * 		Results are appended to this array.
	 * 		Each set of three values represents a single entry in the form: [FLAGS,START,END].
	 * @return False if an error occured during parsing.
	 */
	bool parse_range_list(const string &in str, array<int>@ results)
	{
		int end_index = str.length();
		int i = 0;
		
		while(i < end_index)
		{
			// 44 = ","
			while(i < end_index && (string::is_whitespace(str[i]) || str[i] == 44))
			{
				i++;
			}
			
			if(i >= end_index)
				break;
			
			int chr = str[i++];
			int flags = 0;
			
			// "!" or "+" - Exclusion or inclusion.
			if(chr == 33 || chr == 43)
			{
				// Expected end of input.
				if(i >= end_index)
					return false;
				
				flags |= (chr == 33 ? 1 : 2);
				chr = str[i++];
			}
			
			// Expected a start index, instead found "string::chr(chr)".
			// 45 = "-"
			if(chr != 45 && !string::is_digit(chr))
				return false;
			
			bool is_negative = false;
			if(chr == 45)
			{
				// Expected number after negative sign.
				if(i >= end_index)
					break;
				
				is_negative = true;
				chr = str[i++];
			}
			
			int range_start = 0;
			while(string::is_digit(chr))
			{
				range_start *= 10;
				range_start += chr - 48; // 48 = "0";
				
				if(i >= end_index)
				{
					chr = -1;
					break;
				}
				
				chr = str[i++];
			}
			
			if(is_negative)
			{
				range_start = -range_start;
			}
			
			if(chr != 45) // "-"
			{
				results.insertLast(flags);
				results.insertLast(range_start);
				results.insertLast(range_start);
				if(chr != -1)
				{
					i--;
				}
				continue;
			}
			
			// Expected an end index.
			if(i >= end_index)
				return false;
			
			chr = str[i++];
			
			// Expected an end index, instead found "string::chr(chr)".
			// 45 = "-"
			if(chr != 45 && !string::is_digit(chr))
				return false;
			
			is_negative = false;
			if(chr == 45)
			{
				// Expected number after negative sign.
				if(i >= end_index)
					break;
				
				is_negative = true;
				chr = str[i++];
			}
			
			int range_end = 0;
			while(string::is_digit(chr))
			{
				range_end *= 10;
				range_end += chr - 48; // 48 = "0";
				
				if(i >= end_index)
				{
					chr = -1;
					break;
				}
				
				chr = str[i++];
			}
			
			if(is_negative)
			{
				range_end = -range_end;
			}
			
			results.insertLast(flags);
			results.insertLast(range_start < range_end ? range_start : range_end);
			results.insertLast(range_end > range_start ? range_end : range_start);
			
			if(chr != -1)
			{
				i--;
			}
		}
		
		return true;
	}
	
}
