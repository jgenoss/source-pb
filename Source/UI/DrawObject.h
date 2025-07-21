#ifndef __DRAW_OBJECT_H__
#define __DRAW_OBJECT_H__

struct ImageInfo
{
	i3Texture*	tex;
	INT32		x, y, w, h;
	REAL32		halfW, halfH;
};

class DrawObject
{
public:
	DrawObject();
	virtual ~DrawObject();

	virtual void Update(REAL32 tm) {}

	// i3UIImageBoxEx ������Ű�� �۾��� �ణ Ư���ѵ� AddImage�� ���ؼ� �ؽ�ó�� ����� �� 
	// Create�� ȣ���ؾ� �մϴ�. i3UIImageBoxEx ��ü ���� �ÿ��� �ؽ�ó�� �ݵ�� ��ϵǾ� �־��
	// �ϱ� �����Դϴ�. ��� ����� MissionObject�� Unit�� ���� ��ü �����κ��� �����ϱ� �ٶ��ϴ�.
	void		Create(i3UIFrameWnd* frame);
	void		AddImage(i3Texture* tex, INT32 x, INT32 y, INT32 w, INT32 h);

	void		SetPosition(REAL32 posX, REAL32 posY);
	VEC3D*		GetPosition() const { return m_pImageBox->getPos(); }

	void		SetRotate(REAL32 rotation);
	void		SetScale(REAL32 scale);

	void		SetColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a);
	void		SetColor(COLOR* color);
	COLOR*		GetColor() const { return m_pImageBox->getColor(); }

	void		SetAlpha(UINT8 a);
	UINT8		GetAlpha();

	void		SetCurrentImage(UINT32 index);
	INT32		GetCurrentImage() const;
	ImageInfo&	GetCurrentImageInfo();
	INT32		GetTotalImageCnt() const		{ return m_vecImageInfo.size(); }

	void		Show();
	void		Hide();

	virtual void		Add(DrawObject* obj) {}
	virtual void		Remove(DrawObject* obj) {}
	virtual DrawObject* GetChild(int index) { return 0; }

private:
	i3UIImageBoxEx*			m_pImageBox;

	UINT32					m_nCurImageIdx;
	i3::vector<ImageInfo>	m_vecImageInfo;
};

#endif