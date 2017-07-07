#include "sql_api.h"

sql_api::sql_api()
	:_conn_fd(NULL)
	 ,_host("127.0.0.1")
	 ,_user("root")
	 ,_password("123")
	 ,_db("9_class")
	 ,_port(3306)
	 ,_res(NULL)
{}

sql_api::~sql_api()
{
	if(_res)
	{
		free(_res);
		_res=NULL;
	}
	mysql_close(_conn_fd);
	cout<<"close"<<endl;
}

bool sql_api::_connect_mysql()
{
	bool ret=false;
	_conn_fd=mysql_init(NULL);
	cout<<_conn_fd<<endl;
	cout<<"*****************"<<endl;
	//与运行在host上的mysql数据库建立一个连接
	//mysql_real_connect(_conn_fd,_host.c_str(),_user.c_str(),_password.c_str(),_db.c_str(),_port,NULL,0);
	//return true;
	if(mysql_real_connect(_conn_fd,_host.c_str(),_user.c_str(),_password.c_str(),_db.c_str(),_port,NULL,0))
	{
		cout<<"connect success"<<endl;
		ret=true;
	}
	else
	{
		cout<<"connect error"<<strerror(errno)<<endl;
	}
	return ret;
}

bool sql_api::_close_mysql()
{
	mysql_close(_conn_fd);
	return true;
}

bool sql_api::_op_mysql(MYSQL *_fd,string _sql)
{
	bool ret=false;
	if(mysql_query(_fd,_sql.c_str())==0)
	{
		cout<<"op success"<<endl;
		ret=true;
	}
	else
	{
		cout<<"op fail"<<endl;
	}
	return ret;
}

bool sql_api::_insert_mysql(string name ,string school,string hobby)
{
	string _sql="INSERT INTO info ";
	_sql += "(name,school,hobby) ";
	_sql += "VALUES('";
	_sql += name;
	_sql += "','";
	_sql += school;
	_sql += "','";
	_sql += hobby;
	_sql += "')";
	cout<<_sql<<endl;
	return _op_mysql(_conn_fd,_sql);
}

bool sql_api::_select_mysql(string _sql)
{
	bool ret=false;
	ret=_op_mysql(_conn_fd,_sql);//将查询的全部结果读取到客户端，分配1个MYSQL_RES结构，并将结果置于该结构中。
	if(ret)
	{
		_res=mysql_store_result(_conn_fd);
		int rows=mysql_num_rows(_res);//获取由select语句查询到的结果集中行的数目
		int fields=mysql_num_fields(_res);//函数返回结果集中字段的数(列数)。
		MYSQL_FIELD *fd=NULL;
		string _col_name;
		for(;fd=mysql_fetch_field(_res);) //函数从结果集中取得列信息并作为对象返回
		{
			cout<<fd ->name<<'\t';
		}
		cout << endl;

		while(rows)
		{
			MYSQL_ROW row=mysql_fetch_row(_res);
			int i=0;
			for(;i<fields;++i)
			{
				cout<<row[i]<<'\t';
			}
			cout << endl;
			rows--;
		}
	}
}
void sql_api::select_mysql_all()
{
	string _sql="select * from info";
	_select_mysql(_sql);
}
void sql_api::_select_mysql_name(string name )
{
	string str="select * from info where name='";
	str+=name;
	str+="'";
	cout <<"str:"<<str<<endl;
	_select_mysql(str);
}

bool sql_api::_delete_mysql(string name,string id)
{
	cout<<"###########"<<endl;
	bool ret=false;
	string _sql="delete from info where name='";
	_sql+=name;
	_sql+="'";
	_sql+="and id='";
	_sql+=id;
	_sql+="'";
	cout<<"_sql:"<<_sql<<endl;
	return _op_mysql(_conn_fd,_sql);
}

bool sql_api::_update_mysql(string name,string school,string hobby,string id)
{
	string _sql="update info set name='";
	_sql+=name;
	_sql+="',school='";
	_sql+=school;
	_sql+="',hobby='";
	_sql+=hobby;
	_sql+="' where id=";
	_sql+=id;
	cout<<"_sql:"<<_sql<<endl;
	return _op_mysql(_conn_fd,_sql);
}

void get_data(string &query_string,vector<string>& ret)
{
	int index=query_string.size()-1;
	string str=query_string;
	while(index>=0)
	{
		if(str[index]=='=')
		{
			ret.push_back(&str[index+1]);
		}
		else
		{
			if(str[index]=='&')
			{
				str[index]='\0';
			}
		}
		
		--index;
	}
}
