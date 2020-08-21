enum CharacterValidation
{
	
	/// Any character is valid
	None,
	/// + - 0-9
	Integer,
	/// + - 0-9 .
	Decimal,
	/// 0-9 a-f, and x 0 # for theoptional prefix
	Hex,
	/// Specify custom characters with the allowed_characters field
	Custom,
	
}