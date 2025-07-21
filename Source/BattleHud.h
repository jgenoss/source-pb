#ifndef __CHUD_H__
#define __CHUD_H__

#include "GameStringTable.h"

#include "BattleHUD_Define.h"
#include "UI/UIHudDefine.h"

class CChainKills;
class CBattleHUD_MiniMap;
class CHUD_Scope;
class CHud_TrainingScore;
class CHud_WarnningHack;
class CHud_Helicopter;
class CHUD_ObserverHPGauge;
class CHUD_TargetHP;
class CHUD_Revenge;
class CHUD_TextEtc;
class CHUD_CountDown;
class CHUD_Help;

class CHud;
template<> struct is_tinst_manual<CHud> : std::tr1::true_type {}; 

enum INGAME_MESSAGE_TYPE
{
	INGAME_MESSAGE_NONE,
	INGAME_MESSAGE_NOTICE,
	INGAME_MESSAGE_POINT,
	INGAME_MESSAGE_TRAININGPOINT,
	INGAME_MESSAGE_KILL,
	INGAME_MESSAGE_DEFENCT_HEAD,
	INGAME_MESSAGE_OBJECT_DEFENCE,
	INGAME_MESSAGE_KILL_HEADSHOT,
	INGAME_MESSAGE_ASSIST,

	INGAME_MESSAGE_MAX
};

struct InGameMessage
{
	INGAME_MESSAGE_TYPE	mType;			// �޼��� Ÿ��
	UINT32				nIdx;			// ��� ����(���� ���� �������� ������ ��)
	INT32				nBGImageIdx;	// ������ ���� ���� �� ������ Index
	INT32				nSubImageIdx;	// �߰� Sprite�� Index
	REAL32				fLifeTime;		// ���� �ð�
	REAL32				fScale;			// Sprite ������ ��
	i3TextNodeDX2 *		strNotice;		// ų�� �̸�
	i3TextNodeDX2 *		strLeft;		// ų�� �̸�
	i3TextNodeDX2 *		strRight;		// ���� ���� �̸�
	i3Sprite2D *		pSprite;		// ��������Ʈ ������

	InGameMessage() : mType(INGAME_MESSAGE_NONE), nIdx(0), nBGImageIdx(-1), nSubImageIdx(-1), fLifeTime(0.f), fScale(1.f), strNotice(nullptr), strLeft(nullptr), strRight(nullptr), pSprite(nullptr) { }

	void Reset() {
		mType = INGAME_MESSAGE_NONE;
		nIdx = 0;
		nBGImageIdx = -1;
		nSubImageIdx = -1;
		fLifeTime = 0.f;
		fScale = 1.f;
		strNotice = nullptr;
		strLeft = nullptr;
		strRight = nullptr;
		pSprite = nullptr;
	}
};

class CHud : public i3GameObjBase
{
	I3_CLASS_DEFINE( CHud, i3GameObjBase);
public:
	static CHud*	i() { return tinst<CHud>(); }

protected:
	UIBattleFrame *	m_pBattleFrame;
	INT32			m_CurrentUserIdx;	///<< ���� �������� user�� Slot Index (���Ŀ� ųķ������ ������ �� �ֵ��� ������ �����. <- swoongoo)

	i3Sprite2D *	m_pSpriteHit;
	i3Sprite2D *	m_pSpriteDeath;

	i3Sprite2D *	m_pSpriteKillTextBG;
	i3Sprite2D*		m_pSpriteKillMark[HUD_NOTICE_COUNT];

	i3Sprite2D *	m_pSpriteHeadShot;
	i3Sprite2D *	m_pSpriteObjectDefence;
	i3Sprite2D *	m_pSpriteObjDefenctAndHead;

	i3Sprite2D *	m_pSpriteAssistLine;

	//Key:EscapeMission
	i3Sprite2D *	m_pSpriteLostWorldEP5;
	i3Sprite2D *	m_pSpriteGrenadeLauncherCrossHair;	
	i3Sprite2D *	m_pSpriteMission;
	i3Sprite2D *	m_pSpriteBoss;
	i3Sprite2D *	m_pSpriteObjectReply;

	i3Sprite2D *	m_pSpriteUsurpation;
	i3Sprite2D *	m_pSpriteUsurpationTextBG;
	i3TextNodeDX2 *	m_pTextUsurpation[HUD_USURPATION_TEXT_COUNT];

	i3Sprite2D *	m_pGrenadeInd;
	i3Sprite2D *	m_gmObserverCallSignNumSpr;

	i3::vector<WeaponBase*> m_IndicateWeaponList;

	i3::list<InGameMessage*> m_InGameMessageList;

	UINT32			m_nInGameMessageCount;

	i3AttrSet *		m_pHUDRoot;
	i3AttrSet *		m_pScrEffectAttr;
	i3Sprite2D *	m_pScrEffect;
	i3Sprite2D *	m_pScrTex2D;
	i3Texture *		m_pScrTexture;

	i3Texture *		m_pTexture;
	i3Texture *		m_pTextureHUD2;
	i3Texture *		m_pTextureHUD3;

	i3Texture *		m_pTextureHitMark;
	i3Texture *		m_pTextureGrenadeLauncherCrossHair;
	i3Texture *		m_pTextureRankMark;
	i3TextNodeDX2 * m_pLeftText[HUD_NOTICE_COUNT];
	i3TextNodeDX2 * m_pRightText[HUD_NOTICE_COUNT];
	i3TextNodeDX2 * m_pNoticeText[HUD_NOTICE_COUNT];
	
	i3TextNodeDX2 *	m_pTextTarget;
	i3TextNodeDX2 *	m_pTextBomb[HUD_BOMB_TEXT_COUNT];
	i3TextNodeDX2 *	m_pTextMission[HUD_MISSION_TEXT_COUNT];
	i3TextNodeDX2 *	m_pTextRadioChat[ HUD_RADIOCHAT_COUNT ];
	i3TextNodeDX2 *	m_pTextGrenade[MAX_INDICATOR];
	i3TextNodeDX2 *	m_pTextDummy;
	i3TextNodeDX2 * m_pTextObjectReply[HUD_OBJECT_REPLY_COUNT];

	i3TextNodeDX2 * m_pTextCheatkeyWarp;
	i3TextNodeDX2 * m_pTextTabminimapProps;

	i3TextNodeDX2 * m_pTextProfile;

	REAL32			m_fDeltaTimeObjectReply[HUD_OBJECT_REPLY_COUNT];
	REAL32			m_fBulletScale;

	REAL32			m_fViewHalfWidth;	//ȭ���� �߾� ��ǥ, ȭ��ũ���� ��
	REAL32			m_fViewHalfHeight;	//

	REAL32			m_fBulletBarPosX;
	REAL32			m_fBulletBarPosY;

	bool			m_bUpdateNotice;
	bool			m_bOnlyNoticeUpdate;
	bool			m_bUpdateHeadShot;

	REAL32			m_fHeadShotTime;

	INT32			m_idxDamageDirectionOrder;	// ���� ���� ǥ�� Ȱ�� ���� ������
	REAL32			m_fDamageDirectionAngle[HUD_SPRITE_HIT_ANGLE_MAX];	// ���� �� ���� ����
																		// i3Sprite2D::GetRotation()�� ����

	// ObserverMode
	i3Sprite2D*		m_pSpriteObserver;
	i3TextNodeDX2*	m_pTextObserver[HUD_TEXT_OBSERVER_COUNT];
	i3TextNodeDX2*	m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_COUNT];
	VAR_LR(bool, 92) m_bEnableObserverMode;
	VAR_LR(bool, 93) m_bEnableObserverFlyMode;
	
	// GM Pause

	i3Texture *		m_pTexturePause;
	i3Sprite2D*		m_pSpritePause;

	// ���Ĺ̼� ����
	bool			m_bBombMissionEnable;
	bool			m_bOneTimeAreaUpdate;
	bool			m_bDettachBomb;

	// Ż���� ����
	bool			m_bUsurpationEnable;
	bool			m_bStartUsurpationMode;
	bool			m_bFirstUsurpationUI;
	bool			m_bFirstUsurpationHUD;
	bool			m_bOpenUsurpationHUD;
	bool			m_bUsurpationIndicator;
	bool			m_bUserpationMission;
	bool			m_bGetSoundPlay;
	bool			m_bFirstGetSoundPlay;
	REAL32			m_rUsurpationStartTime;

	// Ż���� ����
	bool			m_bFirstHUD;
	bool			m_bOpenFirstHUD;

	// �̼ǰ���		
	REAL32			m_fMissionMessageTime;
	REAL32			m_fMissionNoticeTimer;
	REAL32			m_fMissionNoticeLifeTime;

	// ���� ����
	bool			m_bEnableDeathSprite;
	REAL32			m_fDeathSpriteTime;

	// Radio Chat
	bool			m_bEnableRadioChat;
	bool			m_bEnableRadio1;
	bool			m_bEnableRadio2;
	bool			m_bEnableRadio3;
	REAL32			m_rRadioReusingTimer;

	// ��������
	bool			m_bGameEnd;
	HUD_MODE		m_HudMode;
	REAL32			m_fEtcMessageLocalTime;

	// ���콺 ����
	REAL32			m_fOriginalMouseSensivity;


	// �ǰ�����
	i3Texture*		m_pDamageFilterTexture;
	i3Sprite2D*		m_pDamageFilterSprite;
	REAL32			m_rDamageFilterTime;	

	bool			m_bShowChatNotice;
	INT32			m_nCurrentChatNotice;

	bool			m_bEnabledGrenadeLauncherCrossHair;
	bool			m_bEnableProfile;
	
	// Ư�� �ǰ� �ؽ��� ( ���� ���� ��ũ�ϴ� sprite2d�� �۵��մϴ�. )
	i3::vector<i3Texture*>  m_vSpecialDamageFilterTexture;
	i3::vector<i3Sprite2D*> m_vSpecialDamageFilterSprite;
	i3::vector<REAL32>		m_vSpecialDamageFilterTime;

	// �ǰ�����
	i3Texture*		m_pDieFilterTexture;
	i3Sprite2D*		m_pDieFilterSprite;
	REAL32			m_rDieFilterTime;

	// Ư�� �ǰ� �ؽ��� ( ���� ���� ��ũ�ϴ� sprite2d�� �۵��մϴ�. )
	i3::vector<i3Texture*>  m_vSpecialDieFilterTexture;
	i3::vector<i3Sprite2D*> m_vSpecialDieFilterSprite;
	i3::vector<REAL32>		m_vSpecialDieFilterTime;
	
	// ä��â ��ǥ ����
	bool ChatMoveFlag;
	bool ChatPosSaveFlag;
	REAL32 ChatPosY[3];
public:
	CHud(void);
	virtual ~CHud(void);

	bool	Create( INT32 InitialUserIdx);
	void	OnUpdate( REAL32 rDeltaSeconds );

	virtual bool OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code) override;
	// 
	void	CreateHitMarkHud();
	void	CreateScreenEffectHUD();
	void	CreateHUD();
	void	CreateGrenadeLauncherCrossHair( void);
	void	CreateRideHelicopter(void);

	// �ٰ���
	void	CreateWarnningHack(void);
	void	SetActivateHackWarnning(void);

	// Hud�� ���Ǵ� �������� �ʱ�ȭ�մϴ�.
	void	InitHud();			

	// HUD�� �����۾��� �����մϴ�. ���� ���� �� ���� ���۽� ȣ���.
	void	ResetHud(void);

	void	SetEnableWithoutNotice( bool bFlag, GAME_GUI_STATUS nState, bool bDeath = false);

	i3Texture*	getHud3Texture(void) const				{ return m_pTextureHUD3;				}

	void	SetHudMode( HUD_MODE mod);
	HUD_MODE getHudMode(void)							{ return m_HudMode;						}

	i3AttrSet *	getHUDRoot(void)						{ return m_pHUDRoot;					}

	////////////////////////////////////////////////////////////////
	// Helper
	REAL32	getHalfViewWidth(void)						{ return m_fViewHalfWidth; }
	REAL32	getHalfViewHeight(void)						{ return m_fViewHalfHeight; }

	void	SetDetectedEnemyInfo( INT32 idx);



	////////////////////////////////////////////////////////////////
	// �̼� ����
	////////////////////////////////////////////////////////////////
	void	SetMissionMessageString(const i3::wliteral_range& wMessageRng);
	void	SetMissionNoticeString( const i3::rc_wstring& wstrMessage, REAL32 rLifeTime = 3.f, INT32 nOffsetX = 0, INT32 nOffsetY = 0, bool bAutoLineUp = false);
	void	SetEnableMissionMessage( bool bFlag);
	void	SetEnableMissionNotice( bool bFlag);
	void	SetEnableMissionResult( bool bFlag);
	void	SetMissionRoundResult( INT32 nWinTeamIdx);
	void	SetRedRoundResult(INT32 nPosX, INT32 nPosY);
	void	SetBlueRoundResult(INT32 nPosX, INT32 nPosY);

	void	UpdateMissionMessage( REAL32 rDeltaSeconds);

	////////////////////////////////////////////////////////////////
	// ���� ����
	////////////////////////////////////////////////////////////////
	void	EnableDeathSprite( bool bFlag);
	void	UpdateDeathSprite( REAL32 rDeltaSeconds);

	////////////////////////////////////////////////////////////////
	// ��ź �̼� ����
	////////////////////////////////////////////////////////////////
	void	SetEnableBombMission(bool bFlag);
	void	ProcessBomb( REAL32 rDeltaSeconds);
	void	UpdateBomb( REAL32 rDeltaSeconds);

	////////////////////////////////////////////////////////////////
	// Ż�� ��� ����
	////////////////////////////////////////////////////////////////
	void	SetEnableUsurpationMission(bool bFlag);
	void	SetEnableUsurpationMessage(bool bFlag, const i3::wliteral_range& wTextRng = i3::wliteral_range() );
	void	ResetUsurpationHUD();
	void	ProcessUsurpation( REAL32 rDeltaSeconds);
	void	ProcessUsurpationHUD(REAL32 rDeltaSeconds);
	void	ProcessUsurpationIndicator(bool bFlag);

	void	ConvoyMsgStart(void);
	void	ConvoyMsgGet(void);
	void	ConvoyMsgDrop(void);

	//Ż�� ��� ����
	void	ProcessEscape(REAL32 rDeltaSeconds);
	void	SetFirstHUD(bool bFlag) { m_bFirstHUD = bFlag; }

	// Space ��� ����
	void	ProcessSpace(REAL32 rDeltaSeconds);

	// ġƮŰ
	void	ShowCheatkeyWarpList(const i3::wstring text);
	void	HideCheatkeyWarpList();
	bool	isEnableCheatkeyWarpList() const;

	// ġƮŰ
	void	ShowTabminimapProps(const i3::wstring text);
	void	HideTabminimapProps();
	void	ShowProfile();
	void	ToggleProfile() { m_bEnableProfile = !m_bEnableProfile; }

	////////////////////////////////////////////////////////////////
	// Radar Hud���� / MiniMap
	////////////////////////////////////////////////////////////////
	//void	CalcRadar();
	REAL32	GetRotationAngle2D( VEC3D *pDirection );

	void	SetMoveChatting();
	void	CreateMiniMap(i3Node* pParent);
	void	SetEnableMiniMap( bool bEnable);
	void	IncreaseMiniMapScale( bool bIncrease, REAL32 rAmount = 0.1f);
	void	SetMiniMapFloorIndex( INT32 idx);
	void	SetMiniMapLocationText( const i3::wliteral_range& wLocationRng);
	const i3::wstring&	getMiniMapLocationText(void) const;
	void	SetDestroyObjectUnderAttack( INT32 idx, bool bFlag);

	////////////////////////////////////////////////////////////////
	// Score Hud����
	////////////////////////////////////////////////////////////////
	void	UpdateScore(bool bVisible);
	void	UpdateStageTrainingScore(bool bVisible);

	////////////////////////////////////////////////////////////////
	// Hit Hud����
	////////////////////////////////////////////////////////////////
	void	SetDamageDirection(REAL32 fAngle);	
	void	UpdateDamageDirection(REAL32 fTime);
	
	////////////////////////////////////////////////////////////////
	// Notice(�˸�) Hud����
	////////////////////////////////////////////////////////////////
	i3Sprite2D*	SetKillWeapon(CWeaponInfo * pWeaponInfo, INT32 nIdx, INT32 nSpriteIdx, bool bHP_Up, INT32 nObjIndex = -1, bool bHeadShot = false);

	bool	CheckIndex(INT32 idx) { return idx >= 0 && idx < HUD_NOTICE_COUNT; }
	void	EditCharaNick(i3::wstring& wstrTarget, const i3::wliteral_range& wSourceRng, INT32 LimitWidth);

	void	CreateInGameMessage(const i3::wliteral_range& wTextRng, COLOR* pColor = nullptr, INT32 AssistIdx = -1, INT32 DeathIdx = -1);
	void	CreateInGameMessage(INT32 nObjectIdx, INT32 nPoint);
	void	CreateInGameMessage(INT32 nUserIdx, INT32 nObjectIdx, INT32 nPoint);
	void	CreateInGameMessage(INT32 KillerIdx, INT32 DeathIdx, INT32 AssistIdx, CWeaponInfo * pWeaponInfo, bool bHeadShot, bool bObjDefence = false, INT32 nObjIndex = -1);
	void	CreateInGameMessage(VEC2D* vec, INT32 AssistIdx);

	void	ResetInGameMessage(InGameMessage* message);
	void	UpdateInGameMessageList(REAL32 fTime);				// �ǽð����� �ð��� ���� ����Ʈ�� ���� 
	void	RealignInGameMessage();								// �޼����� ������ ��Ų��
	void	RealignText(i3TextNodeDX2* text, INT32 idx);
	void	InitText(i3TextNodeDX2* text);

	INT32	GetInGameMessageListCount() { return m_InGameMessageList.size(); }
	INT32	GetEmptyTextIndex(i3TextNodeDX2* text[]);
	INT32	GetEmptySpriteIndex(i3Sprite2D* sprite);
	INT32	GetEmptySpriteIndex(i3Sprite2D* sprite[]);
	InGameMessage*	GetEmptyInGameMessage();

	////////////////////////////////////////////////////////////////
	// WeaponS Select Hud����	
	////////////////////////////////////////////////////////////////
	void	UpdateWeaponSelect( REAL32 rDeltaSeconds);
	void	SetEnableWeaponSelect( bool bEnable = true);

	////////////////////////////////////////////////////////////////
	// Chat ����	
	////////////////////////////////////////////////////////////////
	void	UpdateRadioChat( REAL32 fTime );

	////////////////////////////////////////////////////////////////
	// HUD Nick Name Set	
	////////////////////////////////////////////////////////////////
	void	InitHUDNick();
	void	ShowHUDNick( void );
	void	SetCharaNick( const i3::wliteral_range& wNickRng, bool bFriendly, REAL32 fX, REAL32 fY, REAL32 fLength); 
	void	SetCharaNickForGM( const i3::wliteral_range& wNickRng, INT32 slotIdx, bool IsRed, REAL32 fX, REAL32 fY, REAL32 fLength);
	INT32	SetObjectReply( INT32 nType, REAL32 fX, REAL32 fY); 
	void	SetObjectReplyPos( INT32 idx, REAL32 fX, REAL32 fY);
	void	DisableObjectReply( INT32 idx); 	
	
	////////////////////////////////////////////////////////////////
	//	ä��
	////////////////////////////////////////////////////////////////
	bool GetChatEnable(void) const;
	void	SetChatEnable(bool bFlag);
	bool	IsRadioChat( void ) const { return m_bEnableRadioChat; }


	////////////////////////////////////////////////////////////////
	// ���� ����ó��
	////////////////////////////////////////////////////////////////
	void	SetGameEnd(void);

	////////////////////////////////////////////////////////////////
	// ����ź ���
	////////////////////////////////////////////////////////////////
	void	UpdateThrowWeaponIndicator( REAL32 timeStep, i3::vector<WeaponBase*>& List);
	INT32	GetEnabledThrowWeaponList( i3::vector<WeaponBase*>& List, INT32 wpn, REAL32 rDistance);
	void	UpdateIndicator( REAL32 timeStep);

	//	��ü ��ũ�� ���� ó��
	i3Sprite2D * GetScreenEffect( void)									{	return m_pScrEffect;						}
	void	SetScreenEffectColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a)	{	m_pScrEffect->SetColor( 0, r, g, b, a);		}
	void	SetScreenEffectColor( COLOR * pColor)						{	m_pScrEffect->SetColor( 0, pColor);			}
	I3COLOR * GetScreenEffectColor( void)								{	return m_pScrEffect->GetColor( 0);			}
	void	SetScreenEffectEnable( bool bVal = true)					{	m_pScrEffect->SetEnable( bVal);				}	
	bool	GetScreenEffectEnable( void)								{	return m_pScrEffect->GetEnable( 0);			}

	//	��ü ��ũ�� ����� �̹���
	i3Sprite2D * GetScreenTex2D( void)									{	return m_pScrTex2D;						}
	//void	SetScreenTex2DImage( i3Texture * pTex, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
	//																	{	m_pScrTex2D->SetTexture( pTex); 
	//																		m_pScrTex2D->SetTextureCoord( 0, u1, v1, u2, v2);	}
	void	SetScreenTex2DColor( INT32 idx, UINT8 r, UINT8 g, UINT8 b, UINT8 a)	{	m_pScrTex2D->SetColor( idx, r, g, b, a);	}
	void	SetScreenTex2DColor( INT32 idx, COLOR * pColor)				{	m_pScrTex2D->SetColor( idx, pColor);	}
	void	SetScreenTex2DEnable( bool bVal = true)						{	m_pScrTex2D->SetEnable( bVal);			}
	void	SetScreenTex2DEnable( INT32 idx, bool bVal)					{	m_pScrTex2D->SetEnable( idx, bVal);		}
	bool	GetScreenTex2DEnable( void)									{	m_pScrTex2D->GetEnable( 0);				}
	void	SetCurrentScreenImage( INT32 idx, UINT8 r, UINT8 g, UINT8 b, UINT8 a);
	COLOR * GetCurrentScreenImageColor( INT32 idx)						{   return m_pScrTex2D->GetColor( idx);		}

	void	SetEnableObserver(bool bEnable);
	void	SetEnableObserverFly(bool bEnable);

	void	SetObservedNick(const i3::rc_wstring& wstrNick, bool bIsRed);

	// ChainKills
	void	AddChainKills(INT32 nKillMessage,INT32 nKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, bool revengeMsg = false);	
	void	AddHelmetProtection(void);
	void	AddChallengeClear(void);
	void	AddTargetBonus( INT32 exp = 10);

	void	AddWeapon_BuildUp(void);

	void	AddHP_Recovery(void);
	void	AddDeathBlow(void);
//Key:EscapeMission
	void	AddTouchDown(INT16 nTouchDown,INT32 nSlotIdx);
	void	AddAssist_Kill(void);
	void	AddEscapeBlock(void);
	void	AddMobilityUp(void);

	
	void	AddTrainingKills(INT32 killCount);
	void	ResetKillMessage(void);
	
	//
	bool	GetEnableChatScroll(void);
	bool	IsEnableGui(void);

	// �ǰ�����
	void	SetDamageFilterInGame( i3Texture* pTex );
	void	ClearDamageFilter();							
	void	OnDamageFilter(INT32 index = -1);								//index -1 �϶� ���� ������ ���� ����մϴ�.
	void	OnUpdateDamageFilter(REAL32 rDeltaSeconds);
	
	void	InsertSpecialDamageFilter( INT32 index );
	void	OnUpdateSpecialDamageFilter(REAL32 rDeltaSeconds);

	// Die ����
	void	SetDieFilterInGame(i3Texture* pTex);
	void	ClearDieFilter(void);
	void	OnDieFilter(INT32 index = -1);								//index -1 �϶� ���� ������ ���� ����մϴ�.
	void	OnUpdateDieFilter(REAL32 rDeltaSeconds);

	void	InsertSpecialDieFilter(INT32 index);
	void	OnUpdateSpecialDieFilter(REAL32 rDeltaSeconds);

	void	InsertScopeHudTexture(INT32 index);

	i3Sprite2D *	GetPauseMessageSprite()								{ return m_pSpritePause;}
	void			ClearPauseMessage();
	void			OnPauseMessage();

	INT32	getChatOffsetPos(void);
	static void Bubble( SCORE_ARRAY *pArr, INT32 num);


	////////////////////////////////////////////////////////////////
	// ��ź �߻�� HUD
	void	SetEnableGrenadeLauncherCrossHair( bool bVisible);
	bool	IsEnabledGrenadeLauncherCorssHair( void)					{ return m_bEnabledGrenadeLauncherCrossHair; }


protected:
	//Breakdwon UI UIdate
	void	_UpdateDetectedEnemySprite( REAL32 tm);

private:
	void _InitPing(i3Texture* pTexture);
	void _ShowPing(INT32 nSlot,INT32 nPingLevel);	
	void _ShowHostMark(INT32 nSlot);
	void _ShowClanMark(INT32 nSlot, SCORE_ARRAY * pScoreInfo);
	void _HideClanMark(void);
	void _HidePingAll(void);
	void _ShowRankMark(INT32 nSlot,INT32 nRank);
	void _HideRankMarkAll(void);
	bool _UpdateAttackedEffect(REAL32 rDeltaSeconds);	// Damaged Fx
	bool _UpdateGrazedEffect(REAL32 rDeltaSeconds);		// Warning Fx
	void _ClearGrazedEffect(void);						// Clear Warning Fx
	bool _IsAttackedEffectEnabled(void);				// IsDamagedFx

	void CreateGMObserverCallSignNumSprite();

	// ���� ���� �ȳ� ���� ���
	void	_ProcessChatNotice( void);
	
	// Ż���� ����
	bool _CalcProjectionPos( REAL32 * fOutX, REAL32 * fOutY, VEC3D * vIn);

	// TextName ����.
protected:
	i3TextNodeDX2 *	m_pTextName[HUD_NAME_MAX_COUNT];
	INT32			m_iTextNameSize;

public:
	void			 ChangeTextNameSize(bool bPlus);

	INT32			GetCurrentSlotIndex( void)		{ return m_CurrentUserIdx; }
	CGameCharaBase * GetCurrentChara( void);

private:
	HUD::UITypeList m_UItypelist;

private:
	CBattleHUD_MiniMap* m_pMiniMap;
	CChainKills* m_pChainKills;
	CHud_Helicopter* m_pRideHelicopter;
	CHUD_ObserverHPGauge *	m_pObserverHPGaugeHUD;
	CHUD_TargetHP* m_pPanelTargetHP;
	CHud_WarnningHack* m_pWarnningHack;
	CHUD_Scope* m_pScope;
	CHud_TrainingScore* m_pTrainingScore;
	CHUD_Revenge* m_pRevengeHUD;
	CHUD_TextEtc* m_pTextEtc;
	CHUD_CountDown* m_pCHUD_CountDown;
	CHUD_Help* m_pHelpHUD;

#if legacy_not_use_texture
protected:
	// ���� ���� �ؽ���
	i3Texture *		m_pFrindlyWepMarkTexture;
	// ���� ���� �ؽ���
	i3Texture *		m_pDominationWepMarkTexture;

public:
	i3Texture *	getWepMarkTexture(void)					{ return m_pFrindlyWepMarkTexture;		}
	i3Texture *	getDominationWepMarkTexture(void)		{ return m_pDominationWepMarkTexture;	}
#endif
};

////////////////////////////////////////////////////////////////////////////////////


#endif // __CHUD_H__
