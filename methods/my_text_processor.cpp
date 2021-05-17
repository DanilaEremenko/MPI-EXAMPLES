#include <cmath>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <thread>
#include <future>
#include <cassert>
#include <fstream>
#include <cstdarg>

/***********************************************************************************************************************
------------------------------------------------------- COMMON PART ----------------------------------------------------
***********************************************************************************************************************/
int
count_pattern_from_to(const std::string &full_text, const std::string &curr_pattern, int from, int to) {
    int curr_value = 0;
    assert(from >= 0);
    assert(from < full_text.length());
    assert(to > 0);
    assert(to <= full_text.length());
    assert(curr_pattern.length() > 0);
    if (curr_pattern.length() <= full_text.length())
        for (int start_i = from; start_i + curr_pattern.length() <= to; ++start_i) {
            assert(start_i + curr_pattern.length() <= full_text.length());
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
int my_count_pattern_sequential(
        const std::string &full_text,
        const std::string &curr_pattern,
        int thread_num
) {
    int curr_value = count_pattern_from_to(
            full_text,
            curr_pattern,
            0,
            full_text.length()
    );
    return curr_value;
}

/***********************************************************************************************************************
------------------------------------------------------- BATCH CONFLICTS SOLVER -----------------------------------------
***********************************************************************************************************************/
std::vector<std::pair<int, int>> get_smart_batch_bounds(
        int thread_num,
        const std::string &curr_pattern,
        const std::string &full_text
) {

    // get batch size
    int batch_size = std::ceil((float) full_text.size() / (float) thread_num);
    if (batch_size == curr_pattern.length()) {
        printf("To small text with size %lu for paralleling with thread num = %d\n", full_text.length(), thread_num);
        exit(1);
    }
    std::vector<std::pair<int, int>> batch_bounds;

    for (int i = 0; i < thread_num; ++i) {
        int curr_from = batch_size * i;
        int curr_to = fmin(batch_size * (i + 1), full_text.length());
        batch_bounds.emplace_back(std::make_pair(curr_from, curr_to));
    }

    std::vector<std::pair<int, int>> smart_batch_bounds = batch_bounds;
    for (int i = 0; i < thread_num - 1; ++i) {
        std::pair<int, int> curr_bounds = smart_batch_bounds[i];
        std::pair<int, int> check_bounds = std::make_pair(
                fmax(0, curr_bounds.second - curr_pattern.length() + 1),
                fmin(full_text.length(), curr_bounds.second + curr_pattern.length() - 1)
        );

        int conflict = count_pattern_from_to(
                full_text,
                curr_pattern,
                check_bounds.first,
                check_bounds.second
        );
        if (conflict) {
            smart_batch_bounds[i].second -= curr_pattern.length() - 1;
            smart_batch_bounds[i + 1].first -= curr_pattern.length() - 1;
        }
    }

    return smart_batch_bounds;
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


int my_count_pattern_parallel_posix(
        const std::string &full_text,
        const std::string &curr_pattern,
        int thread_num
) {
    int curr_value = 0;
    std::mutex mutex;
    std::list<std::thread> thread_list;

    std::vector<std::pair<int, int>> batches_bounds = get_smart_batch_bounds(thread_num, curr_pattern, full_text);
    for (std::pair<int, int> curr_bounds:batches_bounds) {

        thread_list.push_front(
                std::thread(
                        //func
                        count_pattern_from_to_wrapper,
                        //args
                        &curr_value,
                        &mutex,
                        full_text,
                        curr_pattern,
                        curr_bounds.first,
                        curr_bounds.second
                )
        );

    }
    for (std::thread &curr_thread:thread_list) curr_thread.join();

    return curr_value;
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

void verbose_print(const char *__restrict _format, ...) {
    if (false) {
        char buffer[256];
        va_list args;
        va_start(args, _format);
        vsnprintf(buffer, 255, _format, args);
        printf("%s", buffer);
        va_end(args);
    }
}

/***********************************************************************************************************************
------------------------------------------------------- FUNC FOR TESTING -----------------------------------------------
***********************************************************************************************************************/
#include <json.hpp>

using json = nlohmann::json;

void assert_count_results(const std::string &test_name,
                          std::list<std::string> word_list,
                          std::list<int> count_list,
                          std::list<int> count_test_list,
                          bool verbose
) {
    // ------------------------ results assertion -----------------------------
    auto word_it = word_list.begin();
    auto cnt_it = count_list.begin();
    auto cnt_test_it = count_test_list.begin();
    for (; word_it != word_list.end() && cnt_it != count_list.end(); ++word_it, ++cnt_it, ++cnt_test_it) {
        if (*cnt_it != *cnt_test_it) {
            std::cout << "TEST FAILED FOR " << test_name << " : '" << *word_it << "' = " << *cnt_it << "\n";
            exit(1);
        }
    }
    verbose_print("ASSERTION PASSED\n", verbose);
}

json test_func(
        const std::string &name,
        json config_json,
        int (*tested_func)(const std::string &input_name,
                           const std::string &curr_pattern,
                           int thread_num),
        bool verbose,
        int thread_num
) {
    verbose_print("---------------TEST %s --------------------------------------------\n", name.c_str());
    json res_json = config_json;
    std::string input_name = config_json["name"];

    verbose_print("-------- FILE = %s -----------------\n", input_name.c_str());
    std::string full_text = read_file(input_name);
    std::list<std::string> word_list = config_json["word_list"];
    std::list<int> count_test_list = config_json["test_list"];

    // ------------------------ function call -----------------------------
    auto begin_time = std::chrono::high_resolution_clock::now();
    std::list<int> count_list;
    for (const std::string &curr_pattern :word_list) {
        int curr_value = tested_func(
                full_text,
                curr_pattern,
                thread_num
        );
        count_list.push_back(curr_value);
    }
    auto end_time = std::chrono::high_resolution_clock::now();

    assert_count_results(
            config_json["name"],
            word_list,
            count_list,
            count_test_list,
            verbose
    );
    return res_json;
}


