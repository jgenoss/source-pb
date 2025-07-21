#if !defined( __STEP_PROFILE_DEF_H__)
#define __STEP_PROFILE_DEF_H__

#define FIXED_BADGE_NUM_RIBBON		14
#define FIXED_BADGE_NUM_ENSIGN		8
#define FIXED_BADGE_NUM_MEDAL		1
#define FIXED_BADGE_NUM_MASTER		19

#define MAX_GAIN_RIBBON_SLOT	10	// ���� ���� ����
#define MAX_GAIN_ENSIGN_SLOT	10	// ���� ����/���� ����
#define MAX_GAIN_TITLE_SLOT		10	// ���� ȣĪ ��ȯ ����/���� ����

#define MAX_RIBBON_COUNT		(MAX_MINIATURE_COUNT + 10)	// �̺�Ʈ ��������
#define MAX_ENSIGN_MEDAL_COUNT	(MAX_INSIGNIA_COUNT + MAX_MEDAL_COUNT)

enum BADGE_TYPE
{
	BADGE_TYPE_UNKOWN = -1,
	BADGE_TYPE_RIBBON = 0,
	BADGE_TYPE_ENSIGN,
	BADGE_TYPE_MEDAL,
	BADGE_TYPE_MASTER,

	BADGE_TYPE_COUNT,
};

// ���� Prize ������ BADGE_TYPE�� �����մϴ�.
// ���� Prize ������ �Ʒ��� �����ϴ�.
// ���� : 1 ~ 50 
// ���� : 51 ~ 100
// ���� : 101 ~ 118
// ������ ���� : 119 ~ 240
inline BADGE_TYPE	PrizeToBadgeType(UINT8 Prize)
{
	if ( Prize == 0 )
		return BADGE_TYPE_UNKOWN;

	if ( Prize > 0 && Prize <= 50 )
		return BADGE_TYPE_RIBBON;

	if ( Prize > 50 && Prize <= 100 )
		return BADGE_TYPE_ENSIGN;

	if ( Prize >= 101 && Prize <= 118 )
		return BADGE_TYPE_MEDAL;

	if ( Prize >= 119 && Prize < 240 )
		return BADGE_TYPE_MASTER;

	return BADGE_TYPE_UNKOWN;
}


#endif
