#ifndef __CLAN_MARK_H
#define __CLAN_MARK_H

/*
����� : �鼺��
�ۼ��� : 2008.5.22

���ο� Ŭ�� ��ũ �̹����� �����ؾ� �� ���ο� �ؽ�ó�� �ʿ��ϸ� �⺻������ 512x512 �������� �ؽ�ó�� �����մϴ�.

�ϳ��� ���ο� �ؽ�ó���� ���� ��ũ �̹���(25x25 �ȼ�)�� ��� ��� 20x20���� ������ �� �ֽ��ϴ�.
�ϳ��� ���ο� �ؽ�ó���� �߰� ��ũ �̹���(52x52 �ȼ�)�� ��� ��� 9x9���� ������ �� �ֽ��ϴ�.
�ϳ��� �ؽ�ó ������ ��� ä��� ���ڸ� ��� �Ǵٸ� �ϳ��� 512x512 �������� ���ο� �ؽ�ó�� �����Ͽ� ����ϴ�.

�ѹ� Ư�� Ŭ�� ��ũ�� ����(���)�Ǹ� info list�� ����Ǹ� ������ �ٽ� ����� ��� list���� ������ ���ŵ��� �ʴ� �̻� 
��������� �ʰ� �ٷ� ���˴ϴ�.

���� Ŭ�� ��ũ�� �߰� Ŭ�� ��ũ�� ���� �ٸ� �ؽ�ó�� ���� �����˴ϴ�.

�� ȭ�鿡 ��������(�����Ǵ�) Ŭ�� ��ũ�� ���� ���� �̻��� ������ �� �����ϴ�. ���� ���� �̻� �ʿ��� ��� �˰��� ������ �ʿ��մϴ�.

���� ���� Ŭ�� ��ũ ���� ���� = 400�� (CLAN_MARK_SMALL_MAX_COUNT)
���� �߰� Ŭ�� ��ũ ���� ���� = 81�� (CLAN_MARK_MID_MAX_COUNT)

mark flag�� 0xFFFFFFFF�� ��Ŭ������ ����̸� Ŭ�� ��ũ�� 0xFFFFFFFF�� �ƴϴ�.

*/



#define CLAN_MARK_SMALL_PIXEL			25		//	���� �̹��� ������ (�ҽ� �̹��� ����)
#define CLAN_MARK_MID_PIXEL				52		//	�߰� �̹��� ������ (�ҽ� �̹��� ����)
#define CLAN_MARK_CREATE_TEX_SIZE		512		//	������ ���� ���� Ŭ����ũ �ؽ�ó �����̽� ũ��
#define CLAN_MARK_COLUMN_MAX			9		//	��ũ �̹��� ���� �ִ� ���� (�ҽ� �̹��� ũ�� ����� ������ �ʿ�)
#define CLAN_MARK_ROW_MAX				9		//	��ũ �̹��� ���� �ִ� ���� (�ҽ� �̹��� ũ�� ����� ������ �ʿ�)

//	����Ʈ���� ������ �� �ִ� �ִ� ��ũ ���� ����
#define CLAN_MARK_SMALL_MAX_COUNT		((CLAN_MARK_CREATE_TEX_SIZE / CLAN_MARK_SMALL_PIXEL) * (CLAN_MARK_CREATE_TEX_SIZE / CLAN_MARK_SMALL_PIXEL))
#define CLAN_MARK_MID_MAX_COUNT			((CLAN_MARK_CREATE_TEX_SIZE / CLAN_MARK_MID_PIXEL) * (CLAN_MARK_CREATE_TEX_SIZE / CLAN_MARK_MID_PIXEL))

#define CLAN_MARK_FLAG_NONE				CLAN_MARK_DEFAULT	//	Ŭ�� ��ũ�� ��Ŭ����
#define CLAN_MARK_FLAG_DEFAULT			0					//	Ŭ�����̸� �⺻ Ŭ�� ��ũ
#define IS_CLAN_MEMBER( markflag)		((markflag != CLAN_MARK_FLAG_NONE) ? 1 : 0)

enum CLAN_MARK_SIZE
{
	CLAN_MARK_SMALL = 0,		//	���� ������
	CLAN_MARK_MEDIUM,			//	�߰� ������

	MAX_CLAN_MARK_SIZE
};

enum CLAN_MARK_LAYER_TYPE
{
	CLAN_MARK_LAYER_FRAME,		//	���� �̹���
	CLAN_MARK_LAYER_CONTENT,		//	��� �̹���

	MAX_CLAN_MARK_LAYER,
};


struct ClanMarkInfo
{
	CLAN_MARK_SIZE _type = CLAN_MARK_SMALL;
	bool		_used = false;		//	��� ����
	UINT32		_flag = 0;		//	Ŭ�� ��ũ �÷���	
	UINT16		_texU = 0;
	UINT16		_texV = 0;
	UINT16		_texW = 0;
	UINT16		_texH = 0;
	i3Texture *	_image = nullptr;		//	Ŭ�� ��ũ �ؽ�ó
};

//	��� ���ڴ� 0~255 �̻� ��ġ�� ���ͼ��� �ȵȴ�. komet
#define MAKE_CLANMARK_FLAG(back_image_idx, shape_image_idx, back_color_idx, shape_color_idx)			\
																			(((UINT8)back_image_idx & 0xFF) << 24 | \
																			((UINT8)shape_image_idx & 0xFF) << 16 | \
																			((UINT8)back_color_idx & 0xFF) << 8 | \
																			((UINT8)shape_color_idx & 0xFF))

#define CLANMARK_FRAME_IMAGE_IDX(clanMarkFlag)		((clanMarkFlag >> 24) & 0xFF)
#define CLANMARK_CONTENT_IMAGE_IDX(clanMarkFlag)		((clanMarkFlag >> 16) & 0xFF)
#define CLANMARK_FRAME_COLOR_IDX(clanMarkFlag)		((clanMarkFlag >> 8) & 0xFF)
#define CLANMARK_CONTENT_COLOR_IDX(clanMarkFlag)		((clanMarkFlag) & 0xFF)

struct	CLAN_MARK_IMAGE;


class CClanMarkManager : public i3ElementBase
{
	I3_CLASS_DEFINE(CClanMarkManager, i3ElementBase);

public:
	CClanMarkManager(void);
	virtual ~CClanMarkManager(void);

	void OnRevive( i3RenderContext * pCtx);

	void Create(void);
	
	void GetUserColorTable(INT32 idx, COLOR * pOut);
	void GetLayerColorTable(CLAN_MARK_LAYER_TYPE LayerType, INT32 idx, COLOR * pOut);
	//void GetBackFrameColorTable(INT32 idx, COLOR * pOut);
	//void GetShapeFrameColorTable(INT32 idx, COLOR * pOut);
    
	//////////////////////////////////////////////////////////////////////////////////////////
	//	data list�� ������ �����˴ϴ�. (Ŭ�� ��ũ �̹��� ����)
	//////////////////////////////////////////////////////////////////////////////////////////

	//	GUI ��Ʈ�ѿ� ���ϴ� Ŭ�� ��ũ�� �����Ͽ� �����մϴ�.
	//	�̹� �����Ǿ� �־� ����Ʈ�� �����Ǵ� Ŭ�� ��ũ��� ��������� �ʰ� find�Ͽ� �������ݴϴ�.
	bool SetClanMarkTex(i3GuiStatic * pControl, CLAN_MARK_SIZE type, 
						INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);

	bool SetClanMarkTex(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, 
						INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);

	//	����Ʈ�� ����Ǿ� �ִ� Ŭ�� ��ũ �� ã�� ��ȯ�մϴ�. ����Ʈ�� ���� ��� nullptr�� ��ȯ�մϴ�.
	ClanMarkInfo * GetClanMarkTex(CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);
	
	//	����Ʈ�� ���ǰ� �ִ� Ŭ�� ��ũ ���� ����
	INT32 GetInfoCount(CLAN_MARK_SIZE type)				{ I3_BOUNDCHK( type, MAX_CLAN_MARK_SIZE); return m_nInfoListCount[type];		}


	//////////////////////////////////////////////////////////////////////////////////////////
	//	data list�� ������ ���� ���� �ʽ��ϴ�. (���� ��� ����)
	//////////////////////////////////////////////////////////////////////////////////////////

	//	GUI ��Ʈ�ѿ� �⺻ PB Ŭ�� ��ũ�� �����մϴ�.
	bool SetDefaultClanMarkTex( i3GuiStatic * pControl, CLAN_MARK_SIZE type);

	bool SetDefaultClanMarkTex( i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type);
	
	//	���ϴ� ������ �ϼ��� Ŭ�� ��ũ �̹����� pDestTex�� �̹��� ���μ����մϴ�. ���� ��ġ�� �̹����� ����� ���ϴ�.
	bool MakeClanMarkTex(i3Texture * pOutTex, CLAN_MARK_SIZE type, 
						 INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx, 
						 INT32 nDstTexU = 0, INT32 nDstTexV = 0);
	
	//	������ �ϼ��� Ŭ�� ��ũ �̹����� pOutTex�� �̹��� ���μ����մϴ�. ���� ��ġ�� �̹����� ����� ���ϴ�.
	bool MakeClanMarkTex(i3Texture * pOutTex, CLAN_MARK_SIZE type, UINT32 markFlag, INT32 nDstTexU = 0, INT32 nDstTexV = 0);

	//	���� �Ϻκ� Ŭ�� ��ũ �̹���(��� �Ǵ� ��� �̹���)�� pOutTex�� �̹��� ���μ����մϴ�. ���� ��ġ�� �̹����� ����� ���ϴ�.
	bool MakeClanMarkShapeTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, CLAN_MARK_LAYER_TYPE STBL_IDX_CLAN_SHAPE_COLOR, 
								INT32 nImageIdx, INT32 nColorIdx, INT32 nDstTexU = 0, INT32 nDstTexV = 0);

	static		INT32		GetMarkBackImageCount();
	static		INT32		GetMarkShapeImageCount();
	static		INT32		GetMarkColorCount();

private:

	bool 	GetMarkTexture(CLAN_MARK_IMAGE & MarkTex, CLAN_MARK_SIZE SizeType, CLAN_MARK_LAYER_TYPE LayerType, INT32 ImageIndex, SIZE32 & Size);

	i3::vector<CLAN_MARK_IMAGE*>		m_ClanMarkTex[MAX_CLAN_MARK_LAYER][MAX_CLAN_MARK_SIZE];	// CLAN_MARK_IMAGE *

	ClanMarkInfo  * m_pClanMarkInfo[MAX_CLAN_MARK_SIZE];

	i3::vector<ClanMarkInfo*> m_pInfoList[MAX_CLAN_MARK_SIZE];

	INT32 m_nInfoListCount[MAX_CLAN_MARK_SIZE];
	
	ClanMarkInfo * m_pLatestClanMarkInfo[MAX_CLAN_MARK_SIZE];

	i3Texture * m_pCombiClanMarkTex[MAX_CLAN_MARK_SIZE];
	i3Texture * m_pDefaultClanMarkTex[MAX_CLAN_MARK_SIZE];
	INT32 m_nDefaultClanMarkU[MAX_CLAN_MARK_SIZE];
	INT32 m_nDefaultClanMarkV[MAX_CLAN_MARK_SIZE];
	INT32 m_nDefaultClanMarkWH[MAX_CLAN_MARK_SIZE];
private:
	ClanMarkInfo * _MakeClanMarkInfo(CLAN_MARK_SIZE type, UINT32 flag);

	ClanMarkInfo * _FindClanMarkInfo(CLAN_MARK_SIZE type, UINT32 flag);
	ClanMarkInfo * _FindEmptyInfo(CLAN_MARK_SIZE type);
	void _ResetClanMarkInfo(CLAN_MARK_SIZE type);
	void _RebuildClanMarkTexture(void);
public:
	INT32 GetDefaultClanMarkU(INT32 type) const { return m_nDefaultClanMarkU[type]; }
	INT32 GetDefaultClanMarkV(INT32 type) const { return m_nDefaultClanMarkV[type]; }
	INT32 GetDefaultClanMarkWH(INT32 type) const { return m_nDefaultClanMarkWH[type]; }
	i3Texture * GetDefaultClanMarkTex(CLAN_MARK_SIZE type) const { return m_pDefaultClanMarkTex[type]; }
};

#endif