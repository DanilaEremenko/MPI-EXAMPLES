#include <string>
#include <chrono>
#include "methods/my_text_processor.cpp"
#include <json.hpp>
#include <fstream>

using json = nlohmann::json;

/***********************************************************************************************************************
------------------------------------------------------- YEAH -----------------------------------------------------------
***********************************************************************************************************************/
std::string read_file(const std::string &input_name) {
    std::ifstream in_stream(input_name.c_str());

    if (!in_stream) {
        std::cerr << "error opening stream";
        exit(0);
    }

    std::string curr_line;
    std::string res_string;
    while (std::getline(in_stream, curr_line)) {
        res_string += curr_line;
    }
    return res_string;
}

void verbose_print(const std::string &msg, bool verbose) {
    if (verbose)
        std::cout << msg;
}

/***********************************************************************************************************************
------------------------------------------------------- FUNC FOR TESTING -----------------------------------------------
***********************************************************************************************************************/
json test_func(
        const std::string &name,
        json config_json,
        std::list<int> (*tested_func)(const std::string &input_name, const std::list<std::string> &word_list),
        bool verbose
) {
    std::cout << "---------------TEST " + name + " --------------------------------------------\n";
    json res_json = config_json;
    int i = 0;
    for (json curr_json:config_json["input_list"]) {
        std::string input_name = curr_json["name"];

        verbose_print("-------- FILE = " + input_name + "-----------------\n", verbose);
        std::string full_text = read_file(input_name);
        std::list<std::string> word_list = curr_json["word_list"];
        std::list<int> count_test_list = curr_json["test_list"];

        // ------------------------ function call -----------------------------
        auto begin_time = std::chrono::high_resolution_clock::now();
        std::list<int> count_list = tested_func(
                full_text,
                word_list
        );
        auto end_time = std::chrono::high_resolution_clock::now();
        res_json["input_list"][i]["time"] = (end_time - begin_time).count() * 1e-9;
        i++;

        // ------------------------ results assertion -----------------------------
        auto word_it = word_list.begin();
        auto cnt_it = count_list.begin();
        auto cnt_test_it = count_test_list.begin();
        for (; word_it != word_list.end() && cnt_it != count_list.end(); ++word_it, ++cnt_it, ++cnt_test_it) {
            if (*cnt_it != *cnt_test_it) {
                std::cout << "TEST FAILED FOR " << curr_json["name"] << " : '" << *word_it << "' = " << *cnt_it << "\n";
                exit(1);
            }
            verbose_print(
                    "\'" + (*word_it) + "\' : " + std::to_string(*cnt_it) + "(" + std::to_string(*cnt_test_it) + ")" +
                    '\n', verbose);
        }
        verbose_print("ASSERTION PASSED\n", verbose);

    }

    return res_json;
}

/***********************************************************************************************************************
------------------------------------------------------- MAIN -----------------------------------------------------------
***********************************************************************************************************************/
int main() {
    json config_json = json::parse(read_file("config.json"));

    bool verbose = false;
    json seq_json = test_func("SEQUENTIAL", config_json, &my_count_words_sequential, verbose);
    json posix_json = test_func("POSIX", config_json, &my_count_words_parallel_posix, verbose);
    assert(seq_json.size() == posix_json.size());

    for (int i = 0; i < seq_json["input_list"].size(); ++i) {
        assert(seq_json["input_list"][i]["name"] == posix_json["input_list"][i]["name"]);
        float seq_time = seq_json["input_list"][i]["time"];
        float pos_time = posix_json["input_list"][i]["time"];
        std::cout << "--------------" << seq_json["input_list"][i]["name"] << "-------------\n";
        std::cout << "sequential : " << seq_time << "\n";
        std::cout << "posix      : " << pos_time << "(" << ((seq_time - pos_time) / seq_time * 100) << "%)" << "\n";
    }

}