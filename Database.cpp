#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"

#include "Database.h"

// ===============================================================================
// SQL���� ��������� ���ڵ��� ���ε�Ǹ�
// ��ɹ�(statement)�� SQLExecDirect�� ����ȴ�.
// ���� ������ ����Ǵ� ������ SQLPrepare()�� �غ��ϰ�,
// SQLExecute�� ����ؾ��ϴ� ���� ���ɸ鿡 �����ϴ�.
//
// ����,
// SQL���� �Ķ���� ��Ŀ���� �����ϸ�, ���ø����̼���
// SQLBindParameter() �� ȣ���Ͽ� �� ���ڵ��� ���ø����̼� ������
// Bind ��Ų��.
//
// ���ڸ� Bind �ϱ�����SQLPrepare()�� �����ؾ��ϸ�,
// ���ε� �Ŀ� SQLExecute()�� ������ �� �ִ�.
//
// ���� ����� ��ɹ�(statement) �� SELECT ���̶��
// ���� calum Count�� �˾ƺ��� ���� SQLNumResultCols()�� ȣ���Ѵ�.
//
// �� ���� SQLDescribeCol()�� �� ��� ���տ��� �����͸� �����´�.
//
// �� �� ���ø����̼� ������ ��� ������ ���� ���ε��ϴ�
// SQLBindCol()�� �̷� ������ ������.
// (�� �������� db�� Attribute �� �˰� �ֱ� ������
// ������ ������ attribute�� �м��ϱ� ���ٴ� �׳� bind�� �������)
//
// ���� SQLFetch() �� ȣ���Ͽ� ù��° �� �����͸� ��������
// SQLBindCol() �� ���ε� �� ������ �����͸� �����Ѵ�.
// �࿡ �� �����Ͱ� ������ SQLGetData()�� �� �� �ְ� ��� ȣ���� �� �� �ִ�.
//
// data�� fetch�� ���Ͽ� ������ ���Ŀ��� �ٸ� Statement��
// SQLPrepare() �� SQLExecute()�� ���� �����ϱ�����
//
// �ݵ�� SQLCloseCurser()�� ȣ���Ͽ� Ŀ���� �ݾ������.
//
// ���� ���� ����� Statement �� INSERT, UPDATE, DELETE ���ٸ�
// SQLRowCount() �� ������ ���� ����� ������ �����´�.
// ===============================================================================


Database::Database()
{
	_ODBC_NAME = (SQLWCHAR*)L"my_SQL";
	_ODBC_ID = (SQLWCHAR*)L"root";
	_ODBC_PW = (SQLWCHAR*)L"7156";

	this->DBConnect();
	this->AttributeBind();
}

Database::~Database()
{}

Database* Database::GetInstance()
{
	static Database dbManager;

	return &dbManager;
}

//----------------------------------------------------------
// Database Connect
//----------------------------------------------------------
bool Database::DBConnect()
{
	// ȯ�� �ڵ� �Ҵ�
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_hEnv) != SQL_SUCCESS)
	{
		printf_s("SQL Handle Allocate Failed \n");

		return false;
	}

	// ���� ȯ�濡 ���� �Ӽ��� ����
	if (SQLSetEnvAttr(_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
	{
		printf_s("SQL Set Env Attr Failed \n");

		return false;
	}

	// ���� �ڵ� ����
	if (SQLAllocHandle(SQL_HANDLE_DBC, _hEnv, &_hDbc) != SQL_SUCCESS)
	{
		printf_s("SQL Handle Allocate Failed \n");

		return false;
	}

	// ������ ����
	if (SQLConnect(_hDbc, _ODBC_NAME, SQL_NTS, _ODBC_ID, SQL_NTS, _ODBC_PW, SQL_NTS) != SQL_SUCCESS)
	{
		printf_s("SQL Handle Connect Failed \n");

		return false;
	}

	// ��ɹ� �ڵ��� �Ҵ�
	if (SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt) != SQL_SUCCESS)
	{
		printf_s("SQL Handle Allocate Failed \n");

		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------
// Database DisConnect
//---------------------------------------------------------------------------------
void Database::DBDisConnect()
{
	if (_hStmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, _hStmt);
	}

	if (_hDbc)
	{
		SQLDisconnect(_hDbc);
	}

	if (_hDbc)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, _hDbc);
	}

	if (_hEnv)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, _hEnv);
	}
}

//----------------------------------------------------------------------------------
// Bind
//----------------------------------------------------------------------------------
void Database::AttributeBind()
{
	SQLBindCol(_hStmt, 1, SQL_WCHAR, _name, sizeof(WCHAR) * 20, &_nameLen);
	SQLBindCol(_hStmt, 2, SQL_WCHAR, _password, sizeof(WCHAR) * 20, &_passwordLen);
}

//---------------------------------------------------------------------------------
// Insert
//---------------------------------------------------------------------------------
bool Database::Insert(std::wstring pName, std::wstring pPassword)
{
	WCHAR cInsertCommand[100] = { 0, };

	wsprintf(cInsertCommand, L"INSERT INTO id_pw (_id, _pw) VALUES ('%s', '%s');", pName.c_str(), pPassword.c_str());
	printf_s("%ws\n", cInsertCommand);
	//SLog(L"%ws\n", cInsertCommand);

	int iRetVal = SQLExecDirect(_hStmt, (SQLWCHAR*)cInsertCommand, SQL_NTS);
	if (iRetVal != SQL_SUCCESS)
	{
		printf_s("\nInsertFaild\n");
		if (_hStmt)
		{
			SQLCloseCursor(_hStmt);
		}

		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------
// Delete
//-----------------------------------------------------------------------------------
bool Database::Delete(std::wstring pID)
{
	WCHAR cDeleteCommand[100] = { 0, };

	wsprintf(cDeleteCommand, L"DELETE FROM id_pw WHERE _id='%s'", pID.c_str());
	printf_s("%ws\n", cDeleteCommand);

	int iRetVal = SQLExecDirect(_hStmt, (SQLWCHAR*)cDeleteCommand, SQL_NTS);
	if (iRetVal != SQL_SUCCESS)
	{
		printf_s("DeleteFaild\n");
		if (_hStmt)
		{
			SQLCloseCursor(_hStmt);
		}

		return false;
	}

	if (_hStmt)
	{
		SQLCloseCursor(_hStmt);
	}

	return true;
}

//--------------------------------------------------------------------------------
// Match
//--------------------------------------------------------------------------------
bool Database::Match(std::wstring pName, std::wstring pPassword, DB_MATCH_TYPE matchType)
{
	WCHAR cMatchCommand[100] = { 0, };

	wsprintf(cMatchCommand, L"Select * FROM id_pw");
	printf_s("%ws\n", cMatchCommand);

	int iRetVal = SQLExecDirect(_hStmt, (SQLWCHAR*)cMatchCommand, SQL_NTS);
	if (iRetVal != SQL_SUCCESS)
	{
		printf_s("MatchFailed\n");
		if (_hStmt)
		{
			SQLCloseCursor(_hStmt);
		}

		return false;
	}

	puts("-------------------------------------------------------------------------------");
	printf_s("ID : %ws\t Password : %ws\n", pName.c_str(), pPassword.c_str());
	puts("-------------------------------------------------------------------------------");

	// ���ε尡 string�ڷ����� ���� �ʾƼ� char���۷� ��ȯ
	WCHAR wid[NAME_SIZE] = { 0, };
	WCHAR wpassword[NAME_SIZE] = { 0, };

	wcscpy(wid, pName.c_str());
	wcscpy(wpassword, pPassword.c_str());

	while (SQLFetch(_hStmt) != SQL_NO_DATA)
	{
		//printf_s("%ws\t%ws\n", _name, _password);

		// ȸ������ match
		if ((wcscmp(_name, wid) == 0) && (matchType == DB_MATCH_TYPE::REGISTER_TYPE))
		{
			if (_hStmt)
			{
				SQLCloseCursor(_hStmt);
			}
			//puts("ȸ������ ����!");

			return true;
		}
		// �α��� match
		else if ((wcscmp(_name, wid) == 0) && (wcscmp(_password, wpassword) == 0)
			&& (matchType == DB_MATCH_TYPE::LOGIN_TYPE))
		{
			if (_hStmt)
			{
				SQLCloseCursor(_hStmt);
			}
			//puts("�α��� ����!\n");

			return true;
		}
	}

	if (matchType == DB_MATCH_TYPE::REGISTER_TYPE)
	{
		//puts("ȸ������ ����!");
		if (_hStmt)
		{
			SQLCloseCursor(_hStmt);
		}
	}
	else if (matchType == DB_MATCH_TYPE::LOGIN_TYPE)
	{
		//puts("�α��� ����!");
		if (_hStmt)
		{
			SQLCloseCursor(_hStmt);
		}
	}

	return false;
}

//--------------------------------------------------------------------------------
// Show
//--------------------------------------------------------------------------------
void Database::Show() const
{
	WCHAR cShowCommand[100] = { 0, };

	wsprintf(cShowCommand, L"Select * FROM id_pw");
	printf_s("%ws\n", cShowCommand);

	int iRetVal = SQLExecDirect(_hStmt, (SQLWCHAR*)cShowCommand, SQL_NTS);
	if (iRetVal != SQL_SUCCESS)
	{
		printf_s("ShowFalied\n");
		if (_hStmt)
		{
			SQLCloseCursor(_hStmt);
		}

		return;
	}

	while (SQLFetch(_hStmt) != SQL_NO_DATA)
	{
		printf_s("ID: %ws\t PW: %ws\n", _name, _password);
	}

	if (_hStmt)
	{
		SQLCloseCursor(_hStmt);
	}
}