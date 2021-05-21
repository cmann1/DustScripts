#include '../string.cpp';

namespace ansi
{

	const string Esc			= '\u001b[';

	const string Reset			= Esc + '0m';
	
	const string Black			= Esc + '30m';
	const string Red			= Esc + '31m';
	const string Green			= Esc + '32m';
	const string Yellow			= Esc + '33m';
	const string Blue			= Esc + '34m';
	const string Magenta		= Esc + '35m';
	const string Cyan			= Esc + '36m';
	const string White			= Esc + '37m';
	const string Grey			= Esc + '90m';
	const string BrightRed		= Esc + '91m';
	const string BrightGreen	= Esc + '92m';
	const string BrightYellow	= Esc + '93m';
	const string BrightBlue		= Esc + '94m';
	const string BrightMagenta	= Esc + '95m';
	const string BrightCyan		= Esc + '96m';
	const string BrightWhite	= Esc + '97m';

	const string BgBlack			= Esc + '40m';
	const string BgRed				= Esc + '41m';
	const string BgGreen			= Esc + '42m';
	const string BgYellow			= Esc + '43m';
	const string BgBlue				= Esc + '44m';
	const string BgMagenta			= Esc + '45m';
	const string BgCyan				= Esc + '46m';
	const string BgWhite			= Esc + '47m';
	const string BgGrey				= Esc + '100m';
	const string BgBrightRed		= Esc + '101m';
	const string BgBrightGreen		= Esc + '102m';
	const string BgBrightYellow		= Esc + '103m';
	const string BgBrightBlue		= Esc + '104m';
	const string BgBrightMagenta	= Esc + '105m';
	const string BgBrightCyan		= Esc + '106m';
	const string BgBrightWhite		= Esc + '107m';

	const string Bold		= Esc + '1m';
	const string Underline	= Esc + '4m';
	const string Reversed	= Esc + '7m';

	const string Up		= 'A';
	const string Down	= 'B';
	const string Right	= 'C';
	const string Left	= 'D';

	const string black(const string &in txt) { return Black + txt + Reset; }
	const string red(const string &in txt) { return Red + txt + Reset; }
	const string green(const string &in txt) { return Green + txt + Reset; }
	const string yellow(const string &in txt) { return Yellow + txt + Reset; }
	const string blue(const string &in txt) { return Blue + txt + Reset; }
	const string magenta(const string &in txt) { return Magenta + txt + Reset; }
	const string cyan(const string &in txt) { return Cyan + txt + Reset; }
	const string white(const string &in txt) { return White + txt + Reset; }
	const string grey(const string &in txt) { return Grey + txt + Reset; }
	const string brightred(const string &in txt) { return BrightRed + txt + Reset; }
	const string brightgreen(const string &in txt) { return BrightGreen + txt + Reset; }
	const string brightyellow(const string &in txt) { return BrightYellow + txt + Reset; }
	const string brightblue(const string &in txt) { return BrightBlue + txt + Reset; }
	const string brightmagenta(const string &in txt) { return BrightMagenta + txt + Reset; }
	const string brightcyan(const string &in txt) { return BrightCyan + txt + Reset; }
	const string brightwhite(const string &in txt) { return BrightWhite + txt + Reset; }

	const string bgblack(const string &in txt) { return BgBlack + txt + Reset; }
	const string bgred(const string &in txt) { return BgRed + txt + Reset; }
	const string bggreen(const string &in txt) { return BgGreen + txt + Reset; }
	const string bgyellow(const string &in txt) { return BgYellow + txt + Reset; }
	const string bgblue(const string &in txt) { return BgBlue + txt + Reset; }
	const string bgmagenta(const string &in txt) { return BgMagenta + txt + Reset; }
	const string bgcyan(const string &in txt) { return BgCyan + txt + Reset; }
	const string bgwhite(const string &in txt) { return BgWhite + txt + Reset; }
	const string bggrey(const string &in txt) { return BgGrey + txt + Reset; }
	const string bgbrightred(const string &in txt) { return BgBrightRed + txt + Reset; }
	const string bgbrightgreen(const string &in txt) { return BgBrightGreen + txt + Reset; }
	const string bgbrightyellow(const string &in txt) { return BgBrightYellow + txt + Reset; }
	const string bgbrightblue(const string &in txt) { return BgBrightBlue + txt + Reset; }
	const string bgbrightmagenta(const string &in txt) { return BgBrightMagenta + txt + Reset; }
	const string bgbrightcyan(const string &in txt) { return BgBrightCyan + txt + Reset; }
	const string bgbrightwhite(const string &in txt) { return BgBrightWhite + txt + Reset; }

	const string bold(const string &in txt) { return Bold + txt + Reset; }
	const string underline(const string &in txt) { return Underline + txt + Reset; }
	const string reversed(const string &in txt) { return Reversed + txt + Reset; }

	const string up(const uint count=1) { return Esc + count + Up; }
	const string down(const uint count=1) { return Esc + count + Down; }
	const string left(const uint count=1) { return Esc + count + Left; }
	const string right(const uint count=1) { return Esc + count + Right; }

}
