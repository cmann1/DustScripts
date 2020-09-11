#include '../std.cpp';
#include '../drawing/common.cpp';
#include '../math/math.cpp';
#include '../layer.cpp';
#include '../enums/GVB.cpp';
#include 'IEditable.cpp';
#include 'EditorMouseResult.cpp';
#include 'draw/EditorBox.cpp';
#include 'draw/EditorHandle.cpp';
#include 'draw/EditorLine.cpp';

namespace BaseEditorScript { const float ED_HANDLE_SIZE = 5; }

class BaseEditorScript
{
	
	scene@ g;
	camera@ cam;
	editor_api@ editor;
	
	Mouse mouse(false);
	float ed_zoom = 1;
	float ed_handle_size;
	float ed_default_thickness = 1;
	
	protected float ed_view_x;
	protected float ed_view_y;
	protected float ed_view_x1, ed_view_y1;
	protected float ed_view_x2, ed_view_y2;
	
	protected int ed_handles_size = 32;
	protected array<EditorHandle> ed_handles(ed_handles_size);
	protected int ed_handles_count;
	
	protected int ed_lines_size = 32;
	protected array<EditorLine> ed_lines(ed_lines_size);
	protected int ed_lines_count;
	
	protected int ed_boxes_size = 32;
	protected array<EditorBox> ed_boxes(ed_boxes_size);
	protected int ed_boxes_count;
	
	protected bool ed_press;
	protected bool ed_left_press;
	protected bool ed_right_press;
	protected bool ed_middle_press;
	protected int ed_layer;
	protected IEditable@ ed_drag_handle_ref;
	protected int ed_drag_handle_index;
	protected float ed_drag_ox;
	protected float ed_drag_oy;
	protected int ed_drag_box_handle_index;
	protected float ed_drag_box_ox, ed_drag_box_oy;
	protected float ed_drag_box_x1, ed_drag_box_y1;
	protected float ed_drag_box_x2, ed_drag_box_y2;
	
	BaseEditorScript()
	{
		@cam = get_active_camera();
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
		
		const bool can_press = @editor == null || !editor.mouse_in_gui() && !editor.key_check_gvb(GVB::Space) && editor.editor_tab() == 'Triggers';
//		const bool can_press = !editor.mouse_in_gui() && !editor.key_check_gvb(GVB::Space);
		
		ed_handles_count = 0;
		ed_lines_count = 0;
		ed_boxes_count = 0;
		
		ed_left_press = can_press && mouse.left_press;
		ed_right_press = can_press && mouse.right_press;
		ed_middle_press = can_press && mouse.middle_press;
		ed_press = ed_left_press || ed_right_press || ed_middle_press;
		
		mouse.step();
		
		if(!mouse.left_down)
		{
			@ed_drag_handle_ref = null;
			ed_drag_handle_index = -1;
		}
	}
	
	void editor_draw(float sub_frame)
	{
		for(int i = 0; i < ed_handles_count; i++)
		{
			EditorHandle@ handle = @ed_handles[i];
			
			float x, y;
			transform_layer_position(g, ed_view_x, ed_view_y, handle.x, handle.y, handle.layer, 22, x, y);
			
			g.draw_rectangle_world(22, 22,
				x - ed_handle_size, y - ed_handle_size,
				x + ed_handle_size, y + ed_handle_size, handle.rotation, handle.colour);
		}
		
		for(int i = 0; i < ed_lines_count; i++)
		{
			EditorLine@ line = @ed_lines[i];
			
			float x1, y1;
			float x2, y2;
			transform_layer_position(g, ed_view_x, ed_view_y, line.x1, line.y1, line.layer, 22, x1, y1);
			transform_layer_position(g, ed_view_x, ed_view_y, line.x2, line.y2, line.layer, 22, x2, y2);
			
			draw_line(g, 22, 20, x1, y1, x2, y2, line.thickness * ed_zoom, line.colour);
		}
		
		for(int i = 0; i < ed_boxes_count; i++)
		{
			EditorBox@ box = @ed_boxes[i];
			
			float x1, y1;
			float x2, y2;
			transform_layer_position(g, ed_view_x, ed_view_y, box.x1, box.y1, box.layer, 22, x1, y1);
			transform_layer_position(g, ed_view_x, ed_view_y, box.x2, box.y2, box.layer, 22, x2, y2);
			
			if(box.draw_snap_tiles)
			{
				x1 = floor(x1 / 48) * 48;
				y1 = floor(y1 / 48) * 48;
				x2 = floor(x2 / 48 + 1) * 48;
				y2 = floor(y2 / 48 + 1) * 48;
			}
			
			outline_rect(g, 22, 20, x1, y1, x2, y2, box.thickness * ed_zoom, box.colour);
		}
	}
	
	//
	
	EditorMouseResult ed_handle(const float x, const float y, IEditable@ ref, const int index, const int layer=19,
		const uint colour=0xffaaaa44, const float rotation=45)
	{
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
		
		if(@ed_drag_handle_ref == @ref && ed_drag_handle_index == index)
		{
			ed_layer = layer;
			return Move;
		}
		
		if(ed_press)
		{
			const float mouse_x = layer == mouse.layer ? mouse.x : g.mouse_x_world(0, layer);
			const float mouse_y = layer == mouse.layer ? mouse.y : g.mouse_y_world(0, layer);
			
			if(dist_sqr(x, y, mouse_x, mouse_y) <= (ed_handle_size * ed_handle_size))
			{
				if(ed_left_press)
				{
					@ed_drag_handle_ref = @ref;
					ed_drag_handle_index = index;
					ed_drag_ox = mouse_x - x;
					ed_drag_oy = mouse_y - y;
					ed_press = false;
					
					editor.key_clear_gvb(GVB::LeftClick);
					return LeftPress;
				}
				else if(ed_right_press)
				{
					editor.key_clear_gvb(GVB::RightClick);
					return RightPress;
				}
				else if(ed_middle_press)
				{
					editor.key_clear_gvb(GVB::MiddleClick);
					return MiddlePress;
				}
			}
		}
		
		return None;
	}
	
	void ed_update_handle(float &out x, float &out y)
	{
		transform_layer_position(g, ed_view_x, ed_view_y, mouse.x, mouse.y, 22, ed_layer, x, y);
		
		x -= ed_drag_ox;
		y -= ed_drag_oy;
	}
	
	//
	
	EditorMouseResult ed_box(const float x1, const float y1, const float x2, const float y2,
		IEditable@ ref, const int index=0,
		const bool draw_snap_tiles=false,
		float thickness=-1, const int layer=19, const uint colour=0xff44aaaa)
	{
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
		
		float v_x1 = min(x1, x2);
		float v_y1 = min(y1, y2);
		float v_x2 = max(x1, x2);
		float v_y2 = max(y1, y2);
		
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
		
		if(ed_boxes_count + 2 > ed_boxes_size)
		{
			ed_boxes.resize(ed_boxes_size += 32);
		}
		
		EditorBox@ box = @ed_boxes[ed_boxes_count++];
		box.x1 = x1;
		box.y1 = y1;
		box.x2 = x2;
		box.y2 = y2;
		box.thickness = thickness;
		box.layer = layer;
		box.colour = colour;
		box.draw_snap_tiles = draw_snap_tiles;
		
		EditorMouseResult result = None;
		ed_drag_box_x1 = x1;
		ed_drag_box_y1 = y1;
		ed_drag_box_x2 = x2;
		ed_drag_box_y2 = y2;
		
		for(int i = 0; i < 9; i++)
		{
			switch(ed_box_handle(rx1, ry1, rx2, ry2, ref, index, i, layer, colour))
			{
				case LeftPress:
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
					
					break;
				case Move:
					result = Move;
					break;
			}
		}
		
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
		const EditorMouseResult result = ed_handle(x - ox, y - oy, ref, -index * 9 + handle_index, layer, colour, 0);
		
		if(result == Move || result == LeftPress)
		{
			ed_drag_box_handle_index = handle_index;
		}
		
		return result;
	}
	
	void ed_update_box(float &out x1, float &out y1, float &out x2, float &out y2)
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
		}
		else if(ed_drag_box_handle_index == 2 || ed_drag_box_handle_index == 3 || ed_drag_box_handle_index == 4)
		{
			x2 = mouse_x - ed_drag_ox + ed_handle_size + ed_drag_box_ox;
		}
		else if(ed_drag_box_handle_index == 8)
		{
			x1 = mouse_x - ed_drag_ox - abs(ed_drag_box_x2 - ed_drag_box_x1) * 0.5 + ed_drag_box_ox;
			x2 = mouse_x - ed_drag_ox + abs(ed_drag_box_x2 - ed_drag_box_x1) * 0.5 + ed_drag_box_oy;
		}
		
		if(ed_drag_box_handle_index == 0 || ed_drag_box_handle_index == 1 || ed_drag_box_handle_index == 2)
		{
			y1 = mouse_y - ed_drag_oy - ed_handle_size + ed_drag_box_oy;
		}
		else if(ed_drag_box_handle_index == 4 || ed_drag_box_handle_index == 5 || ed_drag_box_handle_index == 6)
		{
			y2 = mouse_y - ed_drag_oy + ed_handle_size + ed_drag_box_oy;
		}
		else if(ed_drag_box_handle_index == 8)
		{
			y1 = mouse_y - ed_drag_oy - abs(ed_drag_box_y2 - ed_drag_box_y1) * 0.5 + ed_drag_box_ox;
			y2 = mouse_y - ed_drag_oy + abs(ed_drag_box_y2 - ed_drag_box_y1) * 0.5 + ed_drag_box_oy;
		}
	}
	
	//
	
	void ed_link_position(scripttrigger@ trigger, const float x, const float y, const float thickness=1, const int layer=19, const uint colour=0xffffffff)
	{
		ed_line(trigger.x(), trigger.y(), x, y, thickness, layer, colour);
	}
	
	void ed_link_position(scriptenemy@ enemy, const float x, const float y, const float thickness=1, const int layer=19, const uint colour=0xffffffff)
	{
		ed_line(enemy.x(), enemy.y(), x, y, thickness, layer, colour);
	}
	
	void ed_line(const float x1, const float y1, const float x2, const float y2, float thickness=-1, const int layer=19, const uint colour=0xffffffff)
	{
		const float v_x1 = min(x1, x2);
		const float v_y1 = min(y1, y2);
		const float v_x2 = max(x1, x2);
		const float v_y2 = max(y1, y2);
		
		if(thickness <= 0)
		{
			thickness = ed_default_thickness;
		}
		
		if(
			v_x2 + thickness * ed_zoom < ed_view_x1 || v_x1 - thickness * ed_zoom > ed_view_x2 ||
			v_y2 + thickness * ed_zoom < ed_view_y1 || v_y1 - thickness * ed_zoom > ed_view_y2)
		{
			return;
		}
		
		if(ed_lines_count + 2 > ed_lines_size)
		{
			ed_lines.resize(ed_lines_size += 32);
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
	
}