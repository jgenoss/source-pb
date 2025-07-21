#pragma once

#pragma message ("BattleObjContext.h")

#include "ObjectInfoDef.h"
#include "StageObject/ObjectEvent.h"
#include "StageObject/ObjectWeaponBox.h"


#ifdef DOMI_STING_POISON_GRENADE
#include "Character/Fullbody/DomiDinoGrenades.h"
#endif

class BattleObjContext : public i3::shared_ginst<BattleObjContext>
{
public: 
	typedef void	(BattleObjContext::*netFunc)( i3Object * pObj, CGameCharaBase * pChara);
	void			RegisterFunctionWriteObj( UINT32 eventType, netFunc func);
	void			RegisterFunctionReceveObj( UINT32 eventType, netFunc func);
	void			ObjectHitWriteFunction(OBJ_EVENT_TYPE  ObjIdx, i3Object * pObj, CGameCharaBase * pChara = nullptr);
	void			ObjectHitReceveFunction(OBJ_EVENT_TYPE  ObjIdx, i3Object * pObj, CGameCharaBase * pChara = nullptr);
private : 
	i3::unordered_map< UINT32, netFunc >	m_ObjFuncMap_HitWrite;
	i3::unordered_map< UINT32, netFunc >	m_ObjFuncMap_HitReceve;

	void		__Register_Object_Function( void);
	void		__Function_Object_Write_Mission(i3Object * pObj = nullptr, CGameCharaBase * pChara = nullptr);
	void		__Function_Object_Write_Escape(i3Object * pObj = nullptr, CGameCharaBase * pChara = nullptr);
	
	void		__Function_Object_Receve_Mission(i3Object * pObj = nullptr, CGameCharaBase * pChara = nullptr);
	void		__Function_Object_Write_Mission_Trigger(i3Object * pObj = nullptr, CGameCharaBase * pChara = nullptr);

public:
	BattleObjContext();
	~BattleObjContext();

	const GAMEINFO_THROW_WEAPON *	getGameInfo_ThrowWeapon( INT32 userIdx, INT32 idx ) const
	{
		I3_BOUNDCHK( userIdx, SLOT_MAX_COUNT);	I3_BOUNDCHK( idx, MAX_THROWWEAPON_SLOP);
		return &m_ThrowWeaponGameInfo[ userIdx][ idx];
	}

	GAMEINFO_THROW_WEAPON *	setGameInfo_ThrowWeapon( INT32 userIdx, INT32 idx )
	{
		I3_BOUNDCHK( userIdx, SLOT_MAX_COUNT);	I3_BOUNDCHK( idx, MAX_THROWWEAPON_SLOP);
		return &m_ThrowWeaponGameInfo[ userIdx][ idx];
	}

	const GAMEINFO_THROW_WEAPON *	getGameInfo_ThrowWeapon( INT32 idx ) const
	{
		I3_BOUNDCHK( idx, MAX_THROWWEAPON_COUNT);
		return &m_ThrowWeaponGameInfo[ idx / MAX_THROWWEAPON_SLOP][ idx % MAX_THROWWEAPON_SLOP];
	}

	GAMEINFO_THROW_WEAPON *	setGameInfo_ThrowWeapon( INT32 idx )
	{
		I3_BOUNDCHK( idx, MAX_THROWWEAPON_COUNT);
		return &m_ThrowWeaponGameInfo[ idx / MAX_THROWWEAPON_SLOP][ idx % MAX_THROWWEAPON_SLOP];
	}

	void						ResetGameInfo_ThrowWeapon();

	const GAMEINFO_OBJECT *		getGameInfo_Object( INT32 idx ) const	{ I3_BOUNDCHK( idx, OBJ_TOTAL_COUNT);			return &m_ObjectGameInfo[ idx ]; }
	GAMEINFO_OBJECT *			setGameInfo_Object( INT32 idx )			{ I3_BOUNDCHK( idx, OBJ_TOTAL_COUNT);			return &m_ObjectGameInfo[ idx ]; }

	const GAMEINFO_DROPED_WEAPON *	getGameInfo_DropedWeapon( INT32 idx ) const 	{ I3_BOUNDCHK( idx, MAX_DROPEDWEAPON_COUNT);	return &m_DropedWeaponGameInfo[ idx ]; }
	GAMEINFO_DROPED_WEAPON *		setGameInfo_DropedWeapon( INT32 idx )			{ I3_BOUNDCHK( idx, MAX_DROPEDWEAPON_COUNT);	return &m_DropedWeaponGameInfo[ idx ]; }

	UINT16		getDropedWeaponCount( void) const					{ return m_nDropedWeaponCount; }
	void		SetDropedWeaponCount(UINT16 DropedWeaponCount)		{ m_nDropedWeaponCount = DropedWeaponCount; }

	const OBJECT_COUNT_INFO *	getObjectCountInfo(void) const	{ return &m_ObjectCountInfo;	}
	OBJECT_COUNT_INFO *			setObjectCountInfo(void)		{ return &m_ObjectCountInfo;	}

	// ������Ʈ Type�� ���� List�� ���� �����մϴ�.
	// UDP���� �������� Sync�� ����ϴ�.
	void					AddObject( i3Object * pObj );


#ifdef DOMI_STING_POISON_GRENADE
	const DomiDinoGrenades* getDomiDinoGrenades() const { return &m_domiDinoGrenades; }
	DomiDinoGrenades*		setDomiDinoGrenades()		{ return &m_domiDinoGrenades; }

	GAMEINFO_THROW_WEAPON_DOMI_DINO *	FindDomiDinoGrenade( INT32 idx)	{ return m_domiDinoGrenades.Find( idx); }
#endif

	//���� ����ũ ����ź üũ		// �÷��� ���� ��ü�� ����ũ ����ź�� ���谡 ����, �÷��� �˻�ÿ��� �ణ�� ������ �ִ�...
	bool	IsInterEnterSmokeSend() const		{ return m_bInterEnterSmokeSend; }
	void	SetInterEnterSmokeSend(bool val)	{ m_bInterEnterSmokeSend = val; }

#if !defined( I3_NO_PROFILE )
	void CrashAllStageObjects( void);
#endif

	void					CreateBattle( void);		// ��Ʋ ���Խ� ȣ��
	void					InitRoundData( void);	// ���� ���۽� ȣ��
	void					getObjectMission( i3::vector<i3GameObj *>& pObject);
	void					OnObjectMission( bool bEnable);
	
	/** \brief EventSender(i3GameObj, Chara, Weapon)�� ������ ��� Obj ó��
		\note EventSender�� ��ϵǾ� ������ �����Ѵ�. */
	void				ResetEventHandler( i3GameObj * pObj);

private:
	// ����ź ������Ʈ ReceiveBuffer
	GAMEINFO_THROW_WEAPON	m_ThrowWeaponGameInfo[ SLOT_MAX_COUNT][ MAX_THROWWEAPON_SLOP];	// �ܺο��� �̰Ϳ�  ���� �����Ѵٸ� �̷� ���� ���� �ʴ�..(������ ��Ŷ�� �� ������� ������)
	// ��� ������Ʈ ������
	GAMEINFO_OBJECT			m_ObjectGameInfo		[ OBJ_TOTAL_COUNT ];

	// ������ ���� ����
	GAMEINFO_DROPED_WEAPON	m_DropedWeaponGameInfo	[ MAX_DROPEDWEAPON_COUNT ];
	UINT16					m_nDropedWeaponCount;

	// Object Netindex�� UDP�� Index�� search�ϱ� ���� �뵵�� List�� �����մϴ�.
	OBJECT_COUNT_INFO		m_ObjectCountInfo;

#ifdef DOMI_STING_POISON_GRENADE
	DomiDinoGrenades m_domiDinoGrenades;
#endif	

	bool		m_bInterEnterSmokeSend;					// ��ġ�� ����ġ �ʾ� �������� �̵�...����� ���� ���� �÷����̴�..���õ� ��� �ҽ���
};
