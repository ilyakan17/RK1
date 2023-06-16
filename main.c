#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#define MAX_TEXT_SIZE 1000
#define MAX_INPUT_SIZE 100
#define MAX_FILENAME_SIZE 50

typedef struct {
    int correct_chars;
    int incorrect_chars;
    int total_chars;
    int total_words;
    int elapsed_time;
} GameStatistics;

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J");
    printf("\033[H");
    fflush(stdout);
#endif
}

int load_text(const char* filename, char* text) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size > MAX_TEXT_SIZE - 1) {
        printf("Text file is too large.\n");
        fclose(file);
        return 0;
    }

    size_t bytes_read = fread(text, 1, file_size, file);
    text[bytes_read] = '\0';

    fclose(file);

    return bytes_read;
}

void print_statistics(GameStatistics stats) {
    printf("\nStatistics:\n");
    printf("Correct characters: %d\n", stats.correct_chars);
    printf("Incorrect characters: %d\n", stats.incorrect_chars);
    printf("Total characters: %d\n", stats.total_chars);
    printf("Total words: %d\n", stats.total_words);
    printf("Average typing speed (characters per minute): %.2f\n", (float)stats.correct_chars / stats.elapsed_time * 60);
    printf("Average typing speed (words per minute): %.2f\n", (float)stats.total_words / stats.elapsed_time * 60);
}

#ifdef _WIN32
char get_input_character() {
    return _getch();
}

int get_elapsed_time() {
    return clock() / CLOCKS_PER_SEC;
}
#else
char get_input_character() {
    struct termios old, new;
    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
    int input_char = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return input_char;
}

int get_elapsed_time() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec;
}
#endif

void run_typing_test(const char* text, int time_limit, int max_errors) {
    int text_length = strlen(text);
    int text_index = 0;
    int num_errors = 0;
    int num_characters_typed = 0;
    int num_words_typed = 0;
    double start_time = get_elapsed_time();
    double current_time = start_time;
    double time_elapsed = 0.0;

    printf("%s\n", text);

    while (time_elapsed < time_limit && text_index < text_length) {
        char input = get_input_character();
        current_time = get_elapsed_time();
        time_elapsed = current_time - start_time;

        if (input == '\n') {
            printf("\nText typing test completed.\n");
            break;
        }

        if (input == text[text_index]) {
            printf("\033[32m%c\033[0m", input); // Подсветка правильного символа зеленым
            text_index++;
            num_characters_typed++;
        } else {
            printf("\033[31m%c\033[0m", text[text_index]); // Подсветка неправильного символа красным
            num_errors++;
            num_characters_typed++;
        }

        if (input == ' ') {
            num_words_typed++;
        }

        if (num_errors > max_errors) {
            printf("\nMaximum number of errors exceeded. Game over.\n");
            break;
        }
    }

    printf("\n\n----- Typing Test Statistics -----\n");
    printf("Characters typed correctly: %d\n", num_characters_typed - num_errors);
    printf("Characters typed incorrectly: %d\n", num_errors);
    printf("Total characters typed: %d\n", num_characters_typed);
    printf("Average typing speed (characters per minute): %.2f\n", (num_characters_typed / time_elapsed) * 60);
    printf("Average typing speed (words per minute): %.2f\n", (num_words_typed / time_elapsed) * 60);
}



void print_help() {
    printf("Usage: typing_test <text_file> <time_limit> <max_errors>\n");
    printf("  - text_file: Path to the text file containing the text to type\n");
    printf("  - time_limit: Time limit for the typing test (in seconds)\n");
    printf("  - max_errors: Maximum number of allowed errors\n");
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Invalid arguments.\n");
        print_help();
        return 1;
    }

    char* text_file = argv[1];
    int time_limit = atoi(argv[2]);
    int max_errors = atoi(argv[3]);

    char text[MAX_TEXT_SIZE];
    if (!load_text(text_file, text)) {
        return 1;
    }

    printf("Typing Test\n\n");
    printf("Type the following text:\n\n%s\n\n", text);
    printf("Press Enter to start...\n");
    getchar();

    clear_screen();
    run_typing_test(text, time_limit, max_errors);

    printf("\n\nTest completed.\n");

    return 0;
}
