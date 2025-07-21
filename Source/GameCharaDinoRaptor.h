#if !defined( __GAME_CHARA_DINO_RAPTOR_H__)
#define __GAME_CHARA_DINO_RAPTOR_H__

#include "GameCharaDino.h"

class CGameCharaDinoRaptor : public CGameCharaDino
{
    I3_CLASS_DEFINE( CGameCharaDinoRaptor, CGameCharaDino);

protected:
	INT32				m_idxRaptorBuffEffect = 0;

protected:
	/** \brief ��ü Bone Count */
	virtual INT32		_GetUpperBoneCount( void) const override { return CHARA_UPPER_RAPTOR_BONE_COUNT;}

	virtual void		_CreateDino( void) override;

	virtual void		_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;
	virtual void		_PlayDownfallSound( void) override;

public:
	CGameCharaDinoRaptor();
	
	// ����	
	
public:

	/**
	 * \fn	ID_UPPER_DINO_AI CGameCharaDinoRaptor::DinoAttack(WEAPON::FIRE_ORDER fireOrder);
	 *
	 * \brief	CGameCharaInputControlLocal::ProcessDinoAttack���� �ش� �Լ��� ȣ����
	 * 			���� �ִϸ��̼ǰ� ���带 ������
	 *
	 * \param	fireOrder	The fire order.
	 *
	 * \return	.
	 */

	ID_UPPER_DINO_AI			DinoAttack( WEAPON::FIRE_ORDER fireOrder);

	/**
	 * \fn	void CGameCharaDinoRaptor::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	 *
	 * \brief	������ ���� �ִϸ��̼� �÷��� ���� �� ��� ȣ�� ��\n
	 * 			���� ������ ���� ������ �Ǹ� FireCollisionDino�Լ��� ȣ����\n
	 * 			(FireCollisionDino�Լ����� �浹�� ������ ���� ó���� ��)
	 *
	 * \param [in,out]	pCtx	If non-null, the context.
	 * \param	tm				The time.
	 */

	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	virtual	WEAPON::FIRE_ORDER	GetFireOrder() override;

	virtual void				_PlayBreathSnd(void) override;

	virtual void				ResetChara() override;

	virtual void				SetDinoFireStartPos(VEC3D * pSrc) override;

	virtual EDinoAttackType		GetDinoAttackType(INT32 idx) override;
};

#endif //__GAME_CHARA_DINO_RAPTOR_H__
