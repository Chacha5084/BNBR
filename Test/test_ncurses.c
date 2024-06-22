#include "test_ncurses.h"

int main() {
    Cell matrix[MATRIX_SIZE][MATRIX_SIZE];
    MenuChoixClient(matrix);
    return 0;
}


void init_ncurses(){
    initscr();
    keypad(stdscr, TRUE); // Activer la prise en charge des touches spéciales
    start_color(); // Activer les couleurs
    curs_set(0); // Masquer le curseur
    noecho(); // Désactiver l'affichage des caractères saisis
    cbreak(); // Désactiver la mise en mémoire tampon de ligne


    // Définition des couleurs
    init_pair(1, COLOR_BLACK, COLOR_RED); // Texte rouge sur fond noir
    init_pair(2, COLOR_WHITE, COLOR_GREEN); // Texte rouge sur fond noir
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Texte rouge sur fond noir
    init_pair(4, COLOR_CYAN, COLOR_BLACK); // Texte rouge sur fond noir
}

void AfficherMenu(int *p_cursor_x, int *p_cursor_y, Cell matrix[MATRIX_SIZE][MATRIX_SIZE]) { 
    
    clear(); // Effacer l'écran

    // Afficher la matrice
    for (int y = 0; y < MATRIX_SIZE; y++) {
        for (int x = 0; x < MATRIX_SIZE; x++) {

            if (x == *p_cursor_x && y == *p_cursor_y) {
                
                attron(A_REVERSE);
                printw("[ ]");
                attroff(A_REVERSE);
                printw("");

            } else {

                printw("[ ]");

            }
        }
        printw("\n");
    }
    printw("\n----------------------------------------------\n\n");
    refresh(); // Rafraîchir l'écran
}

void MenuChoixClient(Cell matrix[MATRIX_SIZE][MATRIX_SIZE]){

    int cursor_x = 0;
    int cursor_y = 0;

    // DISPLAY NCURSES  **************************************************************************************
    init_ncurses(); // Initialiser ncurses
    AfficherMenu(&cursor_x, &cursor_y, matrix);

    // CHOIX DU CLIENT **************************************************************************************
    int ch;
    do {
        ch = getch(); // Lire une touche du clavier
        // Déplacer le curseur en fonction de la touche pressée, mais uniquement si la case est disponible
        switch (ch) {
            // Case fleche 
                case KEY_UP:
                        if (cursor_y > 0){
                            cursor_y--;
                            //display_matrix(cursor_x, cursor_y, matrix);
                        }
                    break;
                case KEY_DOWN:
                        if (cursor_y < MATRIX_SIZE - 1){
                            cursor_y++;
                            //display_matrix(cursor_x, cursor_y, matrix);   
                        }
                    break;
                case KEY_LEFT:
                        if (cursor_x > 0) {
                            cursor_x--;
                            //display_matrix(cursor_x, cursor_y, matrix);
                        }
                    break;
                case KEY_RIGHT:
                        if (cursor_x < MATRIX_SIZE - 1) {
                            cursor_x++;
                            //display_matrix(cursor_x, cursor_y, matrix);
                        }
                    break;

            // Case touche entrée
            case 10:
                   
                break;

            // Case touche espace    
            case ' ':

                break;

            // Case touche R = reload
            case 'r':                 
                break;    

            // Case touche q    
            case 'q':                    
                break;
        }

        AfficherMenu(&cursor_x, &cursor_y, matrix);

    } while (ch != 'q'); // Sortir du programme si la touche 'q' est pressée

    endwin();
}