#pragma once

namespace bing	//break into game (���� ����)
{
	bool is_break_into_game();
	void print_room_info(); //���� ���ھ ����Ѵ�.
	INT32 get_room_info_string(i3::vector< i3::wstring > & list); //���� ������ string list�� ��ȯ�Ѵ�.
	
	//get_room_info_string �Լ��� ���� ����̴�. ���� score�� ��ȯ
	INT32 get_room_info_string(i3::vector< i3::wstring > & list, INT32 & red_score, INT32 & blue_score); 
}