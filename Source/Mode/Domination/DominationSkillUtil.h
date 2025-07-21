#pragma once

#undef ENUM_LIST
#define ENUM_LIST(enum_value) \
	enum_value(SentryGun_Transparent) \
	enum_value(SentryGun_Lv1) \
	enum_value(SentryGun_Lv2) \
	enum_value(SentryGun_Lv3) \
	enum_value(SentryGun_Lv4) \
	enum_value(Dummy_Transparent) \
	enum_value(Dummy_Lv1) \
	enum_value(Dummy_Lv2) \
	enum_value(Dummy_Lv3) \
	enum_value(Dummy_Lv4) \
	enum_value(SupplyHP) \
	enum_value(SupplyAmmo) \
	enum_value(SkillShop) \
	enum_value(type_max)

#undef DO_ENUM
#define DO_ENUM(value) value,
#undef DO_DESC
#define DO_DESC(value) _T(#value),

namespace DominationSkill
{
	enum ENUM_TYPE 
	{ ENUM_LIST(DO_ENUM) };

	static const char* enum_Desc[] = 
	{ ENUM_LIST(DO_DESC) };

	struct Info
	{
		ENUM_TYPE		eType;		//class ���� ��

		i3GameResChara* pRes;		//CRawObject �� ���ҽ�
		UINT32			UseItemID;	//CRawObject �� Ȱ��ȭ �� ���� ��ų ������ ID
		UINT32			Level;		//

		Info( ENUM_TYPE e, i3GameResChara* p, UINT32 l = 0, UINT32 u = 0) : eType(e), pRes(p), Level(l), UseItemID(u) {}
	};
};

namespace dss = DominationSkill;

#include "i3Base/itl/map.h"



//
//      class CButtonCallback
//		{
//			virtual	 void OnClick() = 0;
//			virtual ~CButtonCallback() {}
//		};
//		
//		class Custom_Button_CB : CButtonCallback
//		{
//			virtual	 void OnClick() 
//			{
//				// ������ ä��..
//				�̺�Ʈ �ڵ�...
//			}
//		};
//
//      class CButton : public CUIObject
//		{
//			virtual void process_click() 
//			{      
//					// ���� ��ư�� ���� �׷��� ó��..
//					//
//					m_cb->OnClick();
//			}
//			void SetCallback(CButtonCallback* );
//		private:
//			CButtonCallback* m_cb;
//		};
//
//
//		namespace domi
//		{
//			class skill_obj				// ȿ���ִ� ������Ʈ.0
//			{
//				skill_obj(  skill_obj_property*	);
//				skill_obj_property*	m_property;
//
//				bool is_installed() const { return m_property->is_installed(); }
//			};

//			class skill_supplybase : skill_obj	//	stationary_obj
//          {          
//				skill_supplybase() : skill_obj(new stationary_property)
//          };
//
//			class skill_dummy :	skill_obj			// installed_obj   --> installable_obj
//			{
//				skill_dummy() : skill_obj(new installable_property)

//			};
//
//			class skill_sentri : skill_obj			// installed_obj  --> installable_obj
//			{
//				skill_sentri() : skill_obj(new installable_property)
//			};
//





//			class skill_obj_property
//			{
//				virtual bool is_installed() const = 0;
//
//			};
//
//			class stationary_property : skill_obj_property 
//			{
//				virtual bool is_installed() const { return true; }
//			};
//
//			class installable_property : skill_obj_property
//			{
//				installable_property() : m_state( new not_installed_state ) {}
//				install_state*		m_state;
//				virtual bool is_installed() const { return m_state->is_installed(); }
//			};
//			
//			




//
//			class install_state
//			{
//				bool is_installed() const { return m_bInstall; }
//				const bool m_binstall;
//			}

//          �Ӽ� ���Ŭ����..
//			class not_installed_state : install_state
//			{
//				not_installed_state() : install_state( false) {}
//			};

//			class installed_state : install_state
//			{
//				not_installed_state() : install_state( true) {}
//			};





//		}
//
//
//
//