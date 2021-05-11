/***********************************************************************************************************************
------------------------------------------------------- MPI REALIZATION ----------------------------------------------
***********************************************************************************************************************/
#include <mpi.h>
#include <list>
#include <tgmath.h>
#include "methods/my_text_processor.h"

int main() {
    std::string full_text = read_file("input_files/2_1984.txt");
    std::list<std::string> word_list{"капитал", "день"};


    std::list<int> full_result;
    int rank, worker_size;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &worker_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("My rank = %d\n", rank);


    if (rank == 0) {
        std::vector<int> full_vector;
        full_vector.reserve(word_list.size());
        for (int i = 0; i < word_list.size(); ++i) {
            full_vector.push_back(0);
        }

        int received_num = 0;
        while (received_num < worker_size - 1) {
            int curr_count_list[word_list.size()];
            printf("worker_size = %d, received_num = %d\n", worker_size, received_num);
            printf("Root awaiting message\n");
            MPI_Recv(&curr_count_list, word_list.size(), MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, nullptr);
            printf("Root receive message\n");
            int i = 0;
            for (int curr_num:curr_count_list) {
                full_vector[i] += curr_num;
                i++;
            }
            received_num++;
        }
        for (int value :full_vector) {
            full_result.push_back(value);
            std::cout << "value = " << value << "\n";
        }


    } else {
        printf("rank = %d start calculating message\n", rank);

        int count_list[word_list.size()];
        int i = 0;
        for (const std::string &curr_pattern :word_list) {
            int batch_size = fmax(
                    std::ceil((float) full_text.size() / (float) (worker_size - 1)),
                    100
            );

//            func
            int curr_value = count_pattern_from_to(
                    //args
                    full_text,
                    curr_pattern,
                    batch_size * (rank - 1),
                    fmin(batch_size * (rank), full_text.length())
            );


            count_list[i] = curr_value;
            i++;


        }
        printf("rank = %d sending message\n", rank);
        MPI_Send(&count_list, word_list.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
