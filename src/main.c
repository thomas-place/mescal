#include "main.h"

// TODO Clean thes (used to pass values to the 'test' command).
uint slice_test = 1, width_test = 5000000;

int main(int argc, char *argv[]) {
    // avoid warning unused argument.
    assert(argc > 0);

    if (argc >= 2)
        slice_test = atoi(argv[1]);
    if (argc >= 3)
        width_test = atoi(argv[2]);

    // By default, interruptions and timeouts are off.
    ignore_interrupt();

    // Create output directory.
    system("mkdir -p " OUTPUT_DIR);

    // Initialize keywords and history.
    keywords_add_all_keys();
    init_class_info();
    init_membership();
    init_separation();
    init_objects_array();

    initialize_readline(argv[0]);

    print_title_box(5, true, stdout, 1, "Hola amigo de los autómatas. ¡Bienvenido! Ahora estás en el programa MeSCaL");

    // Main loop.
    shell_parse();

    write_history(histfile);

    free(histfile);

    return EXIT_SUCCESS;
}
