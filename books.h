#ifndef BOOKS_H
#define BOOKS_H

#include "structures.h"

void menu_livres(void);

int  books_aucunLivrePourAuteur(int idAuteur);
int  books_aucunLivrePourCategorie(int idCategorie);
int  books_decrementerDisponible(int idLivre);
int  books_incrementerDisponible(int idLivre);

#endif
