/**
 * \file	UI\UIPopupSettingRoom.h
 *
 * Declares the user interface popup setting room class.
 */


#if !defined( __UI_POPUP_SETTINGROOM_H__)
#define __UI_POPUP_SETTINGROOM_H__

#include "UIPopupBase.h"
#include "UIDefine.h"

class UIPopupSelectMode;
class UIPopupSelectStage;

struct LimitAtt
{
	int Primary,Secondary,Melee,Throw, RPG7;

	LimitAtt()	{	Primary = Secondary = Melee = Throw = 1;	RPG7 = 0;	}
};

class UIPopupSettingRoom : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupSettingRoom);
private:
	INT32			m_CurModeSelect;

	bool			m_bRandomMap;

protected:
	STAGE_OPTION_TYPE	m_StageOption;
	UINT8				m_SubType;			

	i3List				m_GameTimeList;
	i3List				m_GameTypeList;

	STAGE_TYPE			m_CreateGameMode;

	UINT8				m_LockWeapon;
	UINT8				m_LockInterEnter;
	UINT8				m_LockObserver;
	UINT8				m_LockTeamBalance;

	INT32				m_nAIEnemyCnt;
	INT32				m_nAILevel;
protected:
	void			_SetDefaultInfo( ROOM_INFO_BASIC * pInfo);
	
	// ��忡 ���� �ɼ� ������ ����
	void			_SetOptionWindow( void);

	/** \brief Stage Map�� �����Ѵ�.
		\note ȣ�� ���� m_nStageID�� �����Ǿ� �մϴ�. */
	void			_SetStageInfo( void);
	void			_SetCondition( void);
	void			_SetLimits( const LimitAtt & limitAtt );

	void			_SetObserver( void);
	void			_SetAIModeCondition(char * szTimes, INT32 curTime);
	void			_SetDominationModeCondition();

	// �˾� â �� �̸��� �����մϴ�. (�κ� - ����� , ����� - �� ����)
	void			_SetWindowName( void);
	
	// �� ����/ųī��Ʈ �������� ���� �߰�
	void			_AddGameType(void* pItem)	{ m_GameTypeList.Add(pItem); }
	UINT8			_GetGameType(INT32 idx)		{ return (UINT8)m_GameTypeList.GetItem(idx); }
	// �� ���ѽð� �������� ���� �߰�
	void			_AddGameTime(void* pItem)	{ m_GameTimeList.Add(pItem); }
	UINT8			_GetGameTime(INT32 idx)		{ return (UINT8)m_GameTimeList.GetItem(idx); }


public:
	UIPopupSettingRoom();
	virtual ~UIPopupSettingRoom();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArv2 = NULL);
	virtual bool	OnKey_Enter( void);

	virtual bool	OnExitStart( void);
	virtual bool	OnCloseButton( void);

	/** \brief Mode�� �����մϴ�. */
	void			ChangeMode( UI_MODE_TYPE mode, UI_LIMIT_MODE limit = ULM_NONE);

	/** \brief Stage Map�� �����մϴ�. */
	void			ChangeStage(STAGE_ID stage);

	void			ChangeTimeLimit( INT32 comboIndex);

	/** \brief Object Condition �����մϴ�. */
	void			ChangeObjectCondition( INT32 comboIndex);

	void			SettingRoomOK( void);
	void			SettingRoomCancel( void);

	void			ClickSelectMode(void);
	virtual void	SelectMode( UI_MODE_TYPE mode, UI_LIMIT_MODE limit = ULM_NONE );

	void			ClickSelectStage( void);
	virtual void	SelectStageOK( STAGE_ID stage);
	
	void SetLockSniper(int lockSecondary, int lockThrow);
	void SetLockRPG7(int lockRpg7);

	//--------------------------------------------------------------------------//
	//								inline function								//
public:
	bool isRPG7Mode();
	bool			isSniperMode( void)					{ return (m_StageOption == STAGE_OPTION_SNIPER); }
	bool			isShotGunMode( void)				{ return (m_StageOption == STAGE_OPTION_SHOTGUN); }
	bool			isHeadHunterMode( void)				{ return (m_StageOption == STAGE_OPTION_HEADHUNTER); }
	bool			isTrainingMode( void)				{ return (m_StageOption == STAGE_OPTION_AI); }
	bool			isKnuckleMode( void)				{ return (m_StageOption == STAGE_OPTION_KNUCKLE); }
	bool			isEscapeMode(void)					{ return (getStageType(m_nStageID) == STAGE_TYPE_ESCAPE);}
	STAGE_ID		getStageID( void)					{ return m_nStageID; }
	void			setStageID(STAGE_ID StageID)		{ m_nStageID = StageID; }

	void			setRandomMap(bool bRandommap)		{m_bRandomMap = bRandommap;}
	bool			getRandomMap()						{ return m_bRandomMap ;}

	void			ChangeALCount(INT32 comboIdx)		{m_nAIEnemyCnt = comboIdx + 1;}
	void			ChangeAILevel(INT32 comboIdx)		{m_nAILevel = comboIdx + 1;}

	/*UIPopupSelectMode *		GetSelectModePopup( void)	{ return m_pSelectModePopup; }
	UIPopupSelectStage *	GetSelectStagePopup( void)	{ return m_pSelectStagePopup; }*/

private:
	void			UpdateRoomInfo( void);//���÷��� ���µɶ� ���� �������� �����´�.

	i3List			m_StageMapList;
	STAGE_ID		m_nStageID;
};

#endif
