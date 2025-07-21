#ifndef _REGISTERING_BY_MODE_H_
#define _REGISTERING_BY_MODE_H_

#ifdef	DOMI_ANIM_CAM

class OutsideEventSubject;
class OutsideEventObserver;

class AnimCamRegisterMode
{
public:
	AnimCamRegisterMode() {}
	virtual ~AnimCamRegisterMode() {}

	virtual void Register() = 0;							// ��庰 �ִϸ��̼�/�̺�Ʈ ���
	virtual const char* GetAnimKey(INT32 idx) const = 0;	// �ִϸ��̼� Ű ��ȯ
};

class AnimCamRegisterDomi : public AnimCamRegisterMode
{
public:
	enum ANIM_INDEX_FROM_SERVER
	{
		ANIM_END = 500,
		ANIM_SCENEA_MUTANT_WALK = 501,
		ANIM_SCENEB_MUTANT_DEATH = 502,
	};

public:
	AnimCamRegisterDomi();
	virtual ~AnimCamRegisterDomi();

	void Register();

	const char* GetAnimKey(INT32 idx) const;

private:
	OutsideEventSubject*	m_evtSubject;
	i3::vector<OutsideEventObserver*> m_evtObservers;
};

#endif

#endif