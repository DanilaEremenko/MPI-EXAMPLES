#include <string>
#include <chrono>
#include "methods/my_text_processor.cpp"
#include <json.hpp>

using json = nlohmann::json;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "List of input files undefined\n";
        std::exit(1);
    }

    json config_json = json::parse(read_file("config.json"));

    auto begin_time = std::chrono::high_resolution_clock::now();
    for (json curr_json:config_json["input_list"]) {
        std::string input_name = curr_json["name"];
        std::list<std::string> word_list = curr_json["word_list"];
        std::list<int> count_test_list = curr_json["test_list"];

        std::cout << "-------- FILE = " << input_name << "-----------------\n";

        // ------------------------ function call -----------------------------
        std::list<int> count_list = my_count_words(
                input_name,
                word_list
        );

        // ------------------------ results assertion -----------------------------
        auto word_it = word_list.begin();
        auto cnt_it = count_list.begin();
        auto cnt_test_it = count_test_list.begin();
        for (; word_it != word_list.end() && cnt_it != count_list.end(); ++word_it, ++cnt_it, ++cnt_test_it) {
            assert(*cnt_it == *cnt_test_it);
            std::cout << "\'" << *word_it << "\'" << " : " << *cnt_it << '\n';
        }
        std::cout << "ASSERTION PASSED\n";

    }

    std::cout << "------------ TIME -------------\n";
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "time : " << (end_time - begin_time).count() * 1e-9 << '\n';

}