#include <cmath>
#include <iostream>
#include <string>
#include <list>
#include <thread>
#include <future>
#include <cassert>
#include <fstream>

/***********************************************************************************************************************
------------------------------------------------------- COMMON PART ----------------------------------------------------
***********************************************************************************************************************/
int count_pattern_from_to(const std::string &full_text, const std::string &curr_pattern, int from, int to) {
    int curr_value = 0;
    assert(from >= 0);
    assert(from < full_text.length());
    assert(to > 0);
    assert(to <= full_text.length());
    assert(curr_pattern.length() > 0);
    if (curr_pattern.length() < full_text.length())
        for (int start_i = from; start_i < to - curr_pattern.length() + 1; ++start_i) {
            std::string curr_sub_string = full_text.substr(start_i, curr_pattern.length());
            if (curr_sub_string == curr_pattern) {
                curr_value++;
            }
        }
    return curr_value;
}

/***********************************************************************************************************************
------------------------------------------------------- SEQUENTIAL REALIZATION -----------------------------------------
***********************************************************************************************************************/
std::list<int> my_count_words_sequential(
        const std::string &full_text,
        const std::list<std::string> &word_list
) {
    std::list<int> count_list;
    for (const std::string &curr_pattern :word_list) {
        int curr_value = count_pattern_from_to(
                full_text,
                curr_pattern,
                0,
                full_text.length()
        );
        count_list.push_back(curr_value);
    }
    return count_list;
}

/***********************************************************************************************************************
------------------------------------------------------- POSIX REALIZATION ----------------------------------------------
***********************************************************************************************************************/
void count_pattern_from_to_wrapper(
        int *counter,
        std::mutex *mutex,
        const std::string &full_text,
        const std::string &curr_pattern,
        int from,
        int to) {
    int curr_value = count_pattern_from_to(
            full_text,
            curr_pattern,
            from,
            to
    );
    if (curr_value > 0) {
        mutex->lock();
        *counter += curr_value;
        mutex->unlock();
    }
}


std::list<int> my_count_words_parallel_posix(
        const std::string &full_text,
        const std::list<std::string> &word_list
) {
    std::list<int> count_list;
    const auto processor_count = std::thread::hardware_concurrency();

    for (const std::string &curr_pattern :word_list) {
        int curr_value = 0;
        std::mutex mutex;
        std::list<std::thread> thread_list;
        int batch_size = fmax(
                std::ceil((float) full_text.size() / (float) processor_count),
                100
        );

        for (int curr_batch_from = 0; curr_batch_from < full_text.size(); curr_batch_from += batch_size) {
            thread_list.push_front(
                    std::thread(
                            //func
                            count_pattern_from_to_wrapper,
                            //args
                            &curr_value,
                            &mutex,
                            full_text,
                            curr_pattern,
                            curr_batch_from,
                            fmin(curr_batch_from + batch_size, full_text.length())
                    )
            );

        }
        for (std::thread &curr_thread:thread_list) curr_thread.join();

        count_list.push_back(curr_value);

    }

    return count_list;
}

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
#include <json.hpp>
#include <fstream>

using json = nlohmann::json;


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
