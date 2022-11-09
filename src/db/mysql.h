#ifndef ALOTZ_MYSQL_H
#define ALOTZ_MYSQL_H

#include <string>
#include <iostream>
#include <memory>
#include <mysql/mysql.h>

namespace alotz {

class MysqlDB {
public:
    typedef std::shared_ptr<MysqlDB> ptr;
    MysqlDB();
    ~MysqlDB();
    
    void connect(const std::string& host, const std::string& user, const std::string& password, const std::string& database);

    void execute(const std::string& sql);

private:
    MYSQL* m_mysql;
    MYSQL_ROW m_row;
    MYSQL_FIELD* m_field;
    MYSQL_RES* m_result;
};

}

#endif