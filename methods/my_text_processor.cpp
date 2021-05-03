#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <list>

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

int count_pattern_from_to(const std::string &full_text, const std::string &curr_pattern, int from, int to) {
    int curr_value = 0;
    for (int start_i = from; start_i < to - curr_pattern.length(); ++start_i) {
        std::string curr_sub_string = full_text.substr(start_i, curr_pattern.length());
        if (curr_sub_string == curr_pattern) {
            curr_value++;
        }
    }
    return curr_value;
}

std::list<int> my_count_words_sequential(
        const std::string &input_name,
        const std::list<std::string> &word_list
) {
    std::list<int> count_list;
    std::string full_text = read_file(input_name);

    for (const std::string &curr_pattern :word_list) {
        int curr_value = count_pattern_from_to(
                full_text,
                curr_pattern,
                0,
                full_text.length()
        );
        count_list.push_front(curr_value);
    }
    return count_list;
}
