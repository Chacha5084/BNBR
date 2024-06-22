//
// Created by groot on 21/06/24.
//
#include <ncurses.h>

int main() {
    initscr();
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        return 1;
    }
    printw("Hello, world!");
    refresh();
    getch();
    endwin();

    return 0;
}