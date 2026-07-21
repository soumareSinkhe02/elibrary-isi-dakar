#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include "structures.h"

void menu_reservations(void);
int  reservations_reserver(int idUtilisateur, int idLivre, Reservation *resultat);
void reservations_verifierApresRetour(int idLivre);

#endif
