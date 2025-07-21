/**
 * \file	UI\UISubBase.h
 *
 * Declares the user interface sub base class.
 */


#if !defined( __UI_SUB_BASE_H__)
#define __UI_SUB_BASE_H__

#include "UIBase.h"
#include "UIDefine.h"

//-------------------------------------------------------------------------------//
//	UIBase�� Scene�� �����Ͽ� ó���ϴ� Class
//	���� i3UIScene�� ���� ���� ������ Parent�� UIBase���� �����Ѵ�.
//-------------------------------------------------------------------------------//

class UIMainFrame;
class UIFramework;

class UISubBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UISubBase, UIBase);

protected:
	LuaState *		_CallLuaFunction( const char * pszFunction);

	UIMainFrame *	GetMainFrame( void);

public:
	UISubBase();
	virtual ~UISubBase();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override {}

	/** \brief Phase �̵��� ȣ��˴ϴ�. */
	virtual void	OnMovePhase( UIPHASE phase) {}

	/** \brief Bind Scene
		\desc Ư�� Control Pointer�� �����ϰų� Lua Glue Function�� ����մϴ�. */
	virtual void	BindScene( i3UIScene * pScene) = 0;

	/** \brief Scene ������ ȣ��Ǿ� ����� �ʱ�ȭ���ش�. */
	virtual void	UnBindScene( void) = 0;
};

#endif
