#include <string>
#include <chrono>
#include "methods/my_text_processor.cpp"
#include <json.hpp>

using json = nlohmann::json;

void verbose_print(const std::string &msg, bool verbose) {
    if (verbose)
        std::cout << msg;
}

void test_realization(
        const std::string &name,
        json config_json,
        std::list<int> (*tested_func)(const std::string &input_name, const std::list<std::string> &word_list),
        bool verbose
) {
    std::cout << "---------------TEST " + name + " --------------------------------------------\n";
    auto begin_time = std::chrono::high_resolution_clock::now();
    for (json curr_json:config_json["input_list"]) {
        std::string input_name = curr_json["name"];
        std::list<std::string> word_list = curr_json["word_list"];
        std::list<int> count_test_list = curr_json["test_list"];

        verbose_print("-------- FILE = " + input_name + "-----------------\n", verbose);

        // ------------------------ function call -----------------------------
        std::list<int> count_list = tested_func(
                input_name,
                word_list
        );

        // ------------------------ results assertion -----------------------------
        auto word_it = word_list.begin();
        auto cnt_it = count_list.begin();
        auto cnt_test_it = count_test_list.begin();
        for (; word_it != word_list.end() && cnt_it != count_list.end(); ++word_it, ++cnt_it, ++cnt_test_it) {
            assert(*cnt_it == *cnt_test_it);
            verbose_print(
                    "\'" + (*word_it) + "\' : " + std::to_string(*cnt_it) + "(" + std::to_string(*cnt_test_it) + ")" +
                    '\n', verbose);
        }
        verbose_print("ASSERTION PASSED\n", verbose);

    }

    auto end_time = std::chrono::high_resolution_clock::now();
    verbose_print("------------ TIME -------------\n", verbose);
    verbose_print("time : " + std::to_string((end_time - begin_time).count() * 1e-9) + '\n', verbose);
}


int main() {
    json config_json = json::parse(read_file("config.json"));

    bool verbose = true;
    test_realization("SEQUENTIAL", config_json, &my_count_words_sequential, verbose);
}