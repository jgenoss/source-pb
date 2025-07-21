#include "pch.h"
#include "UIImageSetControl.h"

I3_CLASS_INSTANCE( UIImageSetControl);//, i3ElementBase);

UIImageSetControl::UIImageSetControl()
{
	m_pImageDummy = nullptr;
}

UIImageSetControl::~UIImageSetControl()
{
}

void UIImageSetControl::_AddTemplate( i3UITemplate * pTemplate)
{
	m_TemplateList.push_back( pTemplate);
}
i3UITemplate * UIImageSetControl::_GetTemplate( INT32 idx)
{
	return m_TemplateList[ idx];
}

void	UIImageSetControl::_AutoResize( i3UITemplate* pTemplate)
{
	
	VEC3D vPos, vCenter;
	VEC2D vSize;
	i3UIShape * pShape;

	I3ASSERT(pTemplate != nullptr);
	I3ASSERT( m_pImageDummy != nullptr);

	pShape = pTemplate->getShape(0);
	I3ASSERT( pShape);

	//	I3ASSERT( pShape->getHeight() > 0);			// �����ؽ��Ļ������ ����������� �ٸ��� �־ ���� �Լ��� �ؽ��� �ʺ� ���̷� �ٲߴϴ�. (����ũ�����ۿ���..)
	//	I3ASSERT( pShape->getWidth() > 0);
	const REAL32 tex_width = pShape->getCoordW();
	const REAL32 tex_height = pShape->getCoordH();
	I3ASSERT(tex_width > 0 && tex_height > 0);

	REAL32 rRateY = getY(&m_Dummy_OrigSize) / tex_height;
	REAL32 rRateX = getX(&m_Dummy_OrigSize) / tex_width;

	if (rRateX >= 1.f && rRateY >= 1.f )	// �� ���, ����������� ���̻���� Ŀ�� Ȯ��ɼ� �ֱ� ������ ��������� ���� ( �������׸񿡼� )
	{
		vSize.x = tex_width; vSize.y = tex_height;
	}
	else
	{
		if( rRateY < rRateX)
			i3Vector::Set( &vSize, tex_width * rRateY, getY(&m_Dummy_OrigSize));
		else
			i3Vector::Set( &vSize, getX(&m_Dummy_OrigSize), tex_height * rRateX);
	}

	i3Vector::Set( &vCenter, getX(&m_Dummy_OrigSize) * 0.5f, getY(&m_Dummy_OrigSize) * 0.5f, 0.f);
	i3Vector::Add( &vPos, &m_Dummy_OrigPos, &vCenter);

	i3Vector::Set( &vPos, getX( &vPos) - getX( &vSize) * 0.5f, getY( &vPos) - getY( &vSize) * 0.5f, 0.f);

	m_pImageDummy->setPos( &vPos);
	m_pImageDummy->setSize( &vSize);
}

void UIImageSetControl::SetPos( REAL32 x, REAL32 y, REAL32 z )
{
	m_pImageDummy->setPos( x, y );
}

void UIImageSetControl::SetSize( REAL32 cx, REAL32 cy)
{
	m_pImageDummy->setSize( cx, cy );
}

void UIImageSetControl::SetSize( VEC2D * pVec)
{
	m_pImageDummy->setSize( pVec);
}
VEC2D * UIImageSetControl::GetSize()
{
	return &m_Dummy_OrigSize;
}

void UIImageSetControl::_AutoResize( INT32 templateIdx)
{
	UIImageSetControl::_AutoResize( _GetTemplate( templateIdx) );
}

void UIImageSetControl::SetVisible( bool bVisible)
{
	I3ASSERT(m_pImageDummy);
	m_pImageDummy->SetVisible( bVisible?true:false);
}

void UIImageSetControl::AddTemplate( const char * pszName)
{
	_AddTemplate( i3UI::getUILibrary()->FindByName( pszName) );
}

INT32 UIImageSetControl::GetTemplateShapeCount( INT32 templateIdx)
{
	I3_BOUNDCHK( templateIdx, (INT32)m_TemplateList.size());
	return _GetTemplate( templateIdx)->getShapeCount();
}

void UIImageSetControl::BindImageSet( i3UIScene * pScene, const char * pszName)
{
	I3ASSERT( pScene != nullptr);
	m_pImageDummy = (i3UIButtonImageSet*) pScene->FindChildByName( pszName);
	I3ASSERT( m_pImageDummy != nullptr);
	m_pImageDummy->SetViewOnly( true);
	
	m_Dummy_OrigPos = *m_pImageDummy->getPos();
	m_Dummy_OrigSize = *m_pImageDummy->getSize();
	
	if ( m_pImageDummy->getTemplate() )
		UIImageSetControl::_AutoResize(  m_pImageDummy->getTemplate() );	// �̸� �ѹ� ���ִ°� ������..�� ������ �־��� ���ҽ� �ʱ����ø��� �������ø��� ���Ƽ��Դϴ�..
	
}

void UIImageSetControl::BindImageSet( i3UIScene * pScene, const char * pszName, bool bResize)
{
	I3ASSERT( pScene != nullptr);
	m_pImageDummy = (i3UIButtonImageSet*) pScene->FindChildByName( pszName);
	I3ASSERT( m_pImageDummy != nullptr);
	m_pImageDummy->SetViewOnly( true);
	
	m_Dummy_OrigPos = *m_pImageDummy->getPos();
	m_Dummy_OrigSize = *m_pImageDummy->getSize();
	
	if ( m_pImageDummy->getTemplate() && bResize)
		UIImageSetControl::_AutoResize(  m_pImageDummy->getTemplate() );	// �̸� �ѹ� ���ִ°� ������..�� ������ �־��� ���ҽ� �ʱ����ø��� �������ø��� ���Ƽ��Դϴ�..
	
}

void UIImageSetControl::SetImage( INT32 templateIdx, INT32 ImageIdx)
{
	I3ASSERT( m_pImageDummy != nullptr);
	if( templateIdx > -1 && ImageIdx > -1)
	{
		
		I3ASSERT( templateIdx < (INT32)m_TemplateList.size() );		// ���ø� ī��Ʈ���� �������� ���..

		if( m_pImageDummy->getTemplate() != _GetTemplate( templateIdx))
		{
			m_pImageDummy->SetTemplate( _GetTemplate( templateIdx));
			_AutoResize( templateIdx);								// �ϰ� ���ø� ���ҽ����� �̹��� ����� ��� ������ ���������Ƿ� ���� ���ǹ��ȿ� �ְ� ���ϴ�.
		}

		m_pImageDummy->SetShapeIdx( ImageIdx);
		m_pImageDummy->EnableCtrl( true);
	}
	else
	{
		m_pImageDummy->EnableCtrl( false);
	}
}


