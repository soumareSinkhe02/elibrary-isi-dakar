#include <stdio.h>
#include <string.h>
#include "books.h"
#include "authors.h"
#include "categories.h"
#include "utils.h"

#define FICHIER_BOOKS "DATABASE/BOOKS.dat"

static int charger_livres(Livre tab[]) {
    FILE *f = fopen(FICHIER_BOOKS, "rb");
    if (!f) return 0;
    int nb = 0;
    while (fread(&tab[nb], sizeof(Livre), 1, f) == 1) nb++;
    fclose(f);
    return nb;
}

static void sauvegarder_livres(Livre tab[], int nb) {
    FILE *f = fopen(FICHIER_BOOKS, "wb");
    if (!f) return;
    for (int i = 0; i < nb; i++) fwrite(&tab[i], sizeof(Livre), 1, f);
    fclose(f);
}

static void ajouter_livre(void) {
    Livre l;
    printf("ISBN : ");
    fgets(l.isbn, ISBN_LEN, stdin);
    l.isbn[strcspn(l.isbn, "\n")] = 0;

    Livre existant[1000];
    int nb = charger_livres(existant);
    for (int i = 0; i < nb; i++) {
        if (strcmp(existant[i].isbn, l.isbn) == 0) {
            printf("ISBN deja utilise.\n");
            return;
        }
    }

    printf("Titre : ");
    fgets(l.titre, TITLE_LEN, stdin);
    l.titre[strcspn(l.titre, "\n")] = 0;

    l.idAuteur = utils_saisirEntier("ID auteur : ");
    if (!auteur_existe(l.idAuteur)) {
        printf("Auteur introuvable.\n");
        return;
    }

    l.idCategorie = utils_saisirEntier("ID categorie : ");
    if (!categorie_existe(l.idCategorie)) {
        printf("Categorie introuvable.\n");
        return;
    }

    printf("Editeur : ");
    fgets(l.editeur, NAME_LEN, stdin);
    l.editeur[strcspn(l.editeur, "\n")] = 0;
    l.anneePublication = utils_saisirEntier("Annee de publication : ");
    printf("Langue : ");
    fgets(l.langue, LANGUE_LEN, stdin);
    l.langue[strcspn(l.langue, "\n")] = 0;
    l.nombrePages = utils_saisirEntier("Nombre de pages : ");
    l.nombreExemplaires = utils_saisirEntier("Nombre d'exemplaires : ");
    printf("Emplacement : ");
    fgets(l.emplacement, EMPLACEMENT_LEN, stdin);
    l.emplacement[strcspn(l.emplacement, "\n")] = 0;
    printf("Resume : ");
    fgets(l.resume, DESC_LEN, stdin);
    l.resume[strcspn(l.resume, "\n")] = 0;

    l.id = utils_prochainId(FICHIER_BOOKS, sizeof(Livre));
    l.nombreDisponibles = l.nombreExemplaires;
    utils_dateActuelleCourte(l.dateAjout);

    FILE *f = fopen(FICHIER_BOOKS, "ab");
    if (f) {
        fwrite(&l, sizeof(Livre), 1, f);
        fclose(f);
        printf("Livre ajoute (ID %d).\n", l.id);
        utils_journaliser("Ajout livre");
    }
}

static void lister_livres(void) {
    Livre tab[1000];
    int nb = charger_livres(tab);
    printf("Liste des livres :\n");
    for (int i = 0; i < nb; i++) {
        printf("[%d] %s (ISBN %s) - %d/%d disponibles\n",
               tab[i].id, tab[i].titre, tab[i].isbn,
               tab[i].nombreDisponibles, tab[i].nombreExemplaires);
    }
}

static void rechercher_par_titre(void) {
    char mot[50];
    printf("Mot-cle du titre : ");
    fgets(mot, sizeof(mot), stdin);
    mot[strcspn(mot, "\n")] = 0;

    Livre tab[1000];
    int nb = charger_livres(tab);
    printf("Resultats :\n");
    for (int i = 0; i < nb; i++) {
        if (strstr(tab[i].titre, mot) != NULL) {
            printf("[%d] %s - %d disponibles\n", tab[i].id, tab[i].titre,
                   tab[i].nombreDisponibles);
        }
    }
}

int books_aucunLivrePourAuteur(int idAuteur) {
    Livre tab[1000];
    int nb = charger_livres(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].idAuteur == idAuteur) return 0;
    }
    return 1;
}

int books_aucunLivrePourCategorie(int idCategorie) {
    Livre tab[1000];
    int nb = charger_livres(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].idCategorie == idCategorie) return 0;
    }
    return 1;
}

int books_decrementerDisponible(int idLivre) {
    Livre tab[1000];
    int nb = charger_livres(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == idLivre) {
            if (tab[i].nombreDisponibles <= 0) return 0;
            tab[i].nombreDisponibles--;
            sauvegarder_livres(tab, nb);
            return 1;
        }
    }
    return 0;
}

int books_incrementerDisponible(int idLivre) {
    Livre tab[1000];
    int nb = charger_livres(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == idLivre) {
            tab[i].nombreDisponibles++;
            sauvegarder_livres(tab, nb);
            return 1;
        }
    }
    return 0;
}

void menu_livres(void) {
    int choix;
    do {
        printf("\n--- GESTION DES LIVRES ---\n");
        printf("1. Ajouter un livre\n");
        printf("2. Lister les livres\n");
        printf("3. Rechercher par titre\n");
        printf("0. Retour\n");
        choix = utils_saisirEntier("Votre choix : ");
        switch (choix) {
            case 1: ajouter_livre(); break;
            case 2: lister_livres(); break;
            case 3: rechercher_par_titre(); break;
        }
    } while (choix != 0);
}
