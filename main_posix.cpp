#include "methods/my_text_processor.cpp"


/***********************************************************************************************************************
------------------------------------------------------- MAIN -----------------------------------------------------------
***********************************************************************************************************************/
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Thread num must be passed\n");
        exit(1);
    }
    int thread_num = atoi(argv[1]);

    json config_json = json::parse(read_file("config.json"));

    bool verbose = false;
    json posix_json = test_func("POSIX", config_json, &my_count_pattern_parallel_posix, verbose, thread_num);
    std::ofstream res_file("res/res_posix.json");
    res_file << posix_json;

}