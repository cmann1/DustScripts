enum InputKeyState
{
	
	Up = 0,
	Down = 1,
	Press = 2,
	
}

namespace InputKeyState
{
	
	bool is_down(const int state) { return state & InputKeyState::Down != 0; }

	bool is_press(const int state) { return state & InputKeyState::Press != 0; }
	
}
