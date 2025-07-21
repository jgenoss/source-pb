/**
 * \file	UI\UISlotBase.h
 *
 * Declares the user interface slot base class.
 */


#if !defined( __UI_SLOT_BASE_H__)
#define __UI_SLOT_BASE_H__

#include "UIBase.h"

class UISlotBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UISlotBase, UIBase);

protected:
	VEC3D			m_vPos;
	VEC3D			m_vOffset;
	VEC3D			m_vOriginalPos;
	VEC2D			m_vSize;

	i3::pack::RECT			m_Rect;

public:
	UISlotBase();
	virtual ~UISlotBase();

	/** \brief ������ ��ġ���� ����Ѵ�.
		\desc Offset Size�� ���� ������ */
	void			CalculatePos( VEC3D * pOut, INT32 argX, INT32 argY, VEC3D * pOffset, VEC2D * pSize, REAL32 px = 0.0f, REAL32 py = 0.0f);

	/** \brief ������ Enable��ŵ�ϴ�.
		\desc Arg ���� ���� ��ġ�� �������ݴϴ�. Size�� ���Ǿ� ���� ��ġ�� ��������..
		\param[in] argX X������ ��ġ Index
		\param[in] argY Y������ ��ġ Index
		\param[in] pOffset ���� ��ġ
		\param[in] pOut ������ ��� ��ġ */
	virtual void	EnableSlot( INT32 argX, INT32 argY, VEC3D * pOffset = nullptr, VEC3D * pOut = nullptr, REAL32 px = 0.0f, REAL32 py = 0.0f);
	virtual void	EnableSlot2( REAL32 totalHeight, REAL32 totalXOffset, REAL32 totalYOffset,VEC3D * pOffset, VEC3D * pOut);

	virtual void	SetEmptySlot(void) {}

	/** \brief ���ÿ� ���� ó�� */
	virtual void	OnSelect( void) {}

	/** \brief ���� ������ ���� ó�� */
	virtual void	OnUnselect( void) {}
	virtual void	UpdateData( void) {}

	void			SetPos( VEC3D * pPos);
	VEC3D *			GetPos();
	VEC2D *			GetSize( void);

	VEC3D *			GetOriginalPos( void)			{ return &m_vOriginalPos; }
	void			SetOriginalPos( VEC3D * pPos)	{ i3Vector::Copy( &m_vOriginalPos, pPos); }
};

#endif
