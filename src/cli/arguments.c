#include <getopt.h>
#include <string.h>

#include "arguments.h"

void free_program_args(program_args_t *a) {
    free_string_node_t(a->install);
    free_string_node_t(a->uninstall);
    free(a->trace_path);
    free(a);
}

void print_help() {
    printf("Usage: lick-cli [options]...\n");
    printf("\n");
    printf("Volume:\n");
    printf("      --verbose          Be verbose (default)\n");
    printf("  -m, --no-menu          Ask questions, but do not show a menu\n");
    printf("      --no-questions     Do not ask questions; assume yes\n");
    printf("  -s, --silent           No output; assume yes\n");
    printf("\n");
    printf("Install/uninstall:\n");
    printf("  -c, --check-loader     Check if boot loader is installed\n");
    printf("      --install-loader   Install boot loader\n");
    printf("      --uninstall-loader   Uninstall boot loader\n");
    printf("  -i, --install          Install the given ISO or drive\n");
    printf("  -u, --uninstall        Uninstall the given ID\n");
    printf("      --uninstall-all    Uninstall all IDs and the bootloader\n");
    printf("\n");
    printf("      --check-program    Check if the binary can run\n");
    printf("      --ignore-errors    If errors occur, continue\n");
    printf("      --trace <file>     Trace debug info to the given file\n");
    printf("      --no-try-uac       Do not ask to elevate process\n");
    printf("      --no-me-check      Ignore possible ME incompatibilities\n");
    printf("  -v  --version          Show the version\n");
    printf("  -h, --help             Show this help\n");
}

program_args_t *handle_args(program_status_t *p, int argc, char **argv) {
    program_args_t *a = malloc(sizeof(program_args_t));
    a->check_program = 0;
    a->try_uac = 1;
    a->ignore_errors = 0;
    a->trace_path = NULL;
    a->me_check = 1;
    a->check_loader = 0;
    a->install_loader = -1;
    a->fix_loader = 0;
    a->check_fix_loader = 0;
    a->install = NULL;
    a->uninstall = NULL;
    a->uninstall_all = 0;
    a->reinstall = 0;

    struct option ops[] = {
        // meta
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"check-program", no_argument, &a->check_program, 1},
        {"trace", required_argument, 0, 't'},
        {"no-try-uac", no_argument, &a->try_uac, 0},
        {"ignore-errors", no_argument, &a->ignore_errors, 1},
        {"no-me-check", no_argument, &a->me_check, 0},
        // volume
        {"verbose", no_argument, 0, 'V'},
        {"no-menu", no_argument, 0, 'm'},
        {"no-questions", no_argument, 0, 'Q'},
        {"silent", no_argument, 0, 's'},
        // install/uninstall
        {"check-loader", no_argument, 0, 'c'},
        {"install-loader", no_argument, &a->install_loader, 1},
        {"uninstall-loader", no_argument, &a->install_loader, 0},
        {"fix-loader", no_argument, &a->fix_loader, 1},
        {"check-fix-loader", no_argument, &a->check_fix_loader, 1},
        {"install", required_argument, 0, 'i'},
        {"uninstall", required_argument, 0, 'u'},
        {"uninstall-all", no_argument, &a->uninstall_all, 1},
        {"reinstall", no_argument, &a->reinstall, 1},
        {0, 0, 0, 0}
    };
    int c;
    while((c = getopt_long(argc, argv, "chi:msu:v", ops, NULL)) != -1) {
        switch(c) {
        case 0: // set flag
            break;
        case 'm':
            p->volume = VOLUME_NO_MENU;
            break;
        case 'Q':
            p->volume = VOLUME_NO_QUESTIONS;
            break;
        case 's':
            p->volume = VOLUME_SILENCE;
            break;
        case 'i':
            if(strcmp(optarg, "--") == 0) {
                for(int i = optind; i < argc; ++i)
                    a->install = new_string_node_t(strdup2(argv[i]), a->install);
                optind = argc;
            } else
                a->install = new_string_node_t(strdup2(optarg), a->install);
            break;
        case 'u':
            if(strcmp(optarg, "--") == 0) {
                for(int i = optind; i < argc; ++i)
                    a->uninstall = new_string_node_t(strdup2(argv[i]), a->uninstall);
                optind = argc;
            } else
                a->uninstall = new_string_node_t(strdup2(optarg), a->uninstall);
            break;
        case 'c':
            a->check_loader = 1;
            break;
        case 't':
            a->trace_path = strdup2(argv[i]);
            break;
        case 'v':
            printf("%s\n", LIBLICK_VERSION);
            free_program_status(p);
            free_program_args(a);
            exit(0);
        case 'h':
        case '?':
        default:
            print_help();
            free_program_status(p);
            free_program_args(a);
            if(c == 'h')
                exit(0);
            else
                exit(1);
        }
    }
    a->install = string_node_t_reverse(a->install);
    a->uninstall = string_node_t_reverse(a->uninstall);
    return a;
}
