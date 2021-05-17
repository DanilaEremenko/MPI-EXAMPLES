#include "methods/my_text_processor.h"

/***********************************************************************************************************************
------------------------------------------------------- MAIN -----------------------------------------------------------
***********************************************************************************************************************/
int main() {
    json config_json = json::parse(read_file("config.json"));

    bool verbose = false;
    json seq_json = test_func("SEQUENTIAL", config_json, &my_count_pattern_sequential, verbose, 0);
    std::ofstream res_file("res/res_sequential.json");
    res_file << seq_json;

}