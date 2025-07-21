#if !defined( __GAMECHARAMANAGER_H__ )
#define __GAMECHARAMANAGER_H__

#include "GameChara.h"
#include "GameContext.h"
#include "GameThread.h"
#include <queue>
#include "i3Base/itl/queue.h"
#include "i3Base/itl/user_data.h"

enum	CHARA_TASK
{
	CHARA_TASK_NONE = 0,
	CHARA_TASK_CREATE,
	CHARA_TASK_RESPAWN,
	CHARA_TASK_DEATH,
};

enum ETextureKind
{
	ETK_DIFFUSE = 0,
	ETK_NORMAL,
	ETK_REFLECT,
	ETK_SPECULAR,
	ETK_COUNT
};

enum eAVATAR_LOAD_STATE
{
	E_AVATAR_INIT,
	E_AVATAR_LOADING,
	E_AVATAR_LOADED,
};

enum PRELOAD_STATE {
	PRELOAD_STATE_NONE = 0,
	PRELOAD_STATE_LOADING,
	PRELOAD_STATE_DONE
};

struct tagPRELOADANIMATION {
	char			_szPath[MAX_PATH] = { 0 };
	PRELOAD_STATE	_State = PRELOAD_STATE_NONE;

	i3::vector<i3ElementBase*> _OwnList;
};



	//���� �̼� AI
	namespace AICharacter_Util
	{
		enum RESERVE_TYPE
		{
			RESERVE_NONE = -1,
			RESERVE_DOMI_RAPTOR = 0,
			RESERVE_DOMI_RAPTOR_BLUE,
			RESERVE_DOMI_RAPTOR_GREEN,
			RESERVE_DOMI_STING,
			RESERVE_DOMI_ACID,
			RESERVE_DOMI_MUTANT,
			RESERVE_DOMI_ACID_NOT_BOMB,
			RESERVE_DOMI_RAPTOR_MERCURY,
			RESERVE_DOMI_STING_MARS,
			RESERVE_DOMI_ACID_VULCAN,
			RESERVE_MAX
		};

		T_ItemID ReverveTypeToItemID( UINT8 nType);
		INT8 ItemIDToReserveType( T_ItemID nItemID);
		UINT16 GetReserveCount( UINT8 nType);

		CHARA_RES_ID DinoKind2CharaResId( DINO_KIND e);
		UINT32 DinoKind2ItemID( DINO_KIND e);
	};

	namespace aiu = AICharacter_Util;


struct RESERVEDEATHCHARA
{
	CGameCharaBase *	_pChara = nullptr;
	INT32				_nShooterIdx = 0;
	VEC3D *				_vDir = nullptr;
	CHARA_DEATH_TYPE	_DeathType = CHARA_DEATH_UN;
	REAL32				_Time = 0.0f;

	void	Reset()
	{
		_pChara			= nullptr;
		_nShooterIdx	= 0;
		_vDir			= nullptr;
		_DeathType		= CHARA_DEATH_UN;
		_Time			= 0.f;
	}
};

struct EquipResSceneGraph
{
	i3GameResSceneGraph **	_pRes;		// [initialize at constructor]
	UINT32					_nCount;	// [initialize at constructor]

	EquipResSceneGraph(EQUIP::ePART part);
	~EquipResSceneGraph(void);

	void ReleaseGameRes(bool is_remove_all = false);

	void SetEquipRes(INT32 idx, INT32 res_subtype, i3GameResSceneGraph * pRes);
	const i3GameResSceneGraph * GetEquipRes(INT32 team, INT32 res_subtype);

	const UINT32 getCount(void)								{ return _nCount; }
};

class CGameRespawn : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameRespawn, i3ElementBase);

protected:
	i3::vector<i3RespawnObj*>			m_RedTeamList;
	i3::vector<i3RespawnObj*>			m_BlueTeamList;
	i3::vector<i3RespawnObj*>			m_RedTeamNPCList;
	i3::vector<i3RespawnObj*>			m_BlueTeamNPCList;

public:
	CGameRespawn();
	virtual ~CGameRespawn();
    void	Create( const i3::vector<i3RespawnObj*>& RespawnObjList);

public:
	UINT32				GetRespawnCount( CHARACTER_TEAM_TYPE nTeam);
	i3RespawnObj	*	GetRespawnObject( INT32 nTeam, INT32 nidx);
};

class CGameCharaDino;
class cAvatarManager;
class BGLoadThread;
class PlayerController;


///////////////////////////////////////////////////////////////////////////////////////////////
class CGameCharaManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameCharaManager, i3ElementBase );

#ifdef DOMI_NEW_FLOW
	friend class RoundStageJumpCharaMgr;
#endif
	friend struct AVATAR_LOAD_INFO; 
	friend class cAvatarManager;

public:
	enum	eRESSTATE {
		RESSTATE_NONE = 0,
		RESSTATE_LOADCMD,		// �ε带 ��� ���� ����
		RESSTATE_LOADING,		// �ε���
		RESSTATE_LOADED,		// �ε� �Ϸ�
	};

private:
	// CharaInfo & GameChara
	// ĳ���� �����͵� ��Ʈ��ũ �ε����� �������ϴ�.
	i3::vector<CGameCharaBase*>		m_CharaList;

	struct	tagCharaData {
		CGameCharaBase *	_pChara = nullptr;		// chara pointer
		CHARA_TASK			_nCharaTask = CHARA_TASK::CHARA_TASK_NONE;	// task state
		GLOBAL_CHARA_INFO	_CharaInfo;		// info structure
		bool				_TeamChange = false;	// ��ü���� ����
	
		eRESSTATE			_CharaResState = eRESSTATE::RESSTATE_NONE;	// ĳ���� �ε� ���� State
	};

	tagCharaData		m_CharaData[ SLOT_MAX_COUNT];
	tagCharaData		m_AICharaData[ MAX_COUNT_DOMI_DINO];

	////TCP, UDP
	i3::vector<RESERVEDEATHCHARA*>	m_ReserveDeathList;
	RESERVEDEATHCHARA	m_ReserveDeathSlot[ SLOT_MAX_COUNT + MAX_COUNT_DOMI_DINO ];

	// Respawn point
	CGameRespawn		* m_pRespawnInfo = nullptr;

	// StageNode pointer
	i3Stage *			m_pStage = nullptr;
	i3GameNode*			m_pCharaLayer = nullptr;	// ���� ī�޶���� Stage�� ���� LayerNode

	i3::vector<i3GameResChara*>	m_CharaRes;
	i3::vector<i3GameResChara*> m_DinoRes;	// ���� Res�� ���� ����

	CGameCharaBase *	m_pMyChara = nullptr;	// ���� ��忡�� �������� ü������ ��� �����͸� ����д�. ������ ����� �̳༮�� ���

	//���ҽ�
	ClanTexture			m_TexClan;
	AI_BIND_DATA		m_AiBindData[MAX_AI_CHARA_COUNT];

	/*��� ������ AI ����Ʈ*/
	i3::vector<CGameCharaBase*>	m_vecReserve[aiu::RESERVE_MAX];
	/*��û�� AI Ÿ�� ����Ʈ*/
	i3::vector<aiu::RESERVE_TYPE>	m_FrequencyOfUse;

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	i3::vector<bool>	m_ResCheckList[ EQUIP::ePART_COUNT][ TEAM_COUNT];
#endif

private:
	inline tagCharaData *	__getCharaData( INT32 idx, bool bAI)
	{
		if( bAI == false)
		{	// �Ϲ� ĳ����
			if( idx < 0 || idx >= SLOT_MAX_COUNT)	return nullptr;
			return &m_CharaData[ idx];
		}
		else
		{	// AI ĳ����
			if( idx < 0 || idx >= MAX_COUNT_DOMI_DINO) return nullptr;
			return &m_AICharaData[ idx];
		}
	}

public:
	CGameRespawn *		getRespawnPointInfo( void)						{ return m_pRespawnInfo; }
	void				LoadAiBindData(INT32 nLevel, INT32 nIndex, TEAM_TYPE team);		// AI�� Data�� �н��ϴ�.
	AI_BIND_DATA*		getAiBindData(INT32 nIndex)						{ return &m_AiBindData[nIndex];	}

protected:
	i3GameResChara * _LoadCharaRes( const char * pszCHR, const char * pszPath);

	i3Texture * _LoadCommonTexture( const char * pszPath);
	bool		_IsSameTexture( i3Texture * pSrcTex, i3Texture * pDstTex);

	void	_ReverseSetPhysixValue( i3Skeleton * pSkel, i3Bone * pBone, VEC3D * pVec );
	void	_ReverseSetPhysixValueToParent( CGameCharaBase * pChara, CHARA_HIT_PART hitpart, VEC3D * pVec );
	void	_ReverseSetPhysixValueToChild( CGameCharaBase * pChara, CHARA_HIT_PART hitpart, VEC3D * pVec);

	void	_SetPhysixValue( CGameCharaBase * pChara, CHARA_DEATH_TYPE deathType, CHARA_HIT_PART hitpart, VEC3D * pHitDir, WeaponBase * pWeapon );
	void	_SetDeathAnimation( CGameCharaBase * pChara, CHARA_DEATH_TYPE deathType, CHARA_HIT_PART hitpart, VEC3D * pHitDir, WeaponBase * pWeapon);


	void	_ProcessDeathDropWeapon( INT32 iDeathUserIdx);
	void	_ProcessPhysixValueByExplosion( CGameCharaBase * pChara, CHARA_HIT_PART hitpart, VEC3D * pHitDir, WeaponBase * pWeapon, VEC3D * pIKPos);
	void	_ProcessPhysixValueByDefault(  CGameCharaBase * pChara, CHARA_HIT_PART hitpart, VEC3D * pHitDir, WeaponBase * pWeapon, VEC3D * pIKPos);

	void	_RemoveChara( CGameCharaBase * pChara);

	void	_FirstSetChara( INT32 idx, CGameCharaBase * pChara, CHARACTER_TEAM_TYPE team);

public:
	CGameCharaManager();
	virtual ~CGameCharaManager();

	void	PreCreate( void);
	void	PostCreate( void);
	void	Destroy( void );
	void	ReleaseRespawnPoint();

	bool	RemoveCharacter_Impl( CGameCharaBase * pChara);

	/** \brief RespawnPosition�� ���´�.
		\param[out] pOut Position Buffer
		\param[in] team CHARACTER_TEAM_TYPE
		\param[in] idx Respawn Point Index
		\return Theta */
	REAL32	GetRespawnPosition( VEC3D * pOut, CHARACTER_TEAM_TYPE team, INT32 idxChara, INT32 idxObj = -1, bool bIsAI = false);

	const i3::vector<CGameCharaBase*>&	getCharaList() const	{ return m_CharaList; }
	INT32	getCharaCount( void )								{ return (INT32)m_CharaList.size(); }
	CGameCharaBase *	getChara( INT32 idx )	const			{ return m_CharaList[idx]; }

	void				SwapMyCharaToLoaded( INT32 idx);

	

	CGameCharaBase *	getCharaByNetIdx( INT32 idx, bool bAI = false) 
		{
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			return pData->_pChara;

		return nullptr;
	}

	void				setCharaByNetIdx( INT32 idx, CGameCharaBase * pChara, bool bAI = false)
	{
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			pData->_pChara = pChara;
	}

	const GLOBAL_CHARA_INFO * getCharaInfo( INT32 idx, bool bAI = false )
	{ 
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			return &pData->_CharaInfo;
		return nullptr;
	}

	GLOBAL_CHARA_INFO * setCharaInfo( INT32 idx, bool bAI = false )
	{ 
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			return &pData->_CharaInfo;
		return nullptr;
	}

	CHARA_TASK			getCharaTask( INT32 idx, bool bAI = false)
	{
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			return pData->_nCharaTask;
		return CHARA_TASK_NONE;
	}

	void				setCharaTask( INT32 idx, CHARA_TASK task, bool bAI = false)
	{
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			pData->_nCharaTask = task;
	}

	// TCP�� ���� ĳ������ ��ũ�� �����ݴϴ�.(�׽� Update-> StageBattle::processGameContext() )
	void				UpdateReserveDeath( REAL32 fDeltatime );

	void				RemoveAllChara( void );
	void				RemoveAllAIChara(void);

	CGameCharaBase *	_AddChara( INT32 idx, GLOBAL_CHARA_INFO * pInfo);
	CGameCharaBase *	AddChara( INT32 idx, CHARA_RES_ID ResType, bool bRespawnPos = true, CHARA::TYPE charaType = CHARA::TYPE_NA, bool bRecord = true);

	void				RemoveChara( INT32 idx, bool bRecord = true);
	/** \brief ĳ���͸� ��������Ų��.
		\desc ���Լ��� ȣ���Ͽ� Ư�� ��ġ�� ĳ���͸� ���̰� �ϰ� �����̵��� �����Ѵ�.
		\param[in] idx UserSlot Index
		\param[in] bRecord
		\param[in] bFirstRespawn ó�� �������� true�� �־�� �ϴ� ���
		*/
	void				RespawnChara( INT32 idx, bool bRecord = true, bool bFirstRespawn = false, bool bInvincible = true);
	void				MoveRespawnPoint();

	CGameCharaBase *	AddAiChara( INT32 idx, CHARA_RES_ID ResType, bool bRespawnPos = true, CHARA::TYPE charaType = CHARA::TYPE_NA, bool bRecord = true);
	void				RemoveAiChara( INT32 idx, bool bRecord = true);
	void				RespawnCharaForAI( INT32 idx, bool bRecord = true );

	void				CreateReserveAI();
	CGameCharaBase*		CreateReserveAI( T_ItemID nItemID);														//����� AI �ɸ��Ͱ� ���� ��� �ϳ� ����.
	void				DestroyAIIReserveAI();
	void				_DestroyReserveAI( CGameCharaBase* pReserveAI);

	void				AfterLoadAIChara( INT32 idx, AI_BIND_DATA * pAIBind);

	bool				ReturnToReserve( T_ItemID nItemID, CGameCharaBase* pReturn);								//�� �� AI�� Free�� ����(���� �� ���).
	CGameCharaBase*		RequestFreeAI( INT32 nSlotIdx, T_ItemID nItemID);											//�̿� ������ AI�� ��ȯ.
	void				_PrepareAI( CGameCharaBase* pReserveAI, INT32 nSlotIdx, T_ItemID nItemID);				//AI �غ�.

	void				ReserveDump();
	
	void				RemoveCharaAddDinoChara(INT32 idx, CHARA_RES_ID ResType);
	
	void	ChangeToTRex(DINO_ROOM_INFO* pDinoRoomInfo, INT32 iMySlotIdx);
	void	ChangeToDino(DINO_ROOM_INFO* pDinoRoomInfo);

	void	InvisibleCharaAndWarp( INT32 idx );
	void	VisibleCharaAndWarpSavedPos( INT32 idx );
	

	void	DeathChara( CGameCharaBase* pChara, INT32 iKillUserIdx, VEC3D * pHitDir, CHARA_DEATH_TYPE deathType = CHARA_DEATH_A, CHARA_HIT_PART hitpart = CHARA_HIT_BODY1);

	void	DeathHuman( CGameCharaBase* pChara, INT32 iKillUserIdx, VEC3D * pHitDir, CHARA_DEATH_TYPE deathType = CHARA_DEATH_A, CHARA_HIT_PART hitpart = CHARA_HIT_BODY1 );
	void	DeathDino( CGameCharaBase* pChara, INT32 iKillUserIdx, VEC3D * pHitDir, CHARA_DEATH_TYPE deathType = CHARA_DEATH_A, CHARA_HIT_PART hitpart = CHARA_HIT_BODY1);
	
	void	DeathPosition( INT32 idx, bool bRespawn = true );
	void	ReserveDeath( INT32 idx, INT32 nShooter, const VEC3D * pHitDir, CHARA_DEATH_TYPE deathType, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER);
	void	SetCharaWeaponSet( CGameCharaBase * pChara, const ITEM_INFO * pList );
	void	SetCharaWeaponSetWithoutLock( CGameCharaBase * pChara, NET_CHARA_INFO * pNetCharaInfo);
	void	SetCharaEquipSet(CGameCharaBase * pChara, const GLOBAL_CHARA_INFO * pCharaInfo);

	// �������� �ε�ÿ� ȣ������ߵ˴ϴ�. Add�ϱ� ���� ���� ȣ������ߵ˴ϴ�.
	void	EnterStage( i3Stage * pStage );
	void	LeaveStage( void );
#ifdef DOMI_NEW_FLOW
	void	EnterStageRoundJump(i3Stage* pStage);
	void	LeaveStageRoundJump();
#endif

	void	SetCharaLayer(i3GameNode* pNode);

	//	stage instance�� �����ϴ°� �ƴ϶� new_object�� �����ϴ� ĳ�����̹Ƿ� ���߿� �ݵ�� �޸𸮿��� �����ؾ� �մϴ�.
	CGameCharaBase *	CreateViewChara(VIEW_CHARA_INFO * pCharaInfo);	

	// ��忡�� ����
	bool	DetachCharaFromStage( INT32 idx, i3GameNode * pStage );
	INT32	GetCharaIndex(const CGameCharaBase* pChara);


	const i3GameResChara *	QueryCharaResource( CHARA_RES_ID type, bool bCommonLoad = false);

	void	GetEquipPath( char * pszOut, T_ItemID ItemID, TEAM_TYPE team, INT32 res_subtype = 0);
	/** \brief ĳ���� ��� ���ҽ� path�� ��ȯ
		\param[out] pszOut path
		\param[in] part EQUIP::ePART
		\param[in] ResID ID
		\param[in] team TEAM_TYPE*/
	void	GetEquipPath( char * pszOut, EQUIP::ePART part, EQUIP::RES_ID ResID, TEAM_TYPE team, INT32 res_subtype = 0);

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	bool						IsExistEquipResource( EQUIP::ePART part, EQUIP::RES_ID ResID, TEAM_TYPE team);
#endif
	const i3GameResSceneGraph *	QueryEquipResource( T_ItemID ItemID, TEAM_TYPE team, INT32 res_subtype = 0, bool bDirectLoad = true);
	void						CheckDiffuseTexture(T_ItemID ItemID, const i3GameResSceneGraph * pRes);

	const i3GameResSceneGraph * GetEquipResource( EQUIP::ePART part, EQUIP::RES_ID ResID, TEAM_TYPE team, INT32 res_subtype);
	void						SetEquipResource( T_ItemID ItemID, TEAM_TYPE team, INT32 res_subtype, const i3GameResSceneGraph * pRes);
	
	CHARA_RES_ID		GetCharaResType(const i3GameResChara * pCharaRes);

	//	ColorSet ĳ���� ����
	const i3Texture *		QueryCharaClanTex( INT32 idx);
	INT32					GetCharaClanTexCount( void);


	//void	DummyFire( void );

	bool IsAvailableCharaResourceFromManager( T_ItemID nItemID);
	void				SetXrayState(CGameCharaBase* pCharaBase = nullptr, bool bState = true);
	void				OutLineUpdate();

	void	ReleaseEquipResource( void);

	void	MakeCharaTeamInfo( GLOBAL_CHARA_INFO * pCharaInfo, INT32 idx, CHARA::TYPE charaType);

protected:
	// CHARA_INFO�� ������ ä���ݴϴ�.
	void	MakeCharaInfo( GLOBAL_CHARA_INFO * pCharaInfo, INT32 idx, CHARA_RES_ID ResType, CHARA::TYPE charaType );

	void	MakeAiCharaInfoForDomination( GLOBAL_CHARA_INFO * pCharaInfo, INT32 idx, CHARA_RES_ID ResType, CHARA::TYPE charaType);

	// 3D ��忡 ���Դϴ�.
	CGameCharaBase *	AttachCharaToStage( GLOBAL_CHARA_INFO * pCharaInfo, i3Stage * pStage );


	//	Resource
	void	LoadCharaResource( void);
	bool	LoadClanTexture( void);

	void	ReleaseCharaResource( void);
	void	ReleaseClanTexture( void);


	// ������ ����
//private:
//	i3Texture* m_arBeretEmblemTexture[ITEM_RES_ID_BERET_COUNT-1][ETK_COUNT];
//public:
//	i3Texture* GetBeretEmblemTexture(ITEM_RES_ID eItemResID, ETextureKind eTextureKind);

	// ���� ����.

public:
	CGameCharaDinoTRex	*	GetTRex(void);
	bool					IsDino(INT32 idx) { return getCharaInfo( idx)->IsDino(); }


protected :
	void		_RespawnOnObject( CGameCharaBase * pChara, const NET_CHARA_INFO * pNetCharaInfo);

	// ���� �̼��� ����..
public:
	void		RaptorHP_Recovery(INT32 iKillerIdx, INT32 iDeathIdx);
	bool		IsHP_Recovery(INT32 iKillerIdx, INT32 iDeathIdx);
	//Key:EscapeMission
	bool		IsAssist_Kill(INT32 iKillerIdx, INT32 iDeathIdx);
	void		Assist_Kill(INT32 iKillerIdx, INT32 iDeathIdx);
	void		SetVisibleAtPlayingChara();
	bool		IsEscapeNearKill(INT32 iDeathIdx);

private:
	i3SceneIOManager *	m_pIOManager = nullptr;
	Avatar*				m_pLobbyAvatar = nullptr;	//�κ�ĳ���� 

	CGameCharaBase *	m_pTempChara[MAX_COUNT_DOMI_DINO] = { nullptr };	//�ӽ����� ĳ����
																			//::AttachCharaToStage ���� TYPE_DOMI_FULLBODY_AI ���н� SLOT_MAX_COUNT�� �ѱ� ������ ū������ ����.
	cAvatarManager*		m_pAvatarManager = nullptr;	//ĳ���� ��׶��� ������ �Ŵ���

	i3::queue< i3::tuple<INT32, CGameCharaBase *>>	m_pAvatarDeleteQueue;

	REAL32				m_IOTrashResTimer = 0.0f;

public:
	void				OnUpdate(REAL32 tm);

	/** \note in ui view */
	bool				CreateAvatarForView(  AvatarLoadedCallback callback, INT32 idx, GLOBAL_CHARA_INFO* pInfo, CHARA_RES_ID ResType,
		CHARA::TYPE charaType = CHARA::TYPE_NA);

	/** \note in battle
		\param[in] callback AvatarLoadedCallback �Լ�
		\param[in] idx User Slot Index
		\param[in] pNetInfo
		\param[in] charaType
		\param[in] bResetInfo GLOBAL_CHARA_INFO�� �ʱ�ȭ ����(AutoTeamChange������ �ʱ�ȭ ���� �ʴ´�.)
		\param[in] pBindData
		\param[in] finish
		\param[in] bForceAvatar */
	Avatar *			CreateAvatarPlayer( AvatarLoadedCallback callback, INT32 idx,
		NET_CHARA_INFO * pNetInfo, CHARA::TYPE charaType, bool bResetInfo = true, bool bForceAvatar = true); 

	Avatar *			CreateAvatarAI( AvatarLoadedCallback finish, INT32 idx, NET_CHARA_INFO * pNetInfo,
		CHARA::TYPE charaType, AI_BIND_DATA * pBindData = nullptr);
	
	Avatar *			ChangeTeamAvatar( AvatarLoadedCallback callback, INT32 idx, CHARA::TYPE charaType);
	Avatar *			ChangeTeamAvatarAI( AvatarLoadedCallback callback, INT32 idx, CHARA::TYPE charaType, AI_BIND_DATA * pBindData = nullptr);

	Avatar*				GetLobbyAvatar() const { return m_pLobbyAvatar; }
	void				SetLobbyAvatar( Avatar * pAvatar);

	void				SetTempAvatar( INT32 idx, CGameCharaBase * pChara)	{ m_pTempChara[idx] = pChara; }

	cAvatarManager *	GetAvatarManager( void)	{ return m_pAvatarManager; }

	//==============================================================================================//
	//										Background loading										//
	//==============================================================================================//	

private:	
	enum eAVATAR_PHYSX_STATE
	{
		eAPS_NONE = 0,
		eAPS_USE
	};

	struct tagPRELOADEDPHYSICSHAPE
	{
		// persistant
		INT32				_shapeCount = 0;
		i3PhysixShapeSet **	_pShapes = nullptr;

		// volatile
		eAVATAR_PHYSX_STATE	_state = eAPS_NONE;
		CGameCharaBase *	_pOwner = nullptr;
	};

	tagPRELOADEDPHYSICSHAPE	m_PhysixShapeArray[ CHARA::PHYSX_SHAPE_TYPE_COUNT][ SLOT_MAX_COUNT];		// ���� 2����

	char 				m_szShapeBoneName[CHARA::PHYSX_SHAPE_TYPE_COUNT][128][MAX_NAME] = { 0 };

	// PhysX Shapes
	i3::vector<tagPRELOADEDPHYSICSHAPE*> m_vPhysxShapeList_Male;
	i3::vector<tagPRELOADEDPHYSICSHAPE*> m_vPhysxShapeList_Female;

	void		__FindBoneName( CHARA_RES_ID resID);
	void		__PreCreateCharaShapes( CHARA_RES_ID type, INT32 idx);
	tagPRELOADEDPHYSICSHAPE * __FindNotUsedPhysXShape( CHARA::PHYSX_SHAPE_TYPE type);
	tagPRELOADEDPHYSICSHAPE * __FindOwnedPhysXShape( CGameCharaBase * pOwner);
	void		__ReleasePhysXShape( void);

public:
	eRESSTATE	getCharaResState( INT32 idx, bool bAI = false)
	{
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			return pData->_CharaResState;

		return RESSTATE_NONE;
	}

	void		setCharaResState( INT32 idx, eRESSTATE state, bool bAI = false)
	{
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			pData->_CharaResState = state;
	}

	bool	getCharaTeamChange( INT32 idx, bool bAI = false)
	{
		tagCharaData * pData = __getCharaData( idx, bAI);
		if( pData != nullptr)
			return pData->_TeamChange;

		return false;
	}

	void		setCharaAutoTeamChange( INT32 idx, bool bAI = false);

	void		setCharaTeamAutoChange( INT32 idx, bool change, bool bAI = false);

	CHARACTER_TEAM_TYPE	getCharaChangeTeam(INT32 idx, const GLOBAL_CHARA_INFO & info);

	/** \brief Stage ��ȯ���� ȣ�� */
	void		PreStartStage( void);

	/** \brief ĳ���Ϳ� PhysixShape�� �ٿ��ش�.
		\desc �̸� ������ Shape�� ĳ���Ϳ� ��ũ*/
	void		SetPhysXShape( CGameCharaBase * pOwner, CHARA_RES_ID type);

	/** \brief ���� Shape�� ��ȯ�Ѵ�.
		\desc ĳ���Ͱ� ���������� ���� ��� ȣ���.*/
	void		FreePhysXShape( CGameCharaBase * pOwner);

	//==========================================================================================//
	//									Dummy Character
	//==========================================================================================//
private:

	struct tagDUMMYCHARAINFO
	{
		bool			_bKeepOriginal = false;
		T_ItemID		_TItemID[EQUIP::ePART_COUNT] = { 0 };

		struct tagDUMMYRESINFO {
			i3Body *	_pBody = nullptr;		// ���� ���ҽ�
			i3Texture *	_pDiffTex = nullptr;	// macro texture
			i3Texture *	_pNormTex = nullptr;
			i3Texture *	_pSpecTex = nullptr;
			i3Texture *	_pRefTex = nullptr;
			INT32		_UseCount = 0;
		};

		tagDUMMYRESINFO	_resTeamRed;
		tagDUMMYRESINFO	_resTeamBlue;
	};

	i3::vector<tagDUMMYCHARAINFO*> m_vDummyCharaInfo;

	AvatarGen *		m_pAvatarGen = nullptr;

public:
	void		CreateDummyCharaInfo( T_ItemID charaID, bool bKeepResource);
	bool		UpdateLoadDummyChara( void);
	void		ReleaseDummyCharaInfo( T_ItemID charaID);

	i3Body *	GetDummyCharacterBody( T_ItemID charaID, TEAM_TYPE team);

	void		SetDummyCharacterOption( bool bNormalMap, bool bSpecular);

	INT32		GetDummyCharaLoadProg( void);

	bool		IsDummyCharaID( T_ItemID charaID);


	//==========================================================================================//
	//								Animation Background load									//
	//==========================================================================================//
private:
	std::queue<tagPRELOADANIMATION*>	m_PreloadAnimQueue;
	i3FixedMemoryPool *					m_pPreloadAnimPool;			// [initialize at constructor]

protected:
	tagPRELOADANIMATION *	_FindPreloadAnim( const char * pszPath);
	void		_FreePreloadAnimQueue( void);

public:
	void		LoadWeaponAnimation( i3ElementBase * pOwner, const char * pszPath);
	void		UpdatePreloadAnim( void);


	//==========================================================================================//
	//										Equip Resource										//
	//==========================================================================================//
private:
	
	typedef i3::vector<EquipResSceneGraph*> tagEquipRes;
	tagEquipRes m_pEquipRes[EQUIP::ePART_COUNT];
																												// �����ڿ��� ��� ������ ���̱⶧���� ��� �����ͷ� ����.(2012.05.31.����)
	enum eBGLOAD_STATE {
		eBGLOAD_STATE_NONE,
		eBGLOAD_STATE_PUSH,		// ���� ����
		eBGLOAD_STATE_LOADING,
		eBGLOAD_STATE_FLUSH,	// ���� ����
	};

	struct sBGEquipLoadInfo {
		CGameCharaBase * _pOwner = nullptr;
		CHARA_RES_ID	_charaID = CHARA_RES_ID_UNKNOWN;
		T_ItemID		_ItemID = 0;
		TEAM_TYPE		_team = TEAM_NONE;
		INT32			_subtype = 0;
		eBGLOAD_STATE	_state = eBGLOAD_STATE_NONE;
		BGLoadThread *	_pThread = nullptr;
	};
	i3FixedMemoryPool *		m_pBGLoadInfoPool;					// [initialize at constructor]
	std::queue<sBGEquipLoadInfo*>		m_EquipLoadQueue;

	bool		__IsLoadPushed( T_ItemID itemID);

	

	void		__BGLoading( void);


public:
	void		PushLoadEquip( CGameCharaBase * pOwner, CHARA_RES_ID CharaType, T_ItemID itemID);

	//======================================================================================//
	//								Face Resource management								//
	//	Desc : Face modeling resource�� ���ؼ� �����Ѵ�.									//
	//======================================================================================//
	
private:
	i3::vector_map<UINT128,i3SceneGraphInfo*>		m_MaskResourceMap;

	i3SceneGraphInfo *		__FindFaceResource( UINT128 hash);

protected:
	void		CreateFaceResource( void);

public:
	i3SceneGraphInfo *	FindFaceResource( const char * pszName);		
	

	//======================================================================================//
	//								Backup													//
	//	Desc : �ʿ��� ���ҽ����� ������ִ� �ý���
	//		-	��� ĳ������ i3Chara�� ������ ������ �Ǿ� �ִٸ� �����Ͽ� ����� �� �ִ�.	//
	//		-	2012.10.31 Chara AI Index�� ã�� ����� Manager�� �Űܼ� Share�մϴ�.
	//======================================================================================//
private:
	// i3AIContext List ���� ��� AIContext���� ���۸��ϸ� ���� �������� ���Խ� �����մϴ�.
	i3::vector<i3GameNode*>		m_listAIContext[2];		// ���� 2������ ����

	// AI Index
	// ���� ĳ���Ϳ��� �����ϴ����� ù �ε��� �����ϰ� ��� ĳ���Ͱ� �����մϴ�.
	// AI������ �ٸ� ĳ���͸� ����Ѵٸ� ���� ������ �մϴ�.
	UINT32			m_uMaxWeaponCount = 0;
	UINT32			m_uDualWeaponCount = 0;

	INT8 *			m_pAIUpperID = nullptr;
	INT8 *			m_pAIDualUpperID = nullptr;

	INT8			m_nAILowerId[CHARA_BODYLOWER_MAXCOUNT][ID_LOWER_AI_MAX] = { 0, };
	INT8			m_nAICommonId[ID_COMMON_AI_MAX] = { 0 };

	/** \brief AI ID�� �̸� ����� ����!!! */
	void			__CreateUpperAI( i3GameResChara * pRes, INT8 * pUpperAI, INT8 * pUpperDualAI);
	void			__CreateLowerAI( i3GameResChara * pRes);

	void			__PreCreateAIID( void);

	void			__CreateAIContext( CHARA_RES_ID resID, UINT32 count);
	

	INT32			getAIUpperIDIndex( CHARA_VIEW_TYPE viewType, WEAPON_CLASS_TYPE weaponClass, UINT32 uWeaponNum, CHARA_ACTION_BODYUPPER body2, ID_UPPER_AI ai)
	{
		I3_BOUNDCHK( viewType, CHARA_VIEW_MAX);
		I3_BOUNDCHK( weaponClass, WEAPON_CLASS_COUNT);
		I3ASSERT( uWeaponNum < m_uMaxWeaponCount);
		I3_BOUNDCHK( body2, CHARA_BODYUPPER_MAXCOUNT);
		I3_BOUNDCHK( ai, ID_UPPER_AI_MAX);

		return (viewType * WEAPON_CLASS_COUNT * m_uMaxWeaponCount * CHARA_BODYUPPER_MAXCOUNT * ID_UPPER_AI_MAX) +
			(weaponClass * m_uMaxWeaponCount * CHARA_BODYUPPER_MAXCOUNT * ID_UPPER_AI_MAX) +
			(uWeaponNum * CHARA_BODYUPPER_MAXCOUNT * ID_UPPER_AI_MAX) +
			(body2 * ID_UPPER_AI_MAX) +	(ai);
	}

	INT32			getAIUpperDualIDIndex( CHARA_VIEW_TYPE viewType, UINT32 uWeaponNum, ID_UPPER_DUAL_AI ai)
	{
		I3_BOUNDCHK( viewType, CHARA_VIEW_MAX);
		I3ASSERT( uWeaponNum < m_uDualWeaponCount);
		I3_BOUNDCHK( ai, ID_UPPER_DUAL_AI_MAX);

		return (viewType * m_uDualWeaponCount * ID_UPPER_DUAL_AI_MAX) +
				(uWeaponNum * ID_UPPER_DUAL_AI_MAX) + ai;
	}

	INT32			_ResID2AICtxSlot( CHARA_RES_ID resID);

public:
	i3GameNode *	FindFreeAIContext( const char * pszAIName, CHARA_RES_ID resID);
	void			AddAIContext( i3GameNode * pNode, CHARA_RES_ID resID);
	void			UnuseAIContext( i3GameNode * pNode);
	void			ClearAIContextList( void);
	void			DumpAIContextList( CHARA_RES_ID resID);

	INT32			getAIUpperID( CHARA_VIEW_TYPE viewType, WEAPON_CLASS_TYPE weaponClass,
		INT32 nWeaponNum, CHARA_ACTION_BODYUPPER body2, ID_UPPER_AI ai)
	{
		INT32 idx = getAIUpperIDIndex(viewType, weaponClass, nWeaponNum, body2, ai);
		I3ASSERT( idx != -1);

		return static_cast<INT32>( m_pAIUpperID[ idx]);
	}

	INT32			getAIUpperDualID( CHARA_VIEW_TYPE viewType, UINT32 uWeaponNum, ID_UPPER_DUAL_AI ai)
	{
		I3_BOUNDCHK( viewType, CHARA_VIEW_MAX);
		I3ASSERT( uWeaponNum < m_uDualWeaponCount);
		I3_BOUNDCHK( ai, ID_UPPER_DUAL_AI_MAX);

		INT32 idx = getAIUpperDualIDIndex(viewType, uWeaponNum, ai);
		I3ASSERT( idx != -1);

		return static_cast<INT32>( m_pAIDualUpperID[ idx]);
	}

	INT32			getAILowerID( CHARA_ACTION_BODYLOWER body, ID_LOWER_AI ai) const
	{
		if (body >= CHARA_BODYLOWER_MAXCOUNT || ai < 0 || ai >= ID_LOWER_AI_MAX)					// ���������� Ŀ����Ƽ�� I3ASSERT���ô� �������� ��Ÿ�Ϸ�!!
		{
			I3ASSERT(body < CHARA_BODYLOWER_MAXCOUNT);
			I3ASSERT((ai != ID_LOWER_UNKNOWN) && (ai < ID_LOWER_AI_MAX));
			return -1;
		}
		return static_cast<INT32>( m_nAILowerId[ body][ ai]);
	}

	//==========================================================================================//
	//										Domination AI										//
	//==========================================================================================//
public:
	void				SetAiCharaData( INT32 idx, AI_CREATE_DATA* pai_create_data );

public:
	void	LostDevice( void);
	void	Revive( i3RenderContext * pCtx);


	//==========================================================================================//
	//										Shared Controller									//
	//==========================================================================================//
private:
	i3::vector<PlayerController*>	m_PhysXControllerList;

	PlayerController *	__FindFreePhysXController( void);
	INT32	__FindPhysXController( PlayerController * pCtrl); 
public:
	PlayerController *	AllocPhysXController( CGameCharaBase * pOwner);
	void	FreePhysXController( PlayerController * pCtrl);

	// ������ ġƮŰ 
private:
	i3::rc_string m_respawnCheatkey;

public:
	void SetRespawnCheatkey(const char* msg) { m_respawnCheatkey = msg; }
	void ExecuteRespawnCheatkey();


public:
	// �ֿܼ��� ������¿� �Լ��θ� ����մϴ�. �������ٻ�� ����
	bool __ConsoleDump_getCharaData(INT32 idx, bool bAI,
		bool& teamChange, const GLOBAL_CHARA_INFO** rstInfo) 
	{ 
		tagCharaData* tcd = __getCharaData(idx, bAI);
		if(!tcd) return false;
		teamChange = tcd->_TeamChange; 	*rstInfo = &tcd->_CharaInfo;
		return true;
	}

};

#endif	// __GAMECHARAMANAGER_H__
