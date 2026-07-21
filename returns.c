#include <stdio.h>
#include <string.h>
#include "returns.h"
#include "borrows.h"
#include "books.h"
#include "reservations.h"
#include "reports.h"
#include "utils.h"

#define FICHIER_RETURNS "DATABASE/RETURNS.dat"
#define MONTANT_PAR_JOUR 500.0

int returns_retourner(int idEmprunt, Retour *resultat) {
    Emprunt emprunts[1000];
    int nbEmp = 0;
    FILE *f = fopen("DATABASE/BORROWS.dat", "rb");
    if (f) {
        while (fread(&emprunts[nbEmp], sizeof(Emprunt), 1, f) == 1) nbEmp++;
        fclose(f);
    }
    int idxEmp = -1;
    for (int i = 0; i < nbEmp; i++) {
        if (emprunts[i].id == idEmprunt) { idxEmp = i; break; }
    }
    if (idxEmp == -1) {
        printf("Emprunt introuvable.\n");
        return 0;
    }
    if (emprunts[idxEmp].etat == EMPRUNT_RETOURNE) {
        printf("Cet emprunt est deja retourne.\n");
        return 0;
    }

    Retour r;
    r.id = utils_prochainId(FICHIER_RETURNS, sizeof(Retour));
    r.idEmprunt = idEmprunt;
    utils_dateActuelleCourte(r.dateRetourReelle);

    r.joursRetard = utils_diffJours(emprunts[idxEmp].dateRetourPrevue, r.dateRetourReelle);
    if (r.joursRetard < 0) r.joursRetard = 0;
    r.montantPenalite = r.joursRetard * MONTANT_PAR_JOUR;

    f = fopen(FICHIER_RETURNS, "ab");
    if (f) {
        fwrite(&r, sizeof(Retour), 1, f);
        fclose(f);
    }

    emprunts[idxEmp].etat = EMPRUNT_RETOURNE;
    f = fopen("DATABASE/BORROWS.dat", "wb");
    for (int i = 0; i < nbEmp; i++) fwrite(&emprunts[i], sizeof(Emprunt), 1, f);
    fclose(f);

    books_incrementerDisponible(emprunts[idxEmp].idLivre);

    reports_genererRecuRetour(r, emprunts[idxEmp]);

    reservations_verifierApresRetour(emprunts[idxEmp].idLivre);

    char msg[200];
    sprintf(msg, "Retour emprunt %d", idEmprunt);
    utils_journaliser(msg);

    if (resultat) *resultat = r;
    printf("Retour enregistre. Retard : %d jour(s), Penalite : %.0f FCFA\n",
           r.joursRetard, r.montantPenalite);
    return 1;
}

void menu_retours(void) {
    int choix;
    do {
        printf("\n--- GESTION DES RETOURS ---\n");
        printf("1. Enregistrer un retour\n");
        printf("2. Lister les retours\n");
        printf("0. Retour\n");
        choix = utils_saisirEntier("Votre choix : ");
        if (choix == 1) {
            int idE;
            idE = utils_saisirEntier("ID de l'emprunt : ");
            Retour r;
            returns_retourner(idE, &r);
        } else if (choix == 2) {
            Retour tab[1000];
            FILE *f = fopen(FICHIER_RETURNS, "rb");
            int nb = 0;
            if (f) {
                while (fread(&tab[nb], sizeof(Retour), 1, f) == 1) nb++;
                fclose(f);
            }
            for (int i = 0; i < nb; i++) {
                printf("[%d] Emprunt %d - retour le %s - retard %d j, %.0f FCFA\n",
                       tab[i].id, tab[i].idEmprunt, tab[i].dateRetourReelle,
                       tab[i].joursRetard, tab[i].montantPenalite);
            }
        }
    } while (choix != 0);
}
