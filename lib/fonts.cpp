/**
 * A list of fonts and sizes, mostly for reference.
 */
namespace font
{
	
	const string ENVY_BOLD			= 'envy_bold';
	const string SANS_BOLD			= 'sans_bold';
	const string CARACTERES			= 'Caracteres';
	const string PROXIMANOVA_REG	= 'ProximaNovaReg';
	const string PROXIMANOVA_THIN	= 'ProximaNovaThin';
	
	const array<int> ENVY_BOLD_SIZES = { 20 };
	const array<int> SANS_BOLD_SIZES = { 20, 26, 36 };
	const array<int> CARACTERES_SIZES = { 26, 36, 40, 52, 64, 72, 92, 140 };
	const array<int> PROXIMANOVA_REG_SIZES = { 20, 26, 36, 42, 58, 72, 100 };
	const array<int> PROXIMANOVA_THIN_SIZES = { 20, 26, 36, 42 };
	
	const array<int>@ get_valid_sizes(const string font_name)
	{
		if(font_name == ENVY_BOLD)
			return @ENVY_BOLD_SIZES;
		
		if(font_name == SANS_BOLD)
			return @SANS_BOLD_SIZES;
		
		if(font_name == CARACTERES)
			return @CARACTERES_SIZES;
		
		if(font_name == PROXIMANOVA_REG)
			return @PROXIMANOVA_REG_SIZES;
		
		if(font_name == PROXIMANOVA_THIN)
			return @PROXIMANOVA_THIN_SIZES;
		
		return null;
	}
	
}