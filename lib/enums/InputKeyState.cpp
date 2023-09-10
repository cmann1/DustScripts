enum InputKeyState
{
	
	Up = 0,
	Down = 1,
	Pressed = 2,
	
}

namespace InputKeyState
{
	
	bool is_down(const int state) { return state & InputKeyState::Down != 0; }

	bool is_pressed(const int state) { return state & InputKeyState::Pressed != 0; }
	
}
