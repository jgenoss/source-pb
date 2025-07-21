#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaAnimControl1PV.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaAnimControl1PV);

void CGameCharaAnimControl1PV::OnChangeAIContext( const char * pszPVState)
{
	CGameCharaAnimControl::OnChangeAIContext( "1PV:");
	//	1��Ī �϶�
	i3AIContext * pAILower = getAnimContext()->getAILower();

	if( pAILower != nullptr)
	{	
		pAILower->SetExclusiveEnable( false, true);
		getAnimContext()->setAILower( nullptr);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameCharaAnimControl1PV::OnMoveRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaAnimControl::OnMoveRun( pCtx, tm);

	REAL32 rElapseTime = getAnimContext()->getRunElapseTime();
	REAL32 rDuration = getAnimContext()->getRunDuration();

	//	���� �̵��Ҷ��� ���ڱ� �Ҹ�������...���� �ɾ� �̵��ϸ� ���ڱ� �Ҹ� �ȳ����� �Ѵ�.
	if( getActionContext()->isLower( CHARA_LOWER_RUN) ||
		getActionContext()->isLower( CHARA_LOWER_WALK))
	{	
		//	if( isNetwork()) I3TRACE("ON MOVE RUN : %f, Elapse : %f\n", tm, rElapseTime);
		//I3TRACE("ON MOVE RUN elapse: %f, duration : %f\n", rElapseTime, rDuration);
		//	���ڱ� �Ҹ� �÷���~
		if( rElapseTime <= tm)
		{
			if( getActionContext()->isUpper( CHARA_UPPER_MOVE ))
			{
				i3AIState * pAI = getAnimContext()->getAIUpper()->getCurrentAIState();
				if( pAI != nullptr)
				{
					rDuration = pAI->getActualDuration();//	1��Ī �ִϸ��̼��� ��ü�� �÷������� �ʱ� ������ ���� �ð� ����
				}
			}
			else
			{
				REAL32	rScaleRate = 0.0f;
				if(m_pOwner->IsDino() == false)
				{

					WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
					if( pWeapon != nullptr )
					{
						if( getActionContext()->isBody( CHARA_BODYLOWER_STAND))
							rScaleRate =  pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_NORMAL) * 0.01f;
						else if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
							rScaleRate = pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_SIT) * 0.01f;

						if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
						{	//	���� �������� �����ÿ��� ����ȵǰ�...
							if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) && 
								!getActionContext()->isUpper( CHARA_UPPER_LOADBULLET) &&
								!getActionContext()->isUpper( CHARA_UPPER_FIREBULLET))	
								rScaleRate *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_ZOOM) * 0.01f;
						}

						if( getActionContext()->isUpper( CHARA_UPPER_FIRE) ||
							getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) ||
							(getActionContext()->getUpperLeft() == CHARA_UPPER_FIRE) )			//	�߻� �����϶�
						{
							rScaleRate *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_FIRE) * 0.01f;
						}

						rDuration = 0.544f * (1.0f / rScaleRate);
					}
				}
			}

			if( rDuration < I3_EPS)
				rDuration = 0.66f ;

			getAnimContext()->setLeftStepTime( rDuration * 0.822f);
			getAnimContext()->setRightStepTime( rDuration * 0.322f);
		}

		rElapseTime = _PlayFootStepSound( rDuration, rElapseTime, tm);
	}

	getAnimContext()->setRunElapseTime( rElapseTime);
	getAnimContext()->setRunDuration( rDuration);
}


