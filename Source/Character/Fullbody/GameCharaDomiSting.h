#ifndef _GAME_CHARA_DOMI_STING_H_
#define _GAME_CHARA_DOMI_STING_H_

#include "GameCharaFullbodyDinosaur.h"

class WeaponBase;
class CGameCharaDomiSting : public CGameCharaFullbodyDinosaur
{
	I3_ABSTRACT_CLASS_DEFINE(CGameCharaDomiSting, CGameCharaFullbodyDinosaur);

public:
	CGameCharaDomiSting();
	virtual ~CGameCharaDomiSting();

	virtual void Create(GLOBAL_CHARA_INFO*, const CCharaInfo*) override;
	virtual void ResetChara() override;
	virtual void OnBindResource() override;
	virtual void OnUpdate(REAL32) override;

	void	Spit(GAMEINFO_CHARA_NETDATA* info);
	void	ThrowPoisonGrenade(GAMEINFO_CHARA_NETDATA* info);
	virtual WeaponBase* GetCurrentCharaWeapon() const override;
	MATRIX*	GetMatWeaponReleasePoint() const;

private:
	// domi_sting�� ����(WeaponBase)�� ������.(domi_raptor �迭, domi_acid �迭�� ��Ǹ� ���ϹǷ� ���Ⱑ ����)
	// spit�� �Ѿ˱����� ��������Ʈ�� �����ؾ��Ѵ�. �̷� ����� WeaponBase�� �����Ǿ� �ִµ�
	// ���� ���� �����ϱⰡ ��ư� �Ǿ��ִ�. �׷��� �׳� WeaponBase�� ����Ѵ�.
	WeaponBase*		m_wpSpit;		// [initialize at constructor]
	i3Transform*	m_wpReleasePoint = nullptr;
};

#endif