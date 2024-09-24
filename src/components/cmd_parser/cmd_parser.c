// Copyright 2024 PWrInSpace, Kuba
#include "cmd_parser.h"


static int cmd_read_data(int argc, char **argv) {
    char buffer[255] = "RZYGANIE";

    CLI_WRITE_OK("%s", buffer);
    return 0;
}

bool cmd_register_common(void) {
    const esp_console_cmd_t open_cmd[] = {
        {.command = "read_data", .help = "read data", .hint = NULL,
        .func = cmd_read_data},
    };
    size_t number_of_commands = sizeof(open_cmd) / sizeof(open_cmd[0]);

    for (size_t i = 0; i < number_of_commands; ++i) {
        if (esp_console_cmd_register(&open_cmd[i]) != ESP_OK) {
            return false;
        }
    }

    return true;
}
