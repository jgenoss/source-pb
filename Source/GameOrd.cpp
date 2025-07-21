#include "pch.h"
#include "GameOrd.h"

//#include <unordered_map>
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/utf16_to_mb.h"

namespace
{
	template<class B, class D>
	struct static_creator {		static 	B*		call() { B* p = new D; MEMDUMP_NEW(p, sizeof(D)); return p;}	};
	
	class gord_factory
	{
	public:

		typedef gord* (*function_pointer)();
		typedef i3::unordered_map<STAGE_UID, function_pointer>		map_type;
		
		
		gord_factory()
		{
			m_map.insert(map_type::value_type(STAGE_UID_UPTOWN,		&static_creator<gord, gord_uptown>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_HELISPOT,	&static_creator<gord, gord_helispot>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_OUTPOST,	&static_creator<gord, gord_outpost>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_STORMTUBE,	&static_creator<gord, gord_stormtube>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_STORMTUBE,	&static_creator<gord, gord_d_stormtube>::call ));
			m_map.insert(map_type::value_type(STAGE_UID_RUSHHOUR,	&static_creator<gord, gord_rushhour>::call ));
		}

		function_pointer	get_fun(STAGE_UID type) const
		{ 
			map_type::const_iterator it = m_map.find(type);
			return (it != m_map.end()) ? it->second : &static_creator<gord, gord>::call;
		}

	private:
		map_type	m_map;		// MS�� unordered_map�� �ؽ��Լ����������� �ְ�, �޸��Ҵ� �ӵ��� ���� ������ ����Ǿ�����..
																				// �ϴ� �����ϰ� ���߿� �����̳��� ���ӽ����̽��� ��ü�� ����..
	} g_factory;


}

void		create_gord(STAGE_UID ord_id)
{
	tinst<gord>() = (*g_factory.get_fun(ord_id))();
}

void		destroy_gord()
{
	I3_SAFE_DELETE( tinst<gord>());
}

///////////////////////////////////////////////

void gord_uptown::ChangeObjectSpecificName( i3Object* pObj)
{
	if(i3::generic_is_equal( pObj->GetName(), "������") )
	{
		i3::string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAMCAR"), str);
		pObj->SetName( str );	// "tramcar"
	}
}

void gord_rushhour::ChangeObjectSpecificName( i3Object* pObj)
{
	if(i3::generic_is_equal( pObj->GetName(), "������") )
	{
		i3::rc_string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAMCAR"), str);

		pObj->SetName( str );	// "tramcar"
	}
}

void gord_helispot::ChangeObjectSpecificName( i3Object* pObj)
{
	if(i3::generic_is_equal( pObj->GetName(), "�������") )
	{
		i3::rc_string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_RED_HELICOPTER"), str);
		pObj->SetName( str  );	// "RED helicopter"
	}
	else if(i3::generic_is_equal( pObj->GetName(), "������") )
	{
		i3::rc_string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_BLUE_HELICOPTER"), str);
		pObj->SetName( str );	// "BLUE helicopter"
	}
}

void gord_outpost::ChangeObjectSpecificName( i3Object* pObj)
{
	if(i3::generic_is_equal( pObj->GetName(), "���") )
	{
		i3::rc_string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_HELICOPTER"), str);
		pObj->SetName( str );	// "helicopter"
	}
	else if(i3::generic_is_equal( pObj->GetName(), "âƲ") )
	{
		i3::rc_string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_WINDOW_FRAME"), str);
		pObj->SetName( str );	// "window frame"
	}
}


void gord_stormtube_base::ChangeObjectSpecificName( i3Object* pObj)
{
	if(i3::generic_is_equal( pObj->GetName(), "���� ���� ����") )
	{
		i3::rc_string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_VEHICLE"), str);
		pObj->SetName( str );	// "rail repair vehicle"
	}
	else if(i3::generic_is_equal( pObj->GetName(), "����ö A") )
	{
		i3::rc_string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAIN_A") , str);
		pObj->SetName( str );	// "Subway train A"
	}
	else if(i3::generic_is_equal( pObj->GetName(), "����ö B") )
	{
		i3::rc_string str;
		i3::utf16_to_mb( GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAIN_B"), str);
		pObj->SetName( str );	// "Subway train B"
	}
}



