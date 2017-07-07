#pragma once

#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <mysql.h>
#include <errno.h>

using namespace std;

class sql_api
{
public:
	sql_api();
	~sql_api();
	bool _connect_mysql();
	void _select_mysql_name(string name);
	void select_mysql_all();
	bool _delete_mysql(string name,string id);
    bool _insert_mysql(string name ,string school,string hobby);
	bool _update_mysql(string name,string school,string hobby,string id);
	bool _close_mysql();
private:
	
	bool _op_mysql(MYSQL* _fd,string _sql);
	bool _select_mysql(string _sql);
private:
	MYSQL* _conn_fd;
	MYSQL_RES* _res;
	string _host;
	string _user;
	string _password;
	string _db;
	short _port;
};

void get_data(string &query_string,vector<string>& ret);
