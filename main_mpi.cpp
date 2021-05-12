/***********************************************************************************************************************
------------------------------------------------------- MPI REALIZATION ----------------------------------------------
***********************************************************************************************************************/
#include <mpi.h>
#include <list>
#include <tgmath.h>
#include "methods/my_text_processor.h"

int main() {

    json config_json = json::parse(read_file("config.json"));

    int max_message_size = 0;
    for (json curr_json:config_json["input_list"]) {
        if (curr_json["word_list"].size() > max_message_size)
            max_message_size = curr_json["word_list"].size();
    }
//    printf("Max message size = %d\n", max_message_size);

    std::list<int> full_result;
    int rank, worker_size;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &worker_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        auto begin_time = std::chrono::high_resolution_clock::now();

        json res_json = config_json;
        for (int i = 0; i < res_json["input_list"].size(); ++i)
            for (int j = 0; j < res_json["input_list"][i]["test_list"].size(); ++j)
                res_json["input_list"][i]["test_list"][j] = 0;


        int received_num = 0;
        int max_message_n = (int) ((worker_size - 1) * res_json["input_list"].size());
        while (received_num < max_message_n) {
            int curr_msg[max_message_size];
//            printf("Root awaiting message(received_num = %d/%d)\n", received_num, max_message_n);
            MPI_Recv(&curr_msg, max_message_size + 5, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, nullptr);
//            printf("Root receive message\n");
//            printf("\tvalues size = %d\n", curr_msg[0]);
//            printf("\tindex of json %d\n", curr_msg[1]);

            int count_list_size = curr_msg[0];
            int json_i = curr_msg[1];

//            for (int j = 2; j < 2 + count_list_size; ++j) {
//                printf("\tcount = %d\n", curr_msg[j]);
//            }


            for (int i = 2; i < 2 + count_list_size; ++i) {
                res_json["input_list"][json_i]["test_list"][i - 2] =
                        int(res_json["input_list"][json_i]["test_list"][i - 2]) + curr_msg[i];

            }
            received_num++;
        }
        std::ofstream res_file("res/res_mpi.json");
        auto end_time = std::chrono::high_resolution_clock::now();
        res_json["input_list"][0]["time"] = (end_time - begin_time).count() * 1e-9;
        res_file << res_json;


    } else {
        int json_i = 0;
        for (json curr_json:config_json["input_list"]) {
            std::string input_name = curr_json["name"];
            std::string full_text = read_file(input_name);
            std::list<std::string> word_list = curr_json["word_list"];
            std::list<int> count_test_list = curr_json["test_list"];

            int batch_size = std::ceil((float) full_text.size() / (float) (worker_size - 1));
            int curr_from = batch_size * (rank - 1);
            int curr_to = fmin(batch_size * (rank), full_text.length());

            int msg_size = word_list.size() + 2;//first element - size, second elements - index of json
            int curr_msg[msg_size];
            curr_msg[0] = word_list.size();
            curr_msg[1] = json_i;
            int i = 2;
            for (const std::string &curr_pattern :word_list) {
                printf("Rank = %d start calculating '%s' (%d - %d)\n()", rank, curr_pattern.c_str(), curr_from,
                       curr_to);

                int curr_value = count_pattern_from_to(
                        //args
                        full_text,
                        curr_pattern,
                        curr_from,
                        curr_to
                );

//                printf("Rank = %d, value = %d\n", rank, curr_value);

                curr_msg[i] = curr_value;
                i++;


            }
//            printf("Rank = %d sending message\n", rank);
//            printf("\tvalues size = %d\n", curr_msg[0]);
//            printf("\tindex of json %d\n", curr_msg[1]);
//            for (int j = 2; j < msg_size; ++j) {
//                printf("\tcount = %d\n", curr_msg[j]);
//            }
            MPI_Send(&curr_msg, msg_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}
