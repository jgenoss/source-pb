#ifndef _SKILL_NOTICE_H_
#define _SKILL_NOTICE_H_

#include "DominationSkillObject.h"
#include <unordered_map>

class CUIHudDomi_Skill;
// ���氡���� ���¸� ǥ��(��ġ, ���׷��̵�, ����, ��� ��)
class SkillNotice
{
public:
	typedef void (SkillNotice::*fn)(CGameCharaBase*, dss::CServiceable*);

	SkillNotice();
	~SkillNotice();

	void Show(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Hide();

private:
	void Setup(CGameCharaBase* customer, dss::CServiceable* targetObj); // setup�� ������ ��Ȳ, E Ű�� �䱸 SP�� ���
	void Upgrade(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void Fix(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void UseSupplyBaseHP(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void UseSupplyBaseAMMO(CGameCharaBase* customer, dss::CServiceable* targetObj);
	void OpenSkillShop(CGameCharaBase* customer, dss::CServiceable* targetObj);

private:
	std::tr1::unordered_map<UINT32, fn> m_map;
	CUIHudDomi_Skill* m_noticeHUD;
};

#endif
