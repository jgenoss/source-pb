/**
 * \file	UI\UIUserActionDef.h
 *
 * Declares the user interface user action def class.
 */


#if !defined( __UI_USERACTION_DEF_H__)
#define __UI_USERACTION_DEF_H__

#define	I3_EVT_USER_ACTION	(0x00100000 | I3_EVT_USER)

/** \brief �����׼� �˾��� ������
	\note ���� �߿��մϴ�. Resource ��ȣ�� ��ġ��. */
enum USER_CONTEXT_MENU
{
	UCM_NONE = -1,
	UCM_PROFILE,					// �� ���� ����
	UCM_ADD_FRIEND,					// ģ�� �߰�
	UCM_CLAN_INVITE,				// Clan �ʴ�
	UCM_SEND_MAIL,					// ���� ������
	UCM_ADD_BLOCK,					// �����ϱ�
	UCM_USER_REPORT,				// �Ű��ϱ�
	UCM_FIND_AWAY,					// ã�ư���/�����ϱ�
	UCM_SEND_PRESENT,				// ���� ������
	UCM_REMOVE_FRIEND,				// ģ�� ����

	// ���ϴ� ������ Resource�� ����.....��?
	UCM_REMOVE_BLOCK,				// ���� ����
	UCM_KICK,						// �����ϱ� (Only ReadyRoom)
	UCM_WHISPER,					// �Ӹ��ϱ�
	UCM_INVITE,						// �ʴ��ϱ�
	UCM_ACCEPT_FRIEND,				// ģ�� ����
	UCM_REFUSE_FRIEND,				// ģ�� ����
	UCM_TRANSFER_HOST,				// ���� �ѱ��	
	UCM_VIEW_OTHER_USER_ITEM_INFO,	// ���� ������ ����
	UCM_GMOPTION,					// GM�ɼ� ����

	UCM_MAX
};

#endif
