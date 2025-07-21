#if !defined( __GAME_CHARA_INPUT_CONTROL_LOCAL_H__)
#define __GAME_CHARA_INPUT_CONTROL_LOCAL_H__

#include "GameCharaInputControl.h"

class CGameCharaInputControlLocal : public CGameCharaInputControl
{
	I3_CLASS_DEFINE( CGameCharaInputControlLocal, CGameCharaInputControl);
protected:
	bool	m_bThrowBomb = false;
	REAL32	m_fTimeLine_Throw;		// [initialize at constructor]

public:
	CGameCharaInputControlLocal();
	
	virtual void	OnFirstUpdate( REAL32 rDeltatime) override;
	virtual void	OnUpdate( REAL32 rDeltatime) override;
	virtual void	OnLastUpdate( REAL32 rDeltatime) override;

	void	SetMoveDir( VEC3D * pVecOut);
	// ���� ������ / �ݱ� ó��
	void	ProcessDropWeapon( REAL32 rDeltatime);
	// �ɱ� / �Ͼ�� ó�� ( return BodyChangeflag)
	bool	ProcessCrouch( REAL32 rDeltatime);
	// �̵� ó��
	void	ProcessMovement( REAL32 rDeltatime, bool bChangeBody);

	// ���� ó��
	void	ProcessWeapon( REAL32 rDeltatime);
	void	ProcessDinoAttack( REAL32 rDeltatime);

	// Extension
	void	ProcessExtension( void);


	void	ProcessInput( REAL32 rDeltatime);

	void	ProcessWeaponCollisionFire(REAL32 rDeltatime);
};

#endif
