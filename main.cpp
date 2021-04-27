#include <string>
#include <map>
#include "methods/my_text_processor.cpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "List of input files undefined\n";
        std::exit(1);
    }


    std::list<std::string> word_list = {
            "капитал",
            "день",
            "she"
    };


    auto begin_time = std::chrono::high_resolution_clock::now();
    for (int i = 1; i < argc; ++i) {
        std::string input_name = argv[i];
        std::cout << "-------- FILE = " << input_name << "-----------------\n";

        std::map<std::string, int> res_map = my_count_words(
                input_name,
                word_list
        );
        std::cout << "-------- RESULTS -----------------\n";
        for (std::pair<std::string, int> it :res_map) {
            std::cout << "\'" << it.first << "\'" << " : " << it.second << '\n';
        }

    }

    std::cout << "------------ TIME -------------\n";
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cout << "time : " << (end_time - begin_time).count() * 1e-9 << '\n';

}