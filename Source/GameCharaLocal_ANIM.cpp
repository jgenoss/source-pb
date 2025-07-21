#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaLocal.h"

/*
void CGameCharaLocal::_WeaponStateIdle()
{
	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pCurWeapon != NULL);

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != NULL);

	WEAPON_STATE		nState = pCurWeapon->getState();

	//	�߻��� ������ Idle ���·� ���ƿ´�.
	if( nState != WEAPON_STATE_FIRE )
	{
		WEAPON_CLASS_TYPE	nType = pCurWeaponInfo->m_TypeClass;

		// ���⿡ ���� ����� �����ϵ��� �Ѵ�.
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE);

		if( getActionContext()->isBody2( CHARA_BODY_IDLE) )
		{	// 3��Ī �丸 �÷���
			_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV());
		}
		
		//	Mission ���� ó�� (���⼭ ó�� �ؾ� ��)
		if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION)
		{
			if( pCurWeaponInfo->m_nNumber == WEAPON_NUM_MISSION_C4)
			{
#if defined( I3_DEBUG )
				if( g_pConfig->m_bNetworking)		//	Network
				{
				}
				else								//	�����ϱ�
				{
					//	��ź ������ ��������
					if( getWeaponContext()->getControl()->OnWeaponDropToWorld( getWeaponContext()->getCurrentSlotNum(), GetPos() ) )
					{
						//	Drop �ƴ�.
						pCurWeapon = NULL;
						pCurWeaponInfo = NULL;
					}
				}
#endif

				removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
			}
		}

		//	�Ϲ� ����
		switch( nState)
		{
			case  WEAPON_STATE_NEEDTO_LOADMAG:
				// �߻� ��, �Ѿ��� ���� źâ�� ���� ������ �ϴ� ����
				//	�Ϲ� ���� Reload ����( LoadMagazine) �ϳ��� �����ϴ�.
				ACTION_LoadMagazine();
				return;

			case WEAPON_STATE_NEEDTO_LOADBULLET :
				// �߻� ��, ������ ���־�� �ϴ� ����
				ACTION_LoadBullet();
				return;
		
			case WEAPON_STATE_NEEDTO_FIRE:
				//	���� �߻��մϴ�.
				//ACTION_Fire();
				getCommandContext()->Command(CHARA_CMD_PRIMATTACK);
				return;

			case WEAPON_STATE_EMPTY_MAGAZINE:
				{
					//	����ź�̳� �������� ��� EMPTY�� 0�� ����� ��ü
					if( nType == WEAPON_CLASS_THROWING || nType == WEAPON_CLASS_ITEM || nType == WEAPON_CLASS_MISSION)
					{
						if( !ACTION_SwapWeapon(0) )
						{
							if( !ACTION_SwapWeapon(1) )
							{
								ACTION_SwapWeapon(2);
							}
						}
					}
					else
						ACTION_SwapNextWeapon();
				}
				return;

			case WEAPON_STATE_NEEDTO_QUICKLOAD :
				ACTION_LoadMagazineDual();
				return;
		}

		//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
		if( !getActionContext()->isLower( CHARA_LOWER_DEATH) )
		{
			if( nState != WEAPON_STATE_FIRING )
			{
				if( pCurWeaponInfo != NULL )
				{
					if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING)
					{
						if( getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK))
						{
							STATE_SET_UPPER( CHARA_UPPER_MOVE);
							ANIM_PLAY_UPPER(ID_UPPER_SWAP_WEAPON);
						}
						else
						{
							STATE_SET_UPPER( CHARA_UPPER_IDLE);
							ANIM_PLAY_UPPER(ID_UPPER_SWAP_WEAPON);
						}
					}
					else
					{
						if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
						{
							ActionParameter * param = GET_PREPARE_PARAM;
							i3Vector::Copy(&param->_dir, getMoveContext()->getMoveDir());

							CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
						}
						else
						{
							ACTION_UPPER_RETURN;
						}
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Anim Callback
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameCharaLocal::OnIdleRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnIdleRun( pCtx, tm);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_WEAPONCHANGE);
}

void CGameCharaLocal::OnAttackRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnAttackRun( pCtx, tm);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	WeaponStateIdle();
}

void CGameCharaLocal::OnJumpStartRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnJumpStartRun( pCtx, tm );

	if( !getActionContext()->isLower( CHARA_LOWER_JUMP) || getActionContext()->isLower( CHARA_LOWER_DEATH))	return;

	if( isStateMask( CHARA_STATE_FLOOR) || isStateMask( CHARA_STATE_CLIMB))		//	���� ������
	{	
		CHARA_ACTION_LOWER lower_state = CHARA_LOWER_MOVE;
		
		//	������ �ٽ� �ѹ� ����Ű �Է��� üũ�մϴ�. (�׳� ���� ���� �ٷ� �̵�����)
		UINT64 key = getKeyPressInput();

		if( key & GAME_KEY_MOVE_FORWARD )
		{
			setZ( getMoveContext()->getMoveDir(), 1.f);
			//	m_MoveDir.z = 1.0f;		
		}
		else if( key & GAME_KEY_MOVE_BACKWARD )
		{
			setZ( getMoveContext()->getMoveDir(), -1.f);
		//	m_MoveDir.z = -1.0f;
		}
		else
		{
			setZ( getMoveContext()->getMoveDir(), 0.f);
			//	m_MoveDir.z = 0.0f;
		}

		if( key & GAME_KEY_MOVE_LEFT )
		{
			setX( getMoveContext()->getMoveDir(), 1.f);
		//	m_MoveDir.x = 1.0f;		
		}
		else if( key & GAME_KEY_MOVE_RIGHT)
		{
			setX( getMoveContext()->getMoveDir(), -1.f);
		//	m_MoveDir.x = -1.0f;
		}
		else
		{
			setX( getMoveContext()->getMoveDir(), 0.f);
			//m_MoveDir.x = 0.0f;
		}

		if( (getX( getMoveContext()->getMoveDir()) == 0.0f) &&
			(getY( getMoveContext()->getMoveDir()) == 0.0f) &&
			(getZ( getMoveContext()->getMoveDir()) == 0.0f))
			lower_state = CHARA_LOWER_IDLE;

		if( getActionContext()->isBody2( CHARA_BODY_IDLE))
		{
			_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV() );
		}
		
		//	���ڸ� ������. -> ���� �� �ִϸ��̼�����
		if( lower_state == CHARA_LOWER_IDLE)
		{
			//ANIM_playLower( CHARA_LOWER_IDLE, ID_LOWER_JUMP_END);
			CALL_ACTION_COMMAND( ACTION_CMD_JUMPLOWER_FINISH, NULL);

			//	(���� ������ �������� �����Ѵ�.)	
			m_pAnimContext->setOldLowerID( ID_LOWER_IDLE);

			if( getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
				getActionContext()->isUpper( CHARA_UPPER_IDLE))
			{
				//ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_JUMP_END);
				CALL_ACTION_COMMAND( ACTION_CMD_JUMPUPPER_FINISH, NULL);
					
				//	(���� ������ �������� �����Ѵ�.)
				m_pAnimContext->setOldUpperID( ID_UPPER_IDLE);
			}
		}
		//	�̵��� ������. -> �̵� �ִϸ��̼�����
		else
		{
			//	�ȱ�
			if( key & GAME_KEY_ACTION_FOOTSTEP)
			{
				ID_LOWER_AI nId = _ANIM_getWalkId( getMoveContext()->getMoveDir());

				ANIM_playLower( CHARA_LOWER_WALK, nId);	
			}
			//	�ٱ�
			else	
			{
				ID_LOWER_AI nId = _ANIM_getMoveId( getMoveContext()->getMoveDir());

				ANIM_playLower( CHARA_LOWER_MOVE, nId);	
			}

			if( getActionContext()->isUpper( CHARA_UPPER_JUMP) ||
				getActionContext()->isUpper( CHARA_UPPER_IDLE) )
			{
				//ACTION_MoveUpper();
				ActionParameter * param = GET_PREPARE_PARAM;
				i3Vector::Copy(&param->_dir, getMoveContext()->getMoveDir());

				CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
			}
		}

		removeEnableStateMask( CHARA_STATE_DISABLE_MOVE	| CHARA_STATE_DISABLE_CROUCH );

		//	���� ������ �ð� ����
		//m_fJumpDelayElapsedTime = getDBInfo()->m_rJumpDelay;
		setJumpDelayElapsedTime( 0.f);
	}
}

void CGameCharaLocal::OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnLoadMagazineRun( pCtx, tm);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pWeapon != NULL );

	switch( pWeapon->getState())
	{
		case  WEAPON_STATE_NEEDTO_LOADMAG:
			ACTION_LoadMagazine();
			break;

		case WEAPON_STATE_NEEDTO_LOADBULLET :
			ACTION_LoadBullet();
			break;
		case WEAPON_STATE_READY:
			{	
				//	���� �ܿ��� �Ϲ� ����� Idle ���·� ���ư��� �ϱ� ������
				//	OnLoadBullet�� ȣ������ �ʱ� ������ (AIState�� �����Ƿ�) ȣ���ؾ� �մϴ�.
				OnLoadBulletRun( pCtx, tm);	
			}
			break;
		default :
			break;
	}
}

void CGameCharaLocal::OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnLoadBulletRun( pCtx, tm);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pWeapon != NULL );

	//	������ ������
	if( pWeapon->getState() == WEAPON_STATE_READY)
	{
		_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, !is1PV());		//	Idle ���´� AttackIdle ���·� �Ѵ�.
		if( isStateMask( CHARA_STATE_ZOOM ) )
			ACTION_CancelZoom();
	
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE 
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_ZOOM | CHARA_STATE_DISABLE_WEAPONCHANGE);

		//	���� �ൿ���� ���ƿɴϴ�.. (Idle �̳� Move)
		if( !getActionContext()->isLower( CHARA_LOWER_DEATH))
		{
			if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
			{
				//ACTION_MoveUpper();
				ActionParameter * param = GET_PREPARE_PARAM;
				i3Vector::Copy(&param->_dir, getMoveContext()->getMoveDir());

				CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
			}
			else
			{
				//ANIM_playUpperReturn( !is1PV());
				ACTION_UPPER_RETURN;
			}
		}

		//I3TRACE("WEAPON_STATE_READY\n");
	}
}

void CGameCharaLocal::OnWeaponSwapRun( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnWeaponSwapRun( pCtx, tm);

	if( !getActionContext()->isUpper( CHARA_UPPER_SWAPWEAPON) ||
		getActionContext()->isLower( CHARA_LOWER_DEATH))	return;

	I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pWeapon != NULL);

	//	�ٸ� �������� ��ȯ�� �ִϸ��̼��� �ٸ������ ���� �����̸� ���� �������� ���ư��� �ʴ´�.		
	//	������ ������.
	if( _ANIM_getUpperLocalTime() >= _ANIM_getUpperDuration())
	{
		//	���� �����̾��� Upper�� Idle �Ǵ� Move�� ���ƿ´�.
		if( !i3Vector::IsSame( getMoveContext()->getMoveDir(), &I3_ZEROVECTOR ) )
		{
			//ACTION_MoveUpper();
			ActionParameter * param = GET_PREPARE_PARAM;
			i3Vector::Copy(&param->_dir, getMoveContext()->getMoveDir());

			CALL_ACTION_COMMAND(ACTION_CMD_RUNUPPER, param);
		}
		else
		{
			//ANIM_playUpperReturn( !is1PV());
			ACTION_UPPER_RETURN;
		}

		if( pWeapon != NULL )
		{
			pWeapon->ReturnToReady();
		}
		
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		//removeEnableStateMask( CHARA_STATE_DISABLE_BODY2);
		//removeStateMask( CHARA_STATE_DISABLE_DODGE);
	}
	else	//	������
	{
		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		//addStateMask( CHARA_STATE_DISABLE_BODY2);
		//addStateMask( CHARA_STATE_DISABLE_DODGE);
	}

	removeEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE);
}

*/
