/**
 * \file	GameCharaMoveContext.h
 *
 * Declares the game chara move context class.
 */


#if !defined( __GAME_CHARA_MOVE_CONTEXT_H__)
#define __GAME_CHARA_MOVE_CONTEXT_H__

#include "GameCharaContextBase.h"

typedef UINT32	CHARA_MOVE_FLAG;
#define CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ	0x00000001
#define CHARA_MOVE_FLAG_TOUCHDOWN			0x00000002		//EscapeMission ���ӵ� �浹üũ�� ���� ���ھ� ������ ���� ���� �����մϴ�.
#define CHARA_MOVE_FLAG_IS_GRAVITY			0x00000004		// �߷��� �׽�Ʈ
#define CHARA_MOVE_FLAG_PENETRATEWALL		0x00000010		// �� ��� Ŀ�ǵ�

#define CONTROLLER_HIT_INFO_COUNT 128
#define CONTROLLER_WORLD_HIT_POS_COUNT 64

class CGameCharaBase;
class CGameCharaMoveControl;
struct LadderStateInfo;

class CGameCharaMoveContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaMoveContext, CGameCharaContextBase);

protected:
	CHARA_MOVE_FLAG		m_CharaMoveFlag = 0;

	CGameCharaMoveControl *			m_pCurrentMoveControl = nullptr;
	CGameCharaMoveControl *			m_pMoveControl = nullptr;
	
	VAR_LR(i3PhysixShapeSet*, 38)	m_pCCD_Box = nullptr;		// ����Ʈ ��Ʈ�� ����Ʈ ��Ʈ �ڽ�

	VEC3D			m_MoveDir;			//	���� �̵� ���� (x,y,z�� 0 �̸� �������� �ʴ´�.) local
	VEC3D			m_MoveAt;			// world matrix
	VEC3D			m_MoveRight;		// world matrix

	VAR_LR(VEC3D*,39)	m_OldVel = new VEC3D();
	VAR_LR(VEC3D*,40)	m_vOldPos = new VEC3D();
	
	VAR_LR(VEC3D*,41)	m_Accel = new VEC3D();
	VAR_LR(VEC3D*,42)	m_VelCharacter = new VEC3D();

	VAR_LR(REAL32,43)	m_rVelocity = 0.0f;
	REAL32				m_tmStopingElapsed = 0.0f;
	REAL32				m_tmAccelElapsed = 0.0f;

	VAR_LR(REAL32,44)	m_rOldMaxMoveSpeed = 0.0f;
	VAR_LR(REAL32,45)	m_rMaxMoveSpeed = 0.0f;

	REAL32				m_rElapsedSpeedReduction = 0.0f;
	REAL32				m_rSpeedReductionRate = 0.0f;
	bool				m_bSpeedReductionEternal = false;

	VAR_LR(VEC3D*,46)	m_TakeOffPos = new VEC3D();				// Jump start position
	VAR_LR(REAL32,47)	m_rFallOldPosY = 0.0f;				// ���� ���� �� ���� ������ ���̰�
	VAR_LR(REAL32,48)	m_rElapsedTakeOffTime = 0.0f;
	VAR_LR(REAL32,49)	m_rCurJumpHeight = 0.0f;
	VAR_LR(REAL32,50)	m_rCurJumpTime = 0.0f;
	VAR_LR(REAL32,51)	m_rHalfAccelOfGravity = 0.0f;		// �߷� ���ӵ�
	VEC3D			m_ForceJumpCharaVel;		// force Jump calc speed ( ������ )
	VEC3D			m_ForceJumpDir;				// force Jump start dir ( ������ )
	REAL32			m_rForceJumpSpeed = 0.0f;			// force start Max speed ( ������ )
	REAL32			m_rJumpUpOldTime = 0.0f;			// ���� �ϰ� �������� ���� ���� ��� ������ ������ �ð� ��.
	
	const LadderStateInfo*	m_ladder_info = nullptr;

	I3_TERRAIN_TYPE	m_CurFloorTerrainType;
	VAR_LR(REAL32,52)	m_rFloorCheckTimer = 0.0f;
	VAR_LR(REAL32,53)	m_rFloorLength = 0.0f;

	VAR_LR(REAL32, 54)	m_fDelayTime = 0.0f;				//EscapeMission ��ġ�ٿ �����̸� �־� �������� ���ھ� ó���� �����ϴ�.
	VAR_LR(REAL32,55)	m_rRideObjectElapseTime = 0.0f;

	// for network assist
	VAR_LR(VEC3D*,56)	m_vPostPosition = new VEC3D();
	VAR_LR(VEC3D*,57)	m_vRecvPosition = new VEC3D();

	VEC3D			m_vRecvDir;
	VEC3D			m_vPostDir;

	REAL32			m_fRecvPosTime = 0.0f;

	REAL32			m_rReceivedLength = 0.0f;
	REAL32			m_fRecvThetaRhoTime = 0.0f;

	REAL32			m_fPostTheta = 0.0f;
	REAL32			m_fPostRho = 0.0f;
	REAL32			m_fRecvTheta = 0.0f;
	REAL32			m_fRecvRho = 0.0f;

	bool			m_bPushingRandomDir = false;

	bool			m_bGravityRecovery = false;

private:
	
	VAR_LR(i3PhysixController*, 58)	m_pController = nullptr;			// ��Ʈ�ѷ�

protected:
	void			_TestCollisionAtRespawn();
	REAL32			_GetMoveVelocity(VEC3D * pVecDir, EMoveSpeedType MoveType);
	INT32			_CalcFallingDamage( VEC3D * pTakeOffPos);

public:
	void			addMoveFlag( CHARA_MOVE_FLAG flag)		{ m_CharaMoveFlag |= flag; }
	void			removeMoveFlag( CHARA_MOVE_FLAG flag)	{ m_CharaMoveFlag &= ~flag; }
	bool			isMoveFlag( CHARA_MOVE_FLAG flag)		{ return (m_CharaMoveFlag & flag) == flag; }

	/** \brief PhysX controller�� ��ȯ�մϴ� */
	i3PhysixController *	getPhysixController( void)		{ return m_pController; }
	
	float	getDeleytime()									{return m_fDelayTime;};
	void	SetDeleyTime(float fTime)						{ m_fDelayTime = fTime;}
	void	SetTouchdown(bool bTrue)						{ if( bTrue) addMoveFlag( CHARA_MOVE_FLAG_TOUCHDOWN); else removeMoveFlag( CHARA_MOVE_FLAG_TOUCHDOWN);}
	bool	GetTouchdown()									{return isMoveFlag( CHARA_MOVE_FLAG_TOUCHDOWN);}

	/** \brief �̵����� ��ȯ�մϴ�.
		\note �Է¿� ���� ������ �����մϴ�.*/
	VEC3D *			getMoveDir( void)						{ return &m_MoveDir; }
	
	/** \brief �̵��� ���� ������ ��ȯ�մϴ�.
		\note ĳ������ �����Դϴ�. */
	VEC3D *			getMoveAt( void)						{ return &m_MoveAt; }

	/** \brief ĳ������ Right ������ ��ȯ�մϴ�. */
	VEC3D *			getMoveRight( void)						{ return &m_MoveRight; }

	/** \brief accelation�� ��ȯ�մϴ�. */
	VEC3D *			getAccel( void)							{ return m_Accel;}

	/** \brief accelation�� �����մϴ�. */
	void			setAccel( REAL32 x, REAL32 y, REAL32 z)	{ i3Vector::Set( m_Accel, x, y, z); }
	void			setAccel( VEC3D * pVec)					{ i3Vector::Copy( m_Accel, pVec); }

	/** \brief velocity�� ��ȯ�մϴ�. */
	VEC3D *			getVelocity( void)						{ return m_VelCharacter; }

	/** \brief velocity�� �����մϴ�. */
	void			setVelocity( REAL32 x, REAL32 y, REAL32 z)	{ i3Vector::Set( m_VelCharacter, x, y, z); }
	void			setVelocity( VEC3D * pVec)				{ i3Vector::Copy( m_VelCharacter, pVec); }

	/** \brief ���� veloctiy�� ��ȯ�մϴ�. */
	VEC3D *			getOldVelocity( void)					{ return m_OldVel; }

	/** \brief ���� velocity�� �����մϴ�. */
	void			setOldVelocity( VEC3D * pVec)			{ i3Vector::Copy( m_OldVel, pVec); }

	/** \brief ���� ��ġ�� ��ȯ�մϴ�. */
	VEC3D *			getOldPos( void)						{ return m_vOldPos; }

	/** \brief ���� ��ġ�� �����մϴ�. */
	void			setOldPos( VEC3D * pVec)				{ i3Vector::Copy( m_vOldPos, pVec); }

	/** \brief ���� �ӷ��� ��ȯ�մϴ�. */
	REAL32			getSpeed( void)							{ return m_rVelocity; }

	/** \brief ���� �ӷ��� �����մϴ�. */
	void			setSpeed( REAL32 speed)					{ m_rVelocity = speed; }

	/** \brief �ִ� �ӷ��� ��ȯ�մϴ�. */
	REAL32			getMaxSpeed( void)						{ return m_rMaxMoveSpeed; }

	/** \brief �ִ� �ӷ��� �����մϴ�. */
	void			setMaxSpeed( REAL32 speed)				{ m_rMaxMoveSpeed = speed; }

	/** \brief �ּ� �̵� ������ ��ȯ�մϴ�. */
	REAL32			getStopingElapsed( void)				{ return m_tmStopingElapsed; }

	/** \brief �ּ� �̵� ������ �����մϴ� */
	void			setStopingElapsed( REAL32 tm)			{ m_tmStopingElapsed = tm; }

	/** \brief �ּ� accelation ������ ��ȯ�մϴ�. */
	REAL32			getAccelElapsed( void)					{ return m_tmAccelElapsed; }

	/** \brief �ּ� acceleation ������ �����մϴ�. */
	void			setAccelElapsed( REAL32 tm)				{ m_tmAccelElapsed = tm; }

	/** \brief �ӵ� ���� ȿ�������� Ȯ���մϴ�. */
	bool			isEnabledSpeedReduction()				{ return m_bSpeedReductionEternal ? true : ( m_rElapsedSpeedReduction > 0.0f);}

	/** \brief ��ٸ��� ������ ���� Set/Get */
	void			setLadderStateInfo(const LadderStateInfo* info);
	const LadderStateInfo* getLadderStateInfo() const;
	bool			isJumpingOnLadder() const;

	/** \brief �ٴ��� ������ ��ȯ�մϴ�. */
	I3_TERRAIN_TYPE	getFloorTerrainType( void)				{ return m_CurFloorTerrainType; }
	void			setFloorTerrainType( I3_TERRAIN_TYPE type)	{ m_CurFloorTerrainType = type; }

	/** \brief �ٴ� ���� �˻� �ð��� ��ȯ�մϴ�. */
	REAL32			getFloorCheckTimer( void)				{ return m_rFloorCheckTimer; }
	/** \brief �ٴ� ���� �˻� �ð��� �����մϴ�. */
	void			setFloorCheckTimer( REAL32 tm)			{ m_rFloorCheckTimer = tm; }

	/** \brief ���ߺξ� �ð� */
	REAL32			getElapsedTakeOffTime( void)			{ return m_rElapsedTakeOffTime; }
	void			setElapsedTakeOffTime( REAL32 tm)		{ m_rElapsedTakeOffTime = tm; }

	/** \brief ���� ������ ���̸� ��ȯ�մϴ�. */
	REAL32			getCurJumpHeight( void)					{ return m_rCurJumpHeight; }

	/** \brief ���� ������ ���̸� �����մϴ�. */
	void			setCurJumpHeight( REAL32 height)		{ m_rCurJumpHeight = height; }

	/** \brief ���� ���� �� �ð��� ��ȯ�Ѵ�. */
	REAL32			getCurJumpTime( void)					{ return m_rCurJumpTime; }

	/** \brief ���� ���� �� �ð��� �����մϴ�. */
	void			setCurJumpTime( REAL32 tm)				{ m_rCurJumpTime = tm; }

	/** \brief ĳ������ Network Index�� ��ȯ�մϴ�. */
	INT32			getCharaNetIndex( void);

	/** \brief ������Ʈ������ ��� ��ǥ�� ��ȯ�մϴ�.*/
	VEC3D *			getRideObjectLocalPos( void)			{ return m_RideLocalPos; }

	/** \brief ������Ʈ������ ��� ��ǥ�� �����մϴ�. */
	void			setRideObjectLocalPos(VEC3D* pPos);

	/** \brief �ö�ź ������Ʈ �ε����� ��ȯ�մϴ�. */
	INT32			getRideObjectIndex( void)				{ return m_iRideObjectIndex; }

	/** \brief ���߿� �ֱ����� �ٴ��� ��ġ�� �����մϴ�.
		\note �߶��� ���� ����� �ϱ� ���� �뵵*/
	void			setTakeOffPos( VEC3D * pPos)			{ i3Vector::Copy( m_TakeOffPos, pPos); m_rFallOldPosY = 0.f; }

	/** \brief ���� ���� ��� x,z �ӵ��� �����մϴ�. ���� Y�� �ӵ��� �����Ͽ� �����մϴ�.*/
	void			setForceJumpCharaVel( VEC3D * pVel )	{ i3Vector::Copy(&m_ForceJumpDir, pVel); m_ForceJumpDir.y = 0.f; }
	void			setForceJumpDir( VEC3D * pVel)			{ i3Vector::Copy(&m_ForceJumpDir, pVel); m_ForceJumpDir.y = 0.f; }
	void			setForceJumpSpeed(REAL32 pSpeed)		{ m_rForceJumpSpeed = pSpeed; }

	void			setRideObjectElapseTime( REAL32 tm)		{ m_rRideObjectElapseTime = tm; }

	/** \brief �㸮���� �ٴڱ����� ���� */
	REAL32			getFloorLength( void)					{ return m_rFloorLength; }
	void			setFloorLength( REAL32 length)			{ m_rFloorLength = length; }

	bool			isStandOnMovingObject( void)			{ return isMoveFlag( CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ); }
	void			setStandOnMovingObject( bool bVal)		{ if( bVal) addMoveFlag( CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ); else removeMoveFlag(CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ);}

	/** \brief ���� ��ġ�� ��ȯ�մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	VEC3D *			getPostPosition( void)					{ return m_vPostPosition; }
	void			setPostPosition( const VEC3D * vec)		{ i3Vector::Copy( m_vPostPosition, const_cast<VEC3D*>(vec)); }

	/** \brief ��ǥ ��ġ�� ��ȯ�մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	const VEC3D *	getRecvPosition( void) const			{ return m_vRecvPosition; }
	void			setRecvPosition( const VEC3D * vec)		{ i3Vector::Copy( m_vRecvPosition, const_cast<VEC3D*>(vec)); }

	/** \brief �ٶ󺸴� ��ǥ ������ ��ȯ�մϴ�. 
		\note ��Ʈ��ũ ���������� ��� */
	VEC3D *			getRecvDirection( void)					{ return &m_vRecvDir; }

	/** \brief ���� �ٶ� ���� ������ ��ȯ�մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	VEC3D *			getPostDirection( void)					{ return &m_vPostDir; }

	/** \brief ���� Ÿ�̸Ӹ� ��ȯ�մϴ�. */
	REAL32			getRecvPosTime( void)					{ return m_fRecvPosTime; }

	/** \brief ���� Ÿ�̸Ӹ� �����մϴ�. */
	void			setRecvPosTime( REAL32 time)			{ m_fRecvPosTime = time; }

	/** \brief ��ǥ ��ġ������ �Ÿ��� ��ȯ�մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	REAL32			getReceivedLength( void)				{ return m_rReceivedLength; }

	/** \brief ��ǥ ��ġ������ �Ÿ��� �����մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	void			setReceivedLength( REAL32 len)			{ m_rReceivedLength = len; }

	/** \brief ���� Ÿ�̸Ӹ� ��ȯ�մϴ�.
		\note ���� interpolate������ ��� */
	REAL32			getRecvThetaRhoTime( void)				{ return m_fRecvThetaRhoTime; }

	/** \brief ���� Ÿ�̸Ӹ� �����մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	void			setRecvThetaRhoTime( REAL32 time)		{ m_fRecvThetaRhoTime = time; }

	/** \brief ���� Theta�� ��ȯ�մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	REAL32			getPostTheta( void)						{ return m_fPostTheta; }

	/** \brief ���� Theta�� �����մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	void			setPostTheta( REAL32 theta)				{ m_fPostTheta = theta; }

	/** \brief ���� Rho�� ��ȯ�մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	REAL32			getPostRho( void)						{ return m_fPostRho; }

	/** \brief ���� Rho�� �����մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	void			setPostRho( REAL32 rho)					{ m_fPostRho = rho; }

	/** \brief ��ǥ Theta�� ��ȯ�մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	REAL32			getRecvTheta( void)						{ return m_fRecvTheta; }

	/** \brief ��ǥ Theta�� �����մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	void			setRecvTheta( REAL32 theta)				{ m_fRecvTheta = theta; }

	/** \brief ��ǥ Rho�� ��ȯ�մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	REAL32			getRecvRho( void)						{ return m_fRecvRho; }

	/** \brief ��ǥ Rho�� �����մϴ�.
		\note ��Ʈ��ũ ���������� ��� */
	void			setRecvRho( REAL32 rho)					{ m_fRecvRho = rho; }

	bool			IsNetworkController( void);

	void			EnablePushingRandomDir()				{ m_bPushingRandomDir = true; }
	
	/** \brief �߷� ���� �������� ����մϴ�.
	\note �߷°� ������ ���� Flag */
	void			SetGravityRecovery(bool bEnable)		{ m_bGravityRecovery = bEnable; }
public:
	CGameCharaMoveContext();
	virtual ~CGameCharaMoveContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	virtual void	OnChangeLocal( void) override;
	virtual void	OnChangeNetwork( void) override;
	virtual void	OnChangeNPC( void) override;
	virtual void	OnChangeAI( void) override;
	virtual void	OnChangeFullbody( void) override;
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief Collision Coltroller�� �����մϴ�. */
	void			CreateController( void);

	/** \brief Collision Controller�� �����մϴ�. */
	void			ReleaseController( void);

	/** \brief Collision Controller�� �ʱ�ȭ�մϴ�. */
	void			ResetController( void);

	/** \brief Collision ������ �����մϴ�. */
	void			EnableController( bool bVal = true);

	/** \brief Controller�� ���̸� �����մϴ�. */
	void			ChangeControllerHeight( REAL32 rHeight);

	/** \brief Collision test�� �մϴ�. */
	void			CheckWorldCollision( REAL32 rDeltatime);
	
	void			UpdateTouchDown(REAL32 rDeltatime); //EscapeMission ���ӵ� �浹üũ�� ���� ���ھ� �ߺ�üũ�� ���� ���� �����մϴ�.
	void			ResetJumping( void);

	/** \brief ���� Update�մϴ�. */
	void			UpdateJumping( REAL32 rDeltatime);

	/** \brief ���� damage�� ����մϴ�. */
	void			UpdateFallDamage( REAL32 rDeltatime);

	REAL32			CalrFallVel(VEC3D& newPos, REAL32 rFac, REAL32 rDeltatime);

	//�� �׷���Ƽ �Լ�
	void 			SetGravity(bool bTrue)					{ if( bTrue) addMoveFlag( CHARA_MOVE_FLAG_IS_GRAVITY); else removeMoveFlag( CHARA_MOVE_FLAG_IS_GRAVITY); }
	bool			isGravity()								{ return isMoveFlag( CHARA_MOVE_FLAG_IS_GRAVITY);}
	
	/** \brief �̵� �ӷ� ���� ȿ���� �����մϴ�.
		\param[in] nHitPart ���� ������ ���� ����Ǵ� ���� �ٸ��ϴ�. */
	void			EnableSpeedReduction( UINT8 nHitPart);

	/** \brief �̵� �ӷ� ���� ȿ���� �����մϴ�.
		\param[in] rRate ���ҷ�
		\param[in] rTime ���� �ð� */
	void			EnableSpeedReduction( REAL32 rRate, REAL32 rTime);

	/** \brief �̵� �ӷ� ���� ȿ���� Update�մϴ�. */
	void			UpdateSpeedReduction( REAL32 rDeltaSeconds);

	CHARA_HIT_PART_SIMPLE PartKind(UINT8 nHitPart);

	// ������Ʈ Ÿ�� ���
	void			SetRideObject( i3Object * pObj);
	void			ResetRideObject( void);
	void			CheckObjectByController( void);
	void			UpdateRideObject( MATRIX * pNewTx, REAL32 tm, bool bPhysixUpdate);
	VEC3D *			TransformByRideObject( MATRIX * pAnimatedMtx, VEC3D * pPos, bool bUpdate);
	void			RotateThetaByRideObejct( MATRIX * pAnimatedMtx, MATRIX * pInvMtx);

	/** \brief �̵��� ����մϴ�. */
	void			Move( REAL32 rDeltatime);
	
	/** \brief ĳ������ ���� ��ġ�� �����մϴ�. */
	void			SetPosition( const VEC3D * pPos);

	/** \brief ĳ������ ���� ��ġ�� ��� �ɴϴ�. */
	void			GetPosition( VEC3D * pPos);

	/** \brief accelation���� ����մϴ�. */
	void			ProcessAccel( void);

	/** \brief ������Ʈ �������� �̵��� ����մϴ�. */
	void			MoveCharaByObject( VEC3D * pPos);

	/** \brief Controller�� �̵���ŵ�ϴ�.
		\param[in] pMoveVec �̵���
		\param[in] minDist �ּ� �̵� �Ÿ�
		\param[in] pHintFlag �浹�� ���� ��� flag
		\param[in] sharpness ���� ���� ������ ���� (���� ���� �̵� ����� �����ϰ� �ȴ�)*/
	void			MoveController( VEC3D * pMoveVec, REAL32 minDist, UINT32 * pHintFlag, REAL32 sharpness = 1.0f);

	/** \brief �̲������� ���� ����մϴ�.
		\param[in] hitMask �浹 ��� flag
		\param[in] minDist �ּ� �̵��� �Ÿ�
		\param[in] vVel �̵� ���� �ӵ�*/
	void			SlipSlope(UINT32& hitMask, REAL32& minDist, VEC3D& vVel);

	// revision 54918
	/** \brief ���� ��Ȳ������ �ø����� üũ�մϴ�. */
	void			CheckCollisionException(UINT32& hitMask, VEC3D& vVel);
	

	// ���� ������Ʈ �浹 ó��
public:
	void			GetNxCapsule(NxCapsule* pCapsule);
	void			CheckOverlapObject();
	void			CreateCCD(VEC3D* pv3Pos, VEC3D* pv3Size, int iCGRP);
	void			FromObjDamageToChara(NxShape* pShape, i3PhysixShapeSet* pi3Shape);
	bool			EnableKill();

	// �ö�ź ������Ʈ
private:
	VAR_LR(i3Object*, 59)	m_pRideObject = nullptr;
	VAR_LR(i3Object*, 60)	m_pOldRideObject = nullptr;
	VAR_LR(VEC3D*,	   61)	m_RideLocalPos = new VEC3D();
	VAR_LR(INT32,	   62)	m_iRideObjectIndex = 0;
	VAR_LR(MATRIX*,   63)	m_matRideObjectPose = new MATRIX();
	VAR_LR(VEC3D*,    64)	m_RideSyncPos = new VEC3D();
	VAR_LR(INT32,     65)	m_iRideObjectCountTime = 0;

	i3Object*		m_pPrevJumpHold = nullptr;

public:
	i3Object *		getRideObject() const			{ return m_pRideObject; }
	void			setRideObject( i3Object * pObj)	{ m_pRideObject = pObj; }

	// �ε�ġ�� �������� �ִ� ������Ʈ
private:
	REAL32			m_rDamageTime = 0.0f;
public:
	//void			FindDamageObjectAndDamageCharaSelf(NxShape* pShape, i3PhysixShapeSet* pi3Shape);
	void			DamageCharaSelf_IfOverlap(NxShape* pShape);

	// �����̴� ������Ʈ�� �浹 �� �о��
private:
	
	i3Object*		m_pContactObject = nullptr;
public:
	
	VEC3D			m_v3ContactObjectVelToChara;

	void			OverlapObjectPush(VEC3D& vVel, REAL32 minDist, UINT32& hitMask);
	void			PushByRaycastResult( const I3_PHYSIX_HIT_RESULT * pResult, const NxCapsule * pCapsule, NxVec3 &vDir, REAL32 skinWidth);
	void			CalcOverlapLength(NxShape* pShape, NxCapsule* pCapsule, REAL32 rSkinWidth, NxVec3 &vDir);
	void			CalcContactObjectVel(NxVec3& nxExtent, NxVec3& nxPos, NxCapsule* pCapsule, REAL32 rSkinWidth);
	void			CalcContactObjectVelConvex(NxVec3& nxExtent, NxVec3& nxPos, NxCapsule* pCapsule, REAL32 rSkinWidth);
	void			PushProcessFromAnimObj(VEC3D& vVel, REAL32 minDist, UINT32& hitMask);

	void			PostUpdate( REAL32 rDeltatime);
	void			OverlapPush();
	// test
	


	// ��Ÿ �� ���� ó��
	bool			IsUpTownMoveTrainArea() { return (m_RideSyncPos->z > -2.0183 && m_RideSyncPos->z < 2.0033); }
};

class CHitInfo
{
private:
	NxShape* m_pHitShape = nullptr;
	VEC3D m_v3HitNormal;

public:
	CHitInfo() {}
	void SetHitInfo(NxShape* pShape, VEC3D* pV) { m_pHitShape = pShape; if( pV != nullptr) i3Vector::Copy(&m_v3HitNormal, pV); }
	NxShape* GetHitShape() { return m_pHitShape; }
	VEC3D* GetHitNormal() { return &m_v3HitNormal; }
};

struct SWorldHitInfo
{
private:
	VEC3D m_vHitPos;
	VEC3D m_vHitNormal;

public:
	SWorldHitInfo() {}
	void SetWorldHitInfo(VEC3D* pPos, VEC3D* pNormal)	{	i3Vector::Copy(&m_vHitPos, pPos); i3Vector::Copy(&m_vHitNormal, pNormal); }
	void InitWorldHitInfo()	{ i3Vector::Zero(&m_vHitPos); i3Vector::Zero(&m_vHitNormal);}
	VEC3D* GetHitWorldPos()	{ return &m_vHitPos; }
	VEC3D* GetHitWorldNormal()	{ return &m_vHitNormal; }
};


class PlayerController : public i3PhysixController
{
	I3_CLASS_DEFINE( PlayerController, i3PhysixController);

protected:
	CGameCharaBase *	m_pOwner = nullptr;
	CGameCharaBase *	m_pHitUser = nullptr;

	INT32				m_nHitShapeCount = 0;
	CHitInfo			m_HitInfo[CONTROLLER_HIT_INFO_COUNT];

	INT32				m_nHitWorldPosCount = 0;
	SWorldHitInfo		m_HitWorldPos[CONTROLLER_WORLD_HIT_POS_COUNT];

	bool				m_bHitObject = false;

public:
	PlayerController(void) {}
	virtual ~PlayerController(void);

	void				setOwner( CGameCharaBase * pPlayer);
	

	CGameCharaBase *	getOwner(void)			 {	return m_pOwner;}

	CGameCharaBase *	getHitUser( void)		{ return m_pHitUser; }

	// NxShape *			getHitShape( INT32 idx)		{ return m_pHitShapeList[ idx]; }
	CHitInfo*			GetHitInfo(INT32 idx) { I3_BOUNDCHK(idx, CONTROLLER_HIT_INFO_COUNT); return &m_HitInfo[idx]; }
	INT32				getHitShapeCount( void)		{ return m_nHitShapeCount; }
	void				setHitShapeCount( INT32 val)	{ m_nHitShapeCount = val; }

	SWorldHitInfo*		GetHitWorld(INT32 idx) { I3_BOUNDCHK(idx, CONTROLLER_WORLD_HIT_POS_COUNT); return &m_HitWorldPos[idx]; }
	INT32				GetHitWorldShapeCount(void)	{ return m_nHitWorldPosCount;}
	void				SetHitWorldShapeCount( INT32 val)	{ m_nHitWorldPosCount = val; }

	void InitWorldHitInfoArray()
	{
		for(INT32 i = 0; i < CONTROLLER_WORLD_HIT_POS_COUNT; i++)
		{
			m_HitWorldPos[i].InitWorldHitInfo();
		}
	}

	virtual NxControllerAction		OnShapeHit( const NxControllerShapeHit * pHit) override;
	virtual	NxControllerAction		OnControllerHit( const NxControllersHit * pHit) override;

	static void			releaseShape( i3ElementBase * pReceiver, NxShape * pShape);

	void				FlushHitShape( void);
};

#endif

