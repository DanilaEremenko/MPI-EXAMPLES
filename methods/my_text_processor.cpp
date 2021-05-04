#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <list>
#include <thread>
#include <future>
#include <cassert>

/***********************************************************************************************************************
------------------------------------------------------- COMMON PART ----------------------------------------------------
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


int count_pattern_from_to(const std::string &full_text, const std::string &curr_pattern, int from, int to) {
    int curr_value = 0;
    assert((to - from) >= curr_pattern.length());
    assert(from >= 0);
    assert(from < full_text.length());
    assert(to > 0);
    assert(to <= full_text.length());
    assert(curr_pattern.length() > 0);
    assert(curr_pattern.length() < full_text.length());
    for (int start_i = from; start_i < to - curr_pattern.length(); ++start_i) {
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
        const std::string &input_name,
        const std::list<std::string> &word_list
) {
    std::list<int> count_list;

    std::string full_text = read_file(input_name);
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

        count_list.push_front(curr_value);

    }

    return count_list;
}
