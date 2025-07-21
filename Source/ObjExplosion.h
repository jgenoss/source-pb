#if !defined(__OBJ_EXPLOSION_H)
#define __OBJ_EXPLOSION_H

#include "tinst.h"

enum HIT_CHECK
{
	HIT_CHECK_HEAD = 0,
	HIT_CHECK_BODY,
	HIT_CHECK_COUNT,
};

class obj_explosion
{
public:
	static obj_explosion*	i() { return tinst<obj_explosion>(); }
	
	void	ProcessCreateStage();
	void	Process_InitObject(INT32 nStageObjCount);
	void	Process_ResetObjectExploded(CStageBattle * sb);
	void	ProcessDestroyStage();
	
	void	ProcessUpdate(CStageBattle* sb);

	/** \brief ������Ʈ ���߿� ���� ó��
		\desc �ٸ� ������Ʈ �� Character�� ���� Hit�� ó���մϴ�. */
	void	call( i3GameObj * pObject, VEC3D * pPos, REAL32 rRange, REAL32 rForce, REAL32 rDamage, REAL32 rSlop );
	/** \brief ������Ʈ ���߿� ���� ó��
		\desc DedicatedMode���� ������Ʈ Hit�� ������ ���߽� �̸� ó���մϴ�.
			�̸� ó���� ��Ŷ�� ������Ʈ ��Ʈ�� ���� �����ϴ�.
			���߰� �������� ���� ������ ���߱� ���ؼ� �Դϴ�. */
	void	call_for_dedi( CGameCharaBase * pShooter,  CHARA_DAMAGEINFO& DamageInfo, i3Object * pObject);

	void	CollisionCalcHitByObject( UINT8 nCharaHitCheck[][HIT_CHECK_COUNT],
		CHARA_DAMAGEINFO& DamageInfo, CHARA_HIT_PART hitPart, i3CollideeLine& ColLine, REAL32& rLength,
		CGameCharaBase* pChara, CGameCharaBase* pShooter);

private:
	
	obj_explosion();
	~obj_explosion();

	friend tinst_creator<obj_explosion>;

	void			CalcHitByObject( UINT8 nCharaHitCheck[][HIT_CHECK_COUNT],
		CHARA_DAMAGEINFO& DamageInfo, i3CollideeLine& ColLine, REAL32& rLength,
		CGameCharaBase* pChara, CGameCharaBase* pShooter);

	// HIT_RESULT�� �Ѱ��ݴϴ�. CheckObjCollision ���ο��� ȣ���մϴ�.
	REAL32			SetObjForce( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pStartPos, REAL32 rRange, REAL32 rForce, REAL32 rSlop );



	UINT8*	m_pObjectHitCheck;
	// for dedi
	REAL32*	m_pObjectAccDamage;	// Object ���� ��ŭ�� �迭 ������ Damage
	UINT8*	m_pObjectExploded;	// Object ������ ��� ����
};





#endif