#include "src/config.h"
#include "src/log.h"
#include <yaml-cpp/yaml.h>

alotz::ConfigVar<int>::ptr g_int_value_config = alotz::Config::Lookup("system.port", (int)8080, "system port");

alotz::ConfigVar<float>::ptr g_float_value_config = alotz::Config::Lookup("system.value", (float)10.2f, "system value");

void test_config() {
    ALOTZ_LOG_INFO(ALOTZ_LOG_ROOT()) << "before:" << g_int_value_config->getValue();
    ALOTZ_LOG_INFO(ALOTZ_LOG_ROOT()) << "before:" << g_float_value_config->toString();

    YAML::Node root = YAML::LoadFile("/home/lighthouse/workspace/alotz/bin/conf/log.yml");
    alotz::Config::LoadFromYaml(root);

    ALOTZ_LOG_INFO(ALOTZ_LOG_ROOT()) << "after:" << g_int_value_config->getValue();
    ALOTZ_LOG_INFO(ALOTZ_LOG_ROOT()) << "after:" << g_float_value_config->toString();
}

int main(int argc, char** argv) {
    test_config();
    return 0;
}