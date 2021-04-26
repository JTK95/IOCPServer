#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"

#include "Database.h"

// ===============================================================================
// SQL문이 만들어지고 인자들이 바인드되면
// 명령문(statement)는 SQLExecDirect로 실행된다.
// 만약 여러번 수행되는 구문은 SQLPrepare()로 준비하고,
// SQLExecute로 사용해야하는 것이 성능면에 유리하다.
//
// 만약,
// SQL문이 파라미터 마커들을 포함하면, 애플리케이션은
// SQLBindParameter() 를 호출하여 각 인자들을 애플리케이션 변수에
// Bind 시킨다.
//
// 인자를 Bind 하기전에SQLPrepare()를 실행해야하며,
// 바인딩 후에 SQLExecute()를 실행할 수 있다.
//
// 만약 실행된 명령문(statement) 가 SELECT 문이라면
// 집합 calum Count를 알아보기 위해 SQLNumResultCols()를 호출한다.
//
// 그 다음 SQLDescribeCol()로 각 결과 집합열의 데이터를 가져온다.
//
// 그 후 애플리케이션 변수와 결과 집합의 열을 바인드하는
// SQLBindCol()에 이런 정보를 보낸다.
// (내 생각에는 db의 Attribute 를 알고 있기 때문에
// 가져온 집합의 attribute를 분석하기 보다는 그냥 bind로 묶으면됨)
//
// 이제 SQLFetch() 를 호출하여 첫번째 행 데이터를 가져오고
// SQLBindCol() 로 바인드 된 변수에 데이터를 저장한다.
// 행에 긴 데이터가 있으면 SQLGetData()를 할 수 있고 계속 호출할 수 도 있다.
//
// data를 fetch를 통하여 가져온 이후에는 다른 Statement를
// SQLPrepare() 과 SQLExecute()를 통해 실행하기전에
//
// 반드시 SQLCloseCurser()를 호출하여 커서를 닫아줘야함.
//
// 만일 전에 수행된 Statement 가 INSERT, UPDATE, DELETE 였다면
// SQLRowCount() 로 영향을 받은 행들의 갯수를 가져온다.
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
	// 환경 핸들 할당
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_hEnv) != SQL_SUCCESS)
	{
		printf_s("SQL Handle Allocate Failed \n");

		return false;
	}

	// 현재 환경에 대한 속성을 설정
	if (SQLSetEnvAttr(_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
	{
		printf_s("SQL Set Env Attr Failed \n");

		return false;
	}

	// 연결 핸들 설정
	if (SQLAllocHandle(SQL_HANDLE_DBC, _hEnv, &_hDbc) != SQL_SUCCESS)
	{
		printf_s("SQL Handle Allocate Failed \n");

		return false;
	}

	// 데이터 연결
	if (SQLConnect(_hDbc, _ODBC_NAME, SQL_NTS, _ODBC_ID, SQL_NTS, _ODBC_PW, SQL_NTS) != SQL_SUCCESS)
	{
		printf_s("SQL Handle Connect Failed \n");

		return false;
	}

	// 명령문 핸들을 할당
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

	// 바인드가 string자료형을 받지 않아서 char버퍼로 변환
	WCHAR wid[NAME_SIZE] = { 0, };
	WCHAR wpassword[NAME_SIZE] = { 0, };

	wcscpy(wid, pName.c_str());
	wcscpy(wpassword, pPassword.c_str());

	while (SQLFetch(_hStmt) != SQL_NO_DATA)
	{
		//printf_s("%ws\t%ws\n", _name, _password);

		// 회원가입 match
		if ((wcscmp(_name, wid) == 0) && (matchType == DB_MATCH_TYPE::REGISTER_TYPE))
		{
			if (_hStmt)
			{
				SQLCloseCursor(_hStmt);
			}
			//puts("회원가입 실패!");

			return true;
		}
		// 로그인 match
		else if ((wcscmp(_name, wid) == 0) && (wcscmp(_password, wpassword) == 0)
			&& (matchType == DB_MATCH_TYPE::LOGIN_TYPE))
		{
			if (_hStmt)
			{
				SQLCloseCursor(_hStmt);
			}
			//puts("로그인 성공!\n");

			return true;
		}
	}

	if (matchType == DB_MATCH_TYPE::REGISTER_TYPE)
	{
		//puts("회원가입 성공!");
		if (_hStmt)
		{
			SQLCloseCursor(_hStmt);
		}
	}
	else if (matchType == DB_MATCH_TYPE::LOGIN_TYPE)
	{
		//puts("로그인 실패!");
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