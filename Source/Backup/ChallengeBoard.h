#ifndef _ChallengeBoard_h
#define _ChallengeBoard_h

#if ������_�͵�

#include "GuiNotifyReceiver.h"

enum ChallengeBoardFrame
{
	CBF_BOARD_TOP = 0,
	CBF_BOARD_MIDDLE,
	CBF_BOARD_BOTTOM,

	CBF_MAX
};

class CChallengeBoard: public i3GameObjBase
{
	I3_CLASS_DEFINE(CChallengeBoard, i3GameObjBase);
public:
	const static UINT32 MaxBoardString = MAX_STRING_COUNT;	// ������ 20 char ����

public:
	CChallengeBoard();
	virtual ~CChallengeBoard();

	virtual BOOL Create(i3Node* pParentNode = NULL,BOOL bEnable = TRUE);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

	void Change(void); 
	
	void OnActivate(void);

	//BOOL IsComplate(UINT32 idxChallenge);
	//INT32 GetChallengeCount(UINT32 idxChallenge);
	
	//UINT32 GetIngCount(void) const	{ return m_IngChallegeCount;	}

private:
	void _InitTextureCoord(INT32 challengIdx,ChallengeBulletType bulletType= CBT_NONE);
	void _InitSpriteRect(INT32 challengIdx);

private:	
	i3Texture	*	m_pTexture;						// HUD texture
	i3Sprite2D	*	m_pSpriteBoard;					// �ӹ� Ʋ
	i3Sprite2D	*	m_pSprite;						// �ӹ� Ʋ
	i3TextNodeDX2*	m_pText[MAX_CHALLENGE_COUNT];	// ��� ����
	
	//Create�Ҷ� ���� �־ ����մϴ�. 
	//UINT8			m_IngChallengeClass[MAX_CHALLENGE_COUNT];	//�ӹ� Ŭ���� 
	//UINT32			m_IngChallegeVale[MAX_CHALLENGE_COUNT];		//�ӹ� 
	//UINT32			m_IngChallegeCount; 						//�ϰ� �ִ� �ӹ� ��ü ī��Ʈ
};

#endif

#endif //_ChallengeBoard_h
