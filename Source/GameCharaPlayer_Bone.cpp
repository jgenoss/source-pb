#include "pch.h"
#include "GameCharaPlayer.h"
#include "GlobalVariables.h"
#include "StageBattle.h"

/*
void CGameCharaPlayer::_InitBones(void)
{
	static char s_szBones[][32] =
	{
		"Root",
		"Pelvis",
		"IK",
		"Spine1",
		"Spine2",
		"Spine3",
		"Neck",
		"Head",
		"L Foot",
		"R Foot",
		"L Forearm",
		"R Forearm",
	};
	INT32 i;

	// Bones
	for( i = 0; i < BONE_MAX; i++)
	{
		m_idxBone[i] = m_pBody->FindBoneByName( s_szBones[i]);
		I3ASSERT( m_idxBone[i] != -1);

		i3Matrix::Identity( &m_Bone[i]);
	}

	i3Matrix::Identity( &m_mtxPivot);
	
	m_PivotYaw = 0.0f;
	m_PivotYawTarget = 0.0f;
	m_timeYawBlend = 0.0f;
	m_timeTargetYawBlend;
	m_bValidPivot = false;
}

void CGameCharaPlayer::_calcSpineBones(void)
{
	INT32 i;
	MATRIX worldTx;
	MATRIX * pLast = &worldTx;

	i3Matrix::Copy( &worldTx, GetMatrix());

	// Head Bone�� ����
	for( i = 0; i < BONE_MAX - 1; i++)
	{
		i3Matrix::Mul( getBoneMatrix( i), m_pBody->getCacheMatrix( m_idxBone[i]), pLast);

		pLast = getBoneMatrix( i);
	}
}

void CGameCharaPlayer::_UpdateAnim( REAL32 tm)
{
	i3SceneTracer * pTracer;

	pTracer = g_pFramework->GetViewer()->GetSceneTracer();

	pTracer->GetModelViewMatrixStack()->Reset();
	
	if( g_pGameContext->IsP2PActivate() )
	{
		pTracer->SetTime( tm);
		pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());
	}
	else
	{
		// GUI���� character animation�� 2�谡 �Ǵ� ���� �����ϴ� - praptor
		pTracer->SetTime( 0.0f);
	}
	
	m_pBody->UpdateAnim( pTracer);
}


void CGameCharaPlayer::_TransformWorldSpace( REAL32 tm)
{
	i3SceneTracer * pTracer;

	pTracer = g_pFramework->GetViewer()->GetSceneTracer();

	pTracer->GetModelViewMatrixStack()->Reset();
	pTracer->SetTime( tm);
	pTracer->SetViewMatrix( g_pCamera->getCamMatrix(), g_pCamera->getCamMatrix());

	GetSceneObject()->OnUpdate( pTracer);
	
	i3Matrix::Copy( getBoneMatrix( BONE_IK), m_pBody->getCacheMatrix( m_idxBone[ BONE_IK]));
	i3Matrix::Copy( getBoneMatrix( BONE_NECK), m_pBody->getCacheMatrix( m_idxBone[ BONE_NECK]));
	i3Matrix::Copy( getBoneMatrix( BONE_HEAD), m_pBody->getCacheMatrix( m_idxBone[ BONE_HEAD]));
}

inline REAL32 _norm_angle( REAL32 rad)
{
	while( rad < 0.0f)		rad += I3_2PI;

	return rad;
}

void CGameCharaPlayer::_calcTheta( REAL32 tm)
{
	VEC3D vTempPos;
	REAL32 recoilHorz;

	if( m_RecoilData.m_RecoilHorzTime < m_RecoilData.m_RecoilHorzTimeTarget)
	{
		// ���� �ݵ��� ����
		if( m_RecoilData.m_HorzState != RECOIL_HORZ_FIRING)
		{
			// ���Ӱ� ���� �ݵ��� ����Ǳ� �����ߴ�. (���ο� ����)
			m_RecoilData.m_HorzStartAngle = m_Theta;
			m_RecoilData.m_HorzState = RECOIL_HORZ_FIRING;			
		}

		recoilHorz = m_RecoilData.m_RecoilHorzAngle * (tm * m_RecoilData.m_rRecoilHorzTimeTarget);

		m_Theta += I3_DEG2RAD( recoilHorz);

		m_RecoilData.m_RecoilHorzTime += tm;
	}
	else
	{
		// ���� �ݵ��� ������� �ʴ´�.
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();

		if( (pWeapon != NULL) && (m_RecoilData.m_HorzState == RECOIL_HORZ_FIRING) )//&& (STATE_IsUpper( CHARA_UPPER_FIRE) == false))
		{
			// �߻� ���ε��� getAutoFireCount() == 0�̸� ���縦 ���� ��Ȳ

			m_RecoilData.m_HorzState = RECOIL_HORZ_RESTORE;		// ������ Theta�� �ǵ��ư��� ���� ��ȯ.
			m_RecoilData.m_timeHorzRestore = 0.0f;
		}
		
		if( m_RecoilData.m_HorzState == RECOIL_HORZ_RESTORE)
		{
			// Theta�� Restore ���¶��...
			m_Theta = _norm_angle( m_Theta);
			REAL32 oldTheta = m_Theta;
			m_RecoilData.m_HorzStartAngle = _norm_angle( m_RecoilData.m_HorzStartAngle);

			REAL32 diff = m_RecoilData.m_HorzStartAngle - m_Theta;

			if( m_RecoilData.m_HorzStartAngle > oldTheta)
			{
				if( oldTheta >= m_RecoilData.m_HorzStartAngle)
				{
					m_RecoilData.m_timeHorzRestore = 2.0f;
					m_Theta = m_RecoilData.m_HorzStartAngle;
				}
			}
			else
			{
				if( oldTheta <= m_RecoilData.m_HorzStartAngle)
				{
					m_RecoilData.m_timeHorzRestore = 2.0f;
					m_Theta = m_RecoilData.m_HorzStartAngle;
				}
			}

			m_Theta += (diff) * 0.07f;

			m_RecoilData.m_timeHorzRestore += tm;

			if( m_RecoilData.m_timeHorzRestore > 2.0f)
			{
				m_RecoilData.m_timeHorzRestore = 0.0f;
				m_RecoilData.m_HorzState = RECOIL_HORZ_NA;
			}
		}
	}

	i3Vector::Copy( &vTempPos, GetPos());

	i3Matrix::SetRotateY( GetMatrix(), m_Theta + I3_DEG2RAD( m_ScopeWaveData.m_ScopeWaveTheta));
	i3Matrix::PostTranslate( GetMatrix(), &vTempPos);
}

void CGameCharaPlayer::_AdjustPivot( REAL32 tm, MATRIX * pInvMtx)
{
	MATRIX tmp, inv;
	REAL32 dot;
	VEC3D at1, at2;

	if( m_bValidPivot)
	{
		BOOL bTrue1PV = FALSE;
		if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{	
			CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
			if( pCharacter == this ) bTrue1PV = TRUE;
		}

		i3Matrix::Mul( &tmp, &m_mtxPivot, getBoneMatrix( BONE_IK));

		i3Matrix::Transpose3x3( &inv, GetMatrix());

		// ȸ�� �� ���ϱ�
		if( !bTrue1PV )
		{
			i3Vector::Normalize( &at1, i3Matrix::GetAt( &tmp));
			i3Vector::Normalize( &at2, i3Matrix::GetAt( GetMatrix()));

			i3Vector::Cross( &m_PivotAxis, &at1, &at2);
			i3Vector::TransformNormal( &m_PivotAxis, &m_PivotAxis, pInvMtx);
			i3Vector::Normalize( &m_PivotAxis, &m_PivotAxis);

			////////////////////////////////////////////////////////
			// Yaw ȸ���� �����ϱ� ���� ���� ���
			{
				dot = i3Vector::Dot( &at1, &at2);
				if( dot > 1.0f)		dot = 1.0f;
				if( dot < -1.0f)	dot = -1.0f;

				m_PivotYawTarget = acosf( dot);
			}
		}
		else
		{
			i3Vector::Copy( &at1, i3Matrix::GetAt( &tmp));
			at1.y = 0.0f;

			i3Vector::Normalize( &at1, &at1);

			i3Vector::TransformNormal( &at1, &at1, &inv);

			////////////////////////////////////////////////////////
			// Yaw ȸ���� �����ϱ� ���� ���� ���
			{
				dot = i3Vector::Dot( &at1, &I3_ZAXIS);
				if( dot > 1.0f)		dot = 1.0f;
				if( dot < -1.0f)	dot = -1.0f;

				m_PivotYawTarget = acos( dot);
			}

			// ĳ������ Local-Space�� ��ȯ
			

			// acos�� Minus ������ ������ �����Ƿ�, Local-Space������ ��ȣ��
			// ���Ͽ� ���� ��������, ���� ���������� Ȯ�� 
			if( at1.x > 0.0)
				m_PivotYawTarget = -m_PivotYawTarget;
		}

		//////////////////////////////////////////////////////////
		// Roll ȸ���� �����ϱ� ���� ���� ���
		VEC3D up1, up2;

		i3Vector::Copy( &up1, i3Matrix::GetUp( &tmp));
		i3Vector::TransformNormal( &up2, &up1, &inv);

		//up1.z = 0.0f;
		up2.z = 0.0f;

		i3Vector::Normalize( &up2, &up2);
		dot = i3Vector::Dot( &up2, &I3_YAXIS);

		if( dot > 1.0f)		dot = 1.0f;
		if( dot < -1.0f)	dot = -1.0f;
		
		m_PivotRoll = -acos( dot);

		{
			if( up2.x > 0.0f)
				m_PivotRoll *= -1.0f;
		}
	}
	else
	{
		m_PivotYawTarget = 0.0f;
		m_PivotRoll = 0.0f;
	}

	if( m_PivotYawTarget != m_PivotYaw)
	{
		REAL32 t;

		m_timeYawBlend += tm;
		if( m_timeYawBlend >= m_timeTargetYawBlend)
		{
			m_timeYawBlend = m_timeTargetYawBlend;
			m_PivotYaw = m_PivotYawTarget;
		}
		else
		{
			if( m_timeTargetYawBlend > I3_EPS)
				t = m_timeYawBlend / m_timeTargetYawBlend;
			else
				t = 1.0f;

			m_PivotYaw = m_PivotYaw + ((m_PivotYawTarget - m_PivotYaw) * t);
		}
		//I3TRACE( "YAW : %f, %f, %f\n", I3_RAD2DEG( m_PivotYaw), m_PivotYawTarget, t);
	}
}

#define RATE		0.25f

void CGameCharaPlayer::_calcRho( REAL32 tm)
{
	//Rho�� ���� ���� ��ü�� ���θ���.
	MATRIX tmp, mod;
	VEC3D axis;
	REAL32 rate;

	// 3��Ī�� ���� IK, Spine1, Spine2, Spine3 Bone���� ��� ���ݾ� �������� ȸ����Ű����
	// 1��Ī�� ���� IK Bone���� ȸ����Ų��.

	BOOL bTrue1PV = FALSE;
	if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bTrue1PV = TRUE;
	}

	if( bTrue1PV )
	{
		rate = 1.0f;
		i3Matrix::Inverse( &tmp, NULL, getBoneMatrix( BONE_NECK));
	}
	else
	{
		rate = RATE;
		i3Matrix::Inverse( &tmp, NULL, getBoneMatrix( BONE_IK));
	}

	//	���� �ִϸ��̼� �÷������� �ƴҶ��� �㸮 ������ �ؾ��Ѵ�.
	if( (getStateMask() & CHARA_STATE_USED_COMBINE_ANIM) == 0)
		_AdjustPivot( tm, &tmp);

	{
		REAL32 recoil = I3_DEG2RAD( m_RecoilData.m_RecoilAngle + m_ScopeWaveData.m_ScopeWaveRho);
		REAL32 rho = m_Rho;

		if( getActionContext()->isLower( CHARA_LOWER_DEATH))		//	�״� ���۶��� Rho���� �������� �ʰ� 
		{
			rho = 0.f;
			recoil = 0.f;
		}

		i3Vector::Copy( &axis, i3Matrix::GetRight( GetMatrix() ) );
		i3Vector::TransformNormal( &axis, &axis, &tmp);
		i3Vector::Normalize( &axis, &axis);

		i3Matrix::Identity( &mod);
		i3Matrix::SetRotateAxis( &mod, &axis, (rho + recoil) * rate);
	}

	// ����� ��鸮�� ���� �����ϱ� ���� ó��
	{
		i3Vector::Copy( &axis, i3Matrix::GetAt( GetMatrix() ) );
		i3Vector::TransformNormal( &axis, &axis, &tmp);
		i3Vector::Normalize( &axis, &axis);

		i3Matrix::PostRotateAxis( &mod, &axis, m_PivotRoll * rate);
	}

	if( !getActionContext()->isLower( CHARA_LOWER_DEATH))		//	�״� ���� �ܴ̿� �Ǻ� ����
	{		
		REAL32 yaw = m_PivotYaw * rate;

		if( m_bValidPivot && (bTrue1PV == false))
			i3Matrix::PostRotateAxis( &mod, &m_PivotAxis, yaw);
		else
			i3Matrix::PostRotateAxis( &mod, i3Matrix::GetRight( &mod), yaw);
	}

	if( bTrue1PV )
	{
		// ��� ��鸲.
		if( m_bShoveActive)
		{
			m_timeShove += tm;
			if( m_timeShove > 0.2f)
			{
				if( m_bShoveReturn == false)
				{
					m_bShoveReturn = true;
					// �ΰ��� �����ӵ�...
					// �� �ð��� �����鼭 ���ʹ� ��ݿ� ���� �������� ������ ���� �ڼ���
					// �ǵ����� �Ѵ�.
					i3Quat::Copy( &m_quatShoveTarget, &m_quatShove);
				}

				//m_timeShove = 0.0f;

				m_acclShove = m_tShove * -0.1f;
			}
			else
			{
			}

			m_velShove += m_acclShove * tm;
			m_tShove += m_velShove;

			if((m_bShoveReturn) && ( i3Math::abs( m_acclShove) < 0.005f))
			{
				m_tShove = 0.0f;
				m_bShoveActive = false;
				i3Matrix::Identity( &m_ShoveMatrix);
			}
			else
			{
				//REAL32 t = min( m_tShove, 1.0f);
				REAL32 t = m_tShove;

				i3Quat::Slerp( &m_quatShove, &m_quatShoveStart, &m_quatShoveTarget, t);
				//i3Quat::Dump( "", &m_quatShove);
				i3Matrix::SetRotateQuat( &m_ShoveMatrix, &m_quatShove);
			}

			i3Matrix::Mul( &mod, &mod, &m_ShoveMatrix);

			I3TRACE( "%f %f %f\n", m_tShove, m_velShove, m_acclShove);
		}

		addX(i3Matrix::GetPos( &mod), m_pCameraContext->getIKHeight());

		MATRIX * pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_NECK]);

		i3Matrix::Mul( pOut, &mod, pOut);
	}
	else
	{
		MATRIX * pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_IK]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE1]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE2]);
		i3Matrix::Mul( pOut, &mod, pOut);

		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_SPINE3]);
		i3Matrix::Mul( pOut, &mod, pOut);
	}	
}

void CGameCharaPlayer::Shove( VEC3D * pDir, REAL32 force)
{
	VEC3D dir, axis;

	i3Vector::Normalize( &dir, pDir);

	REAL32 dot = 1.0f - i3Vector::Dot( &dir, &I3_YAXIS);
	if( i3Math::abs( dot) <= I3_EPS)
	{
		return;
	}

	i3Vector::Cross( &axis, &dir, &I3_YAXIS);
	i3Vector::Normalize( &axis, &axis);		// World-Space������ ȸ���� (Local-Space�� ��ȯ�Ǿ�� �Ѵ�.)

	// World-Space�� ȸ������ Local Space�� �ٲ۴�.
	MATRIX tmp;

	i3Matrix::Inverse( &tmp, NULL, getBoneMatrix( BONE_NECK));
	i3Vector::TransformNormal( &axis, &axis, &tmp);

	//i3Vector::Copy( &axis, i3Matrix::GetRight( getBoneMatrix( BONE_SPINE3)));

	// ���� Shove Rotation���� �߰��� ������
	i3Quat::Copy( &m_quatShoveTarget, &m_quatShove);
	i3Quat::RotateAxis( &m_quatShoveTarget, &axis, force);

	m_acclShove = -1.2f;
	m_velShove = 0.2f;
	m_tShove = 0.0f;
	m_bShoveActive = true;
	m_bShoveReturn = false;
	m_timeShove = 0.0f;
}
*/

/*
REAL32 CGameCharaPlayer::getIKHeightForCam(void)
{
	BOOL bTrue1PV = FALSE;
	if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
		CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == this ) bTrue1PV = TRUE;
	}	

	//if( is1PV())
	if( bTrue1PV )
	{
		return m_pDBInfo->m_vCamIKHeight.vec[0] + m_rIKHeight;
	}
	else
	{
		return m_pDBInfo->m_vCamIKHeight.vec[m_StateForCam];
	}
}
*/


/*
void CGameCharaPlayer::_calcDeviation( REAL32 tm)
{
	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL)
		return;

	CWeaponInfo * pInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pInfo != NULL);

	REAL32 base = pInfo->m_rCrossHairMin;
	REAL32 newDeviation;

	newDeviation = base;
	
	if( getActionContext()->isBody( CHARA_BODY_STAND))
	{
		// Stand
		newDeviation += (base * pInfo->m_rCrossHairSizeStand);
	}

	// xgkick
	if( isStateMask( CHARA_STATE_ZOOM))
	{
		newDeviation += (base * pInfo->m_rCrossHairSizeZoom);
	}

	switch( getActionContext()->getLower())
	{
		case CHARA_LOWER_MOVE :	
			if( getActionContext()->isBody( CHARA_BODY_CROUCH))
				newDeviation += (base * pInfo->m_rCrossHairSizeMoveSit);	
			else
				newDeviation += (base * pInfo->m_rCrossHairSizeMove);	
			break;

		case CHARA_LOWER_WALK :	
			if( getActionContext()->isBody( CHARA_BODY_CROUCH))
				newDeviation += (base * pInfo->m_rCrossHairSizeWalkSit);	
			else
				newDeviation += (base * pInfo->m_rCrossHairSizeWalk);	
			break;

		case CHARA_LOWER_JUMP : newDeviation += (base * pInfo->m_rCrossHairSizeJump);	break;
	}

	switch( getActionContext()->getUpper())
	{
		case CHARA_UPPER_DAMAGE :	newDeviation += (base * pInfo->m_rCrossHairSizeDamage);	break;
	}

	if( newDeviation < 0.0f)
		newDeviation = 0.0f;

	if( newDeviation > pInfo->m_rCrossHairMax)
		newDeviation = pInfo->m_rCrossHairMax;

	if( newDeviation != m_DeviationData.m_DeviationTargetAngle)
	{
		REAL32 speed = pWeapon->getCrosshairSpeedFactor() * pInfo->m_CrossHairContractTime;

		if( newDeviation > m_DeviationData.m_DeviationAngle)
		{
			// Crosshair�� Ŀ���� ���� ���� ������ �Ѵ�.
			speed = pInfo->m_CrossHairExpandTime;
		}

		m_DeviationData.m_DeviationTargetAngle = newDeviation;
		m_DeviationData.m_DeviationStartAngle = m_DeviationData.m_DeviationAngle;
		m_DeviationData.m_DeviationState = DEVIATION_CHANGE;
		m_DeviationData.m_DeviationTime = 0.0f;
		m_DeviationData.m_DeviationTargetTime = speed;

		//I3TRACE( "DEVIATION TARGET : %f\n", newDeviation);
	}
}

void CGameCharaPlayer::_calcRecoil( REAL32 tm)
{
	m_RecoilData._calcRecoil( tm );
	
	_calcDeviation( tm);

	m_DeviationData._calcDeviation( tm );

	//_calcScopeWave( tm);
}

void CGameCharaPlayer::_calcScopeWave( REAL32 tm)
{
	m_ScopeWaveData._calcScopeWave( tm );
}

*/

