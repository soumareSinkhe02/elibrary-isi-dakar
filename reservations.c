#include <stdio.h>
#include <string.h>
#include "reservations.h"
#include "books.h"
#include "utils.h"

#define FICHIER_RESERVATIONS "DATABASE/RESERVATIONS.dat"

static int charger_reservations(Reservation tab[]) {
    FILE *f = fopen(FICHIER_RESERVATIONS, "rb");
    if (!f) return 0;
    int nb = 0;
    while (fread(&tab[nb], sizeof(Reservation), 1, f) == 1) nb++;
    fclose(f);
    return nb;
}

static void sauvegarder_reservations(Reservation tab[], int nb) {
    FILE *f = fopen(FICHIER_RESERVATIONS, "wb");
    if (!f) return;
    for (int i = 0; i < nb; i++) fwrite(&tab[i], sizeof(Reservation), 1, f);
    fclose(f);
}

int reservations_reserver(int idUtilisateur, int idLivre, Reservation *resultat) {
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
    if (livres[idxLivre].nombreDisponibles > 0) {
        printf("Ce livre a encore des exemplaires disponibles. Empruntez-le directement.\n");
        return 0;
    }

    Reservation r;
    r.id = utils_prochainId(FICHIER_RESERVATIONS, sizeof(Reservation));
    r.idUtilisateur = idUtilisateur;
    r.idLivre = idLivre;
    utils_dateActuelleCourte(r.dateReservation);
    r.etat = RES_EN_ATTENTE;

    f = fopen(FICHIER_RESERVATIONS, "ab");
    if (f) {
        fwrite(&r, sizeof(Reservation), 1, f);
        fclose(f);
        printf("Reservation enregistree (ID %d).\n", r.id);
        utils_journaliser("Reservation livre");
        if (resultat) *resultat = r;
        return 1;
    }
    return 0;
}

void reservations_verifierApresRetour(int idLivre) {
    Reservation tab[1000];
    int nb = charger_reservations(tab);
    int idxPlusAncienne = -1;
    for (int i = 0; i < nb; i++) {
        if (tab[i].idLivre == idLivre && tab[i].etat == RES_EN_ATTENTE) {
            if (idxPlusAncienne == -1 ||
                utils_diffJours(tab[i].dateReservation, tab[idxPlusAncienne].dateReservation) < 0) {
                idxPlusAncienne = i;
            }
        }
    }
    if (idxPlusAncienne != -1) {
        tab[idxPlusAncienne].etat = RES_DISPONIBLE;
        sauvegarder_reservations(tab, nb);
        printf("La reservation %d pour le livre %d est maintenant DISPONIBLE.\n",
               tab[idxPlusAncienne].id, idLivre);

        char msg[200];
        sprintf(msg, "Reservation %d passee a DISPONIBLE", tab[idxPlusAncienne].id);
        utils_journaliser(msg);
    }
}

static void annuler_reservation(void) {
    int id;
    id = utils_saisirEntier("ID de la reservation a annuler : ");
    Reservation tab[1000];
    int nb = charger_reservations(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == id && tab[i].etat != RES_ANNULEE) {
            tab[i].etat = RES_ANNULEE;
            sauvegarder_reservations(tab, nb);
            printf("Reservation annulee.\n");
            utils_journaliser("Annulation reservation");
            return;
        }
    }
    printf("Reservation introuvable ou deja annulee.\n");
}

void menu_reservations(void) {
    int choix;
    do {
        printf("\n--- GESTION DES RESERVATIONS ---\n");
        printf("1. Reserver un livre\n");
        printf("2. Lister les reservations\n");
        printf("3. Annuler une reservation\n");
        printf("0. Retour\n");
        choix = utils_saisirEntier("Votre choix : ");
        switch (choix) {
            case 1: {
                int idU, idL;
                idU = utils_saisirEntier("ID utilisateur : ");
                idL = utils_saisirEntier("ID livre : ");
                reservations_reserver(idU, idL, NULL);
                break;
            }
            case 2: {
                Reservation tab[1000];
                int nb = charger_reservations(tab);
                const char *etats[] = {"EN_ATTENTE", "DISPONIBLE", "ANNULEE"};
                for (int i = 0; i < nb; i++) {
                    printf("[%d] User %d, Livre %d, %s (%s)\n",
                           tab[i].id, tab[i].idUtilisateur, tab[i].idLivre,
                           tab[i].dateReservation, etats[tab[i].etat]);
                }
                break;
            }
            case 3: annuler_reservation(); break;
        }
    } while (choix != 0);
}
