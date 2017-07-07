#include"sql_api.h"

int main()
{
	sql_api _sa;
	_sa._connect_mysql();
	_sa.select_mysql_all();
	return 0;
}
