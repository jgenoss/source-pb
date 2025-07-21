/**
 * \file	UI\UIImageSetControl.h
 *
 * Declares the user interface image set control class.
 */


#if !defined( __UI_IMAGESETCONTROL_H__)
#define __UI_IMAGESETCONTROL_H__

class UIImageSetControl : public i3ElementBase
{
	I3_CLASS_DEFINE( UIImageSetControl, i3ElementBase);
private:
	i3::vector<i3UITemplate*> m_TemplateList;
	i3UIButtonImageSet *	m_pImageDummy;
	VEC3D					m_Dummy_OrigPos;				// ������ ī�װ��� �̹�������� �ٸ�ſ��, ���ø���ü�� ���� �۾����� �־�, ���� pos/size�� Ȯ���صΰ� ��꿡 Ȱ��.
	VEC2D					m_Dummy_OrigSize;
protected:
	void			_AddTemplate( i3UITemplate * pTemplate);
	void			_AutoResize( i3UITemplate* pTemplate);	
	void			_AutoResize( INT32 templateIdx);
	i3UITemplate *	_GetTemplate( INT32 idx);

public:
	UIImageSetControl();
	virtual ~UIImageSetControl();

	void			SetVisible( bool bVisible);
	void			SetSize( REAL32 cx, REAL32 cy);
	void			SetSize( VEC2D * pVec);
	VEC2D *			GetSize();
	void			SetPos( REAL32 x, REAL32 y, REAL32 z = 0.0f);
	INT32			GetTemplateSize()
	{
		return (INT32)m_TemplateList.size();
	}
	
	void			AddTemplate( const char * pszName);

	/** \brief Template�� Shape count�� ��ȯ�Ѵ�. */
	INT32			GetTemplateShapeCount( INT32 templateIdx);

	/** \brief ImageSet�� ã�� Bind�Ѵ�.
		\param[in] pScene i3UIScene
		\param[in] pszName i3UIButtonImageSet Name		*/
	void			BindImageSet( i3UIScene * pScene, const char * pszName);
	void			BindImageSet( i3UIScene * pScene, const char * pszName, bool bResize);

	/** \brief �ش� Image Shape���� �����մϴ�. */
	void			SetImage( INT32 templateIdx, INT32 ImageIdx);
};

#endif
