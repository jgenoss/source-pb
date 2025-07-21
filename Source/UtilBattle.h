#if !defined(__UTIL_BATTLE_H)
#define __UTIL_BATTLE_H

namespace util_battle {}
namespace ub = util_battle;

namespace util_battle
{
	bool is_live_character(INT32 slot_idx);
	

	bool is_current_toggle_camera_fps();

	void toggle_3p_camera();
	void toggle_observer_fps_camera();				// ����������Ʋ���� �Ű�����, ���� ���̰� ���� ���� ī�޶� �����Լ�...

	void toggle_normal_observe_mode(void);
	void toggle_GM_observe_mode(void);				// ����������Ʋ���� �Ű�����, ���� ���̴� ���� ���� �Լ��Դϴ�. (����)
	void toggle_flying_camera_mode(void);

	void set_topview_camera_pos(const VEC3D& p);
	void toggle_topview_camera(void);

// ObserverMode
	void	SetToOnObjectCamera(i3Body* pBody, INT32 idx, i3Object* pObj, WeaponBase * pWeapon);
	void	SetToOldCameraIfOnObjectCamera();
	void	SetToObserveCamera();


	//ai
	void SetAiCharaData(INT32 idx, AI_BIND_DATA* pbind_data, INT8 nteam);

	void AddAiChara(INT32 slotIdx);
	void AddAiCharacter(void);						// ���� �� �Լ��� ��� �����Դϴ�..�������̼Ǽ����� ���� �����ؼ� Ŭ���̾�Ʈ�� ai������ ���Ÿ� �޽��ϴ�..
	void RespawnAiCharacter( INT32 slotIdx);
	void ChangeAiCharacter( void);
	void StartAiForClient( INT32 slotIdx);			// �ӽ��ڵ�θ� ���̴µ� �ϰ� ���� .. EVENT_START_BATTLEó������ �ּ��Ǿ� ����..

	void AddDinoChara();

	void	ApplySkillStatus(INT32 idx, bool);			// ��ų ����

	// ��ų �ý��� ����
	void	SetSkillsystemValue(INT32 CharaSlotIdx, INT32 SkillSlotIdx);		// �̰��� CommandSet���� ���� ȣ��
	void	ApplySkillsystem(INT32 idx);

	// �޼�, ������ ������Ʈ
	void	ApplyLRHandFromEnvSet();

	void	CheckAIData( AI_BIND_DATA * pAiData);
	
}

AI_BIND_DATA * _SetAiInfo( INT32 slotIdx, INT32 difficultyLevel);


#endif