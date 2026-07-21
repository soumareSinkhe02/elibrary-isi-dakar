#include <stdio.h>
#include <string.h>
#include "borrows.h"
#include "books.h"
#include "users.h"
#include "reports.h"
#include "utils.h"

#define FICHIER_BORROWS "DATABASE/BORROWS.dat"
#define MAX_EMPRUNTS_SIMULTANES 3
#define DUREE_MAX_JOURS 14

static int charger_emprunts(Emprunt tab[]) {
    FILE *f = fopen(FICHIER_BORROWS, "rb");
    if (!f) return 0;
    int nb = 0;
    while (fread(&tab[nb], sizeof(Emprunt), 1, f) == 1) nb++;
    fclose(f);
    return nb;
}

static void sauvegarder_emprunts(Emprunt tab[], int nb) {
    FILE *f = fopen(FICHIER_BORROWS, "wb");
    if (!f) return;
    for (int i = 0; i < nb; i++) fwrite(&tab[i], sizeof(Emprunt), 1, f);
    fclose(f);
}

int borrows_emprunter(int idUtilisateur, int idLivre, Emprunt *resultat) {
    Livre livre;
    Livre livres[1000];
    int nbLivres = 0;
    FILE *f = fopen("DATABASE/BOOKS.dat", "rb");
    if (f) {
        while (fread(&livres[nbLivres], sizeof(Livre), 1, f) == 1) nbLivres++;
        fclose(f);
    }
    int idxLivre = -1;
    for (int i = 0; i < nbLivres; i++) {
        if (livres[i].id == idLivre) { idxLivre = i; break; }
    }
    if (idxLivre == -1) {
        printf("Livre introuvable.\n");
        return 0;
    }
    if (livres[idxLivre].nombreDisponibles <= 0) {
        printf("Aucun exemplaire disponible.\n");
        return 0;
    }

    Emprunt emprunts[1000];
    int nbEmp = charger_emprunts(emprunts);
    int nbEnCours = 0;
    for (int i = 0; i < nbEmp; i++) {
        if (emprunts[i].idUtilisateur == idUtilisateur &&
            (emprunts[i].etat == EMPRUNT_EN_COURS || emprunts[i].etat == EMPRUNT_EN_RETARD)) {
            nbEnCours++;
        }
    }
    if (nbEnCours >= MAX_EMPRUNTS_SIMULTANES) {
        printf("Vous avez deja %d emprunts en cours (max %d).\n", nbEnCours, MAX_EMPRUNTS_SIMULTANES);
        return 0;
    }

    Emprunt e;
    e.id = utils_prochainId(FICHIER_BORROWS, sizeof(Emprunt));
    utils_genererNumeroEmprunt(e.numeroEmprunt);
    e.idUtilisateur = idUtilisateur;
    e.idLivre = idLivre;
    utils_dateActuelleCourte(e.dateEmprunt);
    utils_ajouterJours(e.dateEmprunt, DUREE_MAX_JOURS, e.dateRetourPrevue);
    e.etat = EMPRUNT_EN_COURS;

    f = fopen(FICHIER_BORROWS, "ab");
    if (!f) return 0;
    fwrite(&e, sizeof(Emprunt), 1, f);
    fclose(f);

    books_decrementerDisponible(idLivre);

    reports_genererRecuEmprunt(e);

    char msg[200];
    sprintf(msg, "Emprunt livre ID %d", idLivre);
    utils_journaliser(msg);

    if (resultat) *resultat = e;
    printf("Emprunt enregistre : %s (retour prevu le %s)\n", e.numeroEmprunt, e.dateRetourPrevue);
    return 1;
}

void borrows_actualiserRetards(void) {
    Emprunt tab[1000];
    int nb = charger_emprunts(tab);
    char aujourdhui[11];
    utils_dateActuelleCourte(aujourdhui);
    int modifie = 0;
    for (int i = 0; i < nb; i++) {
        if (tab[i].etat == EMPRUNT_EN_COURS) {
            if (utils_diffJours(tab[i].dateRetourPrevue, aujourdhui) < 0) {
                tab[i].etat = EMPRUNT_EN_RETARD;
                modifie = 1;
            }
        }
    }
    if (modifie) sauvegarder_emprunts(tab, nb);
}

int borrows_listerTous(Emprunt *tableau, int tailleMax) {
    return charger_emprunts(tableau);
}

void menu_emprunts(void) {
    int choix;
    do {
        printf("\n--- GESTION DES EMPRUNTS ---\n");
        printf("1. Nouvel emprunt\n");
        printf("2. Lister tous les emprunts\n");
        printf("0. Retour\n");
        choix = utils_saisirEntier("Votre choix : ");
        switch (choix) {
            case 1: {
                int idU, idL;
                idU = utils_saisirEntier("ID utilisateur : ");
                idL = utils_saisirEntier("ID livre : ");
                Emprunt e;
                borrows_emprunter(idU, idL, &e);
                break;
            }
            case 2: {
                Emprunt tab[1000];
                int n = borrows_listerTous(tab, 1000);
                const char *etats[] = {"EN_COURS", "RETOURNE", "EN_RETARD"};
                for (int i = 0; i < n; i++) {
                    printf("[%d] %s - User %d, Livre %d, Retour prevu %s (%s)\n",
                           tab[i].id, tab[i].numeroEmprunt, tab[i].idUtilisateur,
                           tab[i].idLivre, tab[i].dateRetourPrevue, etats[tab[i].etat]);
                }
                break;
            }
        }
    } while (choix != 0);
}
