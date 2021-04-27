//
// Created by danila on 4/27/21.
//

#ifndef MPI_EXAMPLES_MY_TEXT_PROCESSOR_H
#define MPI_EXAMPLES_MY_TEXT_PROCESSOR_H

#include "my_text_processor.cpp"

std::map<std::string, int> my_count_words(
        const std::string &input_name,
        const std::list<std::string> &word_list
);

#endif //MPI_EXAMPLES_MY_TEXT_PROCESSOR_H
