#include 'TileShape.cpp';
#include '../math/math.cpp';

//bool get_tile_edge_top_angle(int shape)
//{
//	switch(shape)
//	{
//		case TileShape::Full:
//		case TileShape::Big_2:
//		case TileShape::Big_3:
//		case TileShape::Small_3:
//		case TileShape::Big_6:
//		case TileShape::Big_7:
//		case TileShape::Small_7:
//		case TileShape::Half_B:
//		case TileShape::Half_C:
//			x1 = 0; y1 = 0;
//			x2 = 48; y2 = 0;
//			break;
//		case TileShape::Big_1:
//			x1 = 0; y1 = 0;
//			x2 = 48; y2 = 24;
//			break;
//		case TileShape::Small_1:
//			x1 = 0; y1 = 24;
//			x2 = 48; y2 = 48;
//			break;
//		case TileShape::Small_2:
//		case TileShape::Big_8:
//			x1 = 0; y1 = 0;
//			x2 = 24; y2 = 0;
//			break;
//		case TileShape::Big_4:
//		case TileShape::Small_6:
//			x1 = 24; y1 = 0;
//			x2 = 48; y2 = 0;
//			break;
//		case TileShape::Small_4:
//			return false;
//		case TileShape::Big_5:
//			x1 = 0; y1 = 24;
//			x2 = 48; y2 = 0;
//			break;
//		case TileShape::Small_5:
//			x1 = 0; y1 = 48;
//			x2 = 48; y2 = 24;
//			break;
//		case TileShape::Small_8:
//			return false;
//		case TileShape::Half_A:
//			x1 = 0; y1 = 0;
//			x2 = 48; y2 = 48;
//			break;
//		case TileShape::Half_D:
//			x1 = 0; y1 = 48;
//			x2 = 48; y2 = 0;
//			break;
//	}
//	
//	return true;
//}

//float get_tile_edge_float_angle(int shape)
//{
//	switch(shape)
//	{
//		case TileShape::Full:
//		case TileShape::Big_1:
//		case TileShape::Big_2:
//		case TileShape::Small_2:
//		case TileShape::Big_7:
//		case TileShape::Big_8:
//		case TileShape::Small_8:
//		case TileShape::Half_A:
//		case TileShape::Half_B:
//			x1 = 0; y1 = 48;
//			x2 = 0; y2 = 0;
//			break;
//		case TileShape::Small_1:
//		case TileShape::Big_5:
//			x1 = 0; y1 = 48;
//			x2 = 0; y2 = 24;
//			break;
//		case TileShape::Big_3:
//			x1 = 0; y1 = 24;
//			x2 = 0; y2 = 0;
//			break;
//		case TileShape::Small_3:
//			return false;
//		case TileShape::Big_4:
//			x1 = 0; y1 = 48;
//			x2 = 24; y2 = 0;
//			break;
//		case TileShape::Small_4:
//			x1 = 24; y1 = 48;
//			x2 = 48; y2 = 0;
//			break;
//		case TileShape::Small_5:
//			return false;
//		case TileShape::Big_6:
//			x1 = 24; y1 = 48;
//			x2 = 0; y2 = 0;
//			break;
//		case TileShape::Small_6:
//			x1 = 48; y1 = 48;
//			x2 = 24; y2 = 0;
//			break;
//		case TileShape::Small_7:
//			x1 = 0; y1 = 24;
//			x2 = 0; y2 = 0;
//			break;
//		case TileShape::Half_C:
//			return false;
//		case TileShape::Half_D:
//			return false;
//	}
//	
//	return true;
//}