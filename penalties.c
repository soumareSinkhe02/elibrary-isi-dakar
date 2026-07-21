#include <stdio.h>
#include "penalties.h"
#include "structures.h"

#define FICHIER_PENALTIES "DATABASE/PENALTIES.dat"

void menu_penalites(void) {
    printf("\n--- PENALITES ---\n");
    FILE *f = fopen(FICHIER_PENALTIES, "rb");
    if (!f) {
        printf("Aucune penalite enregistree.\n");
        return;
    }
    Penalite p;
    while (fread(&p, sizeof(Penalite), 1, f) == 1) {
        printf("[%d] User %d, Emprunt %d, Retard %d j, Montant %.0f FCFA, Date %s\n",
               p.id, p.idUtilisateur, p.idEmprunt, p.joursRetard, p.montant, p.date);
    }
    fclose(f);
}
