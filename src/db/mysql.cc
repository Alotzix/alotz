#include "mysql.h"
#include "src/log.h"

namespace alotz {

static alotz::Logger::ptr g_logger = ALOTZ_LOG_ROOT();

MysqlDB::MysqlDB() {
    m_mysql = mysql_init(NULL);
    if (m_mysql == NULL) {
        ALOTZ_LOG_ERROR(g_logger) << "mysql_init error." << mysql_error(m_mysql);
        exit(1);
    }
}

MysqlDB::~MysqlDB() {
    if (m_mysql != NULL) {
        mysql_close(m_mysql);
    }
}

void MysqlDB::connect(const std::string& host, const std::string& user, const std::string& password, const std::string& database) {
    char value = 1;
    mysql_options(m_mysql, MYSQL_OPT_RECONNECT, (char *)&value);
    m_mysql = mysql_real_connect(m_mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0);
    if (m_mysql == NULL) {
        ALOTZ_LOG_ERROR(g_logger) << "mysql connect error.";
        exit(1);
    }
}

void MysqlDB::execute(const std::string& sql) {
    if (mysql_query(m_mysql, sql.c_str())) {
        ALOTZ_LOG_ERROR(g_logger) << "execute sql error.";
        exit(1);
    } else {
        m_result = mysql_use_result(m_mysql);
        for (size_t i = 0; i <= mysql_field_count(m_mysql); ++i) {
            m_row = mysql_fetch_row(m_result);
            if (m_row <= 0) {
                break;
            }
            for (size_t j = 0; j < mysql_num_fields(m_result); ++j) {
                std::cout << m_row[j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    mysql_free_result(m_result);
}

}