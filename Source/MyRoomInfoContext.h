#pragma once

#include "CSI_Stage.h"

//
//  �κ� ��ġ�� �ֱ⿡ ���� ����ġ �ʾƺ��̹Ƿ�, MyRoomInfo���ַ� �� ���� ����..
//

class MyRoomInfoContext : public i3::shared_ginst<MyRoomInfoContext>
{
public:
	MyRoomInfoContext();
	~MyRoomInfoContext();

	void			InitMyRoomInfo();											// �ε�ȰͿ��� �Է¹޴� �κ��� �־� �Ұ����ϰ� �߰��Ѵ�.

	const char *	getMyRoomPW()	const	{ return m_RoomPasswd; }			// �н������ Ư���ϰԵ� �ΰ��� ���µ� �ϴ�..(�������� �κ���)
	void			ClearMyRoomPW();
	void			SetMyRoomPW(const char* pw, INT32 len);

	void		getMyRoomInfo( ROOM_INFO_BASIC * pInfo ) const;
	void		setMyRoomInfo( const ROOM_INFO_BASIC * pInfo );
	INT32		getMyRoomIndex( void ) const;
	void		setMyRoomIndex( INT32 idx );

	UINT8		GetMyRoomWeaponFlag() const;
	UINT32		GetMyRoomStageID() const;
	UINT8		GetMyRoomInfoFlag() const;
	
	bool		IsAutoTeamChangeableRoomState();		// �ڵ��������밡 ���õ� ���ΰ�.(��������, �ʸ�忡 ����޽��ϴ�)
	bool		IsAutoTeamChanged() const;			    // �ڵ��������밡 ����Ǿ� ���̹ٲ�����ΰ�

	void		setMyRoomScore(const ROOM_NOW_SCORE& pScore)	{ m_RoomNowScore = pScore; }
	const ROOM_NOW_SCORE* getMyRoomScore() const				{ return &m_RoomNowScore; }	

	CSI_STAGE*	getStage( void);
	void		setDummyStage(STAGE_ID stage);			// �̱۸��� ���������� �����Ѵ�. �ش� ���������� ��ϵǸ� getStage���� �ش� �������� ������ �ҷ������ ���� ���.

	void		getMyRoomInfoAdd( ROOM_INFO_ADD * pInfo ) const;
	void		setMyRoomInfoAdd( const ROOM_INFO_ADD * pInfo );

	UINT8		GetMyRoomSubType() const;
	UINT8		GetMyRoomLockObserver() const;

	// Room
	REAL32		GetInvincibleTime();
	REAL32		GetRespawnTime( INT32 userIdx);

	void		SetManualReadyFlag(bool bEnable)				{ m_bManualReadyFlag = bEnable; }
	bool		IsManualReady(void) const						{ return m_bManualReadyFlag; }

	bool		IsTeamBalanceMessageFlag(void) const			{ return m_bTeamBalanceMsgFlag; }
	void		SetTeamBalanceMessageFlag(bool bEnable)			{ m_bTeamBalanceMsgFlag = bEnable; }
	
	bool		IsTeamChangeFlag(void) const					{ return m_bTeamChangeFlag; }
	void		SetTeamChangeFlag(bool bEnable)					{ m_bTeamChangeFlag = bEnable; }

	bool		IsTeamChangeByHostFlag(void) const { return m_bTeamChangeByHostFlag; }
	void		SetTeamChangeByHostFlag(bool bEnable) { m_bTeamChangeByHostFlag = bEnable; }

	bool		IsSoloPlayFlag(void) const						{ return m_bSoloPlay; }
	void		SetSoloPlayFlag(bool bEnable)					{ m_bSoloPlay = bEnable; }

	// ���� ���� Ȯ��
	bool		IsLockedWeaponSlot(UINT32 idxSlot) const;

	bool		JoinRoom(const ROOM_INFO_BASIC* pInfo);

	INT32		GetCountDown( void)	const						{ return m_CountDown; }
	void		SetCountDown( INT32 cnt)						{ m_CountDown = cnt; }

	void		SetQuickJoin(bool enable)						{ m_bQuickJoin = enable; }
	bool		IsQuickJoin() const								{ return m_bQuickJoin;   }

	bool		IsRandomMap(void) const { return ((GetMyRoomInfoFlag() & ROOM_INFO_FLAG_RANDOM_MODE) == ROOM_INFO_FLAG_RANDOM_MODE); }

	//----------------------------------------------------------------------------------//
	//									keyword : Star Player							//
	//----------------------------------------------------------------------------------//
	void		setInReadyRoom(const bool bFlag)				{ m_bInReadyRoom = bFlag; }
	const bool	getInReadyRoom(void)							{ return m_bInReadyRoom; }

	void		PrintStarPlayerEntranceMsg(const INT32 Idx);

private:
	enum {	ENC_SEED_ROOMINFO		= 3, };

	ROOM_INFO_BASIC	m_MyRoomInfo;							//Room				(C->G)//�ڽ��� �� �ִ� ������
	ROOM_INFO_ADD	m_MyRoomInfoAdd;						//Room				(C->G)//�ڽ��� �� �ִ� ������
	char			m_RoomPasswd[NET_ROOM_PW];				//Room				(G->C)//������ ��й�ȣ

	ROOM_NOW_SCORE		m_RoomNowScore;					// RoomNowScore
	
	bool			m_bManualReadyFlag;	// �뿡�� �������� ���� �ߴ��� Ȯ�ο� - ���������� ������ �����ϴ� �Ͱ� �����ϱ� ���� �ʿ�
	bool			m_bTeamBalanceMsgFlag; //
	bool			m_bTeamChangeFlag;  // 
	bool			m_bTeamChangeByHostFlag;	//���忡 ���ؼ� �ϰ� �� ��ü�� �Ǿ��°�

	INT32			m_CountDown;		// ī��Ʈ �ٿ� �� (�濡 ����� �̰��� ������ ī��Ʈ �ٿ��� �Ѵ�)
	bool			m_bQuickJoin;		// ��������
	bool			m_bSoloPlay;		// �ַ��÷��� ���ɿ���

	bool			m_bInReadyRoom;		// ���� �濡 ���� ������ üũ (keyword : Star Player)

	CSI_STAGE		m_EmptyStage;
	CSI_STAGE*		m_DummyStage;
};

