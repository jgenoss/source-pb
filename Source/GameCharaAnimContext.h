/**
 * \file	GameCharaAnimContext.h
 *
 * Declares the game chara animation context class.
 */

#if !defined( __GAME_CHARA_ANIM_CONTEXT_H__)
#define __GAME_CHARA_ANIM_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaDef.h"
#include "./Character/GameCharaAnimControlDefine.h"
#include "GameCharaWeaponControlDef.h"

#define CHARA_UPPER_BONE_BODY_COUNT		5
#define CHARA_UPPER_BONE_LEFT_COUNT		23
#define CHARA_UPPER_BONE_RIGHT_COUNT	23

class CGameCharaAnimControl;
class CGameCharaAnimControl1PV;
class CGameCharaAnimControl3PV;

class CGameCharaAnimContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaAnimContext, CGameCharaContextBase);
	
	friend class Avatar;
	friend class CGameCharaBase;
	friend class CGameCharaDino;
	friend class CGameCharaDinoRaptor;
	friend class CGameCharaDinoTRex;
	friend class CGameCharaDinoSting;
	friend class CGameCharaDinoTank;
	friend class CGameCharaDinoAcid;
	friend class CGameCharaDinoElite;

	friend class CGameCharaDinoCCRaptor;
	friend class CGameCharaDinoCCSting;
	friend class CGameCharaDinoCCAcid;
	friend class CGameCharaDinoRaptorMercury;
	friend class CGameCharaDinoStingMars;
	friend class CGameCharaDinoAcidVulcan;

	friend class CGameCharaActionContext;
	friend class CGameCharaBoneContext;
	friend class CGameCharaWeaponContext;

	friend class CGameCharaAnimControl;
	friend class CGameCharaAnimControl3PV;
	
	

protected:
	CGameCharaAnimControl *			m_pCurrentAnimControl = nullptr;
	CGameCharaAnimControl1PV *		m_p1PVAnimControl = nullptr;
	CGameCharaAnimControl3PV *		m_p3PVAnimControl = nullptr;

	ID_LOWER_AI			m_nOldLowerId = ID_LOWER_UNKNOWN;
	ID_UPPER_AI			m_nOldUpperId = ID_UPPER_UNKNOWN;

	ID_UPPER_DUAL_AI	m_nOldUpperDualLeftId = ID_UPPER_DUAL_UNKNOWN;
	ID_UPPER_DUAL_AI	m_nOldUpperDualRightId = ID_UPPER_DUAL_UNKNOWN;

	// Animation index list;
	INT8				m_nAICommonId[ID_COMMON_AI_MAX] = { 0, 0 };

	

	i3AIContext *		m_pAILower = nullptr;			//	��ü AI (Anim)	nullptr�ϼ� �ֽ��ϴ�.
	i3AIContext *		m_pAIUpper = nullptr;			//	��ü AI (Anim)	nullptr�ϼ� �ֽ��ϴ�.
	i3AIContext *		m_pCommon = nullptr;			// ����ü ��ü �ִϸ��̼��� �ִ� context�Դϴ�.(�κ� ĳ������ ceremony�� ���)
	i3AIContext *		m_pAIUpper_Left = nullptr;	// ��ü ���� animation context

	INT8*				m_UpperBoneList = nullptr; //[ CHARA_UPPER_BONE_COUNT ];	// Animation Scaling�� ���� ������Ʈ
	INT8*				m_LowerBoneList = nullptr; //[ CHARA_LOWER_BONE_COUNT ];	// Animation Scaling�� ���� ������Ʈ�� ������ �ֽ��ϴ�.

	REAL32				m_fUpperAniTimer = 0.0f;				// ��ü ��ũ Ÿ��

	// FootStep
	REAL32				m_fLeftStepTime = 0.0f;
	REAL32				m_fRightStepTime = 0.0f;

	INT32				m_nAccessory = 0;				//�׼����� ����� ��ġ�� �Ҹ��ε���( �������� ���� �ʵ��� ��û�޾ұ⶧��)
	REAL32				m_fRunElapseTime = 0.0f;
	REAL32				m_fRunDuration;					//1 [initialize at constructor]

	// Run Jump
	bool				m_bRunJump = false;
	REAL32				m_rJumpAnimScaleValue = 0.0f;
	REAL32				m_rLowerJumpStepTime = 0.0f;
	REAL32				m_rJumpUpperPoseChangeTime = 0.0f;

	REAL32				m_rLowerAnimScale = 1.0f;
	REAL32				m_rUpperAnimScale = 1.0f;

	bool				m_bUpperWalk = false;

	// search�� ���� �ʱ� ���� �̸� ��Ƶд�. 
	UINT32				m_MaxWeaponNumber;				// [initialize at constructor]
	UINT32				m_MaxDualWeaponNumber;			// [initialize at constructor]

public:
	/** \brief Lower�� AI ID�� ��ȯ�մϴ�. */
	INT32				getAILowerID( CHARA_ACTION_BODYLOWER body, ID_LOWER_AI ai);

	#if !defined( AI_MOVE_TO_WEAPON)
	/** \brief Upper AI id�� ��ȯ�մϴ�.
		\param[in] wepaonClass ���� Ŭ����
		\param[in] nWeaponNum ���� ��ȣ
		\param[in] ai ID_UPPER_AI index */ 
	INT32				getAIUpperID( WEAPON_CLASS_TYPE weaponClass, WEAPON::RES_ID nWeaponNum, ID_UPPER_AI ai);

	/** \brief ���� Upper AI id Index�� ��ȯ�մϴ�.
		\param[in] viewType 1/3��Ī flag
		\param[in] nWeaponNum ���� ��ȣ
		\param[in] ai ID_UPPER_DUAL_AI index */
	INT32				getAIUpperDualIDIndex(CHARA_VIEW_TYPE viewType, WEAPON::RES_ID nWeaponNum, ID_UPPER_DUAL_AI ai)
	{
		I3_BOUNDCHK( viewType, CHARA_VIEW_MAX);
		I3ASSERT( nWeaponNum < m_MaxDualWeaponNumber);
		I3_BOUNDCHK( ai, ID_UPPER_DUAL_AI_MAX);

		return (viewType * m_MaxDualWeaponNumber * ID_UPPER_DUAL_AI_MAX) +
				(nWeaponNum * ID_UPPER_DUAL_AI_MAX) + ai;
	}

	/** \brief ���� Upper AI Id�� ��ȯ�մϴ�.
		\param[in] viewType 1/3��Ī flag
		\param[in] nWeaponNum ���� ��ȣ
		\param[in] ai ID_UPPER_DUAL_AI index */
	INT32				getAIUpperDualID( WEAPON::RES_ID nWeaponNum, ID_UPPER_DUAL_AI ai);
	#endif

	/** \brief ���� lower ai id�� ��ȯ�մϴ�. */
	ID_LOWER_AI			getOldLowerID( void);

	/** \brief ���� lower ai id�� �����մϴ�. */
	void				setOldLowerID( ID_LOWER_AI id)			{ I3ASSERT( id < ID_LOWER_AI_MAX); m_nOldLowerId = id; }

	/** \brief ���� upper ai id�� ��ȯ�մϴ�. */
	ID_UPPER_AI			getOldUpperID( void);

	/** \brief ���� upper ai id�� �����մϴ�. */
	void				setOldUpperID( ID_UPPER_AI id)			{ I3ASSERT( id < ID_UPPER_AI_MAX); m_nOldUpperId = id; }

	/** \brief Dual�� ���� Left ai id�� ��ȯ�մϴ�. */
	ID_UPPER_DUAL_AI	getOldUpperDualLeftID( void)			{ return m_nOldUpperDualLeftId; }

	/** \brief Dual�� ���� Left ai id�� �����մϴ�. */
	void				setOldUpperDualLeftID( ID_UPPER_DUAL_AI id)	{ I3ASSERT( id < ID_UPPER_DUAL_AI_MAX); m_nOldUpperDualLeftId = id; }

	/** \brief Dual�� ���� right ai id�� ��ȯ�մϴ�. */
	ID_UPPER_DUAL_AI	getOldUpperDualRightID( void)			{ return m_nOldUpperDualRightId; }

	/** \brief Dual�� ���� right ai id�� �����մϴ�. */
	void				setOldUpperDualRightID( ID_UPPER_DUAL_AI id) { I3ASSERT( id < ID_UPPER_DUAL_AI_MAX); m_nOldUpperDualRightId = id; }

	/** \brief Lower Ai State ���� ���� */
	bool				isAILower( void)						{ return (m_pAILower != nullptr ? true : false);		}

	/** \brief Upper Ai State ���� ���� */
	bool				isAIUpper( void)						{ return (m_pAIUpper != nullptr ? true : false);		}

	/** \brief Lower Ai State�� ��ȯ�մϴ�. */
	i3AIContext *		getAILower( void)						{ return m_pAILower;		}

	/** \brief Lower AI State�� �����մϴ�. */
	void				setAILower( i3AIContext * pCtx)			{ m_pAILower = pCtx; }

	/** \brief Upper AI State�� ��ȯ�մϴ�. */
	i3AIContext *		getAIUpper( void)						{ return m_pAIUpper;		}

	/** \brief Upper AI State�� �����մϴ�. */
	void				setAIUpper( i3AIContext * pCtx)			{ m_pAIUpper = pCtx; }

	/** \brief Dual�� Upper Left AI State�� ��ȯ�մϴ�. */
	i3AIContext *		getAIUpper_Left( void)					{ return m_pAIUpper_Left; }

	/** \brief Dual�� Upper Left AI State�� �����մϴ�. */
	void				setAIUpper_Left( i3AIContext * pCtx)	{
		if( m_pAIUpper_Left != nullptr )m_pAIUpper_Left->SetExclusiveEnable( false, false);
		m_pAIUpper_Left = pCtx;
	}

	/** \brief Upper Bone ID�� ��ȯ�մϴ�. */
	INT8				getUpperBoneID( INT32 idx)				
	{	//���ͽ���
		I3_BOUNDCHK( idx, m_pOwner->_GetUpperBoneCount());
		return m_UpperBoneList[ idx]; 
	}

	/** \brief Lower Bone ID�� ��ȯ�մϴ�. */
	INT8				getLowerBoneID( INT32 idx)				
	{ 
		if(m_pOwner->IsDino())
		{
			I3_BOUNDCHK( idx, CHARA_LOWER_DINO_BONE_COUNT); 
		}
		else
		{
			I3_BOUNDCHK( idx, CHARA_LOWER_BONE_COUNT); 
		}

		return m_LowerBoneList[ idx]; 
	}

	/** \brief �޹� ���� �ð��� ��ȯ�մϴ�. */
	REAL32				getLeftStepTime( void)					{ return m_fLeftStepTime; }

	/** \brief �޹� ���� �ð��� �����մϴ�. */
	void				setLeftStepTime( REAL32 tm)				{ m_fLeftStepTime = tm; }

	/** \brief ������ ���� �ð��� ��ȯ�մϴ�. */
	REAL32				getRightStepTime( void)					{ return m_fRightStepTime; }

	/** \brief �޹� ���� �ð��� �����մϴ�. */
	void				setRightStepTime( REAL32 tm)			{ m_fRightStepTime = tm; }

	/** \brief �ٴ� ���⿡ ���� Lower AI id�� ��ȯ�մϴ�. */
	ID_LOWER_AI			getMoveIdByDir( VEC3D * vDir);

	/** \brief �ȴ� ���⿡ ���� Lower AI id�� ��ȯ�մϴ�. */
	ID_LOWER_AI			getWalkIdByDir( VEC3D * vDir);

	/** \brief �ٱ� Elapse �ð��� ��ȯ�մϴ�. */
	REAL32				getRunElapseTime( void)					{ return m_fRunElapseTime; }

	/** \brief �ٱ� Elapse �ð��� �����մϴ�. */
	void				setRunElapseTime( REAL32 tm)			{ m_fRunElapseTime = tm; }

	/** \brief ��� �Ҹ� �ε����� ��ȯ�մϴ�. */
	INT32				getAccessorySoundIdx( void)				{ return m_nAccessory; }

	/** \brief ��� �Ҹ� �ε����� �����մϴ�. */
	void				setAccessorySoundIdx( INT32 idx)		{ m_nAccessory = idx; }

	/** \brief �ٴ� �ð��� ��ȯ�մϴ�. 
		\note ���⿡ ���� �� ������ �����Դϴ�. */
	REAL32				getRunDuration( void)					{ return m_fRunDuration; }

	/** \brief �ٴ� �ð��� �����մϴ�.
		\note �� �ִϸ��̼��� �� ���̰� �˴ϴ�. */
	void				setRunDuration( REAL32 tm)				{ m_fRunDuration = tm; }

	/** \brief ��ü�� �ִϸ��̼� �ð��� ��ȯ�մϴ�. */
	REAL32				getUpperAniTimer( void)					{ return m_fUpperAniTimer; }

	/** \brief ��ü�� �ִϸ��̼� �ð��� �����մϴ�. */
	void				setUpperAniTimer( REAL32 tm)			{ m_fUpperAniTimer = tm; }

	/** \brief Common AI Context�� �����մϴ�. */
	void				setAICommon( i3AIContext * pCtx)		{ m_pCommon = pCtx; }

	/** \brief Common AI Context�� ��ȯ�մϴ�. */
	i3AIContext *		getAICommon( void)						{ return m_pCommon; }

	/** \brief Common AI Id�� �����մϴ�. */
	void				setAICommonID( INT32 idx, INT32 id)
	{
		I3_BOUNDCHK( idx, ID_COMMON_AI_MAX);
		I3ASSERT( id < 128); // -1�� ������ ��찡 �ֱ� ������. 2011.11.29. ugeun.ji
		m_nAICommonId[ idx] = (INT8) id;
	}

	/** \brief Common AI id�� ��ȯ�մϴ�. */
	INT32				getAICommonID( INT32 idx )				{ I3ASSERT( idx < ID_COMMON_AI_MAX);	return (INT32) m_nAICommonId[ idx]; }

	/** \brief �̵� ���� ���θ� ��ȯ�մϴ�. */
	bool				isEnableRunJump( void)					{ return m_bRunJump; }

	/** \brief �̵� ���� ���θ� �����մϴ�. */
	void				setEnableRunJump( bool bVal)			{ m_bRunJump = bVal; m_rJumpAnimScaleValue = -1.f;}

	/** \brief �̵� ������ ���� �ִϸ��̼� �ӵ��� ��ȯ�մϴ�. */
	REAL32				getRunJumpAnimScale( void)				{ return m_rJumpAnimScaleValue; }

	/** \brief �̵� ������ ���� �ִϸ��̼� �ӵ��� �����մϴ�. */
	void				setRunJumpAnimScale( REAL32 rVal)		{ m_rJumpAnimScaleValue = rVal; }

	/** \brief �̵� ������ ���� �ð��� ��ȯ�մϴ�. */
	REAL32				getRunJumpStepTime( void)				{ return m_rLowerJumpStepTime; }

	/** \brief �̵� ������ ���� �ð��� �����մϴ�. */
	void				setRunJumpStepTime( REAL32 rVal)		{ m_rLowerJumpStepTime = rVal; }

	/** \brief ��ü�� �ȱ� �������� ��ȯ�մϴ�. */
	bool				isUpperWalk(void)						{ return m_bUpperWalk;}

	/** \brief ��ü�� �ȱ� ���� ���θ� �����մϴ�. */
	void				setUpperWalk(bool bWalk)				{ m_bUpperWalk = bWalk;}

	/** \brief ��ü �ִϸ��̼��� ���̸� ��ȯ�մϴ�. */
	inline REAL32		getUpperDuration( void)
	{
		if( !isAIUpper())	return 0.0f;
		return (getAIUpper()->getCurrentAIState() != nullptr ? getAIUpper()->getCurrentAIState()->getDuration() : 0.0f);
	}

	/** \brief ��ü �ִϸ��̼��� �ӵ��� ������ ���̸� ��ȯ�մϴ�. */
	inline REAL32		getUpperActualDuration( void)
	{
		if( !isAIUpper())	return 0.0f;
		return (getAIUpper()->getCurrentAIState() != nullptr ? getAIUpper()->getCurrentAIState()->getActualDuration() : 0.0f);			
	}

	/** \brief ��ü�� ���� �ִϸ��̼� �ð��� ��ȯ�մϴ�. */
	inline REAL32		getUpperLocalTime( void)
	{
		if( !isAIUpper())	return 0.0f;
		return getAIUpper()->getLocalTime();
	}

	/** \brief ��ü�� �ִϸ��̼� ���̸� ��ȯ�մϴ�. */
	inline REAL32		getLowerDuration( void)
	{
		if( !isAILower())	return 0.0f;
		return (m_pAILower->getCurrentAIState() != nullptr ? m_pAILower->getCurrentAIState()->getDuration() : 0.0f);
	}

	/** \brief ��ü �ִϸ��̼��� �ӵ��� ������ ���̸� ��ȯ�մϴ�. */
	inline REAL32		getLowerActualDuration( void)
	{
		if( !isAILower())	return 0.0f;
		return (m_pAILower->getCurrentAIState() != nullptr ? m_pAILower->getCurrentAIState()->getActualDuration() : 0.0f);
	}

	/** \brief ��ü�� ���� �ִϸ��̼� �ð��� ��ȯ�մϴ�. */
	inline REAL32		getLowerLocalTime( void)
	{
		if( !isAILower())	return 0.0f;
		return m_pAILower->getLocalTime();
	}

public:
	CGameCharaAnimContext();
	virtual ~CGameCharaAnimContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;

	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV() override;
	virtual void	OnChange3PV() override;

	virtual void	OnChangeLocal() override;
	virtual void	OnChangeNetwork() override;
	virtual void	OnChangeNPC() override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;
	
	/** \brief Bone List�� �ۼ��մϴ�.
		\note Bone�� ���� ó���� �� �� �ʿ���. */
	virtual void	BindBoneList();

	/** \brief ��ü Animation�� ���� SyncTime�� ��ȯ�մϴ�. */
	REAL32			GetUpperSyncTime( void);

	/** \brief ��ü Animation�� ���� SyncTime�� ��ȯ�մϴ�. */
	REAL32			GetLowerSyncTime( void);

	/** \brief ��ü�� ���� Time Scale�� ��ȯ�մϴ�. */
	REAL32			getLowerTimeScale( void)								{ return m_rLowerAnimScale; }

	/** \brief ��ü�� ���� time Scale�� �����մϴ�
		\note ��ü Bone�� ���ؼ� ���� �� Update�� �մϴ�. */
	void			SetLowerTimeScale( REAL32 scale);

	/** \brief ��ü�� ���� Time Scale�� ��ȯ�մϴ�. */
	REAL32			getUpperTimeScale( void)								{ return m_rUpperAnimScale; }

	/** \brief ��ü�� ���� Time Scale�� �����մϴ�.
		\note ��ü Bone�� ���ؼ� ���� �� Update�� �մϴ�. */
	void			SetUpperTimeScale( REAL32 scale);

	/** \brief ��ü�� ���� Bone�� ���ؼ� time scale�� �����մϴ�. */
	void			SetUpperLeftTimeScale( REAL32 scale);

	/** \brief ��ü�� ���� bone�� ���ؼ� time scale�� �����մϴ�. */
	void			SetUpperRightTimeScale( REAL32 scale);

	/** \brief 1,3��Ī Animation�� �����մϴ�. */
	void			ChangeAnimSet( CHARA_VIEW_TYPE type, bool bAnimSync);

	/** \brief ��ü�� ���� Animation View�� �����մϴ�. */
	void			ChangeAnimSetLowerSync( CHARA_VIEW_TYPE type);

	// �ִϸ��̼��� ������ ������ ���� �뵵
	void			SetAIStateImmediately( REAL32 tm);

	/** \brief Animation State�� �����ϰ� Animation�� �����մϴ�. */
	void			ChangeAnimationContext( void);

	// revision 36076 C4 ��ġ�� ���� ���� �� �� �߻� �����ϰ� C4 ��ġ ���尡 ��� play�Ǵ� ���� ����
	/** \brief ���� AI�� �����մϴ�.*/
	// �������� ���� AI�� ����� ���Ǵ� �ڵ�� ����. 
	// �����ʿ��� �Լ��� ���� ���� �ҵ������� �ϴ��� Ŭ�� �߰�
	// ������ �ؾߵɶ��� ����ϱ� ��õ
	// AI�� �ִ� ���� ����� ���
	// tm �� Finish�� �� �ʿ��� default�� 0.0f
	void			FinishCurrentAIUpper( REAL32 tm = 0.f);

	/************************************************************************************/
	/*							Animation Callback functions							*/
	/************************************************************************************/
	/** \brief Idle ������ */
	void			CallbackAnim_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ������ */
	void			CallbackAnim_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime);
	
	/** \brief ���� ���� */
	void			CallbackAnim_AttackFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ���� �غ� ������ ���� */
	void			CallbackAnim_AttackReadyFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief �̵� ������ */
	void			CallbackAnim_MoveRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ���� ��ü�� */
	void			CallbackAnim_WeaponSwapRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ź�� ������ */
	void			CallbackAnim_LoadBulletRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ź�� ���� ���� */
	void			CallbackAnim_LoadBulletFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ��Ʈ�׼��� */
	void			CallbackAnim_FireBulletRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ��Ʈ�׼� ���� */
	void			CallbackAnim_FireBulletFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief źâ ������ */
	void			CallbackAnim_LoadMagazineRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief �ٱ��� */
	void			CallbackAnim_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ������ */
	void			CallbackAnim_JumpEndRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ���� ���� */
	void			CallbackAnim_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief �ǰ� ������ */
	void			CallbackAnim_DamageRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ��� ���� ���� */
	void			CallbackAnim_DeathFinish( i3AIContext * pCtx, REAL32 rDeltatime);
	
	/** \brief Extension Ȱ��ȭ �� */
	void			CallbackAnim_ExtensionActivateRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Extension ��Ȱ��ȭ �� */
	void			CallbackAnim_ExtensionDeactivateRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle A �� */
	void			CallbackAnim_Profile_IdleARun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle A ���� */
	void			CallbackAnim_Profile_IdleAFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle B �� */
	void			CallbackAnim_Profile_IdleBRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle B ���� */
	void			CallbackAnim_Profile_IdleBFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle Change �� */
	void			CallbackAnim_Profile_IdleChangeRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle Change ���� */
	void			CallbackAnim_Profile_IdleChangeFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle Stand �� */
	void			CallbackAnim_Profile_IdleStandRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle Stand ���� */
	void			CallbackAnim_Profile_IdleStandFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Proflie Ceremony�� */
	void			CallbackAnim_Profile_CeremonyRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ���� Idle �� */
	void			CallbackAnim_Dual_Left_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime);
	
	/** \brief ���� ������ */
	void			CallbackAnim_Dual_Left_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ���� �ٱ��� */
	void			CallbackAnim_Dual_Left_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ���� ���� ���� */
	void			CallbackAnim_Dual_Left_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ������ Idle �� */
	void			CallbackAnim_Dual_Right_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ������ ������ */
	void			CallbackAnim_Dual_Right_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ������ �ٱ��� */
	void			CallbackAnim_Dual_Right_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ������ ���� ���� */
	void			CallbackAnim_Dual_Right_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ��� ���� ��ü�� */
	void			CallbackAnim_Dual_SwapWeaponRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief ��� ���� źâ ��ü�� */
	void			CallbackAnim_Dual_LoadMagazineRun( i3AIContext * pCtx, REAL32 rDeltatime);

	// ShotGun3
	void			CallbackAnim_LoadMagazine_ReadyRun( i3AIContext * pCtx, REAL32 rDeltatime);

protected:
	/** \brief ��ü animation�� �����Ѵ�. 
		\param[in] aiID ID_UPPER_AI index
		\param[in] startTime ���� �ð�
		\param[in] scale animation scale �� */
	bool			_Play_Anim_Upper( ID_UPPER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief ��ü animation�� �����Ѵ�.
		\param[in] aiID ID_LOWER_AI index
		\param[in] startTime ���� �ð�
		\param[in] scale animation scale �� */
	bool			_Play_Anim_Lower( ID_LOWER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief Dual ��ü animation�� �����Ѵ�.
		\param[in] aiID ID_UPPER_DUAL_AI index
		\param[in] startTime ���� �ð�
		\param[in] scale animation scale �� */
	bool			_Play_Anim_DualUpper( ID_UPPER_DUAL_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief animation�� �����Ѵ�. */
	void			_Stop_Anim( void);

	/** \brief ��ü Pose�� ���ؼ� Start Time�� �����. */
	bool			_Play_Body_Sync( void);

	/** \brief Common Animation�� �Ѵ�.*/
	bool			_Play_Anim_Common( ID_COMMON_AI aiID);

	/** \brief ���⿡ �´� �ִϸ��̼��� �ε��Ѵ�. */
	void			_PreloadAnimation( T_ItemID itemID);
};


#endif

