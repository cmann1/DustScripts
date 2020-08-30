#include  '../elements/TextBox.cpp';

/// An element that can be temporarily edited with a TextBox
interface ITextEditable
{
	
	/// The element must add the TextBox to the hierarchy.
	/// All TextBox properties are reset so the element must also set the text, etc. each time.
	void text_editable_start(TextBox@ text_box);
	
	/// The element must remove the text box from the hierarchy.
	/// `event_type` will be either EventType::ACCEPT or EventType::CANCEL
	void text_editable_stop(TextBox@ text_box, const string event_type);
	
}