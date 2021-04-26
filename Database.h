#pragma once

enum class DB_MATCH_TYPE
{
	REGISTER_TYPE,
	LOGIN_TYPE
};

//---------------------------------------------------------------------
// mySQL 연동 (Singleton Class)
//---------------------------------------------------------------------
class Database
{
public:
	static Database* GetInstance();

	bool DBConnect();
	void DBDisConnect();

	void AttributeBind();
	
	bool Insert(std::wstring pName, std::wstring pPassword);
	bool Delete(std::wstring pID);

	bool Match(std::wstring pName, std::wstring pPassword, DB_MATCH_TYPE matchType);

	void Show() const;

public:
	SQLWCHAR* _ODBC_NAME;
	SQLWCHAR* _ODBC_ID;
	SQLWCHAR* _ODBC_PW;

	SQLHENV _hEnv; // 환경 핸들
	SQLHDBC _hDbc; // 연결 핸들의 포인터
	SQLHSTMT _hStmt;

	WCHAR _name[20];
	WCHAR _password[20];

	SQLLEN _idLen;
	SQLLEN _nameLen;
	SQLLEN _passwordLen;

private:
	Database();
	~Database();
};
