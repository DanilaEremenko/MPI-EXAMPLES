import time
from numba import njit


def my_count_words(
        input_text: str,
        word_list: list
) -> dict:
    word_map = dict(zip(word_list, [0] * len(word_list)))
    for word in word_list:
        for start_i in range(0, len(input_text)):
            if input_text[start_i:start_i + len(word)] == word:
                word_map[word] += 1

    return word_map


if __name__ == '__main__':
    input_list = [
        "input_files/2_1984.txt",
        "input_files/2_The Haunted Man and the Ghost's Bargain.txt"
    ]
    word_list = [
        "капитал",
        "день",
        "she"
    ]
    start_time = time.time()
    for input_name in input_list:
        print(f"----- {input_name} -------")
        with open(input_name) as input_fp:
            input_text = input_fp.read()

        word_map = my_count_words(
            input_text=input_text,
            word_list=word_list
        )
        for word, num in word_map.items():
            print(f"{word} : {num}")

    print(f"{time.time() - start_time}")
