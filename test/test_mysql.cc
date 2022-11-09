#include "src/db/mysql.h"
#include "src/alotz.h"

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

int main(int argc, char** argv) {

    const std::string host = "43.143.146.32";
    const std::string user = "root";
    const std::string password = "1979Algopat#";
    const std::string database = "server_dev_1";
    alotz::MysqlDB::ptr dbtool(new alotz::MysqlDB);
    dbtool->connect(host, user, password, database);
    const std::string sql = "select * from test";
    dbtool->execute(sql);
    
    return 0;
}