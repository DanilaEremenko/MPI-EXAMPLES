#include "methods/my_text_processor.h"

/***********************************************************************************************************************
------------------------------------------------------- MAIN -----------------------------------------------------------
***********************************************************************************************************************/
int main() {
    json config_json = json::parse(read_file("config.json"));

    bool verbose = false;
    json seq_json = test_func("SEQUENTIAL", config_json, &my_count_words_sequential, verbose);
    std::ofstream res_file("res/seq_res.json");
    res_file << seq_json;
//    json posix_json = test_func("POSIX", config_json, &my_count_words_parallel_posix, verbose);
//    assert(seq_json.size() == posix_json.size());
//
//    for (int i = 0; i < seq_json["input_list"].size(); ++i) {
//        assert(seq_json["input_list"][i]["name"] == posix_json["input_list"][i]["name"]);
//        float seq_time = seq_json["input_list"][i]["time"];
//        float pos_time = posix_json["input_list"][i]["time"];
//        std::cout << "--------------" << seq_json["input_list"][i]["name"] << "-------------\n";
//        std::cout << "sequential : " << seq_time << "\n";
//        std::cout << "posix      : " << pos_time << "(" << ((seq_time - pos_time) / seq_time * 100) << "%)" << "\n";
//    }

}