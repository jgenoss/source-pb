#ifndef _LocaleValue_H__
#define _LocaleValue_H__

/*	���� ���� �� ��� ��
����	:	������ ���� ���� �� ���� �ٸ��� ������ Pef �� ����� �Ӽ� ����
			��Ʈ�� �ε����� �˻��� ���� ��� �´�.
			��Ʈ�� �ε��� ���� ����� �θ� ��Ʈ��/�ڽ� ��Ʈ�� ���� �����ȴ�. (������ '/')
*/
namespace LocaleValue
{
	/*	���� ���� ���� �ʱ�ȭ �Ѵ�.
	return	:	�ʱ�ȭ ���� true, ���н� false�� ��ȯ�Ѵ�.
	*/
	bool Init();

	/*	���� ���� ���� �����Ѵ�.
	*/
	void Release();

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(false ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	bool �������� true, false �� ��ȯ�Ѵ�. 
	*/
	bool Enable(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(false ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	bool �������� true, false �� ��ȯ�Ѵ�.
	*/
	bool GetBOOL(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��( 0 ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	INT32 �������� ��ȯ�Ѵ�.
	*/
	INT32 GetInt32(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(�� ��Ʈ�� ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	std::string �������� ��ȯ�Ѵ�.
	*/
	i3::rc_wstring GetString(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(0.0f ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	REAL32 �������� ��ȯ�Ѵ�
	*/
	REAL32 GetReal(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(x = 0.0f, y = 0.0f ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	VEC2D �������� ��ȯ�Ѵ�
	*/
	VEC2D GetVec2d(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(x = 0.0f, y = 0.0f, z = 0.0f ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	VEC3D �������� ��ȯ�Ѵ�
	*/
	VEC3D GetVec3d(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	VEC4D �������� ��ȯ�Ѵ�
	*/
	VEC4D GetVec4d(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	I3COLOR �������� ��ȯ�Ѵ�
	*/
	I3COLOR GetColor(const i3::string & ValueID);

	/*	������ �Ӽ����� �о� �´�.
	�޸�	:	�ش� ��Ʈ�� �ε����� �Ӽ� ���� ���� ���, �⺻��(��� ��ȯ)
	ValueID	:	�о� �� ��Ʈ�� �ε���
	return	:	MATRIX �������� ��ȯ�Ѵ�
	*/
	MATRIX GetMatrix(const i3::string & ValueID);
};

//#if defined( 
#define __L_DEFINED__( arg )	if( LocaleValue::Enable( arg ) )\
{
#define __L_ELSE_IF__ }\
else if( LocaleValue::Enable( arg ) )\
{
#define __L_ELSE__ }\
else\
{
#define __L_END__	}

#endif //_CONFIG_H__

