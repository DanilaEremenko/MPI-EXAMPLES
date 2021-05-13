/***********************************************************************************************************************
------------------------------------------------------- MPI REALIZATION ----------------------------------------------
***********************************************************************************************************************/
#include <mpi.h>
#include <list>
#include "methods/my_text_processor.h"

int main() {

    json config_json = json::parse(read_file("config.json"));

    int max_message_size = 0;
    if (config_json["word_list"].size() > max_message_size)
        max_message_size = config_json["word_list"].size();

    verbose_print("Max message size = %d\n", max_message_size);

    std::list<int> full_result;
    int rank, worker_size;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &worker_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        json res_json = config_json;
        for (int j = 0; j < res_json["test_list"].size(); ++j) {
            res_json["test_list"][j] = 0;
        }

        std::string full_text = read_file(config_json["name"]);

        for (const std::string &curr_pattern :config_json["word_list"]) {
            std::vector<std::pair<int, int>> batches_bounds = get_smart_batch_bounds(
                    worker_size - 1,
                    curr_pattern,
                    full_text
            );
            int worker_id = 1;
            for (std::pair<int, int> curr_bounds:batches_bounds) {
                int curr_batch_info[2] = {curr_bounds.first, curr_bounds.second};
                MPI_Send(&curr_batch_info, 2, MPI_INT, worker_id, 0, MPI_COMM_WORLD);
                worker_id++;
            }
        }

        int received_num = 0;
        int max_message_n = worker_size - 1;
        while (received_num < max_message_n) {
            int curr_msg[max_message_size];
            verbose_print("Root awaiting message(received_num = %d/%d)\n", received_num, max_message_n);
            MPI_Recv(&curr_msg, max_message_size + 5, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, nullptr);
            verbose_print("Root receive message\n");
            verbose_print("\tvalues size = %d\n", curr_msg[0]);

            int count_list_size = curr_msg[0];

            for (int j = 1; j < 1 + count_list_size; ++j) {
                verbose_print("\tcount = %d\n", curr_msg[j]);
            }


            for (int i = 1; i < 1 + count_list_size; ++i) {
                res_json["test_list"][i - 1] = int(res_json["test_list"][i - 1]) + curr_msg[i];

            }
            received_num++;
        }
        std::ofstream res_file("res/res_mpi.json");
        auto end_time = std::chrono::high_resolution_clock::now();
        res_file << res_json;


    } else {
        std::string input_name = config_json["name"];
        std::string full_text = read_file(input_name);
        std::list<std::string> word_list = config_json["word_list"];
        std::list<int> count_test_list = config_json["test_list"];
        int curr_task_info[2];
        MPI_Recv(&curr_task_info, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, nullptr);
        int my_from = curr_task_info[0];
        int my_to = curr_task_info[1];

        int msg_size = word_list.size() + 1;//first element - size, second elements - index of json
        int curr_msg[msg_size];
        curr_msg[0] = word_list.size();
        int i = 1;
        for (const std::string &curr_pattern :word_list) {
            verbose_print("Rank = %d start calculating '%s' (%d - %d)\n()", rank, curr_pattern.c_str(), my_from,
                          my_to);
            int curr_value = count_pattern_from_to(
                    //args
                    full_text,
                    curr_pattern,
                    my_from,
                    my_to
            );

            verbose_print("Rank = %d, value = %d\n", rank, curr_value);

            curr_msg[i] = curr_value;
            i++;

        }
        verbose_print("Rank = %d sending message\n", rank);
        verbose_print("\tvalues size = %d\n", curr_msg[0]);
        verbose_print("\tindex of json %d\n", curr_msg[1]);
        for (int j = 1; j < msg_size; ++j) {
            verbose_print("\tcount = %d\n", curr_msg[j]);
        }
        MPI_Send(&curr_msg, msg_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
