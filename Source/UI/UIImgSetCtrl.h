#if !defined( __UI_IMGSET_CTRL_H__)
#define __UI_IMGSET_CTRL_H__

//
//  ������ �ִ� UIImgSet_Items ������ ����ϰ�, UIImageSetControl ������ �����Ͽ� ��� ������ �̹��� ���� ��¿� ��Ʈ���� �籸���� �����Դϴ�..
//
//  �ľǵ� ���� UIImageSetControl������ ��� �������� ��� �����ؼ� UIImageSetControl���� ���� �ݿ��� ����..
//
//  template group�� Add�̿ܿ� ����Ʈ�� �����ϴ� ���� �ƹ��͵� ���� �ʽ��ϴ�..
//

class UIImgSetCtrl;

class UIImgSetTplGroup
{
public:
	void				AddTemplate(i3UITemplate * pTemplate);
	i3UITemplate*		GetTemplate(INT32 idx) const; 
	INT32				GetNumTemplate() const;

	UIImgSetCtrl*		CreateImgSetCtrl( i3UIButtonImageSet* bound_rsc ) const;
	UIImgSetCtrl*		CreateImgSetCtrl( i3UIScene* scn, const char* bound_rsc) const;

private:

	i3::vector<i3UITemplate*> m_tpls;
	friend class UIImgSetCtrl;
};

class UIImgSetCtrl : public i3ElementBase
{
	I3_CLASS_DEFINE( UIImgSetCtrl, i3ElementBase);

public:
	void	SetImage(INT32 img_idx);
	void	SetImage(INT32 tpl_idx, INT32 img_idx);	
	void	SetEnable(bool bEnable);
	void	RestoreSizePos();
	virtual ~UIImgSetCtrl();

public:
	//ó�� ��ġ
	void SetOriginPos();
	//ó�� ũ��
	void SetOriginScale();
	//���� ������(255)
	void SetOriginAlpha();

	//Control ���� ��ǥ(��� ��ǥ)
	VEC2D GetPos() const;
	void SetPos(const VEC2D & pos);
	void SetPos(REAL32 x, REAL32 y);

	void AddPos(const VEC2D & pos);
	void AddPos(REAL32 x, REAL32 y);

	//��� ���� ��ǥ(���� ��ǥ)
	VEC2D _GetDerivedPos() const;
	void _SetDerivedPos(const VEC2D & pos);
	void _SetDerivedPos(REAL32 x, REAL32 y);

	//AddPos�� ���� ��� ������ ���� ��ǥ�� �ǹ̷� ���� AddPos�� ����ص� ����
	void _AddDerivedPos(const VEC2D & pos);
	void _AddDerivedPos(REAL32 x, REAL32 y);

	//�Ű� ���ڴ� 0~1 ���, 1~ Ȯ��.
	//force �� false �̸� 0~1 ���̷� ��Ҹ� �ȴ�.
	void Scale(REAL32 xy, bool force = false);
	void Scale(REAL32 x, REAL32 y, bool force = false);
	void SetAlpha(REAL32 alpha);
	void SetVisible(bool show);
	void SetBoundImageSize( i3UITemplate* tpl );
	

private:
	void _translate();
	void _Resize(REAL32 scale_w, REAL32 scale_h);
	void	_AutoResize( i3UITemplate* tpl );
	void	Initialize(const UIImgSetTplGroup* tpl_grp, i3UIButtonImageSet* bound_rsc);
	
		
	i3UIButtonImageSet*			m_pBoundImage;					// ���� �پ��ִ� ��Ʈ���� �״�� ����� ���̹Ƿ�, �����͸� ������ ������ ���� �ʽ��ϴ�.
	VEC3D m_vBoundImage_OrigPos;
	VEC2D m_vBoundImage_OrigSize;
	VEC2D m_vTex_OrigSize;

	VEC3D m_vBoundImage_Pos;
	VEC2D m_vOffset;

	const UIImgSetTplGroup*		m_tpl_grp;
	friend class UIImgSetTplGroup;
};








#endif