/**
 * \file	InBattleDef.h
 *
 * Declares the in battle def class.
 */


#if !defined( __IN_BATTLE_DEF_H__)
#define __IN_BATTLE_DEF_H__

struct KILLING_INFO
{
	// ���� ����
	VAR_LR(UINT16,85)	m_KillType;
	// ���� ����
	VAR_LR(UINT32,86)	m_KillWeaponInfo;	//0x00 F(ExtState) F(EventType) FF(ItemType) FF(ClassType) FF(Number)
	// ���� ų ī��Ʈ
	VAR_LR(INT32,87)	m_KillerAgainCount;
	// ��������� ����ִ� ����
	VAR_LR(UINT8,88)	m_ui8Weapon[ MAX_KILLUSER_COUNT ];
	// �޽��� data
	VAR_LR(UINT32,89)	m_KillBuffer[ MAX_KILLUSER_COUNT ];	// 0x000 F(DefenceSystem) FF(Message) F(DeathType) F(Index)
	// ���� ��ġ
	VAR_LR(REAL32,90)	m_KillPositionBuf[ MAX_KILLUSER_COUNT ][3];
	// ������Ʈ ����� ���� ������Ʈ �ε���
	VAR_LR(INT32,91)	m_KillObjectIdx;

	void	Reset( void);
	void	QueKillingInfo( UINT8 ui8Weapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx );
};







#endif
