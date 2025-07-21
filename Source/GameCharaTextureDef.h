#if !defined( _GAME_CHARA_TEXTURE_DEFINE)
#define _GAME_CHARA_TEXTURE_DEFINE

//--------------------------------------------------------------------------------------------------
//	ĳ���� �ؽ�ó �����̳� ����
//
//	  (CharaTexture)		(CharaColorSet)		(CharaTexSet)		   (i3Texture)
//
//	m_TexChara[ TypeA] --> _ColorSetList(0) --> _TexSet[ ARMS] --> _pTexDiff (i3Texture)
// 			:		    |					 |					|
//					    -> _ColorSetList(1)  -> _TexSet[ FACE]  -> _pTexNorm (i3Texture)
//					    :					 |					|
//											 -> _TexSet[ BODY]  -> _pTexSpec (i3Texture)
//
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//	��� �ؽ�ó �����̳� ����
//
//	  (CharaTexture)			(CharaColorSet)		(CharaTexSet)		   (i3Texture)
//
//	m_TexEquip[ type][ num] --> _ColorSetList(0) --> _TexSet[ 0] --> _pTexDiff (i3Texture)
// 			:		         |					  |			      |
//					         -> _ColorSetList(1)  -> _TexSet[ 0]  -> _pTexNorm (i3Texture)
//					         :					  |		   	      |
//											      -> _TexSet[ 0]  -> _pTexSpec (i3Texture)
//
//--------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Chara Texture Resource Container (CharaManager�� ����)
//////////////////////////////////////////////////////////////////////////////////////////////////////

enum CHARATEX_BODY
{
	CHARATEX_BODY_ARM = 0,		//	1��Ī ��	
	CHARATEX_BODY_BODY,			//	3��Ī ����
	//CHARATEX_BODY_FACE,			//	3��Ī ��

	CHARATEX_BODY_COUNT
};

enum CHARATEX_TYPE
{
	CHARATEX_TYPE_DIFF = 0,		
	CHARATEX_TYPE_NORM,			
	CHARATEX_TYPE_SPEC,			

	CHARATEX_TYPE_COUNT
};

struct CharaTexSet
{
	i3Texture *		_pTexDiff = nullptr;	//	diffuse �ؽ�ó
	i3Texture *		_pTexNorm = nullptr;	//	normal map �ؽ�ó (null�� ��� ���� �ؽ�ó�� ����մϴ�.)
	i3Texture *		_pTexSpec = nullptr;	//	specular �ؽ�ó (null�� ��� ���� �ؽ�ó�� ����մϴ�.)

} ;

struct CharaColorSet
{
	CharaTexSet		_TexSet[ CHARATEX_BODY_COUNT];
	
} ;

struct ClanTexture
{
	i3::vector<i3Texture*>	_ClanList;								//	Ŭ�� ��ũ �ؽ�ó ����Ʈ (i3Texture)

};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	Chara Texture Bind Container (CharaBase�� ����)
//////////////////////////////////////////////////////////////////////////////////////////////////////

struct CharaTexBindSet
{
	i3TextureBindAttr *		pDiffBind = nullptr;	//	diffuse �ؽ�ó ���ε�
	i3NormalMapBindAttr *	pNormBind = nullptr;	//	normal map �ؽ�ó ���ε�
	i3SpecularMapBindAttr * pSpecBind = nullptr;	//	specular �ؽ�ó ���ε�

} ;

//	3��Ī LOD ��
#define	CHARA_3PV_LOD_COUNT		3


//--------------------------------------
//	CHARA_ATTRSET
//--------------------------------------
enum CHARA_ATTRSET
{
	CHARA_ATTRSET_ARM = 0,
	CHARA_ATTRSET_LOW_BODY,
	CHARA_ATTRSET_UPPER_BODY,
	CHARA_ATTRSET_CLAN,
	CHARA_ATTRSET_FACE,

	CHARA_ATTRSET_COUNT
};

//	ĳ���� �𵨸��� Geometry�� ������ AttrSet �̸��� �ݵ�� ��ġ�ؾ� �ϸ� ��ġ�� �̸��� ����� �Ѵ�.

//	���� ĳ���͸� CharaTool ���� Export�ϸ� �ϳ��� LOD�� "Model_UpperBody_Material" AttrSet�� 2���� ���ɴϴ�.
//	���⼭ Ŭ�� �ؽ�ó�� ����ϰ� �ִ� "Model_UpperBody_Material" �̸��� "Model_ClanMark_Material"�� GraphTool���� �ٲ���� �Ѵ� 
//	�̸��� �������� ������ �ٸ� Ŭ�� ��ũ �ؽ�ó�� ��ü�� �Ұ����ϴ�. ���� - komet
const char g_szCharaAttrSetName[ CHARA_ATTRSET_COUNT][256] = 
{
	"Model_Arms_Material",			// CHARA_ATTRSET_ARM
	"Model_LowBody_Material",		// CHARA_ATTRSET_LOW_BODY
	"Model_UpperBody_Material",		// CHARA_ATTRSET_UPPER_BODY
	"Model_Clan_Material",		// CHARA_ATTRSET_CLAN
	"Model_Face_Material",			// CHARA_ATTRSET_FACE
};




#endif //_GAME_CHARA_TEXTURE_DEFINE