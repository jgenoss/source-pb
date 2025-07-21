#include "pch.h"
#include "GlobalVariables.h"
#include "TracerBullet.h"

I3_CLASS_INSTANCE( CTracerBullet);

bool CTracerBullet::Create( INT32 Index)
{    
	m_nIdx = Index;

	Reset( false);

	return true;
}

void CTracerBullet::OnUpdate( REAL32 rDeltaSeconds)
{
	if( !m_bActive)		return;

	//	��ǥ������ �����ϸ� ����
	if( IsArrive())
	{
		Reset( false);

#ifdef DEBUG_TRACER_BULLET
		I3TRACE("BulletTracer (%d) - Arrive\n", m_nIdx);
#endif
	}
	//	Ȱ����
	else
	{
		OnActive( rDeltaSeconds);
	}
}

bool CTracerBullet::Start( i3Billboard * pImage, TracerBulletInfo * pInfo)
{
	I3ASSERT( pInfo != nullptr);
	I3ASSERT( pImage != nullptr);

	VEC3D vGapLength;

	memcpy( &m_Info, pInfo, sizeof(TracerBulletInfo));

	m_pBillboard = pImage;
	SetColor(&m_Info._color);

	m_fCurThickness = m_Info._Thickness;
	m_fCurTime = m_Info._LifeTime;

#if defined( NO_GRENADE_TRACE_EFFECT)
	if( type == TRACER_TYPE_GRENADE || type == TRACER_TYPE_SMOKE || type == TRACER_TYPE_FLASH)
		return false;
#endif

	i3Vector::Sub( &m_vDir, &m_Info._vDestPos, &m_Info._vBeginPos);		//	�Ѿ� ����
	m_fDestLengthSq = i3Vector::LengthSq( &m_vDir);	
	m_fDestLengthSq = i3Math::abs( m_fDestLengthSq);	//	�Ѿ��� Total ���� �Ÿ�(Sq)
	i3Vector::Normalize( &m_vDir, &m_vDir);

	i3Vector::Copy( &m_vCurStart, &m_Info._vBeginPos);							//	���� ���ư��� �Ѿ��� Start
	
	if( m_Info._Type == TRACER_TYPE_BULLET_SMOKE)
	{
		i3Vector::Copy( &m_vCurEnd, &m_Info._vBeginPos);								//	���� ���ư��� �Ѿ��� End
	}
	else
	{
		i3Vector::Scale( &m_vCurEnd, &m_vDir, m_Info._Length);
		i3Vector::Add( &m_vCurEnd, &m_vCurStart, &m_vCurEnd);		//	���� ���ư��� �Ѿ��� End
	}

	i3Vector::Sub( &vGapLength, &m_Info._vDestPos, &m_vCurEnd);
	m_fGapLengthSq = i3Vector::LengthSq( &vGapLength);
	m_fGapLengthSq = i3Math::abs( m_fGapLengthSq);

	I3ASSERT( m_pBillboard != nullptr);	
	m_pBillboard->SetSize( m_nIdx, m_Info._Thickness);		//	�Ѿ��� �β�
	m_pBillboard->SetEnable( m_nIdx, true);
	m_pBillboard->SetTextureCoord( m_nIdx, 0.f, 0.f, 1.f, 1.f);

	I3ASSERT( m_pBillboard != nullptr);
	//	�Ѿ��� ��ġ ����
	m_pBillboard->SetStart( m_nIdx, &m_vCurStart);
	m_pBillboard->SetEnd( m_nIdx, &m_vCurEnd);

	m_bActive = true;
	this->SetEnable( true);

#ifdef DEBUG_TRACER_BULLET
	I3TRACE("BulletTracer (%d) - Start\n", m_nIdx);
#endif

	return true;
}
/*
bool CTracerBullet::Start( TRACER_TYPE type, i3Billboard * pImage, VEC3D * pBegin, VEC3D * pDest, 
						   REAL32 fTracerLength, REAL32 fTracerThickness, REAL32 fTracerSpeed, I3COLOR * pColor, UINT32 style)
{	//	�߻�
	I3ASSERT( pBegin != nullptr);
	I3ASSERT( pDest != nullptr);	
	I3ASSERT( pImage != nullptr);

	CWeaponGlobal * pGlobalInfo = g_pWeaponInfoDataBase->getGlobal();
	VEC3D vGapLength;

	m_nType = type;
	m_pBillboard = pImage;
	m_nStyle = style;
	m_fCurThickness = fTracerThickness;		//	�β�
	m_fTracerStartSpeed = m_fTracerEndSpeed = fTracerSpeed;				//	�ӵ�
	m_fTracerLength = fTracerLength;			//	����

	if( pColor != nullptr)
	{
		i3Color::Copy( &m_color, pColor);
	}

	SetColor(&m_color);

	//	���� ���� �ð�
	switch(type)  
	{
	case TRACER_TYPE_GRENADE:	m_fLifeTime = m_fCurTime = pGlobalInfo->m_rTracerGrenadePieceLifeTime;		break;
	case TRACER_TYPE_SMOKE:		m_fLifeTime = m_fCurTime = pGlobalInfo->m_rTracerSmokePieceLifeTime;		break;
	case TRACER_TYPE_FLASH:		m_fLifeTime = m_fCurTime = pGlobalInfo->m_rTracerFlashPieceLifeTime;		break;
	default:	m_fCurTime = 0.0f;	break;
	}

#if defined( NO_GRENADE_TRACE_EFFECT)
	if( type == TRACER_TYPE_GRENADE || type == TRACER_TYPE_SMOKE || type == TRACER_TYPE_FLASH)
		return false;
#endif

	i3Vector::Copy( &m_vBegin, pBegin);			//	�Ѿ� ������
	i3Vector::Copy( &m_vDest, pDest);			//	�Ѿ� ������
	i3Vector::Sub( &m_vDir, pDest, pBegin);		//	�Ѿ� ����
	m_fDestLengthSq = i3Vector::LengthSq( &m_vDir);	
	m_fDestLengthSq = i3Math::abs( m_fDestLengthSq);	//	�Ѿ��� Total ���� �Ÿ�(Sq)
	i3Vector::Normalize( &m_vDir, &m_vDir);

	i3Vector::Copy( &m_vCurStart, pBegin);							//	���� ���ư��� �Ѿ��� Start
	i3Vector::Scale( &m_vCurEnd, &m_vDir, m_fTracerLength);
	i3Vector::Add( &m_vCurEnd, &m_vCurStart, &m_vCurEnd);		//	���� ���ư��� �Ѿ��� End

	i3Vector::Sub( &vGapLength, &m_vDest, &m_vCurEnd);
	m_fGapLengthSq = i3Vector::LengthSq( &vGapLength);
	m_fGapLengthSq = i3Math::abs( m_fGapLengthSq);

	I3ASSERT( m_pBillboard != nullptr);	
	m_pBillboard->SetSize( m_nIdx, fTracerThickness);		//	�Ѿ��� �β�
	m_pBillboard->SetEnable( m_nIdx, true);
	m_pBillboard->SetTextureCoord( m_nIdx, 0.f, 0.f, 1.f, 1.f);

	I3ASSERT( m_pBillboard != nullptr);
	//	�Ѿ��� ��ġ ����
	m_pBillboard->SetStart( m_nIdx, &m_vCurStart);
	m_pBillboard->SetEnd( m_nIdx, &m_vCurEnd);

	m_bActive = true;
	this->SetEnable( true);

#ifdef DEBUG_TRACER_BULLET
	I3TRACE("BulletTracer (%d) - Start\n", m_nIdx);
#endif

	return true;
}

bool CTracerBullet::StartBulletSmoke(i3Billboard * pImage, VEC3D * pBegin, VEC3D * pDest, 
						REAL32 fLifeTime, REAL32 fTracerThickness, REAL32 fTracerStartSpeed, REAL32 fTracerEndSpeed, I3COLOR * pColor)
{
	//	�߻�
	I3ASSERT( pBegin != nullptr);
	I3ASSERT( pDest != nullptr);	
	I3ASSERT( pImage != nullptr);

	VEC3D vGapLength;

	m_nType = TRACER_TYPE_BULLET_SMOKE;
	m_pBillboard = pImage;
	m_nStyle = 0;
	m_fCurThickness = fTracerThickness;			//	�β�
	m_fTracerStartSpeed = fTracerStartSpeed;	//	���� �ӵ�
	m_fTracerEndSpeed	= fTracerEndSpeed;		//	���� �ӵ�
	m_fLifeTime = m_fCurTime = fLifeTime;	//	���� �ð�
	m_fTracerLength = 0;			//	����

	if( pColor != nullptr)
	{
		i3Color::Copy( &m_color, pColor);
	}

	SetColor(&m_color);

	i3Vector::Copy( &m_vBegin, pBegin);			//	�Ѿ� ������
	i3Vector::Copy( &m_vDest, pDest);			//	�Ѿ� ������
	i3Vector::Sub( &m_vDir, pDest, pBegin);		//	�Ѿ� ����
	m_fDestLengthSq = i3Vector::LengthSq( &m_vDir);	
	m_fDestLengthSq = i3Math::abs( m_fDestLengthSq);	//	�Ѿ��� Total ���� �Ÿ�(Sq)
	i3Vector::Normalize( &m_vDir, &m_vDir);

	i3Vector::Copy( &m_vCurStart, pBegin);							//	���� ���ư��� �Ѿ��� Start
	i3Vector::Copy( &m_vCurEnd, pBegin);								//	���� ���ư��� �Ѿ��� End

	i3Vector::Sub( &vGapLength, &m_vDest, &m_vCurEnd);
	m_fGapLengthSq = i3Vector::LengthSq( &vGapLength);
	m_fGapLengthSq = i3Math::abs( m_fGapLengthSq);

	I3ASSERT( m_pBillboard != nullptr);	
	m_pBillboard->SetSize( m_nIdx, fTracerThickness);		//	�Ѿ��� �β�
	m_pBillboard->SetEnable( m_nIdx, true);
	m_pBillboard->SetTextureCoord( m_nIdx, 0.f, 0.f, 1.f, 1.f);

	I3ASSERT( m_pBillboard != nullptr);
	//	�Ѿ��� ��ġ ����
	m_pBillboard->SetStart( m_nIdx, &m_vCurStart);
	m_pBillboard->SetEnd( m_nIdx, &m_vCurEnd);

	m_bActive = true;
	this->SetEnable( true);

#ifdef DEBUG_TRACER_BULLET
	I3TRACE("BulletTracer Smoke (%d) - Start\n", m_nIdx);
#endif

	return true;
}*/

void CTracerBullet::Reset( bool bEnable)
{
	m_bActive = false;

	i3Vector::Zero( &m_vDir);
	i3Vector::Zero( &m_vCurStart);
	i3Vector::Zero( &m_vCurEnd);

	m_fDestLengthSq = 0.0f;
	m_fCurLengthSq = 0.0f;
	m_fGapLengthSq = 0.0f;
	m_fCurTime = 0.0f;

	m_nSrcAlpha		= 255;

	if( m_pBillboard)
	{
		m_pBillboard->SetEnable( m_nIdx, bEnable);
		m_pBillboard->SetColor( m_nIdx, &m_Info._color);
	}

	this->SetEnable( bEnable);
}

REAL32 CTracerBullet::GetCurLength( void)
{	//	������� ���ư� �Ÿ� ���
	VEC3D vLength;
	REAL32 fLength;
	i3Vector::Sub( &vLength, &m_vCurEnd, &m_Info._vBeginPos);
	fLength = i3Vector::Length( &vLength);

	return i3Math::abs( fLength);
}

void CTracerBullet::SetColor( COLOR * pColor)
{
	I3ASSERT( m_pBillboard != nullptr);

	m_pBillboard->SetColor( m_nIdx, pColor);

	m_nSrcAlpha = (INT32) i3Color::GetA( pColor);
}

void CTracerBullet::SetColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	I3ASSERT( m_pBillboard != nullptr);

	m_pBillboard->SetColor( m_nIdx, r, g, b, a);

	m_nSrcAlpha = (INT32) a;
}

void CTracerBullet::SetAlpha(UINT8 a)
{
	COLOR * color = m_pBillboard->GetColor( m_nIdx);

	m_pBillboard->SetColor( m_nIdx, i3Color::GetR( color), i3Color::GetG( color), i3Color::GetB( color), a);

	m_nSrcAlpha = (INT32) a;
}

//////////////////////////////////////////////////////////////////////////////////////
void CTracerBullet::OnActive( REAL32 tm)
{	
	//	���ư��� ���� ���İ��� ���� ����
	/*if( m_nStyle & TRACER_STYLE_REDUCE_ALPHA_BY_DIST)		
	{
		REAL32 fRemainLengthSq = 0.0f;
		VEC3D vRemainLength;

		COLOR * color = m_pBillboard->GetColor( m_nIdx);

		i3Vector::Sub( &vRemainLength, &m_vDest, &m_vCurEnd);
		fRemainLengthSq = i3Vector::LengthSq( &vRemainLength);
		fRemainLengthSq = i3Math::abs( fRemainLengthSq);

		UINT8 a = (UINT8) ((fRemainLengthSq / m_fGapLengthSq) * (REAL32) m_nSrcAlpha);
				
		SetAlpha( (UINT8) a);
	}*/
	//	�ð��� ���� ���� ���� ����

	switch( m_Info._Type)
	{
		case TRACER_TYPE_BULLET:
			{
				//	�Ѿ� �̵�
				VEC3D vVel, vDist;
				VEC3D vCurLength;
				
				i3Vector::Scale( &vVel, &m_vDir, m_Info._EndSpeed);		//	1�ʿ� ���ư��� �Ÿ�
				i3Vector::Scale( &vVel, &vVel, tm);				//	���� �����ӿ��� ���ư��� �Ÿ�

				//	���ư��� �Ѿ��� Start�� End�� ��ǥ�� ���
				i3Vector::Add( &m_vCurStart, &m_vCurStart, &vVel);
				i3Vector::Scale( &vDist, &m_vDir, m_Info._Length);	
				i3Vector::Add( &m_vCurEnd, &m_vCurStart, &vDist);

				//	������� ���ư� �Ÿ�(Sq)
				i3Vector::Sub( &vCurLength, &m_vCurStart, &m_Info._vBeginPos);
				m_fCurLengthSq = i3Vector::LengthSq( &vCurLength);
				m_fCurLengthSq = i3Math::abs( m_fCurLengthSq);

				//	���� ������ ��� ������ �İ���� ���ϰ� ������ �߶���ϴ�.
				i3Vector::Sub( &vCurLength, &m_vCurEnd, &m_Info._vBeginPos);
				REAL32 fTracerTailLengthSq = i3Vector::LengthSq( &vCurLength);
				if( fTracerTailLengthSq > m_fDestLengthSq)
				{
					REAL32 gap = i3Math::Sqrt(fTracerTailLengthSq) - i3Math::Sqrt(m_fDestLengthSq); 
					i3Vector::Scale( &vDist, &m_vDir, MAX(0, m_Info._Length - gap));
					i3Vector::Add( &m_vCurEnd, &m_vCurStart, &vDist);
				}	
				
				I3ASSERT( m_pBillboard != nullptr);
				//	�Ѿ��� ��ġ ����
				m_pBillboard->SetStart( m_nIdx, &m_vCurStart);
				m_pBillboard->SetEnd( m_nIdx, &m_vCurEnd);
			}
			break;
		case TRACER_TYPE_BULLET_SMOKE:
			{
				//	�Ѿ� �̵�
				VEC3D vVelStart, vVelEnd;
				VEC3D vCurLength;
				REAL32 fTracerLengthSq = 0.0f;

				i3Vector::Scale( &vVelStart, &m_vDir, m_Info._StartSpeed);		//	1�ʿ� ���ư��� �Ÿ�
				i3Vector::Scale( &vVelEnd, &m_vDir, m_Info._EndSpeed);			//	1�ʿ� ���ư��� �Ÿ�
				i3Vector::Scale( &vVelStart, &vVelStart, tm);	 		 	//	���� �����ӿ��� ���ư��� �Ÿ�
				i3Vector::Scale( &vVelEnd, &vVelEnd, tm);	 		 		//	���� �����ӿ��� ���ư��� �Ÿ�

				i3Vector::Add( &m_vCurStart, &m_vCurStart, &vVelStart);
				i3Vector::Add( &m_vCurEnd, &m_vCurEnd, &vVelEnd);

				i3Vector::Sub( &vCurLength, &m_vCurEnd, &m_Info._vBeginPos);
				m_fCurLengthSq = i3Vector::LengthSq( &vCurLength);
				m_fCurLengthSq = i3Math::abs( m_fCurLengthSq);

				//	���� ������ ��� ������ �İ���� ���ϰ� ������ �߶���ϴ�.
				fTracerLengthSq = i3Vector::LengthSq( &vCurLength);
				if( fTracerLengthSq > m_fDestLengthSq)
				{
					i3Vector::Copy( &m_vCurEnd, &m_Info._vDestPos);
				}

				i3Vector::Sub( &vCurLength, &m_vCurStart, &m_Info._vBeginPos);
				fTracerLengthSq = i3Vector::LengthSq( &vCurLength);
				if( fTracerLengthSq > m_fDestLengthSq)
				{	//	�̹� ���������Ƿ� ������.
					i3Vector::Copy( &m_vCurStart, &m_Info._vDestPos);
					SetAlpha( 0);

					return;
				}
				
				I3ASSERT( m_pBillboard != nullptr);
				//	�Ѿ��� ��ġ ����
				m_pBillboard->SetStart( m_nIdx, &m_vCurStart);
				m_pBillboard->SetEnd( m_nIdx, &m_vCurEnd);

				//	�޽��� ���̰� �ؽ�ó�� ������ ���̺��� ������� texture UV���� ������� �ε巴�� Ÿ�ϸ��ǵ��� �����Ѵ�.
				i3Vector::Sub( &vCurLength, &m_vCurEnd, &m_vCurStart);
				REAL32 totalLen = i3Vector::Length(&vCurLength);  
				REAL32 repeatCnt = totalLen / m_Info._RepeatTexSize; 
				m_pBillboard->SetTextureCoord( m_nIdx, 0.f, -repeatCnt, 1.f, 1.0f);

				if( m_fCurTime > 0.0f) 
				{
					//	���� ���� �ð� 
					m_fCurTime = MAX( 0.0f, m_fCurTime - tm); 
										
					SetAlpha( (UINT8) (m_Info._color.a * MAX(0.f, (m_fCurTime / m_Info._LifeTime))));

					m_fCurThickness = m_Info._Thickness + 
										((m_Info._GrowThickness - m_Info._Thickness) * (1.0f - (m_fCurTime / m_Info._LifeTime)));

					m_pBillboard->SetSize( m_nIdx, m_fCurThickness);
				}
				else
				{
					SetAlpha(0);
				}
			}
			break;
		case TRACER_TYPE_HAND_GRENADE:
		case TRACER_TYPE_SHELL_GRENADE:
		case TRACER_TYPE_ARROW:
		case TRACER_TYPE_SMOKE:
		case TRACER_TYPE_FLASH:
			{
				if( m_fCurTime > 0.0f)
				{
					m_fCurTime = MAX( 0.0f, m_fCurTime - tm);
										
					SetAlpha( (UINT8) (m_Info._color.a * MAX(0.f, (m_fCurTime / m_Info._LifeTime))));
				}
			}
			break;
	}
}

