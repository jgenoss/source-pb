#if !defined( __USER_CONTEXT_H)
#define __USER_CONTEXT_H

#include "InBattleDef.h"

class UserContext : public i3::shared_ginst<UserContext>
{
public:
	/** \brief Game �����ð� */
	REAL32 fRemainingBattleTime; // fBattleTime���� ���̹� ����

	/** \brief Battle, Mission���� ������������ üũ�մϴ�.  */
	UINT16 PlayingSlotFlag;
	
	/** \brief ������ ���� �Ǻ���(GM������) �Դϴ�. ( �������� �ƴ� �� �÷��װ� ���� )*/
	UINT16 BattleUserFlag;

	/** \brief ���� ���� �˻�� flag */
	bool Death[SLOT_MAX_COUNT];		
	
	/** \bref ??? */
	bool bBowFireOrder;

	/** \bref �̺�Ʈ ������ ���� ���� �˾� ó�� */
	bool ReceivedEvent;

	/** \brief Only For Host */
	/** \brief Kill ������ ������ ������ ���� ����	*/
	KILLING_INFO KillInfo[SLOT_MAX_COUNT];

public:
	UserContext();
	
	bool		IsPlayingSlot(INT32 idxSlot) const;
	bool		IsBattleSlot(INT32 idxSlot) const;

	void		QueDeathChara( INT32 idx, UINT8 ui8Weapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx);
	void		SendDeathChara();

	void		CreateBattle( void);
	void		InitRoomData( void);

	INT32		GetIndexedRankderIdx(TEAM_TYPE team, INT32 idx);
	INT32		GetTopRankerIdx(TEAM_TYPE team);

protected:
	bool		_IsLiveCharacter(TEAM_TYPE team, INT32 idx);
};

#endif