#if !defined( __RADIOCHATDEF_H__ )
#define __RADIOCHATDEF_H__


////////////////////////////////////////////////////////////////////////////////////////////////////
// ��Ʈ��ũ���� idx�� �� �׷쿡 ���� 10�� ���� ���� �˴ϴ�.

#define RADIOCHAT_MSG_SIZE	128

typedef struct _tagRadiochat
{
	UINT8	idx;
	char	szMessageUS[ RADIOCHAT_MSG_SIZE ];
	char	szMessageKR[ RADIOCHAT_MSG_SIZE ];
} RADIOCHAT;


extern RADIOCHAT g_RadioChat[3][10];

#endif
