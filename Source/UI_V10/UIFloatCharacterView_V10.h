#if !defined( __UI_FLOAT_CHARACTERVIEW__V10_H__)
#define __UI_FLOAT_CHARACTERVIEW__V10_H__

#include "UI/UIFloatCharacterView.h"

typedef UINT32 REVERT_TYPE;
#define REVERT_TYPE_WEAPON 0x01
#define REVERT_TYPE_EQUIP 0x02
#define REVERT_TYPE_ALL (REVERT_TYPE_WEAPON | REVERT_TYPE_EQUIP)

class UICharaView;

class UIFloatCharacterView_V10 : public UIFloatCharacterView
{
	I3_CLASS_DEFINE( UIFloatCharacterView_V10, UIFloatCharacterView);


private:
	bool			m_bDurability;
	REAL32			m_rVersionPosX;

protected:


public:
	UIFloatCharacterView_V10();
	virtual ~UIFloatCharacterView_V10();

	virtual void			UpdateCharaPosition(void) override;
	// �̸����� �� ���·� ����
	virtual void			RevertChara(REVERT_TYPE type = REVERT_TYPE_ALL) override;
	void			RevertCharaData(REVERT_TYPE type = REVERT_TYPE_ALL);
	virtual void			SelectChara(T_ItemDBIdx chara_db_idx) override;
	virtual void			UpdateCharaSlot(INT32 charaSlot = -1) override;

	/** \brief �ƹ�Ÿ ������ ����
	\param[in] bDummyChara ���� ĳ�������� �ƴ���
	\param[in] mustUpdateWeapon �ݵ�� ������Ʈ�� ����� �����ϸ� �� */
	virtual void			DoUpdateCharacter(bool bDummyChara = false, WEAPON_SLOT_TYPE mustUpdateWeapon = WEAPON_SLOT_UNKNOWN) override;

	virtual void			ReceiveGameEvent_RevertParts(INT32 arg, const i3::user_data&) override;

	virtual bool			UIEvent_Change_Parts(i3ElementBase * pObj, UINT32 param, I3_EVT_CODE) override;
	virtual void			SetChangeTeam(TEAM_TYPE teamType) override;
	
};

#endif
