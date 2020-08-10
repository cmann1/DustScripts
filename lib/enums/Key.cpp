namespace Key
{
	
	const int LeftButton = 0x01; // Left mouse button
	const int RightButton = 0x02; // Right mouse button
	const int Cancel = 0x03; // Control-break processing
	const int MiddleButton = 0x04; // Middle mouse button (three-button mouse)
	const int XButton1 = 0x05; // X1 mouse button
	const int XButton2 = 0x06; // X2 mouse button
	const int Back = 0x08; // BACKSPACE key
	const int Tab = 0x09; // TAB key
	const int Clear = 0x0C; // CLEAR key
	const int Return = 0x0D; // ENTER key
	const int Shift = 0x10; // SHIFT key
	const int Control = 0x11; // CTRL key
	const int Menu = 0x12; // ALT key
	const int Pause = 0x13; // PAUSE key
	const int Capital = 0x14; // CAPS LOCK key
	const int Kana = 0x15; // IME Kana mode
	const int Hanguel = 0x15; // IME Hanguel mode (maintained for compatibility; use HANGUL)
	const int Hangul = 0x15; // IME Hangul mode
	const int ImeOn = 0x16; // IME On
	const int Junja = 0x17; // IME Junja mode
	const int Final = 0x18; // IME final mode
	const int Hanja = 0x19; // IME Hanja mode
	const int Kanji = 0x19; // IME Kanji mode
	const int ImeOff = 0x1A; // IME Off
	const int Escape = 0x1B; // ESC key
	const int Convert = 0x1C; // IME convert
	const int NonConvert = 0x1D; // IME nonconvert
	const int Accept = 0x1E; // IME accept
	const int ModeChange = 0x1F; // IME mode change request
	const int Space = 0x20; // SPACEBAR
	const int Prior = 0x21; // PAGE UP key
	const int Next = 0x22; // PAGE DOWN key
	const int End = 0x23; // END key
	const int Home = 0x24; // HOME key
	const int Left = 0x25; // LEFT ARROW key
	const int Up = 0x26; // UP ARROW key
	const int Right = 0x27; // RIGHT ARROW key
	const int Down = 0x28; // DOWN ARROW key
	const int Select = 0x29; // SELECT key
	const int Print = 0x2A; // PRINT key
	const int Execute = 0x2B; // EXECUTE key
	const int Snapshot = 0x2C; // PRINT SCREEN key
	const int Insert = 0x2D; // INS key
	const int Delete = 0x2E; // DEL key
	const int Help = 0x2F; // HELP key
	const int Digit0 = 0x30;
	const int Digit1 = 0x31;
	const int Digit2 = 0x32;
	const int Digit3 = 0x33;
	const int Digit4 = 0x34;
	const int Digit5 = 0x35;
	const int Digit6 = 0x36;
	const int Digit7 = 0x37;
	const int Digit8 = 0x38;
	const int Digit9 = 0x39;
	const int A = 0x41;
	const int B = 0x42;
	const int C = 0x43;
	const int D = 0x44;
	const int E = 0x45;
	const int F = 0x46;
	const int G = 0x47;
	const int H = 0x48;
	const int I = 0x49;
	const int J = 0x4A;
	const int K = 0x4B;
	const int L = 0x4C;
	const int M = 0x4D;
	const int N = 0x4E;
	const int O = 0x4F;
	const int P = 0x50;
	const int Q = 0x51;
	const int R = 0x52;
	const int S = 0x53;
	const int T = 0x54;
	const int U = 0x55;
	const int V = 0x56;
	const int W = 0x57;
	const int X = 0x58;
	const int Y = 0x59;
	const int Z = 0x5A;
	const int LeftWin = 0x5B; // Left Windows key (Natural keyboard)
	const int RightWin = 0x5C; // Right Windows key (Natural keyboard)
	const int Apps = 0x5D; // Applications key (Natural keyboard)
	const int Sleep = 0x5F; // Computer Sleep key
	const int Numpad0 = 0x60; // Numeric keypad 0 key
	const int Numpad1 = 0x61; // Numeric keypad 1 key
	const int Numpad2 = 0x62; // Numeric keypad 2 key
	const int Numpad3 = 0x63; // Numeric keypad 3 key
	const int Numpad4 = 0x64; // Numeric keypad 4 key
	const int Numpad5 = 0x65; // Numeric keypad 5 key
	const int Numpad6 = 0x66; // Numeric keypad 6 key
	const int Numpad7 = 0x67; // Numeric keypad 7 key
	const int Numpad8 = 0x68; // Numeric keypad 8 key
	const int Numpad9 = 0x69; // Numeric keypad 9 key
	const int Multiply = 0x6A; // Multiply key
	const int Add = 0x6B; // Add key
	const int Separator = 0x6C; // Separator key
	const int Subtract = 0x6D; // Subtract key
	const int Decimal = 0x6E; // Decimal key
	const int Divide = 0x6F; // Divide key
	const int F1 = 0x70; // F1 key
	const int F2 = 0x71; // F2 key
	const int F3 = 0x72; // F3 key
	const int F4 = 0x73; // F4 key
	const int F5 = 0x74; // F5 key
	const int F6 = 0x75; // F6 key
	const int F7 = 0x76; // F7 key
	const int F8 = 0x77; // F8 key
	const int F9 = 0x78; // F9 key
	const int F10 = 0x79; // F10 key
	const int F11 = 0x7A; // F11 key
	const int F12 = 0x7B; // F12 key
	const int F13 = 0x7C; // F13 key
	const int F14 = 0x7D; // F14 key
	const int F15 = 0x7E; // F15 key
	const int F16 = 0x7F; // F16 key
	const int F17 = 0x80; // F17 key
	const int F18 = 0x81; // F18 key
	const int F19 = 0x82; // F19 key
	const int F20 = 0x83; // F20 key
	const int F21 = 0x84; // F21 key
	const int F22 = 0x85; // F22 key
	const int F23 = 0x86; // F23 key
	const int F24 = 0x87; // F24 key
	const int Numlock = 0x90; // NUM LOCK key
	const int Scroll = 0x91; // SCROLL LOCK key
	const int LeftShift = 0xA0; // Left SHIFT key
	const int RightShift = 0xA1; // Right SHIFT key
	const int LeftControl = 0xA2; // Left CONTROL key
	const int RightControl = 0xA3; // Right CONTROL key
	const int LeftMenu = 0xA4; // Left MENU key
	const int RightMenu = 0xA5; // Right MENU key
	const int BrowserBack = 0xA6; // Browser Back key
	const int BrowserForward = 0xA7; // Browser Forward key
	const int BrowserRefresh = 0xA8; // Browser Refresh key
	const int BrowserStop = 0xA9; // Browser Stop key
	const int BrowserSearch = 0xAA; // Browser Search key
	const int BrowserFavorites = 0xAB; // Browser Favorites key
	const int BrowserHome = 0xAC; // Browser Start and Home key
	const int VolumeMute = 0xAD; // Volume Mute key
	const int VolumeDown = 0xAE; // Volume Down key
	const int VolumeUp = 0xAF; // Volume Up key
	const int MediaNextTrack = 0xB0; // Next Track key
	const int MediaPrevTrack = 0xB1; // Previous Track key
	const int MediaStop = 0xB2; // Stop Media key
	const int MediaPlayPause = 0xB3; // Play/Pause Media key
	const int LaunchMail = 0xB4; // Start Mail key
	const int LaunchMediaSelect = 0xB5; // Select Media key
	const int LaunchApp1 = 0xB6; // Start Application 1 key
	const int LaunchApp2 = 0xB7; // Start Application 2 key
	const int Oem1 = 0xBA; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
	const int OemPlus = 0xBB; // For any country/region, the '+' key
	const int OemComma = 0xBC; // For any country/region, the ',' key
	const int OemMinus = 0xBD; // For any country/region, the '-' key
	const int OemPeriod = 0xBE; // For any country/region, the '.' key
	const int Oem2 = 0xBF; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
	const int Oem3 = 0xC0; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
	const int Oem4 = 0xDB; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
	const int Oem5 = 0xDC; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
	const int Oem6 = 0xDD; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
	const int Oem7 = 0xDE; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
	const int Oem8 = 0xDF; // Used for miscellaneous characters; it can vary by keyboard.
	const int Oem102 = 0xE2; // Either the angle bracket key or the backslash key on the RT 102-key keyboard
	const int Processkey = 0xE5; // IME PROCESS key
	const int Packet = 0xE7; // Used to pass Unicode characters as if they were keystrokes. The PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WMKEYDOWN, and WMKEYUP
	const int Attn = 0xF6; // A ttn key
	const int Crsel = 0xF7; // CrSel key
	const int Exsel = 0xF8; // ExSel key
	const int Ereof = 0xF9; // Erase EOF key
	const int Play = 0xFA; // Play key
	const int Zoom = 0xFB; // Zoom key
	const int Noname = 0xFC; // Reserved
	const int Pa1 = 0xFD; // PA1 key
	const int OemClear = 0xFE; // Clear key
	
}