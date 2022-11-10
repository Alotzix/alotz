#include <iostream>
#include "src/db/mysql.h"
#include "src/iomanager.h"

void run() {
    do {
        std::map<std::string, std::string> params;
        params["host"] = "43.143.146.32";
        params["user"] = "root";
        params["password"] = "1979Algopat#";
        params["database"] = "server_dev_1";

        alotz::MySQL::ptr mysql(new alotz::MySQL(params));
        if (!mysql->connect()) {
            std::cout << "connect fail" << std::endl;
            return;
        }
        
        alotz::MySQLStmt::ptr stmt = alotz::MySQLStmt::Create(mysql, "update test set count = ? where id = 1");
        stmt->bind(1, 100);
        int rt = stmt->execute();
        std::cout << "rt=" << rt << std::endl;
    } while (false);
    std::cout << "over" << std::endl;
}

int main(int argc, char** argv) {
    alotz::IOManager iom(1);
    iom.addTimer(1000, run, true);
    return 0;
}