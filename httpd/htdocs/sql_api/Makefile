MYSQL_INCLUDE=/usr/include/mysql
MYSQL_LIB=/usr/lib/mysql

BIN_INSERT=sql_cgi_insert
BIN_SELECT=sql_cgi_select
BIN_DELETE=sql_cgi_delete
BIN_UPDATE=sql_cgi_update
BIN_SELECT_NAME=sql_cgi_select_name

INCLUDE=-I$(MYSQL_INCLUDE)
LIB=-L$(MYSQL_LIB)
LIB_LINK=-lmysqlclient

all:$(BIN_INSERT) $(BIN_SELECT) $(BIN_SELECT_NAME) $(BIN_DELETE) $(BIN_UPDATE)
$(BIN_INSERT):sql_api.cpp insert.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK)
$(BIN_SELECT):sql_api.cpp select.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK)
$(BIN_SELECT_NAME):sql_api.cpp select_name.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK)
$(BIN_DELETE):sql_api.cpp delete.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK)
$(BIN_UPDATE):sql_api.cpp update.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB) $(LIB_LINK)

.PHONY:clean
clean:
	rm -f sql_cgi_insert sql_cgi_select $(BIN_SELECT_NAME) $(BIN_DELETE) $(BIN_UPDATA)
