#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define MATRIX_SIZE 8

typedef enum {
    DISPONIBLE,
    INDISPONIBLE,
    RESERVEE
} Status;

/**
 * @name Cell
 * @brief  Structure pour représenter une case de la matrice
 * @param status L'état de la place.
 */
typedef struct {
    Status status;
} Cell;


void init_ncurses();
void AfficherMenu(int *p_cursor_x, int *p_cursor_y, Cell matrix[MATRIX_SIZE][MATRIX_SIZE]);
void MenuChoixClient(Cell matrix[MATRIX_SIZE][MATRIX_SIZE]);