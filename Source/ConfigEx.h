#ifndef __CONFIG_EX_H__
#define __CONFIG_EX_H__

#include "../../../Common/CommonSource/QuestDef.h"

#define	MAX_PING_LEVEL			5
#define MAX_TWITCH_SERVER		4

struct	EndurancePenalty
{
	INT32		Max = 0;
	INT32		Min = 0;
	REAL32		Damage = 0.0f;
	REAL32		HorzRecoil = 0.0f;
	REAL32		VertRecoil = 0.0f;
	REAL32		Deviation = 0.0f;
};

struct	ConfigGame
{
	// ������ 0�� ���⿡ ����Ǵ� ��ġ
	REAL32	BrokenWeaponDecreaseDamage = 0.0f;		//	���� ������
	REAL32	BrokenWeaponVertRecoil = 0.0f;			//	���� �ݵ�
	REAL32	BrokenWeaponHorzRecoil = 0.0f;			//	���� �ݵ�
	bool	Enable_RPG7 = false;					// RPG7 ���� �ɼ��� Ȱ��ȭ �ϴ���. ( OPTION_LIMIT_RPG7_ENABLE ���� �����.)
	bool	UnlimitedForRPG7 = false;				// RPG7 ���� ��� �� �÷��̰���. Enable_RPG7�� true �Ǿ�� �Ѵ�. (�±� ��û���� ��� �̼�/�ʿ� �����)
	bool	CheckMapData = false;					// ���ӽ� ���� �ε��� �� �ʵ����͸� üũ�ϴ���. UserFileList.dat�� �ݵ�� �־����.
	INT32	EndurPenaltyCount = 0;
	bool	CheckMapCollision = false;
	EndurancePenalty*	pEndurancePenalty = nullptr;

	REAL32	LadderPenaltyRate_CrossHairSize = 0.0f;
	REAL32	LadderPenaltyRate_Deviation = 0.0f;
};


struct	ConfigPhysix
{
	REAL32	HeightDecal = 0.0f;
	REAL32	HeightChara = 0.0f;
	REAL32	HeightEquip = 0.0f;
	REAL32	HeightWeapon = 0.0f;
	REAL32	HeightThrow = 0.0f;
	REAL32	HeightBomb = 0.0f;
};


struct	ConfigClan
{
	i3::rc_wstring	AzitAddressBase;		//	Ŭ�� ����Ʈ URL �ּ�
	bool	Enable = false;
	bool	Enable_Battle = false;
	bool	Enable_Cancellation = false;
	bool	Enable_Search = false;
	bool	Enable_Chat = false;
	INT32	Enable_AdmissionForAge = 0;		// Ŭ�� ���� ���� ����
};


struct	ConfigGUI
{
	INT32	CustomClanMarkContentCount = 0, CustomClanMarkFrameCount = 0;
	INT32	DefaultTextSpaceX = 0, DefaultTextSpaceY = 0;
	REAL32	DefaultTextOffsetX = 0.0f, DefaultTextOffsetY = 0.0f;
	I3COLOR	DurabilityBarColorInSlot = { 0, 0, 0, 0 };
	bool	Enable_LoadProgressBar = false;
	REAL32	JackpotNoticeLifeTime = 0.0f;

	i3::vector<INT32>	pGaugeLevel_ServerList;
	i3::vector<INT32>	pGaugeLevel_ChannelList;

	typedef		pair<SChannelType, INT32>	ServerTypeIndex;
	typedef		vector<ServerTypeIndex>		ServerTypeIndexContainer;
	ServerTypeIndexContainer *	pServerListTab = nullptr;
};


struct	ConfigMisc
{
public:
	struct	WebAnnounce
	{
		bool	IsActive() const	{return ! URL.empty();}
		i3::rc_wstring	URL;
		REAL32	posX = 0.0f, posY = 0.0f;
		REAL32	sizeWidth = 0.0f, sizeHeight = 0.0f;
		INT32	msgBoxOffsetX = 0, msgBoxOffsetY = 0;
	};

	INT32	AdultAge = 0;			// ���� ����
	i3::rc_wstring	JoinAddress;	// �����ϱ� �ּ�
	i3::rc_wstring	FindIDAddress;	// IDã�� �ּ�
	i3::rc_wstring	LobbyBGM;
	REAL32	AnnounceFlipTime = 0.0f;	// �̹��� ���� ��ȯ �ð�
	REAL32	AnnounceWaitingTimeAfterClicked = 0.0f;		//	������ �̹��� ������ Ŭ������ ��� �ٽ� ��ȯ���� ��� �ð�

	WebAnnounce	webAnnounce;
};

struct	ConfigNetwork
{
	// �α��� �������� ��� �ð�. ���ӽ��� �� ����� �Է���
	// �ִ��� �α����� ���� ������ �������Ḧ �ϴ� �ð�.
	REAL32		GameRunWaitTime = 0.0f;
	REAL32		LogInWaitTime_Sec = 0.0f;
	REAL32		RefreshInterval_ServerList = 0.0f;
	REAL32		RefreshInterval_RoomList = 0.0f;
	REAL32		RefreshInterval_RoomInfo = 0.0f;
	INT32		ServerReconnectCount = 0;
	REAL32		PingLevelCheckValue[MAX_PING_LEVEL] = { 0.0f };	//	���÷��̿��� ���̴� �� ���� ������ �����մϴ�.
};

struct	ConfigShop
{	
	struct	Tab {	bool	Enable = false;  	};

	bool	Enable = false;
	bool	Enable_ChargeCash = false;
	bool	ChargeCash_Preparing = false;
	bool	ChargeCash_InGame = false;
	INT32	ChargeCash_InGameSizeWidth = 0;
	INT32	ChargeCash_InGameSizeHeight = 0;
	i3::rc_wstring	ChargeURL;
	i3::rc_wstring	ChargeURL2;
	i3::rc_wstring	ChargeURL_QA;		// QA�� �ּ�
	i3::rc_wstring  ChargeURL2_QA;		// QA�� �ּ�
	Tab		Item;
	Tab		Set;
};

struct	ConfigCommunity
{
	bool		Enable_Note = false;
};

struct ConfigTwitchTV
{
	i3::rc_wstring Server0;
	i3::rc_wstring Server1;
	i3::rc_wstring Server2;
	i3::rc_wstring Server3;
};

/*	ä�ο� ���� ������ �ڰ� ����ü
	ConfigEx.pef�� Logic/ChannelCondition �� ��� �Ǿ� ����

	CONDITION_ENUM	:	���� �ڰ� ����
		KILLDEATH	:	ų����
		RANK	:	���	(enum RANK_COUNT ����)
	ChannelType	:	ä�� ����
	value_min :	�ּ� �ڰ�(�ּ� ����)
	value_max : �ִ� �ڰ�(�ִ� ����)	���� : -1�� ��� �ִ밪�� �̾߱���
*/
struct	ConfigLogic
{
	struct	ChannelConditionInfo
	{
		enum CONDITION_ENUM { INVALID, KILLDEATH, RANK };

		ChannelType		type = CHANNEL_TYPE_NONE;
		CONDITION_ENUM	condition = INVALID;
		INT32			value_min = 0, value_max = -1;
	};

	typedef		vector<ChannelConditionInfo>			ChannelConditionContainer;

	const ChannelConditionInfo *	GetChannelConditionInfo(ChannelType type) const;



	ChannelConditionContainer	ChannelConditions;
};

class CConfigEx :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfigEx, i3ElementBase );

public:
	CConfigEx(void);
	~CConfigEx(void);


private:
	void	Init();


public:
	bool	Load();
	
	const ConfigGame &		GetGame()				{return m_ConfigGame;}
	const ConfigPhysix &	GetPhysix()				{return m_ConfigPhysix;}
	const ConfigClan &		GetClan()				{return m_ConfigClan;}
	const ConfigGUI &		GetGUI()				{return m_ConfigGUI;}
	const ConfigMisc &		GetMisc()				{return m_ConfigMisc;}
	const ConfigNetwork &	GetNetwork()			{return m_ConfigNetwork;}
	const ConfigShop &		GetShop()				{return m_ConfigShop;}
	const ConfigCommunity &	GetCommunity()			{return m_ConfigCommunity;}
	const ConfigLogic &		GetLogic()				{return m_ConfigLogic;}
	const ConfigTwitchTV &  GetTwitchTV()			{return m_ConfigTwitchTV;}
	char *					GetIP()					{return m_strLabel;}

	void SetIP(char *);
	void SetCardSetComboBox( UINT8 ui8ActiveQuestCount, UINT32*	pui8ActiveQuest );

	INT32				GetEndurancePenaltyNum( UINT8 Endurance );
	void				GetEndurancePenaltyName( UINT8 Endurance, i3::string& Label );		// ���� ��Ʈ���� ���� �������ش�
	EndurancePenalty*	GetEndurancePenalty( UINT8 Endurance );

	REAL32				GetLadderPenaltyRate_CrossHairSize() const;
	REAL32				GetLadderPenaltyRate_Deviation() const;


private:
	ConfigGame		m_ConfigGame;
	ConfigPhysix	m_ConfigPhysix;
	ConfigClan		m_ConfigClan;
	ConfigGUI		m_ConfigGUI;
	ConfigMisc		m_ConfigMisc;
	ConfigNetwork 	m_ConfigNetwork;
	ConfigShop		m_ConfigShop;
	ConfigCommunity	m_ConfigCommunity;
	ConfigLogic		m_ConfigLogic;
	ConfigTwitchTV  m_ConfigTwitchTV;
	
	char			m_strLabel[512];
	UINT8			m_uiEncryptKey;		// �ϴ� ��ٸ� ���Ƽ�뵵�λ��..
};


#ifdef USE_I3EXEC
extern "C" __declspec( dllexport) CConfigEx *			g_pConfigEx;
#else
extern CConfigEx* g_pConfigEx;
#endif



#ifdef NC_BUILD
#ifdef USE_I3EXEC
	//�ܺ� ���
	extern "C" void __declspec( dllexport) __ProhibitFile_Install();
	//extern "C" void __declspec( dllexport) __ProhibitFile_GetFileMD5();
	extern "C" void __declspec( dllexport) __ProhibitFile_GetUserFileListMD5();
#else
	void __ProhibitFile_Install();
	void __ProhibitFile_GetUserFileListMD5();
#endif
	//���� ���
	void __ProhibitFile_CheckOut();
	void __ProhibitFile_Unstall();
	void __ProhibitFile_AreYouCheating();
#endif
 
#ifdef USE_I3EXEC
	extern "C" void __declspec( dllexport) _setLogName_(LPCSTR logName);
#else
	void _setLogName_(LPCSTR logName);
#endif

	LPCSTR getLogName();


#endif	//__CONFIG_EX_H__

