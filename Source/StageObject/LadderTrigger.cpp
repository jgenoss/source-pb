#include "pch.h"
#include "LadderTrigger.h"
#include "../BattleSlotContext.h"
#include "../GameCharaInputContext.h"
#include "../GameCharaMoveContext.h"

#if defined( USE_EDITDLG)
#include "i3TDKUpdateSystem.h"
#include "i3Level/i3LevelElement3D.h"
#endif

#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"

struct LadderStateInfo
{
	CGameCharaBase* m_localClimber;
	MATRIX*			m_matTrigger;

	VEC3D			m_vAxisHalfLength;				// width,height,depth	

	VEC4D			m_planeLadderLeft;
	VEC4D			m_planeLadderRight;
	VEC4D			m_planeLadderFront_Ortho;
	VEC4D			m_planeLadderBack_Ortho;
	VEC4D			m_planeLadderTop_Ortho;
	VEC4D			m_planeLadderBottm_Ortho;		// �� �عٴ��̶�⺸�ٴ�, ��ٸ� ������� �عٴ����� ���� ������ ���Ѵ�..
	VEC4D			m_planeLadderFront;
	
	VEC3D			m_vNegativeLadderAxisY;			// ���⼭���� �۾��簳�Ұ� ..(2015.01.06.����)
	const VEC3D*	m_CurrAccDir;	
	bool			m_bJumping;
};

const VEC3D*	GetCurrLadderAccAxis(const LadderStateInfo* info)
{
	I3ASSERT(info != nullptr);
	return info->m_CurrAccDir;
}

const VEC3D*	GetCurrLadderRightAxis(const LadderStateInfo* info)
{
	I3ASSERT(info != nullptr);
	return reinterpret_cast<const VEC3D*>(&info->m_planeLadderRight);		
}



bool			IsJumpingOnLadder(const LadderStateInfo* info)
{
	return info != nullptr && info->m_bJumping;
}

void			ResetLadderJump(const LadderStateInfo* info)
{
	if (IsJumpingOnLadder(info) == false)
		return;

	const_cast<LadderStateInfo*>(info)->m_bJumping = false;
	
	if (info->m_localClimber)
		info->m_localClimber->removeEnableStateMask(CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
}

namespace
{
	const VEC3D		NEGATIVE_YAXIS = VEC3D(0.f, -1.f, 0.f);
	
	bool			FaceEachOther(LadderStateInfo* info)
	{
		const VEC3D* vAtTrigger = reinterpret_cast<const VEC3D*>(&info->m_planeLadderBack_Ortho);
		VEC3D vAtClimber;
		
		i3Vector::Cross(&vAtClimber, i3Matrix::GetRight(info->m_localClimber->getSceneObject()->GetCacheMatrix()), &I3_YAXIS);
		i3Vector::Normalize(&vAtClimber, &vAtClimber);
		
		REAL32 r = i3Vector::Dot(vAtTrigger, &vAtClimber);
// cos 60 == 1/2  // 0.5���� ������ ��ٸ� �β��� ���� ����� �÷��̾� ���Ⱓ ������ 60������ Ŀ���� ����....   
		return r > 0.5f;
	}

	// ���⼭�� �Ʒ��� 30�� ���� ��쿡 ���ؼ� �Ʒ������� ���Ѵٰ� ó���Ѵ�...

	// ī�޶� ��������Ʈ�� ����� 3��Ī �������� ���� �ʰų� Y������ �ݿ����� ���ϰų� ������ ���� �ȵȴ�...
	static const REAL32 lower_threshold_rad = I3_DEG2RAD(-30.f);

	bool			IsCharacterHeadedToLower(CGameCharaBase* chara)
	{
		REAL32 rho = chara->getBoneContext()->getRhoValue();				// getViewRho()
		
		return rho < lower_threshold_rad;

//		MATRIX* matCam = g_pCamera->getCamMatrix();
//		VEC3D* camAt = i3Matrix::GetAt(matCam);				// ����Ե� ī�޶��� at���ʹ� ���� ������ �ݴ�����̴�.. (�� �̷��� ������..)
//		return ( camAt->y > 0.5f);		// �ϴ� ���⺤�Ͱ� �����̸� ������ false.. ( ������ camAt->y < -0.5f �̾�� ������, ī�޶� At���Ͱ� �ݴ�����̹Ƿ� ��ȣ�� �ٲ۴�..)
	}									// ���Լ������Ϳ��� ������ 120�� �а��� �̷�� �Ǳ� ������ cos 120 ���� �������̸� �Ʒ��� ���Ѵٰ� �����Ѵ�.

	const VEC3D*	GetTriggerAxis(LadderStateInfo* info, INT32 axis)
	{
		return reinterpret_cast<const VEC3D*>(&info->m_matTrigger->vec[ axis ] );			// vec[ info->m_idxAxis[axis]  ]
	}

	void			CalcTriggerDirOrtho(LadderStateInfo* info, VEC3D* outDirOrtho)
	{
		const VEC3D* pvDir = GetTriggerAxis(info, 2);
		i3Vector::Cross(outDirOrtho, pvDir, &I3_YAXIS);			// ���� Right Axis�� ���ϰ�..
		i3Vector::Normalize(outDirOrtho, outDirOrtho);
		i3Vector::Cross(outDirOrtho, &I3_YAXIS, outDirOrtho);	// �װɷ� �ٽ� DirOrtho�� ���ϸ� ��..
	}

	const VEC3D*	GetTriggerCenter(LadderStateInfo* info)
	{
		return reinterpret_cast<const VEC3D*>(&info->m_matTrigger->vec[3]);
	}
	
	REAL32			GetTriggerHalfLength(LadderStateInfo* info, INT32 axis)
	{
		return info->m_vAxisHalfLength.vec[axis];
	}

	NxVec3 GetTriggerLength(CLadderTrigger* trigger)
	{
		NxShape* const * ashapeTrigger = trigger->getNxActor()->getShapes();
		I3ASSERT( ashapeTrigger != nullptr );
		I3ASSERT( ashapeTrigger[0]->isBox() );
		const NxBoxShape* shapeTrigger = static_cast<const NxBoxShape*>(ashapeTrigger[0]);
		return shapeTrigger->getDimensions();
	}

	void CalcLadderLength(CLadderTrigger* trigger, VEC3D* outHalfLength)
	{
		NxVec3 vDim = GetTriggerLength(trigger);
		
		outHalfLength->x = vDim.get()[0];
		outHalfLength->y = vDim.get()[1];
		outHalfLength->z = vDim.get()[2];
	}

	/*
	void CalcAxisIndexAndLength( CLadderTrigger* trigger, INT32 (&outAxis)[3], VEC3D* outHalfLength )
	{
		INT32 HeightAxis = 0;

		{
			const MATRIX* matTrigger = trigger->GetMatrix();
			REAL32 fMaxUpValue = -2.f;			// ��������� ���ü� ���� Min������ ���� ����..(-1�� ��� �ּҰ���)

			for (INT32 i = 0 ; i < 3 ; ++i)
			{
				const VEC3D* vAxis = reinterpret_cast<const VEC3D*>(&matTrigger->vec[i]);
				REAL32 dotVal = i3Vector::Dot(vAxis, &I3_YAXIS);

				if (  dotVal > fMaxUpValue )
				{
					fMaxUpValue = dotVal;	HeightAxis = i;
				}
			}
		}

		NxVec3 vDim = GetTriggerLength(trigger);

		INT32 DepthAxis = 0;
		{
			REAL32 minVal = FLT_MAX;			// ��������� ���ü� ���� Max������ ���漳��..
		
			for (INT32 i = 0 ; i < 3; ++i)
			{
				if (i == HeightAxis ) continue;
				REAL32 rVal = vDim.get()[i];	

				if ( minVal > rVal ) { minVal = rVal;	DepthAxis = i; }
			}
		}

		INT32 WidthAxis = 0;
		{
			for (INT32 i = 0 ; i < 3; ++i )
			{
				if ( i == HeightAxis ) continue;
				if ( i == DepthAxis ) continue;
				WidthAxis = i;
			}
		}

		outAxis[0] = WidthAxis;
		outAxis[1] = HeightAxis;
		outAxis[2] = DepthAxis;
		outHalfLength->x = vDim.get()[WidthAxis];
		outHalfLength->y = vDim.get()[HeightAxis];
		outHalfLength->z = vDim.get()[DepthAxis];
	}
	*/

	void    CalcClimberPos(LadderStateInfo* info, VEC3D* out)
	{
		info->m_localClimber->getMoveContext()->GetPosition(out);
	}
	
	void	CalcClimberRadius(LadderStateInfo* info, REAL32& outRadius)
	{
		NxCapsuleController* nxCtrler = (NxCapsuleController*)info->m_localClimber->getMoveContext()->getPhysixController()->getNxController();
		outRadius = nxCtrler->getRadius();
	}

	REAL32 CalcDist_LeftPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderLeft, playerPos);
	}

	REAL32 CalcDist_RightPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderRight, playerPos);
	}


	REAL32 CalcDist_FrontPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderFront, playerPos);
	}

	REAL32 CalcDist_FrontOrthoPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderFront_Ortho, playerPos);
	}

	REAL32	CalcDist_BackOrthoPlane( LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderBack_Ortho, playerPos);
	}
	
	REAL32 CalcDist_TopOrthoPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderTop_Ortho, playerPos);
	}

	REAL32 CalcDist_BottomOrthoPlane(LadderStateInfo* info, VEC3D* playerPos)
	{
		return i3Plane::Dot(&info->m_planeLadderBottm_Ortho, playerPos);
	}
	
	LadderStateInfo*	CreateLadderInfo(CLadderTrigger* trigger)
	{
		LadderStateInfo* new_info = new LadderStateInfo;
		new_info->m_localClimber = nullptr;
		new_info->m_matTrigger = trigger->GetMatrix();

	//	CalcAxisIndexAndLength(trigger, new_info->m_idxAxis, &new_info->m_vAxisHalfLength);
		CalcLadderLength(trigger, &new_info->m_vAxisHalfLength);

		const VEC3D* vTriggerRight = GetTriggerAxis(new_info, 0);
		const VEC3D* vTriggerUp = GetTriggerAxis(new_info, 1);
		const VEC3D* vTriggerDir   = GetTriggerAxis(new_info, 2);

		new_info->m_vNegativeLadderAxisY = *vTriggerUp;
		new_info->m_vNegativeLadderAxisY *= -1.f;

//		if (new_info->m_idxAxis[2] == 0)		// �β��� �ش�Ǵ� ���� X���̶�� ���� �������� �ڹٲ�� �ȴ�..
//			vTriggerRight *= -1.f;
		
		const VEC3D* vTriggerPos = GetTriggerCenter(new_info);

		{
			VEC3D vTriggerWidth = *vTriggerRight;
			vTriggerWidth *= GetTriggerHalfLength(new_info, 0);
			

			{	// �����..
				new_info->m_planeLadderLeft.x = -vTriggerRight->x;
				new_info->m_planeLadderLeft.y = -vTriggerRight->y;
				new_info->m_planeLadderLeft.z = -vTriggerRight->z;

				VEC3D vTriggerLeftPos = *vTriggerPos;	vTriggerLeftPos -= vTriggerWidth;
				new_info->m_planeLadderLeft.w = 
					-i3Vector::Dot( reinterpret_cast<VEC3D*>( &new_info->m_planeLadderLeft), &vTriggerLeftPos);
			}

			{	// �����
				new_info->m_planeLadderRight.x = vTriggerRight->x;
				new_info->m_planeLadderRight.y = vTriggerRight->y;
				new_info->m_planeLadderRight.z = vTriggerRight->z;

				VEC3D vTriggerRightPos = *vTriggerPos;	vTriggerRightPos += vTriggerWidth;
				new_info->m_planeLadderRight.w = 
					-i3Vector::Dot( reinterpret_cast<VEC3D*>( &new_info->m_planeLadderRight), &vTriggerRightPos);
			}
		}


		{	
			VEC3D	vDirOrtho;
			CalcTriggerDirOrtho(new_info, &vDirOrtho);

			VEC3D vTriggerHeight = *vTriggerUp;
			vTriggerHeight *= GetTriggerHalfLength(new_info, 1);
			VEC3D vTriggerDepth  = *vTriggerDir;
			vTriggerDepth *= GetTriggerHalfLength(new_info, 2);

			{	// ��ٸ� ���� ���� ���..
				new_info->m_planeLadderFront_Ortho.x = -vDirOrtho.x;
				new_info->m_planeLadderFront_Ortho.y = -vDirOrtho.y;
				new_info->m_planeLadderFront_Ortho.z = -vDirOrtho.z;

				VEC3D vTriggerFrontPos = *vTriggerPos;
				vTriggerFrontPos -= vTriggerHeight;		vTriggerFrontPos -= vTriggerDepth;

				new_info->m_planeLadderFront_Ortho.w =
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderFront_Ortho), &vTriggerFrontPos);
			}


			{	// ��ٸ� �ĸ� ���� ���..
				new_info->m_planeLadderBack_Ortho.x = vDirOrtho.x;
				new_info->m_planeLadderBack_Ortho.y = vDirOrtho.y;
				new_info->m_planeLadderBack_Ortho.z = vDirOrtho.z;

				VEC3D vTriggerBackPos = *vTriggerPos;
				vTriggerBackPos += vTriggerHeight;		vTriggerBackPos += vTriggerDepth;

				new_info->m_planeLadderBack_Ortho.w =
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderBack_Ortho), &vTriggerBackPos);
			}

			{	// ��ٸ� ��� ���� ���..
				new_info->m_planeLadderTop_Ortho.x = 0.f;
				new_info->m_planeLadderTop_Ortho.y = 1.f;
				new_info->m_planeLadderTop_Ortho.z = 0.f;

				VEC3D vTriggerTopPos = *vTriggerPos;
				vTriggerTopPos += vTriggerHeight;		vTriggerTopPos -= vTriggerDepth;

				new_info->m_planeLadderTop_Ortho.w = 
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderTop_Ortho), &vTriggerTopPos);
			}
			{
				// ��ٸ� �ϴ� ���� ���..
				new_info->m_planeLadderBottm_Ortho.x = 0.f;
				new_info->m_planeLadderBottm_Ortho.y = -1.f;
				new_info->m_planeLadderBottm_Ortho.z = 0.f;
				
				VEC3D vTriggerBottomPos = *vTriggerPos;
				vTriggerBottomPos -= vTriggerHeight;	vTriggerBottomPos -= vTriggerDepth;
				new_info->m_planeLadderBottm_Ortho.w = 
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderBottm_Ortho), &vTriggerBottomPos);
			}
		//	const VEC3D* pvDir = GetTriggerAxis(new_info, 2);

			{	// ��ٸ� �� ���..
				new_info->m_planeLadderFront.x = -vTriggerDir->x;
				new_info->m_planeLadderFront.y = -vTriggerDir->y;
				new_info->m_planeLadderFront.z = -vTriggerDir->z;

				VEC3D vTriggerFrontPos = *vTriggerPos;
				vTriggerFrontPos -= vTriggerDepth;	

				new_info->m_planeLadderFront.w =
					-i3Vector::Dot(reinterpret_cast<VEC3D*>( &new_info->m_planeLadderFront), &vTriggerFrontPos);
			}

		}

		new_info->m_CurrAccDir = GetTriggerAxis(new_info, 1);		// �������� ����Ʈ �������..
		new_info->m_bJumping = false;

		return new_info;
	}


	void	ResetClimberFromLadderInfo(LadderStateInfo* info)
	{
		CGameCharaBase* climber = info->m_localClimber;
		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		climber->getMoveContext()->setLadderStateInfo(nullptr);
		info->m_localClimber = nullptr;
		info->m_bJumping = false;
	}

	void	SetClimberIntoLadderInfo(LadderStateInfo* info, CGameCharaBase* climber)
	{
		info->m_localClimber = climber;

		const LadderStateInfo* old_info =	climber->getMoveContext()->getLadderStateInfo();
		
		if (old_info != nullptr )
		{
			I3ASSERT(old_info != info);
			ResetClimberFromLadderInfo( const_cast<LadderStateInfo*>(old_info) );
		}

		climber->getMoveContext()->setLadderStateInfo(info);
	}
			
	bool	IsClimberExistedFromLadderInfo(LadderStateInfo* info )
	{
		return info->m_localClimber != nullptr;
	}

}


I3_CLASS_INSTANCE( CLadderTrigger);

CLadderTrigger::CLadderTrigger() : m_LadderInfo(nullptr) 
{

}

CLadderTrigger::~CLadderTrigger()
{
	delete m_LadderInfo;
}


void	CLadderTrigger::OnInitTrigger()
{
	m_LadderInfo = CreateLadderInfo(this);
}

void CLadderTrigger::OnEnter(i3PhysixShapeSet* pTriggerShapeSet, i3ElementBase* pDest)
{
	if ( IsClimberExistedFromLadderInfo(m_LadderInfo) )		// info�� �����ϸ� Ʈ���������� �����Ѵ�.
	{
		if (!m_LadderInfo->m_bJumping)
			return;

		CGameCharaBase * climber = m_LadderInfo->m_localClimber;

		// ���� Ȥ�� ���� ���̶�� ����ó���� �Ѵ�..
		//	if (climber->isCharaStateMask(CHARA_STATE_JUMP))			// �����ϸ� ��ٸ��� �ö�ź ��� ���׷� �ڵ��̵��Ǵ°� ���� ���ؼ���, ������ ������ ���߰� ���� ������ �������־���Ѵ�...
		if (climber->getActionContext()->isLower( CHARA_LOWER_JUMP) )
		{															// ���� �� �ڵ����� ��ġ�� �� ������ ������, �������̸� LadderClimbStates.cpp �ȿ� ���ֳ��� ���°� ���ڴ�....(2014.12.03.����)
			climber->Cmd_JumpEnd();
			//		climber->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE);
			climber->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

			CGameCharaMoveContext* move_context = climber->getMoveContext();
			i3Vector::Zero( move_context->getMoveDir() );
			move_context->ResetJumping();
		}

		m_LadderInfo->m_bJumping = false;
		return;
	}

	if( !i3::kind_of<i3PhysixShapeSet*, CGameCharaBase*>(pDest) )
		return;
	
	CGameCharaBase * climber = nullptr;

	if( i3::kind_of<i3PhysixShapeSet*>(pDest) )
	{
		i3PhysixShapeSet * pShapeSet = 	static_cast<i3PhysixShapeSet*>( pDest);
		climber = i3::kind_cast<CGameCharaBase*>( pShapeSet->getEventReceiver() );
	}
	else
	{
		climber = static_cast<CGameCharaBase*>( pDest );
	}
	
	if (!climber) return;
	if (climber->IsDino()) return;
	if (!climber->isLocal()) return;

	SetClimberIntoLadderInfo(m_LadderInfo, climber);

	if (climber->getActionContext()->isLower( CHARA_LOWER_JUMP) )
	{															// ���� �� �ڵ����� ��ġ�� �� ������ ������, �������̸� LadderClimbStates.cpp �ȿ� ���ֳ��� ���°� ���ڴ�....(2014.12.03.����)
		climber->Cmd_JumpEnd();
//		climber->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE);
		climber->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		CGameCharaMoveContext* move_context = climber->getMoveContext();
		i3Vector::Zero( move_context->getMoveDir() );
		move_context->ResetJumping();
	}

	CLadderTrigger::UpdateTriggerImp();
}

void	CLadderTrigger::UpdateTriggerImp()
{
	if (IsClimberExistedFromLadderInfo(m_LadderInfo) == false )
		return;
	
	CGameCharaBase* climber = m_LadderInfo->m_localClimber;

	if ( m_LadderInfo->m_bJumping )		// ���� ������...
	{
		// CLIMB ���´� �������� �ʰ�, ���� ���¿� �����ϰ� ����..
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		return;
	}

	if ( FaceEachOther(m_LadderInfo) == false)
	{
		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		return;
	}
	
	UINT64 keyStroke = climber->getInputContext()->getKeyStrokeInput();
	if( keyStroke & GAME_KEY_ACTION_JUMP )
	{
		m_LadderInfo->m_bJumping = true;
		// CLIMB ���´� �������� �ʰ�, ���� ���¿� ������ ���·� ����.
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		climber->Cmd_JumpStart();
		return;
	}

	CGameCharaMoveContext* move_context = climber->getMoveContext();

	VEC3D* moveDir   = move_context->getMoveDir();	
	
	bool   isCharacterHeadedLower = IsCharacterHeadedToLower(climber);

//	VEC3D* moveAt	 = move_context->getMoveAt();
//	VEC3D* moveRight = move_context->getMoveRight();
//	VEC3D  inputVelAt = *moveAt;	inputVelAt *= moveDir->z;
//	VEC3D  inputVelRight = *moveRight;	inputVelRight *= moveDir->x;
//	VEC3D  inputVel = inputVelAt;		inputVel += inputVelRight;
		
	bool   bFloor    = climber->isCharaStateMask(CHARA_STATE_FLOOR);
	VEC3D playerPos;		CalcClimberPos(m_LadderInfo, &playerPos);
//	VEC3D playerPos = *i3Matrix::GetPos(climber->getSceneObject()->GetCacheMatrix());

	REAL32 playerRadius;	CalcClimberRadius(m_LadderInfo, playerRadius);
	playerPos.y += playerRadius;		// �̸� ��������ŭ ���ؼ�, ĸ���عٴ��� �ƴ� �Ʒ� ���Ǿ��߽��� �ǵ��� �����Ѵ�..

	// �¿� ��� �˻� ����� �׳� out��Ŵ..

	REAL32 dist_Left = CalcDist_LeftPlane(m_LadderInfo, &playerPos);

	if (dist_Left > playerRadius * 2.f)		// ���� �ٱ��̸� ������ ��� ������ ����..
	{
		ResetClimberFromLadderInfo(m_LadderInfo);
		return;
	}
	else
	if (dist_Left > 0.f)  // playerRadius
	{
		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		return;
	}
	
	REAL32 dist_right = CalcDist_RightPlane(m_LadderInfo, &playerPos);

	if (dist_right > playerRadius * 2.f)
	{
		ResetClimberFromLadderInfo(m_LadderInfo);
		return;
	}
	else
	if (dist_right > 0.f)		//playerRadius ���� �ٱ��̸� ������ ��� ������ ����..
	{

		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);

		return;
	}

	REAL32 dist_FrontOrtho = CalcDist_FrontOrthoPlane(m_LadderInfo, &playerPos);
	
	if (dist_FrontOrtho > playerRadius)
	{
		if (moveDir->z < 0.f )		// ��ٸ� �Ʒ����� ������ ��� Ʈ���� ����
		{
			ResetClimberFromLadderInfo(m_LadderInfo);
			return;
		}

		climber->removeCharaStateMask(CHARA_STATE_CLIMB);
		climber->removeEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);
		// �׷��� ������ ������ ���� ���¿��� Ʈ���Ŵ� �������� ����..
		return;
	}
	

	climber->addEnableStateMask(CHARA_STATE_DISABLE_GRAVITY);

	REAL32 dist_BackOrtho	= CalcDist_BackOrthoPlane( m_LadderInfo, &playerPos);

	if ( dist_BackOrtho <= playerRadius )	// ��ٸ� ��� �޸� ���� �ܰ�.... // CLIMB or Not // But Gravity Disabled..
	{
	
		REAL32 dist_TopOrtho = CalcDist_TopOrthoPlane(m_LadderInfo, &playerPos);

		if ( dist_TopOrtho >= playerRadius)		// �������� �ö� ���...
		{
			if (moveDir->z < 0.f)
			{
				REAL32 dist_Front = CalcDist_FrontPlane(m_LadderInfo, &playerPos);

				if ( dist_Front > 0.f )
				{
					// ���� ��ٸ� �� �� ������ ��ó�� �ٴٸ���, �������� ��ٸ� �������� ����..
					climber->addCharaStateMask(CHARA_STATE_CLIMB);

					// ����⿡�� �������� �����϶�, �Ʒ��� ���ϰ� �ִٸ� �׳� ���������� ��ȹ�� ����...
					if ( !isCharacterHeadedLower )
						m_LadderInfo->m_CurrAccDir = GetTriggerAxis(m_LadderInfo, 1);	
					else
					{
						m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
						move_context->setVelocity(0.f,0.f,0.f);
					}

					return;
				}

			}
			else
			if (moveDir->z > 0.f)
			{
				if ( isCharacterHeadedLower )
				{
					REAL32 dist_Front = CalcDist_FrontPlane(m_LadderInfo, &playerPos);

					if ( dist_Front > 0.f )		// ����⿡�� �������ε�, ���� �Ĺ��� �ִٸ� ����� ���������� �����Ѵ�..
					{
						// ���� ��ٸ� �� �� ������ ��ó�� �ٴٸ���, �������� ��ٸ� �������� ����..
						climber->addCharaStateMask(CHARA_STATE_CLIMB);
						m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
						return;
					}
					else
					{
						climber->removeCharaStateMask(CHARA_STATE_CLIMB);
						move_context->setVelocity(0.f,0.f,0.f);
						return;
					}
				}
			}

		//	if (isCharacterHeadedLower)				// �������̶� ���� ������������ �� �����̰� �������� ������ ���ݾ����ݾ� �����ϼ� �ִ�...
		//	{
		//		climber->addCharaStateMask(CHARA_STATE_CLIMB);
		//		m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
		//		move_context->setVelocity(0.f,0.f,0.f);
		//		return;
		//	}

			climber->removeCharaStateMask(CHARA_STATE_CLIMB);
			move_context->setVelocity(0.f,0.f,0.f);
			return;
		}
	
		if ( dist_BackOrtho > -playerRadius )
		{
			
			if ( moveDir->z < 0.f  )		// ���� ��..
			{
				//
				if (dist_TopOrtho < playerRadius)		// �� ��� �޺κ� ��ٸ� �𼭸��� �ε�ģ��..��������� �ٲ۴�...I3_YAXIS�� �ٲ۴�....
				{
					climber->addCharaStateMask(CHARA_STATE_CLIMB);

					// ����⿡�� �������� �����϶�, �Ʒ��� ���ϰ� �ִٸ� �׳� ���������� ��ȹ�� ����...
//					if ( !isCharacterHeadedLower )
						m_LadderInfo->m_CurrAccDir = &NEGATIVE_YAXIS;
//					else
//					{
//						m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
//						move_context->setVelocity(0.f,0.f,0.f);
//					}
					return;
				}
				else
				{
//					if ( !isCharacterHeadedLower )
//					{
						climber->removeCharaStateMask(CHARA_STATE_CLIMB);
						move_context->setVelocity(0.f,0.f,0.f);
//					}
//					else		// ���� ���������� ���� �������̰� ���´�...
//					{
//						climber->addCharaStateMask(CHARA_STATE_CLIMB);
//						m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
//						move_context->setVelocity(0.f,0.f,0.f);
//					}
					return;
				}
			}
			else
			{

				if ( isCharacterHeadedLower && moveDir->z == 0.f )	// ���� ���Ȱ�, ���� ���¶�� ������ ZEROACC�� ����� ���߰� ó��..
				{
					climber->addCharaStateMask(CHARA_STATE_CLIMB);
					m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
					move_context->setVelocity(0.f,0.f,0.f);
				}
				else
				{
					climber->removeCharaStateMask(CHARA_STATE_CLIMB);
					move_context->setVelocity(0.f,0.f,0.f);
				}
			}
			return;
		}
		
		if (bFloor)		// �Ʒ��� ���鿡 ���� ���
		{
			// ������ ���� ���̸� �������Ѿ��Ѵ�..
			if ( moveDir->z > 0.f && isCharacterHeadedLower)
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
				return;
			}			// �ݸ�, ���鿡 ���� �����϶��� �����Ҷ��� ���� �����̴� ������ �ʴ´�....�׳� ����...
			else	
			if ( moveDir->z < 0.f)			// �������̶�� ��ٸ��� ������ �ʴ� ������ �����Ѵ�.
			{
				if (!isCharacterHeadedLower)
				{
					climber->removeCharaStateMask(CHARA_STATE_CLIMB);
					move_context->setVelocity(0.f,0.f,0.f);
				}
				else
				{
					climber->addCharaStateMask(CHARA_STATE_CLIMB);
					m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
				}

				return;
			}
			else
			if (moveDir->z == 0.f)		// �������϶� �ƿ� �������̰� ����...
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
			}
		}
		
		REAL32 bottom_dist = CalcDist_BottomOrthoPlane(m_LadderInfo, &playerPos);

		if ( bottom_dist >= -playerRadius)			// �عٴ� ������� ���� �������� ��Ƴ��Ҵ�..
		{
			// ������ ���� ���̸� �������Ѿ��Ѵ�..
			if ( moveDir->z > 0.f && isCharacterHeadedLower)
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
				return;
			}			// �ݸ�, ���鿡 ���� �����϶��� �����Ҷ��� ���� �����̴� ������ �ʴ´�....�׳� ����...
			else	
			if ( moveDir->z < 0.f)			// �������̶�� ��ٸ��� ������ �ʴ� ������ �����Ѵ�.
			{
				if (!isCharacterHeadedLower)
				{
					climber->removeCharaStateMask(CHARA_STATE_CLIMB);
					move_context->setVelocity(0.f,0.f,0.f);
				}
				else
				{
					climber->addCharaStateMask(CHARA_STATE_CLIMB);
					m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
				}
				return;
			}
			else
			if (moveDir->z == 0.f)		// �������϶� �ƿ� �������̰� ����...
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
			}
		}


		/*
		if ( dist_FrontOrtho > playerRadius * .5f )		//  Ʈ���� �ո鿡 ������ �߰����� ��ģ ��쿣 ���� �ø����� �ʴ´�..
		{
				// ������ ���� ���̸� �������Ѿ��Ѵ�..
			if (moveDir->z > 0.f && isCharacterHeadedLower)
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
				return;
			}
			else	// ������ ���� �����ٸ� �÷����Ѵ�..??
			if ( moveDir->z < 0.f && isCharacterHeadedLower)
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
			}
			else
			if (moveDir->z == 0.f)		// �������϶� �ƿ� �������̰� ����...
			{
				climber->addCharaStateMask(CHARA_STATE_CLIMB);
				m_LadderInfo->m_CurrAccDir = &I3_ZEROVECTOR;
				move_context->setVelocity(0.f,0.f,0.f);
			}
			else
			{
				climber->removeCharaStateMask(CHARA_STATE_CLIMB);
				move_context->setVelocity(0.f,0.f,0.f);
			}
		}
		else		// �� ���� ���� ������ ��ٸ��� �����ų� ������ ���·� �����Ѵ�..	// ������ �����ų� �������� ���� ���� ���ο� ���� ������ �����ȴ�..
		*/
		{
			climber->addCharaStateMask(CHARA_STATE_CLIMB);
			if (!isCharacterHeadedLower)
				m_LadderInfo->m_CurrAccDir = GetTriggerAxis(m_LadderInfo, 1);		// �������� ����Ʈ �������..
			else
				m_LadderInfo->m_CurrAccDir = &m_LadderInfo->m_vNegativeLadderAxisY;
		}
/*
		if ( (bFloor && moveDir->z < 0.f) || dist_FrontOrtho > playerRadius * .5f )
		{
			climber->removeCharaStateMask(CHARA_STATE_CLIMB);
			move_context->setVelocity(0.f,0.f,0.f);
		}
		else
		{
			climber->addCharaStateMask(CHARA_STATE_CLIMB);
			m_LadderInfo->m_CurrAccDir = GetTriggerAxis(m_LadderInfo, 1);		// �������� ����Ʈ �������..
		}
*/
		return;
	}
	
	// ��ٸ� �ĸ��� ��� ����..

	if ( moveDir->z > 0.f )		// ��ٸ� ������ ����� ���⿡ �ִٸ� Ʈ���� ����� �����Ѵ�.
	{
		ResetClimberFromLadderInfo(m_LadderInfo);
		return;
	}
		
}


void	CLadderTrigger::OnUpdate( REAL32 rDeltaSeconds)
{
	i3Trigger::OnUpdate(rDeltaSeconds);

	CLadderTrigger::UpdateTriggerImp();
}
