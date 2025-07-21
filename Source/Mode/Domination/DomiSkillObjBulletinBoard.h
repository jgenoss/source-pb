#ifndef _DOMI_SKILL_OBJ_BULLETINBOARD_H_
#define _DOMI_SKILL_OBJ_BULLETINBOARD_H_

#include "DominationSkillObject.h"

class CUIHudDomi_Skill;
// ���氡���� ���¸� ǥ��(��ġ, ���׷��̵�, ����, ��� ��)
template <> struct is_tinst_manual<class DomiSkillObjBulletinBoard> : std::tr1::true_type {};
class DomiSkillObjBulletinBoard
{
public:
	typedef void (DomiSkillObjBulletinBoard::*fn)(CGameCharaBase*, dss::CServiceable*);

	static DomiSkillObjBulletinBoard* i() { return tinst<DomiSkillObjBulletinBoard>(); }

	DomiSkillObjBulletinBoard();
	~DomiSkillObjBulletinBoard();

	void Show(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Hide();

private:
	void None(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Setup(CGameCharaBase* customer, dss::CServiceable* targetObj); // setup�� ������ ��Ȳ, E Ű�� �䱸 SP�� ���
	void Upgrade(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Fix(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void UseSupplyBaseHP(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void UseSupplyBaseAMMO(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void OpenSkillShop(CGameCharaBase* customer, dss::CServiceable* targetObj);

private:
	fn m_funcPtrs[DominationSkill::SERVICEABLE_STATE_MAX];
	CUIHudDomi_Skill* m_noticeHUD;
	i3::wstring m_objectName;
};

namespace SkillObjBulletinBoardUtil
{
	bool NeedToSupplyAmmo(WeaponBase* wp, CGameCharaBase* customer);
}

#endif
