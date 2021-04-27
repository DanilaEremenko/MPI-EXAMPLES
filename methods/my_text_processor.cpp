#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <map>
#include <list>

std::map<std::string, int> my_count_words(
        const std::string &input_name,
        const std::list<std::string> &word_list
) {
    std::map<std::string, int> word_map;
    for (const std::string &word :word_list) {
        word_map[word] = 0;
    }

    std::ifstream in_stream(input_name.c_str());

    if (!in_stream) {
        std::cerr << "error opening stream";
        exit(0);
    }

    std::string curr_text;


    int line_i = 0;
    while (!in_stream.eof()) {
        line_i++;
        std::getline(in_stream, curr_text);

        if (curr_text != "\r") {
            for (std::pair<std::string, int> it :word_map) {
                if (curr_text.length() < it.first.length()) // avoid pointless actions
                    continue;
                else { // finding words
                    int step_size = it.first.length();
                    int curr_value = 0;
                    for (int start_i = 0; start_i < curr_text.length() - step_size + 1; start_i += 1) {
                        std::string curr_sub_string = curr_text.substr(start_i, step_size);

                        if (curr_sub_string == it.first) {
                            curr_value++;
                        }
                    }

                    word_map[it.first] += curr_value;
//                    if (curr_value > 0)
//                        std::cout << line_i << " : " << it.first << "(" << curr_value << ")\n";
                }
            }
        }

    }
    in_stream.close();

    return word_map;
}
