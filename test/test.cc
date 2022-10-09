#include <iostream>
#include "src/log.h"
#include "src/util.h"

int main(int argc, char** argv) {

    alotz::Logger::ptr logger(new alotz::Logger);
    logger->addAppender(alotz::LogAppender::ptr(new alotz::StdoutLogAppender));

    alotz::FileLogAppender::ptr file_appender(new alotz::FileLogAppender("./log.txt"));
    alotz::LogFormatter::ptr fmt(new alotz::LogFormatter("%d%t%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(alotz::LogLevel::ERROR);

    logger->addAppender(file_appender);

    std::cout << "hello log" << std::endl;
    ALOTZ_LOG_INFO(logger) << "test macro";
    ALOTZ_LOG_ERROR(logger) << "test macro error";

    ALOTZ_LOG_FMT_ERROR(logger, "test macro formatter error %s", "aa");

    auto l = alotz::LoggerMgr::GetInstance()->getLogger("xx");
    ALOTZ_LOG_INFO(l) << "xxx";
    return 0;
}