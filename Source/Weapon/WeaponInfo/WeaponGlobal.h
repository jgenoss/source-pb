#if !defined( _WEAPON_GLOBAL_H__)
#define _WEAPON_GLOBAL_H__

typedef UINT32 WEAPON_ATTRIBUTE;
#define WEAPON_ATTRIBUTE_FASTZOOM				0x0001			// Fast Zoom�� ����ϴ� ����
#define WEAPON_ATTRIBUTE_USINGPAIRWEAPON		0x0002			// Dual Weapon�� Info�� ����ϴ� ����
#define WEAPON_ATTRIBUTE_ATTACHEDSUB			0x0004			// ���� ���⿡ ������ �ͽ��ټ� ������ ��� (��: LSS)
#define WEAPON_ATTRIBUTE_ALPHABLEND				0x0008			// Alpha blend�� Ű�� ���� (���������� �ִ� �����)
#define WEAPON_ATTRIBUTE_USINGDESCRIPTION		0x0010			// �󼼼����� �ִ� ����(�󼼺��� ����)
#define WEAPON_ATTRIBUTE_PUMPACTION				0x0020			// Pump Action ��� ����
#define WEAPON_ATTRIBUTE_FASTARELOAD			0x0040			// ���� ������ �ִϸ��̼� ����
#define WEAPON_ATTRIBUTE_SHOTGUNMASTERC			0x0080			// ShotGun master C �ִϸ��̼� ����
#define WEAPON_ATTRIBUTE_LOADBULLETLEFTBARREL	0x0100			// �븮�� ������ �� �ٷ��� �ٴ� ���� �޼������� ���� ����
#define WEAPON_ATTRIBUTE_LOADMAGLEFTBARREL		0x0200			// źâ ��ü �� �ٷ��� �ٴ� ���� �޼������� ���� ���� 
#define WEAPON_ATTRIBUTE_LOADMAGREADY			0x0400			// ������ ���� ������ �غ����� �Ѵ�. (��: M1887 ������ ����)
#define WEAPON_ATTRIBUTE_CIC					0x0800			// CIC ����
#define WEAPON_ATTRIBUTE_LOCKEDDOTLASER			0x1000			// DotLaser�� �׽� On
#define WEAPON_ATTRIBUTE_NEEDTOLOADBULLET		0x2000			// Load Magazine ��, Load Bullet�� �ؾ��ϴ°�?
#define WEAPON_ATTRIBUTE_THROW_WEAPON			0x4000			// ������ ���� ����
#define WEAPON_ATTRIBUTE_MODE_WEAPON			0x8000			// ��� ���� ���� ����
#define WEAPON_ATTRIBUTE_EXCEPTION_ENDURANCE	0x00010000		// ������ ������
#define WEAPON_ATTRIBUTE_WORLD_WEAPON			0x00020000		// ������ ����
#define WEAPON_ATTRIBUTE_NO_EXJECTSHELL			0x00040000		// ź�� ������ ����
#define WEAPON_ATTRIBUTE_NOTOFF_EXTENSION_AT_CHANGE 0x00080000	// ���� ���ҽ� �ͽ��ټ� Off ó�� ����
#define WEAPON_ATTRIBUTE_DUAL_WEAPON			0x00100000		// ����� ����
#define WEAPON_ATTRIBUTE_HITED_ANIMATION		0x00200000		// ���� ��Ʈ�� �ִϸ��̼��� ���� �÷���(M-7)
#define WEAPON_ATTRIBUTE_BOUNCED_ATTACK			0x00400000		// ���� ������
#define WEAPON_ATTRIBUTE_DELAYED_FIRE_SOUND		0x00800000		// ������ ���� ���� �÷���  (for Melee)
#define WEAPON_ATTRIBUTE_NOMORE_SECONDARY_SOUND	0x01000000		// 2��° ���� ���尡 ����. (for Melee)
#define WEAPON_ATTRIBUTE_DO_NOT_THROW_LAST		0x02000000		// 1�� ���� ������ ��� ���� �� ����. (for Melee)
#define WEAPON_ATTRIBUTE_SPRAY_SECONDARYATTACK	0x04000000		// ���� ���ݽ� ���� �ø��� ó�� (for Melee)
#define WEAPON_ATTRIBUTE_DELAYED_THROWING		0x08000000		// ���� ������ �Ұ� (for Grenade)
#define WEAPON_ATTRIBUTE_USE_BOMBTRIGGER		0x10000000		// Bomb Trigger ��� ����(for C5)
#define WEAPON_ATTRIBUTE_LOADMAG_TO_LOADBULLET 	0x20000000		// Load Mag ���� LoadBullet�� ����ؾ� �ϴ� ����
#define WEAPON_ATTRIBUTE_RELOAD_LOOP_ANIMATION	0x40000000		// Reload Animation�� Loop�Ǵ� Animation�ΰ� �ϴ� ����.

//	���� ���� ����
class CWeaponGlobal : public i3NamedElement
{
	I3_CLASS_DEFINE( CWeaponGlobal, i3NamedElement);

public:

	REAL32		m_rTracerBulletLength;		//	�Ѿ� ���� ����
	REAL32		m_rTracerBulletThickness;	//	�Ѿ� ���� ����
	REAL32		m_rTracerBulletSpeed;		//	�Ѿ� ���� �ӵ�
	INT32		m_iTracerBulletRate;		//	�Ѿ� �߻� Ȯ�� ( Max: 100%)
	i3::rc_wstring	m_strTracerBulletEffectFile;	//	�Ѿ� ����Ʈ ���� �̸�
	i3::rc_wstring	m_strTracerHandGrenadeEffectFile;	//	����ź ���� ���� �̸�
	i3::rc_wstring	m_strTracerShellGrenadeEffectFile;	//	��ź ���� ���� �̸�
	i3::rc_wstring	m_strTracerArrowEffectFile;	//	ȭ�� ���� ���� �̸�
	i3::rc_wstring	m_strTracerSmokeEffectFile;		//	����ź ���� ���� �̸�
	i3::rc_wstring	m_strTracerFlashEffectFile;		//	����ź ���� ���� �̸�

	REAL32		m_rTracerHandGrenadeThickness;	//	����ź ���� ���� (����:m)
	REAL32		m_rTracerShellGrenadeThickness;	//	��ź ���� ���� (����:m)
	REAL32		m_rTracerArrowThickness;	//	ȭ�� ���� ���� (����:m)
	REAL32		m_rTracerSmokeThickness;	//	����ź ���� ���� (����:m)
	REAL32		m_rTracerFlashThickness;	//	����ź ���� ���� (����:m)
	REAL32		m_rTracerHandGrenadePieceLifeTime;	//	����ź ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerShellGrenadePieceLifeTime;	//	��ź ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerArrowPieceLifeTime;	//	ȭ�� ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerSmokePieceLifeTime;	//	����ź ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerFlashPieceLifeTime;	//	����ź ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerHandGrenadePieceCreateTime;	//	����ź ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerShellGrenadePieceCreateTime;	//	��ź ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerArrowPieceCreateTime;	//	ȭ�� ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerSmokePieceCreateTime;		//	����ź ���� ���丷 ���� �ð� (����: ��)
	REAL32		m_rTracerFlashPieceCreateTime;		//	����ź ���� ���丷 ���� �ð� (����: ��)
	I3COLOR		m_TracerHandGrenadeColor;		//	����ź ���� ����
	I3COLOR		m_TracerShellGrenadeColor;		//	��ź ���� ����
	I3COLOR		m_TracerArrowColor;		//	ȭ�� ���� ����
	I3COLOR		m_TracerSmokeColor;			//	����ź ���� ����
	I3COLOR		m_TracerFlashColor;			//	����ź ���� ����
	I3COLOR		m_DamageEffectColor;		//	������ ����Ʈ ���� (������ �����)
	REAL32		m_DamageEffectBlurStart;	//	������ ����Ʈ �� ���۰� (������ �����)
	REAL32		m_DamageEffectBlurEnd;		//	������ ����Ʈ �� ���ᰪ (������ �����)	
	
public:
	CWeaponGlobal( void);
	virtual ~CWeaponGlobal( void);

	void	ReadInfo( i3RegKey * pKey);
};


class CUITemplateInfo_Weapon : public i3NamedElement
{
	I3_CLASS_DEFINE(CUITemplateInfo_Weapon, i3NamedElement);

public:
	CUITemplateInfo_Weapon();
	virtual ~CUITemplateInfo_Weapon();

	void ReadInfo(i3RegKey* rk);
	const i3::rc_wstring& GetName() const;

private:
	i3::rc_wstring m_name;
};
#endif