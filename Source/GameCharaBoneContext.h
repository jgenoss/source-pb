/**
 * \file	GameCharaBoneContext.h
 *
 * Declares the game chara bone context class.
 */

#if !defined( __GAME_CHARA_BONE_CONTEXT_H__)
#define __GAME_CHARA_BONE_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaBoneDefine.h"

class CWeaponInfo;
class CGunInfo;
class WeaponBase;

typedef UINT32	CHARA_BONE_FLAG;
#define CHARA_BONE_FLAG_VALID_PIVOT		0x00000001
#define CHARA_BONE_FLAG_SHOVE_ACTIVE	0x00000002
#define CHARA_BONE_FLAG_SHOVE_RETURN	0x00000004
#define CHARA_BONE_FLAG_AIM_ROTATE		0x00000008
#define CHARA_BONE_FLAG_DIFF_BONECOUNT	0x00000010		///< LOD0�� LOD1�� Bone Count�� �ٸ����� ���� ���� (�ٸ��ٸ� ShapeSet�� �������� Update�ؾ��Ѵ�.)


class CGameCharaBoneContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaBoneContext, CGameCharaContextBase);

	friend class CGameCharaInputContext;

	friend class CGameCharaUpdater;
	friend class CGameCharaUpdater1PV;
	friend class CGameCharaUpdater3PV;
	friend class CGameCharaUpdater3PVProfile;

	friend class CGameCharaWeaponControl;
	friend class CGameCharaWeaponControl1PVLocal;
	friend class CGameCharaWeaponControl3PVLocal;
private:
	VAR_LR(REAL32, 26)	m_Theta = 0.0f;			//	ĳ���� �¿� ȸ�� ��
	VAR_LR(REAL32, 27) m_Rho = 0.0f;			//	ĳ���� ���Ʒ� ȸ�� ��

protected:
	CHARA_BONE_FLAG		m_CharaBoneFlag = 0;

	//	Recoil
	CHARAPLAYER_RECOIL	m_RecoilData;
	//	Deviation
	CHARAPLAYER_DEVIATION	m_DeviationData;
	//	Scope
	CHARAPLAYER_SCOPEWAVE	m_ScopeWaveData;
	
	//	Pivot
	REAL32				m_PivotYaw = 0.0f;		//	��ü Animation�� �������� �����ϱ� ���� ����
	REAL32				m_PivotYawTarget = 0.0f;
	REAL32				m_timeYawBlend = 0.0f;
	REAL32				m_timeTargetYawBlend = 0.0f;
	REAL32				m_PivotRoll = 0.0f;
	VEC3D				m_PivotAxis;

	VAR_LR(MATRIX*, 28)	m_mtxPivot = new MATRIX();
	VAR_LR(INT32, 29)		m_iBoneMax = 0;
	VAR_LR(i3Body*,30)		m_pBody = nullptr;

	VAR_LR(INT32*, 31)			m_idxBone = nullptr;//[BONE_MAX];
	VAR_LR(MATRIX*, 32)		m_Bone = nullptr;//[BONE_MAX];


	REAL32				m_velShove, m_acclShove;
	REAL32				m_tShove, m_timeShove;
	QUATERNION			m_quatShoveTarget, m_quatShoveStart, m_quatShove;
	MATRIX				m_ShoveMatrix;

	/////////////////////////////////////////////
	// Head�� ���� �ü� ó��
	REAL32				m_HeadInterpolateTime = 0.0f;
	REAL32				m_HeadTheta = 0.0f;
	REAL32				m_HeadRho = 0.0f;
	REAL32				m_HeadTargetTheta = 0.0f;
	REAL32				m_HeadTargetRho = 0.0f;
	INT32				m_HeadWatchUserIdx = -1;

	////////////////////////////////////////////
	REAL32				m_AimStartTheta = 0.0f;
	REAL32				m_AimTheta = 0.0f;
	REAL32				m_AimTime = 0.0f;
	REAL32				m_AimInterpolate = 0.0f;

	i3BoneRef *			m_pEyeDummyLeft = nullptr;
	i3BoneRef *			m_pEyeDummyRight = nullptr;

	///////////////////////////////////////////////////////////////////////////////////////////
	// internal functions
private:
	/** \brief deviation�� �����մϴ�. */
	void			__SetDeviation(bool bApplyExt, CWeaponInfo* pGunInfo, WeaponBase* pWeapon);

	/** \brief raptor�� rho�� ����մϴ�. */
	void			__CalcRho3PVRaptor( REAL32 rDeltatime);

	/** \brief rho�� ����մϴ�.
		\note ���*/
	void			__CalcRho3PVCommon( REAL32 rDeltatime);

	//////////////////////////////////////////////////////////////////////////////////
	//		internal functions at character
	//		���� �Լ��� �ܺ� class���� ����� �� friend class�� ����ؾ� �մϴ�.
protected:
	/** \brief Bone List�� �����մϴ�. */
	virtual void	__InitBone( void);

	virtual void	_DoUpdateRhoBone1PV( MATRIX * pMod);

	virtual void	_DoUpdateRhoBone3PV( MATRIX * pMod);

	// �߻� ���� �� ó��
	virtual void	_ProcessFire( void);

	/** \brief Update Animation �� Spine�� ������ �۾� */
	void			_CalcSpineBones( void);

	/** \brief 1��Ī������ rho�� ����մϴ�. */
	virtual void	_CalcRho( REAL32 rDeltatime);

	/** \brief 3��Ī������ rho�� ����մϴ�. */
	void			_CalcRho3PV( REAL32 rDeltatime);

	/** \brief Theta�� ����մϴ�. */
	void			_CalcTheta( REAL32 rDeltatime);

	/** \brief Theta�� ����մϴ�.
		\note Network�� character */
	void			_CalcThetaNetwork( REAL32 rDeltatime);

	/** \brief �Ӹ� ���� ���ؼ� Update�մϴ�.
		\note 3��Ī�� �ش�		*/
	void			_CalcHeadBone( REAL32 rDeltatime);

	/** \brief ��ü ������ ���� Update�� �մϴ�.
		\note 3��Ī�� �ش� */
	virtual void	_CalcAimBone( REAL32 rDeltatime);

public:
	void			addBoneFlag( CHARA_BONE_FLAG flag)		{ m_CharaBoneFlag |= flag; }
	void			removeBoneFlag( CHARA_BONE_FLAG flag)	{ m_CharaBoneFlag &= ~flag; }
	bool			isBoneFlag( CHARA_BONE_FLAG flag)		{ return (m_CharaBoneFlag & flag) == flag; }

	/** \brief Recoil ������ ��ȯ�մϴ�. */
	CHARAPLAYER_RECOIL * getRecoilData( void)				{ return &m_RecoilData; }

	/** \brief deviation ������ ��ȯ�մϴ�. */
	CHARAPLAYER_DEVIATION *	getDeviationData( void)			{ return &m_DeviationData; }

	/** \brief scope �ݵ� ������ ��ȯ�մϴ�. */
	CHARAPLAYER_SCOPEWAVE *	getScopeWaveData( void)				{ return &m_ScopeWaveData; }

	/** \brief theta�� �����մϴ�. */
	void			setTheta( REAL32 fTheta )
	{
		m_Theta = fTheta;
	}
	/** \brief theta�� ��ȯ�մϴ�. */
	REAL32			getTheta( void)
	{
		return m_Theta;
	}

	/** \brief rho�� �����մϴ�. */
	void			setRho( REAL32 fRho )
	{
		m_Rho = fRho;
	}
	/** \brief rho�� ��ȯ�մϴ�.
		\note ���� ���� ���Դϴ�. */
	REAL32			getRho( void)
	{
		return m_Rho + I3_DEG2RAD(m_RecoilData.m_RecoilAngle + m_ScopeWaveData.m_ScopeWaveRho);
	}

	/** \brief rho�� ��ȯ�մϴ�. */
	REAL32			getRhoValue( void)
	{
		return m_Rho;
	}


	/** \brief View���� rho�� ��ȯ�մϴ�.
		\note ���⿡ ���� ������ ���Ե˴ϴ�. */
	virtual REAL32	getViewRho( void);

	/** \brief �����ִ� Matrix���� ��ȯ�մϴ�. */
	MATRIX *		getShoveMatrix(void)				{ return &m_ShoveMatrix; }

	/** \brief ĳ���� ������ �����մϴ�. */
	void			SetCharaDir(REAL32 rX, REAL32 rY, REAL32 rZ);

	/** \brief IK bone�� Matrix�� ��ȯ�մϴ�. */
	MATRIX *		getIKMatrix(void)					{ return &m_Bone[ BONE_IK];			}

	/** \brief bone�� Matrix�� ��ȯ�մϴ� */
	MATRIX *		getBoneMatrix( INT32 idx)			{ return &m_Bone[ idx];				}

	/** \brief head bone�� matrix�� ��ȯ�մϴ�*/
	MATRIX *		getHeadMatrix(void)					{ return &m_Bone[ BONE_HEAD];		}	
	
	/** \brief recoil angle�� ��ȯ�մϴ�. */
	REAL32			getRecoilAngle(void)				{ return m_RecoilData.m_RecoilAngle; }

	/** \brief deviation angle�� ��ȯ�մϴ�. */
	REAL32			getDeviationAngle(void)				{ return m_DeviationData.m_DeviationAngle; }

	/** \brief fire deviation�� ��ȯ�մϴ�. */
	REAL32			getFireDeviation(void)				{ return m_DeviationData.m_FireDeviation; }
	
	/** \brief bone list�� Bone Index�� ��ȯ�մϴ�. */
	INT32			getBoneIndex( INT32 idx)			
	{ 
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		if(m_pOwner->IsDino())
		{
			if( m_pOwner->IsRaptorBase()){			I3_BOUNDCHK( idx, RAPTOR_BONE_MAX); }
			else if( m_pOwner->IsRex()){			I3_BOUNDCHK( idx, TREX_BONE_MAX); }
		}
	#ifdef DOMI_DINO
		else if (m_pOwner->IsDomiDino())
		{
			if (m_pOwner->IsDomiRaptorBoneBase())	{ I3_BOUNDCHK(idx, DOMI_RAPTOR_BONE_MAX);	}
			else if (m_pOwner->IsDomiMutantRex())	{ I3_BOUNDCHK(idx, DOMI_MUTANTREX_BONE_MAX);}
		}
	#endif	// DOMI_DINO
		else
		{
			I3_BOUNDCHK( idx, BONE_MAX); 
		}
#endif	// I3_DEBUG
		
		return m_idxBone[ idx]; 
	}

	/** \brief Yaw blend ��ǥ �ð��� �����մϴ�. */
	void			setTimeTargetYawBlend( REAL32 tm)	{ m_timeTargetYawBlend = tm; }

	/** \brief Yaw blend �ð��� �����մϴ�. */
	void			setTimeYawBlend( REAL32 tm)			{ m_timeYawBlend = tm; }

	/** \brief pivot matrix�� ��ȯ�մϴ�. */
	MATRIX *		getPivotMatrix( void)				{ return m_mtxPivot; }

	/** \brief pivot matrix�� ���� spine ��� ���θ� �����մϴ�.
		\note 3��Ī�� ��� pivot�� ���� ������ ����Ͽ� ������ �ٿ���� �մϴ�. */
	void			setValidPivot( bool bVal)			{ if(bVal) addBoneFlag( CHARA_BONE_FLAG_VALID_PIVOT); else removeBoneFlag(CHARA_BONE_FLAG_VALID_PIVOT); }

	/** \brief pivot yaw�� ��ǥ���� �����մϴ� .*/
	void			setPivotYawTarget( REAL32 rVal)		{ m_PivotYawTarget = rVal; }

	/** \brief head bone�� �ֽ��ϴ� user index�� �����մϴ�.
		\note -1�̸� �ֽ��ϴ� ������ ����. */
	void			setHeadWatchUserIdx( INT32 idx);

	virtual INT32	GetRootBone( void)		{ return getBoneIndex( BONE_ROOT); }
	virtual INT32	GetSpine3Bone( void)	{ return getBoneIndex( BONE_SPINE3); }

	void			setEyeDummyLeft( i3BoneRef * pBoneRef)		{ I3_REF_CHANGE( m_pEyeDummyLeft, pBoneRef); }
	i3BoneRef *		getEyeDummyLeft( void) const				{ return m_pEyeDummyLeft; }
	void			setEyeDummyRight( i3BoneRef * pBoneRef)		{ I3_REF_CHANGE( m_pEyeDummyRight, pBoneRef); }
	i3BoneRef *		getEyeDummyRight( void) const				{ return m_pEyeDummyRight; }

public:
	CGameCharaBoneContext();
	virtual ~CGameCharaBoneContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;

	virtual void	OnReset( void) override;

	virtual void	OnChange1PV( void) override;
	virtual void	OnChange3PV( void) override;

	void			InitBone( void)			{ __InitBone(); }
	
	/** \brief Animation�� Update�մϴ�.
		\note Local ��ǥ���� Update�Դϴ�. */
	void			UpdateAnim( REAL32 rDeltatime);

	/** \brief World ��ǥ�� �� Bone�� Update�մϴ�. */
	void			TransformWorldSpace( REAL32 rDeltatime);

	/** \brief Pivot�� ���ؼ� ������ �մϴ�.
		\note �㸮�� ��鸮�� ���� ����*/
	void			AdjustPivot( REAL32 rDeltatime, MATRIX * pInvMtx);
	
	/** \brief ���� ��..
		\note ������ */
	void			Shove( VEC3D * pDir, REAL32 force);

	/** \brief Deviation�� ���� �㸮 ���� ����մϴ�. */
	virtual void	CalcDeviation( REAL32 rDeltatime);

	/** \brief Recoil�� ���� �㸮 ���� ����մϴ�. */
	void			CalcRecoil( REAL32 rDeltatime);

	/** \brief Scope ��鸲�� ���� ����� �մϴ�. */
	void			CalcScopeWave( REAL32 rDeltatime);

	// ���� �ݵ� ȸ��(���� �� ���� �ڸ��� ����)�� ����Ѵ�.
	void			cancelRecoilHorz(void)						{ m_RecoilData.cancelRecoilHorz(); }

	void			SetUpdateShapeSet(BONES eBone);

};

#endif
