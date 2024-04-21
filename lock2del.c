#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <linux/input.h>

// clang-format off
const struct input_event
syn       = {.type = EV_SYN , .code = SYN_REPORT   , .value = 0},
del_up = {.type = EV_KEY , .code = KEY_DELETE      , .value = 0},
del_down = {.type = EV_KEY , .code = KEY_DELETE      , .value = 1},
del_pressed = {.type = EV_KEY , .code = KEY_DELETE , .value = 2};
// clang-format on

void print_usage(FILE *stream, const char *program) {
    // clang-format off
    fprintf(stream,
            "lock2del\n"
            "\n"
            "usage: %s [-h | [-m mode] [-t delay]]\n"
            "\n"
            "options:\n"
            "    -h         show this message and exit\n",
            program);
    // clang-format on
}

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    for (int opt; (opt = getopt(argc, argv, "h")) != -1;) {
        switch (opt) {
            case 'h':
                return print_usage(stdout, argv[0]), EXIT_SUCCESS;
        }

        return print_usage(stderr, argv[0]), EXIT_FAILURE;
    }

    struct input_event input;
    enum { START, SCREENLOCK_HELD } state = START;

    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        if (input.type == EV_MSC && input.code == MSC_SCAN)
            continue;

        if (input.type != EV_KEY && input.type != EV_REL &&
            input.type != EV_ABS) {
            write_event(&input);
            continue;
        }

        switch (state) {
            case START:
                if (input.type == EV_KEY && input.code == KEY_SCREENLOCK &&
                    input.value == 1) {
                        state = SCREENLOCK_HELD;
                        write_event(&del_down);
                    }
                else
                    write_event(&input);
                break;
            case SCREENLOCK_HELD:
                if (input.type == EV_KEY && input.code == KEY_SCREENLOCK) {
                    if (input.value == 0) {
                        // Button released
                        write_event(&del_up);
                        write_event(&syn);
                        state = START;
                    } else if (input.value == 2) {
                        write_event(&del_pressed);
                        write_event(&syn);
                    }
                } else
                    write_event(&input);
                break;
        }
    }
}
