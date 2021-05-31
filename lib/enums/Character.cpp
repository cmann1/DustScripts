namespace Character
{

	const int None = 0;

	const int DustMan = 1;
	const int DustGirl = 2;
	const int DustKid = 3;
	const int DustWorth = 4;

	const int DustWraith = 5;
	const int LeafSprite = 6;
	const int TrashKing = 7;
	const int SlimeBoss = 8;

	int get_character(string &in character, bool &out is_virtual)
	{
		if(character[0] == 118) // v
		{
			character = character.substr(1);
			is_virtual = true;
		}
		else
		{
			is_virtual = false;
		}

		if(character == 'dustman')
			return DustMan;
		if(character == 'dustgirl')
			return DustGirl;
		if(character == 'dustkid')
			return DustKid;
		if(character == 'dustworth')
			return DustWorth;

		if(character == 'dustwraith')
			return DustWraith;
		if(character == 'leafsprite')
			return LeafSprite;
		if(character == 'trashking')
			return TrashKing;
		if(character == 'slimeboss')
			return SlimeBoss;

		return None;
	}

}
