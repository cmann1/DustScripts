#include '../std.cpp';
#include '../drawing/common.cpp';
#include '../drawing/circle.cpp';
#include '../drawing/Sprite.cpp';
#include '../input/Mouse.cpp';
#include '../fonts.cpp';
#include '../math/math.cpp';
#include '../layer.cpp';
#include '../enums/GVB.cpp';
#include 'IEditable.cpp';
#include 'EditorMouseResult.cpp';
#include 'draw/EditorArc.cpp';
#include 'draw/EditorArrow.cpp';
#include 'draw/EditorBox.cpp';
#include 'draw/EditorCircleHandle.cpp';
#include 'draw/EditorHandle.cpp';
#include 'draw/EditorLine.cpp';
#include 'draw/EditorText.cpp';

namespace BaseEditorScript { const float ED_HANDLE_SIZE = 5; }

class BaseEditorScript
{
	
	scene@ g;
	camera@ cam;
	editor_api@ editor;
	input_api@ input;
	textfield@ text_field;
	
	Mouse mouse(false);
	float ed_zoom = 1;
	float ed_handle_size;
	float ed_default_thickness = 1;
	bool ed_disable_handles;
	bool ed_disable_snap;
	int ed_index = 0;
	int ed_secondary_index = -1;
	int ed_box_handle_index = -1;
	
	float ed_view_x;
	float ed_view_y;
	float ed_view_x1, ed_view_y1;
	float ed_view_x2, ed_view_y2;
	Line ed_line1;
	Line ed_line2;
	
	bool ed_space;
	bool ed_ctrl;
	bool ed_shift;
	bool ed_alt;
	bool ed_mouse_in_gui;
	bool ed_press_control;
	bool ed_press_shift;
	bool ed_press_alt;
	bool ed_press;
	bool ed_left_press;
	bool ed_right_press;
	bool ed_middle_press;
	float ed_float_val;
	
	protected int ed_handles_size = 32;
	protected array<EditorHandle> ed_handles(ed_handles_size);
	protected int ed_handles_count;
	
	protected int ed_circle_handles_size = 32;
	protected array<EditorCircleHandle> ed_circle_handles(ed_circle_handles_size);
	protected int ed_circle_handles_count;
	
	protected int ed_lines_size = 32;
	protected array<EditorLine> ed_lines(ed_lines_size);
	protected int ed_lines_count;
	
	protected int ed_boxes_size = 32;
	protected array<EditorBox> ed_boxes(ed_boxes_size);
	protected int ed_boxes_count;
	
	protected int ed_arrows_size = 32;
	protected array<EditorArrow> ed_arrows(ed_arrows_size);
	protected int ed_arrows_count;
	
	protected int ed_arcs_size = 32;
	protected array<EditorArc> ed_arcs(ed_arcs_size);
	protected int ed_arcs_count;
	
	protected int ed_texts_size = 32;
	protected array<EditorText> ed_texts(ed_texts_size);
	protected int ed_texts_count;
	
	protected EditorHandle@ ed_last_handle;
	
	protected float ed_rel_x, ed_rel_y;
	protected int ed_layer;
	protected IEditable@ ed_drag_handle_ref;
	protected int ed_drag_handle_index = -1;
	protected int ed_drag_handle_secondary_index = -1;
	protected float ed_drag_ox;
	protected float ed_drag_oy;
	protected int ed_drag_box_handle_index;
	protected float ed_drag_box_ox, ed_drag_box_oy;
	protected float ed_drag_box_x1, ed_drag_box_y1;
	protected float ed_drag_box_x2, ed_drag_box_y2;
	protected float ed_drag_radius_offset;
	protected bool ed_drag_radians;
	
	BaseEditorScript()
	{
		@g = get_scene();
		@editor = get_editor_api();
		@input = get_input_api();
		
		@cam = get_active_camera();
		@text_field = create_textfield();
		text_field.set_font(font::ENVY_BOLD, 20);
	}
	
	void editor_step()
	{
		ed_view_x = cam.x();
		ed_view_y = cam.y();
		ed_zoom = 1 / cam.editor_zoom();
		ed_handle_size = BaseEditorScript::ED_HANDLE_SIZE * ed_zoom;
		
		float view1_x, view1_y, view1_w, view1_h;
		float view2_x, view2_y, view2_w, view2_h;
		cam.get_layer_draw_rect(0, 19, view1_x, view1_y, view1_w, view1_h);
		cam.get_layer_draw_rect(1, 19, view2_x, view2_y, view2_w, view2_h);
		ed_view_x1 = min(view1_x, view2_x);
		ed_view_y1 = min(view1_y, view2_y);
		ed_view_x2 = max(view1_x + view1_w, view2_x + view2_w);
		ed_view_y2 = max(view1_y + view1_h, view2_y + view2_h);
		
		//const bool can_press = @editor == null || !editor.mouse_in_gui() && !input.key_check_gvb(GVB::Space) && editor.editor_tab() == 'Triggers';
		ed_space = input.key_check_gvb(GVB::Space);
		ed_ctrl = input.key_check_gvb(GVB::Control);
		ed_shift = input.key_check_gvb(GVB::Shift);
		ed_alt = input.key_check_gvb(GVB::Alt);
		ed_mouse_in_gui = editor.mouse_in_gui();
		const bool can_press = !ed_mouse_in_gui && !ed_space && (editor.editor_tab() == 'Triggers' || editor.editor_tab() == 'Scripts');
		
		ed_rel_x = 0;
		ed_rel_y = 0;
		ed_handles_count = 0;
		ed_lines_count = 0;
		ed_boxes_count = 0;
		ed_circle_handles_count = 0;
		ed_arrows_count = 0;
		ed_arcs_count = 0;
		ed_texts_count = 0;
		
		mouse.step();
		
		ed_left_press = can_press && mouse.left_press;
		ed_right_press = can_press && mouse.right_press;
		ed_middle_press = can_press && mouse.middle_press;
		ed_press = ed_left_press || ed_right_press || ed_middle_press;
		
		if(!mouse.left_down)
		{
			@ed_drag_handle_ref = null;
			ed_drag_handle_index = -1;
			ed_drag_handle_secondary_index = -1;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(ed_disable_handles)
			return;
		
		for(int i = 0; i < ed_handles_count; i++)
		{
			ed_handles[i].draw(this);
		}
		
		for(int i = 0; i < ed_lines_count; i++)
		{
			ed_lines[i].draw(this);
		}
		
		for(int i = 0; i < ed_boxes_count; i++)
		{
			ed_boxes[i].draw(this);
		}
		
		for(int i = 0; i < ed_circle_handles_count; i++)
		{
			ed_circle_handles[i].draw(this);
		}
		
		for(int i = 0; i < ed_arrows_count; i++)
		{
			ed_arrows[i].draw(this);
		}
		
		for(int i = 0; i < ed_arcs_count; i++)
		{
			ed_arcs[i].draw(this);
		}
		
		for(int i = 0; i < ed_texts_count; i++)
		{
			ed_texts[i].draw(this);
		}
	}
	
	//
	
	bool ed_inside(const float x, const float y, controllable@ e)
	{
		return @e != null ? ed_inside(x, y, e.as_entity()) : false;
	}
	
	bool ed_inside(const float x, const float y, scripttrigger@ e)
	{
		return @e != null ? ed_inside(x, y, e.as_entity()) : false;
	}
	
	bool ed_inside(const float x, const float y, scriptenemy@ e)
	{
		return @e != null ? ed_inside(x, y, e.as_entity()) : false;
	}
	
	bool ed_inside(const float x, const float y, entity@ e)
	{
		if(@e == null)
			return false;
		
		rectangle@ r = e.base_rectangle();
		const float x1 = e.x() + r.left();
		const float y1 = e.y() + r.top();
		const float x2 = e.x() + r.right();
		const float y2 = e.y() + r.bottom();
		
		return x >= x1 && x <= x2 && y >= y1 && y <= y2;
	}
	
	//
	
	BaseEditorScript@ ed_rel(const float x, const float y)
	{
		ed_rel_x = x;
		ed_rel_y = y;
		
		return this;
	}
	
	BaseEditorScript@ ed_abs()
	{
		ed_rel_x = ed_rel_y = 0;
		
		return this;
	}
	
	void ed_set_drag_handle_index(const int index)
	{
		ed_drag_handle_index = index;
	}
	
	protected void ed_do_snap(float x, float &out ox)
	{
		const float snap = ed_disable_snap ? 0 : ed_ctrl ? 24 : ed_shift ? 48 : 0;
		ox = snap != 0 ? round(x / snap) * snap : x;
	}
	
	protected void ed_do_snap(float x, float y, float &out ox, float &out oy)
	{
		const float snap = ed_disable_snap ? 0 : ed_ctrl ? 24 : ed_shift ? 48 : 0;
		ox = snap != 0 ? round(x / snap) * snap : x;
		oy = snap != 0 ? round(y / snap) * snap : y;
	}
	
	//
	
	EditorMouseResult ed_handle(float x, float y, IEditable@ ref, const int index, const int layer=19,
		const uint colour=0xffaaaa44, const float rotation=45, const float size=-1)
	{
		if(ed_disable_handles)
			return None;
		
		x += ed_rel_x;
		y += ed_rel_y;
		
		if(
			x + ed_handle_size < ed_view_x1 || x - ed_handle_size > ed_view_x2 ||
			y + ed_handle_size < ed_view_y1 || y - ed_handle_size > ed_view_y2)
		{
			return None;
		}
		
		if(ed_handles_count + 2 > ed_handles_size)
		{
			ed_handles.resize(ed_handles_size += 32);
		}
		
		EditorHandle@ handle = @ed_handles[ed_handles_count++];
		handle.x = x;
		handle.y = y;
		handle.layer = layer;
		handle.colour = colour;
		handle.rotation = rotation;
		handle.size = size;
		handle.circle = false;
		@ed_last_handle = @handle;
		
		if(@ed_drag_handle_ref == @ref && ed_drag_handle_index == index && ed_drag_handle_secondary_index == ed_secondary_index)
		{
			ed_layer = layer;
			ed_secondary_index = -1;
			return Move;
		}
		
		if(ed_press)
		{
			const float mouse_x = layer == mouse.layer ? mouse.x : g.mouse_x_world(0, layer);
			const float mouse_y = layer == mouse.layer ? mouse.y : g.mouse_y_world(0, layer);
			
			ed_press_control = input.key_check_gvb(GVB::Control);
			ed_press_shift = input.key_check_gvb(GVB::Shift);
			ed_press_alt = input.key_check_gvb(GVB::Alt);
			
			if(dist_sqr(x, y, mouse_x, mouse_y) <= (ed_handle_size * ed_handle_size))
			{
				if(ed_left_press)
				{
					@ed_drag_handle_ref = @ref;
					ed_drag_handle_index = index;
					ed_drag_handle_secondary_index = ed_secondary_index;
					ed_drag_ox = mouse_x - x;
					ed_drag_oy = mouse_y - y;
					ed_press = false;
					
					ed_secondary_index = -1;
					
					input.key_clear_gvb(GVB::LeftClick);
					return LeftPress;
				}
				else if(ed_right_press)
				{
					input.key_clear_gvb(GVB::RightClick);
					return RightPress;
				}
				else if(ed_middle_press)
				{
					input.key_clear_gvb(GVB::MiddleClick);
					return MiddlePress;
				}
			}
		}
		
		ed_secondary_index = -1;
		return None;
	}
	
	void ed_update_handle(float &out x, float &out y)
	{
		transform_layer_position(g, ed_view_x, ed_view_y, mouse.x, mouse.y, 22, ed_layer, x, y);
		
		x -= ed_drag_ox;
		y -= ed_drag_oy;
		
		ed_do_snap(x, y, x, y);
		
		x -= ed_rel_x;
		y -= ed_rel_y;
	}
	
	//
	
	EditorMouseResult ed_box(float x1, float y1, float x2, float y2,
		IEditable@ ref, const int index=0,
		const bool draw_snap_tiles=false,
		float thickness=-1, const int layer=19, const uint colour=0xff44aaaa)
	{
		if(ed_disable_handles)
			return None;
		
		x1 += ed_rel_x;
		y1 += ed_rel_y;
		x2 += ed_rel_x;
		y2 += ed_rel_y;
		
		float rx1 = x1;
		float ry1 = y1;
		float rx2 = x2;
		float ry2 = y2;
		
		if(draw_snap_tiles)
		{
			rx1 = floor(x1 / 48) * 48;
			ry1 = floor(y1 / 48) * 48;
			rx2 = floor(x2 / 48 + 1) * 48;
			ry2 = floor(y2 / 48 + 1) * 48;
		}
		
		if(thickness <= 0)
		{
			thickness = ed_default_thickness;
		}
		
		if(
			x2 + thickness * ed_zoom < ed_view_x1 || x1 - thickness * ed_zoom > ed_view_x2 ||
			y2 + thickness * ed_zoom < ed_view_y1 || y1 - thickness * ed_zoom > ed_view_y2)
		{
			return None;
		}
		
		if(ed_boxes_count + 3 > ed_boxes_size)
		{
			ed_boxes.resize(ed_boxes_size += 32);
		}
		
		EditorBox@ box = @ed_boxes[ed_boxes_count++];
		box.x1 = x1;
		box.y1 = y1;
		box.x2 = x2;
		box.y2 = y2;
		box.rotation = 0;
		box.thickness = thickness;
		box.layer = layer;
		box.colour = colour;
		box.draw_snap_tiles = draw_snap_tiles;
		
		EditorMouseResult result = None;
		ed_drag_box_x1 = x1;
		ed_drag_box_y1 = y1;
		ed_drag_box_x2 = x2;
		ed_drag_box_y2 = y2;
		
		const int secondary = ed_secondary_index;
		
		const float rel_x = ed_rel_x;
		const float rel_y = ed_rel_y;
		ed_rel_x = 0;
		ed_rel_y = 0;
		
		for(int i = 0; i < 9; i++)
		{
			ed_secondary_index = secondary;
			
			EditorMouseResult handle_result = ed_box_handle(rx1, ry1, rx2, ry2, ref, index, i, layer, colour);
			
			if(handle_result == LeftPress)
			{
				if(ed_drag_box_handle_index == 0 || ed_drag_box_handle_index == 6 || ed_drag_box_handle_index == 7)
					ed_drag_box_ox = x1 - rx1;
				else if(ed_drag_box_handle_index == 2 || ed_drag_box_handle_index == 3 || ed_drag_box_handle_index == 4)
					ed_drag_box_ox = x2 - rx2;
				else
					ed_drag_box_ox = 0;
				
				if(ed_drag_box_handle_index == 0 || ed_drag_box_handle_index == 1 || ed_drag_box_handle_index == 2)
					ed_drag_box_oy = y1 - ry1;
				else if(ed_drag_box_handle_index == 4 || ed_drag_box_handle_index == 5 || ed_drag_box_handle_index == 6)
					ed_drag_box_oy = y2 - ry2;
				else
					ed_drag_box_oy = 0;
			}
			
			if(handle_result != None)
			{
				ed_box_handle_index = i;
				result = handle_result;
			}
		}
		
		ed_rel_x = rel_x;
		ed_rel_y = rel_y;
		
		return result;
	}
	
	private EditorMouseResult ed_box_handle(const float x1, const float y1, const float x2, const float y2,
		IEditable@ ref, const int index, const int handle_index, const int layer, const uint colour)
	{
		float x = 0, y = 0;
		float ox = 0, oy = 0;
		const float mx = (x1 + x2) * 0.5;
		const float my = (y1 + y2) * 0.5;
		
		switch(handle_index)
		{
			case 0: ox = -1; oy = -1; x = x1; y = y1; break;
			case 1: ox =  0; oy = -1; x = mx; y = y1; break;
			case 2: ox =  1; oy = -1; x = x2; y = y1; break;
			case 3: ox =  1; oy =  0; x = x2; y = my; break;
			case 4: ox =  1; oy =  1; x = x2; y = y2; break;
			case 5: ox =  0; oy =  1; x = mx; y = y2; break;
			case 6: ox = -1; oy =  1; x = x1; y = y2; break;
			case 7: ox = -1; oy =  0; x = x1; y = my; break;
			case 8: ox =  0; oy =  0; x = mx; y = my; break;
		}
		
		ox *= ed_handle_size;
		oy *= ed_handle_size;
		const EditorMouseResult result = ed_handle(x - ox, y - oy, ref, -1 - index * 9 - handle_index, layer, colour, 0);
		
		if(result == Move || result == LeftPress)
		{
			ed_drag_box_handle_index = handle_index;
		}
		
		return result;
	}
	
	void ed_update_box(float &out x1, float &out y1, float &out x2, float &out y2, const float rx=0, const float ry=0)
	{
		float mouse_x = 0, mouse_y = 0;
		transform_layer_position(g, ed_view_x, ed_view_y, mouse.x, mouse.y, 22, ed_layer, mouse_x, mouse_y);
		
		x1 = ed_drag_box_x1;
		y1 = ed_drag_box_y1;
		x2 = ed_drag_box_x2;
		y2 = ed_drag_box_y2;
		
		if(ed_drag_box_handle_index == 0 || ed_drag_box_handle_index == 6 || ed_drag_box_handle_index == 7)
		{
			x1 = mouse_x - ed_drag_ox - ed_handle_size + ed_drag_box_ox;
			ed_do_snap(x1, x1);
		}
		else if(ed_drag_box_handle_index == 2 || ed_drag_box_handle_index == 3 || ed_drag_box_handle_index == 4)
		{
			x2 = mouse_x - ed_drag_ox + ed_handle_size + ed_drag_box_ox;
			ed_do_snap(x2, x2);
		}
		else if(ed_drag_box_handle_index == 8)
		{
			x1 = mouse_x - ed_drag_ox - abs(ed_drag_box_x2 - ed_drag_box_x1) * 0.5 + ed_drag_box_ox;
			x2 = mouse_x - ed_drag_ox + abs(ed_drag_box_x2 - ed_drag_box_x1) * 0.5 + ed_drag_box_oy;
			ed_do_snap(x1, x2, x1, x2);
		}
		
		if(ed_drag_box_handle_index == 0 || ed_drag_box_handle_index == 1 || ed_drag_box_handle_index == 2)
		{
			y1 = mouse_y - ed_drag_oy - ed_handle_size + ed_drag_box_oy;
			ed_do_snap(y1, y1);
		}
		else if(ed_drag_box_handle_index == 4 || ed_drag_box_handle_index == 5 || ed_drag_box_handle_index == 6)
		{
			y2 = mouse_y - ed_drag_oy + ed_handle_size + ed_drag_box_oy;
			ed_do_snap(y2, y2);
		}
		else if(ed_drag_box_handle_index == 8)
		{
			y1 = mouse_y - ed_drag_oy - abs(ed_drag_box_y2 - ed_drag_box_y1) * 0.5 + ed_drag_box_ox;
			y2 = mouse_y - ed_drag_oy + abs(ed_drag_box_y2 - ed_drag_box_y1) * 0.5 + ed_drag_box_oy;
			ed_do_snap(y1, y2, y1, y2);
		}
		
		x1 -= ed_rel_x;
		y1 -= ed_rel_y;
		x2 -= ed_rel_x;
		y2 -= ed_rel_y;
	}
	
	void ed_outline(float x1, float y1, float x2, float y2, float rotation=0,
		const bool draw_snap_tiles=false,
		float thickness=-1, const int layer=19, const uint colour=0xff44aaaa)
	{
		if(ed_disable_handles)
			return;
		
		x1 += ed_rel_x;
		y1 += ed_rel_y;
		x2 += ed_rel_x;
		y2 += ed_rel_y;
		
		float rx1 = x1;
		float ry1 = y1;
		float rx2 = x2;
		float ry2 = y2;
		
		if(draw_snap_tiles)
		{
			rx1 = floor(x1 / 48) * 48;
			ry1 = floor(y1 / 48) * 48;
			rx2 = floor(x2 / 48 + 1) * 48;
			ry2 = floor(y2 / 48 + 1) * 48;
		}
		
		if(thickness <= 0)
		{
			thickness = ed_default_thickness;
		}
		
		if(
			x2 + thickness * ed_zoom < ed_view_x1 || x1 - thickness * ed_zoom > ed_view_x2 ||
			y2 + thickness * ed_zoom < ed_view_y1 || y1 - thickness * ed_zoom > ed_view_y2)
		{
			return;
		}
		
		if(ed_boxes_count + 3 > ed_boxes_size)
		{
			ed_boxes.resize(ed_boxes_size += 32);
		}
		
		EditorBox@ box = @ed_boxes[ed_boxes_count++];
		box.x1 = x1;
		box.y1 = y1;
		box.x2 = x2;
		box.y2 = y2;
		box.rotation = rotation;
		box.thickness = thickness;
		box.layer = layer;
		box.colour = colour;
		box.draw_snap_tiles = draw_snap_tiles;
	}
	
	//
	
	EditorMouseResult ed_radius_handle(float x, float y, const float radius,
		IEditable@ ref, const int index=0,
		float thickness=-1, const int layer=19, const float angle=0, const uint colour=0xffe46d35)
	{
		if(ed_disable_handles)
			return None;
		
		x += ed_rel_x;
		y += ed_rel_y;
		thickness = thickness < 0 ? ed_default_thickness : thickness;
		
		const float x1 = x - radius - thickness * ed_zoom;
		const float y1 = y - radius - thickness * ed_zoom;
		const float x2 = x + radius + thickness * ed_zoom;
		const float y2 = y + radius + thickness * ed_zoom;
		
		if(x2 < ed_view_x1 || x1 > ed_view_x2 || y2 < ed_view_y1 || y1 > ed_view_y2)
			return None;
		
		if(ed_circle_handles_count + 2 > ed_circle_handles_size)
		{
			ed_circle_handles.resize(ed_circle_handles_size *= 32);
		}
		
		if(thickness > 0)
		{
			EditorCircleHandle@ circle = @ed_circle_handles[ed_circle_handles_count++];
			circle.x = x;
			circle.y = y;
			circle.radius = radius;
			circle.thickness = thickness;
			circle.layer = layer;
			circle.colour = colour;
		}
		
		const float rel_x = ed_rel_x;
		const float rel_y = ed_rel_y;
		ed_rel_x = 0;
		ed_rel_y = 0;
		
		EditorMouseResult result = ed_handle(
			x + cos(angle * DEG2RAD) * radius,
			y + sin(angle * DEG2RAD) * radius,
			ref, index, layer, colour, 0);
		
		if(result == LeftPress)
		{
			ed_drag_ox = x;
			ed_drag_oy = y;
			const float mouse_x = layer == mouse.layer ? mouse.x : g.mouse_x_world(0, layer);
			const float mouse_y = layer == mouse.layer ? mouse.y : g.mouse_y_world(0, layer);
			ed_drag_radius_offset = distance(mouse_x, mouse_y, x, y) - radius;
		}
		
		ed_rel_x = rel_x;
		ed_rel_y = rel_y;
		
		return result;
	}
	
	void ed_update_radius(float &out radius)
	{
		float x, y;
		transform_layer_position(g, ed_view_x, ed_view_y, mouse.x, mouse.y, 22, ed_layer, x, y);
		
		radius = distance(x, y, ed_drag_ox,  ed_drag_oy) - ed_drag_radius_offset;
	}
	
	//
	
	EditorMouseResult ed_angle_handle(float x, float y, const float angle, const float radius,
		IEditable@ ref, const int index=0, const bool is_radians=true,
		float thickness=-1, const int layer=19, const uint colour=0xffaa44a7)
	{
		if(ed_disable_handles)
			return None;
		
		x += ed_rel_x;
		y += ed_rel_y;
		
		const float x1 = x - radius;
		const float y1 = y - radius;
		const float x2 = x + radius;
		const float y2 = y + radius;
		
		if(thickness < 0)
		{
			thickness = ed_default_thickness;
		}
		
		if(
			x2 + thickness * ed_zoom < ed_view_x1 || x1 - thickness * ed_zoom > ed_view_x2 ||
			y2 + thickness * ed_zoom < ed_view_y1 || y1 - thickness * ed_zoom > ed_view_y2)
		{
			return None;
		}
		
		ed_drag_radians = is_radians;
		const float radians = is_radians ? angle : angle * DEG2RAD;
		const float handle_x = x + cos(radians) * radius;
		const float handle_y = y + sin(radians) * radius;
		
		if(ed_lines_count + 32 > ed_lines_size)
		{
			ed_lines.resize(ed_lines_size += 32);
		}
		
		if(thickness > 0)
		{
			EditorLine@ line = @ed_lines[ed_lines_count++];
			line.x1 = x;
			line.y1 = y;
			line.x2 = handle_x;
			line.y2 = handle_y;
			line.thickness = thickness;
			line.layer = layer;
			line.colour = colour;
		}
		
		const float rel_x = ed_rel_x;
		const float rel_y = ed_rel_y;
		ed_rel_x = 0;
		ed_rel_y = 0;
		
		EditorMouseResult result = ed_handle(handle_x, handle_y, ref, index, layer, colour, 0);
		
		if(@ed_last_handle != null)
		{
			ed_last_handle.circle = true;
		}
		
		if(result == LeftPress)
		{
			const float mouse_x = layer == mouse.layer ? mouse.x : g.mouse_x_world(0, layer);
			const float mouse_y = layer == mouse.layer ? mouse.y : g.mouse_y_world(0, layer);
			ed_drag_ox = x;
			ed_drag_oy = y;
			ed_drag_radius_offset = shortest_angle(atan2(mouse_y - y, mouse_x - x), radians);
		}
		
		ed_rel_x = rel_x;
		ed_rel_y = rel_y;
		
		return result;
	}
	
	void ed_update_angle(float &out angle)
	{
		float x, y;
		transform_layer_position(g, ed_view_x, ed_view_y, mouse.x, mouse.y, 22, ed_layer, x, y);
		
		angle = atan2(y - ed_drag_oy, x - ed_drag_ox) + ed_drag_radius_offset;
		
		const float snap =
			  input.key_check_gvb(GVB::Alt)   ? PI / 180
			: input.key_check_gvb(GVB::Control)  ? PI / 36
			: input.key_check_gvb(GVB::Shift)  ? PI / 8 : 0;
		
		if(snap != 0)
		{
			angle = round(angle / snap) * snap;
		}
		
		if(!ed_drag_radians)
		{
			angle *= RAD2DEG;
		}
	}
	
	//
	
	void ed_link_position(scripttrigger@ trigger, const float x, const float y, const float thickness=1, const int layer=19, const uint colour=0x66ffffff)
	{
		ed_line(trigger.x(), trigger.y(), x, y, thickness, layer, colour);
	}
	
	void ed_link_position(scriptenemy@ enemy, const float x, const float y, const float thickness=1, const int layer=19, const uint colour=0x66ffffff)
	{
		ed_line(enemy.x(), enemy.y(), x, y, thickness, layer, colour);
	}
	
	void ed_line(float x1, float y1, float x2, float y2, float thickness=-1, const int layer=19, const uint colour=0xffffffff)
	{
		if(ed_disable_handles)
			return;
		
		x1 += ed_rel_x;
		y1 += ed_rel_y;
		x2 += ed_rel_x;
		y2 += ed_rel_y;
		thickness = thickness <= 0 ? ed_default_thickness : thickness;
		
		const float v_x1 = min(x1, x2) - thickness * ed_zoom;
		const float v_y1 = min(y1, y2) - thickness * ed_zoom;
		const float v_x2 = max(x1, x2) + thickness * ed_zoom;
		const float v_y2 = max(y1, y2) + thickness * ed_zoom;
		
		if(v_x2 < ed_view_x1 || v_x1 > ed_view_x2 || v_y2 < ed_view_y1 || v_y1 > ed_view_y2)
			return;
		
		if(ed_lines_count + 2 > ed_lines_size)
		{
			ed_lines.resize(ed_lines_size *= 2);
		}
		
		EditorLine@ line = @ed_lines[ed_lines_count++];
		line.x1 = x1;
		line.y1 = y1;
		line.x2 = x2;
		line.y2 = y2;
		line.thickness = thickness;
		line.layer = layer;
		line.colour = colour;
	}
	
	void ed_arrow(float x1, float y1, float x2, float y2,
		const float head_size, const float head_position, float thickness=-1, const int layer=19, const uint colour=0xffffffff)
	{
		if(ed_disable_handles)
			return;
		
		x1 += ed_rel_x;
		y1 += ed_rel_y;
		x2 += ed_rel_x;
		y2 += ed_rel_y;
		thickness = thickness <= 0 ? ed_default_thickness : thickness;
		
		const float v_x1 = min(x1, x2) - (thickness + head_size) * ed_zoom;
		const float v_y1 = min(y1, y2) - (thickness + head_size) * ed_zoom;
		const float v_x2 = max(x1, x2) + (thickness + head_size) * ed_zoom;
		const float v_y2 = max(y1, y2) + (thickness + head_size) * ed_zoom;
		
		if(v_x2 < ed_view_x1 || v_x1 > ed_view_x2 || v_y2 < ed_view_y1 || v_y1 > ed_view_y2)
			return;
		
		if(ed_arrows_count + 2 > ed_arrows_size)
		{
			ed_arrows.resize(ed_arrows_size *= 2);
		}
		
		EditorArrow@ arrow = @ed_arrows[ed_arrows_count++];
		arrow.x1 = x1;
		arrow.y1 = y1;
		arrow.x2 = x2;
		arrow.y2 = y2;
		arrow.thickness = thickness;
		arrow.head_size = head_size;
		arrow.head_position = head_position;
		arrow.layer = layer;
		arrow.colour = colour;
	}
	
	void ed_circle(float x, float y, const float radius, float thickness=-1, const int layer=19, const uint colour=0xffffffff)
	{
		if(ed_disable_handles)
			return;
		
		x += ed_rel_x;
		y += ed_rel_y;
		thickness = thickness <= 0 ? ed_default_thickness : thickness;
		
		const float v_x1 = x - radius - thickness * ed_zoom;
		const float v_y1 = y - radius - thickness * ed_zoom;
		const float v_x2 = x + radius + thickness * ed_zoom;
		const float v_y2 = y + radius + thickness * ed_zoom;
		
		if(v_x2 < ed_view_x1 || v_x1 > ed_view_x2 || v_y2 < ed_view_y1 || v_y1 > ed_view_y2)
			return;
		
		if(ed_circle_handles_count + 2 > ed_circle_handles_size)
		{
			ed_arcs.resize(ed_circle_handles_size *= 2);
		}
		
		EditorCircleHandle@ circle = @ed_circle_handles[ed_circle_handles_count++];
		circle.x = x;
		circle.y = y;
		circle.radius = radius;
		circle.thickness = thickness;
		circle.layer = layer;
		circle.colour = colour;
	}
	
	void ed_arc(float x, float y, const float radius, const float angle_min=0, const float angle_max=0, float thickness=-1, const int layer=19, const uint colour=0xffffffff)
	{
		if(ed_disable_handles)
			return;
		
		x += ed_rel_x;
		y += ed_rel_y;
		thickness = thickness <= 0 ? ed_default_thickness : thickness;
		
		const float v_x1 = x - radius - thickness * ed_zoom;
		const float v_y1 = y - radius - thickness * ed_zoom;
		const float v_x2 = x + radius + thickness * ed_zoom;
		const float v_y2 = y + radius + thickness * ed_zoom;
		
		if(v_x2 < ed_view_x1 || v_x1 > ed_view_x2 || v_y2 < ed_view_y1 || v_y1 > ed_view_y2)
			return;
		
		if(ed_arcs_count + 2 > ed_arcs_size)
		{
			ed_arcs.resize(ed_arcs_size *= 2);
		}
		
		EditorArc@ arc = @ed_arcs[ed_arcs_count++];
		arc.x = x;
		arc.y = y;
		arc.radius = radius;
		arc.angle_min = angle_min;
		arc.angle_max = angle_max;
		arc.thickness = thickness;
		arc.layer = layer;
		arc.colour = colour;
	}
	
	void ed_text(float x, float y, const string text, const float scale=1, const float rotation=0,
		const int layer=19, const uint colour=0xffffffff)
	{
		if(ed_disable_handles)
			return;
		
		x += ed_rel_x;
		y += ed_rel_y;
		
		if(
			x < ed_view_x1 || x > ed_view_x2 ||
			y < ed_view_y1 || y > ed_view_y2)
		{
			return;
		}
		
		if(ed_texts_count + 2 > ed_texts_size)
		{
			ed_texts.resize(ed_texts_size *= 2);
		}
		
		EditorText@ txt = @ed_texts[ed_texts_count++];
		txt.x = x;
		txt.y = y;
		txt.text = text;
		txt.scale = scale;
		txt.rotation = rotation;
		txt.layer = layer;
		txt.colour = colour;
	}
	
	//
	
	int ed_circle_segments(const float radius)
	{
		return ceil_int(map_clamped(radius / ed_zoom, 0, 850, 14, 64));
	}
	
}
