#pragma once

//SOUND�� ANNOUNCE�� ����Ǿ��ֽ��ϴ�. 
//�߰��� �߰��Ǵ� ��� �� Ȯ�����ּ��� -_-
enum CK_TYPE
{
	CK_TYPE_NONE = -1,

	// 1Page
	CK_CHAINKILLER = 0,
	CK_MASSKILL,
	CK_DOUBLEKILL,
	CK_CHAINSTOPPER,
	CK_TRIPLEKILL,
	CK_HEADSHOT,
	CK_CHAINSLUGGER,
	CK_PIERCINGSHOT,
	CK_CHAINHEADSHOT,
	CK_HELMET_PROTECTION,
	CK_CHALLENGE_CLEAR,
	CK_TARGETBONUS,
	CK_DEATH_BLOW,
	CK_HP_RECOVERY,
	CK_OBJECT_DEFENCE,
	CK_HP_CANDYRECOVERY,
	CK_TOUCHDOWN,
	CK_ACEATTACKER,
	CK_HATTRICK,
	CK_GAMEMAKER,

	// 2Page
	CK_ASSISTKILL,
	CK_DEFAULT_KILL,
	CK_DEFAULT_KILL1,
	CK_DEFAULT_KILL2,
	CK_INTERCEPT,
	CK_HOLDTHELINE,
	CK_MOBILITYUP,
	CK_REVENGE_LOVESHOT,
	CK_REVENGE_HOTKILLER,
	CK_REVENGE_PAYBACK,
	CK_REVENGE_NIGHTMARE,
	CK_REVENGE_AVENGER,
	CK_REVENGE_MEATCHOPPER,
	CK_REVENGE_STALKER,
	CK_SPECIAL_GUNNER,
	CK_ASSASSIN,
	CK_BOMB_SHOT,
	CK_GAS_ATTACK,
	CK_ONE_SHOT_ONE_KILL,
	CK_REVENGE_AWESOME,

	// 3Page
	CK_ASSIST,

	// ������� �߰�

	// Traning
	CK_TRAINING_20_KILL,
	CK_TRAINING_40_KILL,
	CK_TRAINING_60_KILL,
	CK_TRAINING_80_KILL,
	CK_TRAINING_100_KILL,
	CK_TRAINING_120_KILL,
	CK_TRAINING_140_KILL,
	CK_TRAINING_160_KILL,
	CK_TRAINING_180_KILL,
	CK_TRAINING_200_KILL,

	MAX_CK
};

enum CK_HISTORY_TYPE
{
	CK_HISTORY_TYPE_NONE = -1,
	CK_HISTORY_DEFAULT_KILL = 0,

	CK_HISTORY_HEADSHOT,
	CK_HISTORY_HEADKNIFE,
	CK_HISTORY_KNIFE,

	CK_HISTORY_STOPPER,
	CK_HISTORY_STOPPER_HEADSHOT,
	CK_HISTORY_STOPPER_HEADKNIFE,
	CK_HISTORY_STOPPER_KNIFE,

	CK_HISTORY_TOUCHDOWN,	// Ż���带 ���� TouchDown �����丮 Ÿ�� �߰� ( 13.04.24 ���� )

	// ������� HUD0�� ���, ���� HUD3�� ���

	CK_HISTORY_GUNNER,  //������ ����Ͽ� ���� óġ
	CK_HISTORY_ASSASSIN, //���� �Ĺ濡�� ��������� ���� óġ
	CK_HISTORY_BOMB_SHOT, //����ź�̳� ��ź���� ���� óġ
	CK_HISTORY_GAS_ATTACK, //WP����ũ�� ���� óġ
	CK_HISTORY_ONE_SHOT_ONE_KILL, //������ �� �� ù ��°�� �� �Ѿ˷� ���� �ѹ��� óġ, �ֹ��⸸ ����

	CK_HISTORY_HOT_KILLER, //CK_REVENGE_HOTKILLER
	CK_HISTORY_NIGHTMARE, //CK_REVENGE_NIGHTMARE
	CK_HISTORY_MEAT_CHOPPER, //CK_REVENGE_MEATCHOPPER

	// ���⼭���� HUD �ٽ� ���
	CK_HISTORY_ASSIST,		// ��ý�Ʈ

	MAX_CK_HISTORY
};

namespace CK
{
	enum
	{
		MAX_BUFFER = 16	// ��� CK_MAX_�� �ִ�ġ
	};
}

struct SKillMessage
{
	INT32 _aBonusPoint[CK::MAX_BUFFER];
	INT32 _BonusPointCount;
	INT32 _KillMessageType; //�⺻ ų�޽���
	INT32 _TouchDownMessageType; //Key:EscapeMission ��ġ�ٿ� �޽��� Ÿ���� ����ϴ�.

	void Reset(void)
	{
		_TouchDownMessageType = _BonusPointCount = _KillMessageType = -1;
		memset(_aBonusPoint, -1, sizeof(INT32) * CK::MAX_BUFFER );
	}
};

struct SKillHistory
{
	INT32 _KillHistoryType;
	INT32 _KillCount;
	INT32 _KillImgIdx;

	void Reset(void)
	{
		_KillHistoryType = -1;
		_KillCount = -1;
		_KillImgIdx = -1;
	}
};

namespace CK
{
	struct Context
	{
		SKillMessage msg;
		INT32 mark;
		INT32 kill_count;
	};
}

class CChainKills: public i3GameObjBase
{
	I3_CLASS_DEFINE(CChainKills, i3GameObjBase);

	enum //���� �� �մ� ��
	{
		HUD_PROTECTION, //��� �����ؼ� �� ������ ���� �� �ִ�.
		HUD_C5,
		HUD_MAX,
	};

public:
	CChainKills();
	~CChainKills();

public:
	static CChainKills* instance();

public:
	virtual bool Create( i3Node *pParentNode = nullptr, bool bEnable = true ) override;
	virtual void OnUpdate(REAL32 rDeltaSeconds) override;

	void Set_ChainKill_WeaponInfo(CWeaponInfo* p) { m_pKillWeaponInfo = p; }
	void Set_Killer_Victim_Chara(CGameCharaBase* killer, CGameCharaBase* victim) { m_pKillerChara = killer; m_pVictimChara = victim;  }

	void AddChainKills(INT32 nKillMessage,INT32 nPreKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, bool revengeMsg);
	void AddHelmetProtection(void);
	void AddTargetBonus(void);
	void AddChallengeClear(void);
	void AddTrainingKills(INT32 killCount);
	void Reset(void);
	void AddLargeMessage(CK_TYPE ckType);
	//Key:EscapeMission
	void AddTouchDown(INT16 nTouchDown,INT32 nSlotIdx);

	void SetEnableWithoutNotice( bool bFlag );

private:
	//AddChainKills �Լ� ���� ����
	void Set_Kill_Message(SKillMessage & kmBuffer, INT32 nKillMessage, INT32 nPreKillCount);
	INT32 Set_Special_Kill_Message(SKillMessage & kmBuffer, INT32 nWeaponSlot);
	INT32 Set_Revenge_Message(SKillMessage & kmBuffer, INT32 nKillMessage);
	INT32 Set_Kill_Bonus_Point(SKillMessage & kmBuffer, INT32 nKillMessage, INT32 nPreKillCount,
		INT32 nNowKill, INT32 nWeaponSlot, INT32 nHistoryType);
	INT32 Set_Kill_Bonus_Score(INT32 nKillMessage, INT32 nPreKillCount, INT32 nCurKillCount, bool RevengeFlag);

	INT32	Get_History_Type(INT32 nKillMessage, INT32 nWeaponSlot);

	void AddChainKills_Normal_Mode(INT32 nKillMessage,INT32 nPreKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, bool revengeMsg);
	void AddChainKills_Boss_Mission_Mode(INT32 nKillMessage,INT32 nPreKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, bool revengeMsg);

	typedef void (CChainKills::*pf_add_chain_kill)(INT32 nKillMessage,INT32 nPreKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, bool revengeMsg);
	pf_add_chain_kill pf_add_chain_kil_listl[2];

private:
	void _StartShowBonusPoint(INT32 nBonusPointCount);
	void _StartRevengeMessage();

	void _Reset_Sprite_Value();

	void _Set_Sprite_TrainingMessage(const SKillMessage& sm);
	void _Set_Sprite_KillMessage(const SKillMessage& sm);

	void _Set_Sprite_BonusPoint(const SKillMessage& sm);
	void _Set_Sprite_RevengeMessage(const SKillMessage& sm);
	void _Resize_Sprite_KillMessage(REAL32 rDeltaSeconds);

	bool _PushHistory(INT32 ckHistoryType,INT32 nKillCount);
	bool _PushKillMessage(SKillMessage* pKillMessage);
	bool _PopKillMessage(SKillMessage* pKillMessage);

	void _FadeOutKillMessage(REAL32 rDeltaSeconds);
	void _UpdateBonusPoint(REAL32 rDeltaSeconds);
	void _UpdateKillMessage(REAL32 rDeltaSeconds);
	void _UpdateLoveShotMessage(REAL32 rDeltaSeconds);
	void _UpdateHelmetProtection(REAL32 rDeltaSeconds);
	void _UpdateMissionMessage(REAL32 rDeltaSeconds);
	void _UpdateC5(REAL32 rDeltaSeconds);

	//kill mark  ǥ��
	void _SetSpriteKillHistory(INT32 img_idx, INT32 idxSpr,INT32 ckHistoryType);
	void _SetSpriteDinoHistory(INT32 idxSpr);

	void _UpdateKillHistory_Step1(INT32 img_idx, INT32 idx, REAL32 rDeltaSeconds);
	void _UpdateKillHistory_Step2(INT32 img_idx,INT32 idx, REAL32 rDeltaSeconds);
	void _UpdateKillHistory_Step3(INT32 img_idx,INT32 idx, REAL32 rDeltaSeconds);
	void _UpdateKillHistory_Step4(INT32 img_idx,INT32 idx, REAL32 rDeltaSeconds);
	void _UpdateKillHistory_Step5(INT32 img_idx,INT32 idx, REAL32 rDeltaSeconds);
	void _UpdateKillHistory_Step6(INT32 img_idx,INT32 nKillCount, INT32 idx, REAL32 rDeltaSeconds);
	void _UpdateKillHistory(REAL32 rDeltaSeconds);

	void Process_KillMessage_Queue();
	void Push_KillMessage_Queue(const SKillMessage & msg, INT32 mark, INT32 count);

private:
	// KillMessage Queue
	SKillMessage m_qKillMessage[CK::MAX_BUFFER];
	INT32 m_nFrontKillMessage;
	INT32 m_nRearKillMessage;
	INT32 m_nCountKillMessage;

	// KillHistory array
	SKillHistory m_qKillHistory[CK::MAX_BUFFER];
	INT32 m_nStartHistory;
	INT32 m_nCountHistory;

	// Resource
	i3Texture* m_pTexKillMessage;
	i3Texture* m_pTexKillMessage1;
	i3Texture* m_pTexKillMessage2;
	i3Texture* m_pTexTrainingKillMessage;
	i3Texture* m_pTexKillHistory[2];
	i3Texture* m_pTexKillHistoryBG;

	i3Sprite2D* m_pSprKillMessage;
	i3Sprite2D* m_pSprKillMessage1;
	i3Sprite2D* m_pSprKillMessage2;
	i3Sprite2D*	m_pSprTrainingKillMessage;
	i3Sprite2D* m_pSprKillHistory[2];
	i3Sprite2D* m_pSprKillHistoryBG;

	i3TextNodeDX2* m_pTextBonusPoint[CK::MAX_BUFFER];
	i3TextNodeDX2* m_pTextKillCount[CK::MAX_BUFFER];
	i3TextNodeDX2* m_pTextTrainingMessage;
	i3TextNodeDX2* m_pTextRevengeMessage;

	i3Texture * m_pTex;
	i3Sprite2D *	m_pSprite;
	bool m_bUpdateHelmet;
	bool m_bUpdateC5;

	// TimeLine
	REAL32 m_rKillMessageTerm;
	REAL32 m_rBonusPointTerm;
	REAL32 m_rMedalMessageTerm;
	INT32 m_nBonusPointCount;
	INT32 m_nCurrentShowPoint;

	// 
	INT32	m_CkStep[CK::MAX_BUFFER];
	INT32 m_DisplayChainkiller;

	REAL32 m_rMessageScale;

	REAL32 m_fViewerWidthRate;
	REAL32 m_fViewerHeightRate;

	//bool m_difficultyLevelUpEnable;
	bool m_levelUpTextEnable;

	//HUD Kill Bonus Score
private:
	INT32 m_BonusScore;
public:
	INT32 TakeBonusScore(){INT32 BonusScore = m_BonusScore; m_BonusScore = 0; return BonusScore;}
private:
	CWeaponInfo* m_pKillWeaponInfo; //AddChainKills �Լ��� �ʿ��� �����̴�. �Ű����ڷ� �ֱ� ���ؼ� ���� ����..
	CGameCharaBase *m_pKillerChara, *m_pVictimChara;

	std::deque< CK::Context > m_skill_message_que;
	bool m_finish_kill_msg, m_finish_kill_mark; //kill message, kill mark sync�� ���߱� ���� �Լ�
};
