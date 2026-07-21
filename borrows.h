#ifndef BORROWS_H
#define BORROWS_H

#include "structures.h"

void menu_emprunts(void);

int  borrows_emprunter(int idUtilisateur, int idLivre, Emprunt *resultat);

void borrows_actualiserRetards(void);

int  borrows_listerTous(Emprunt *tableau, int tailleMax);

#endif
