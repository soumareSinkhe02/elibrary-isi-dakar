#include <stdio.h>
#include <string.h>
#include "categories.h"
#include "utils.h"

#define FICHIER_CATEGORIES "DATABASE/CATEGORIES.dat"

static int charger_categories(Categorie tab[]) {
    FILE *f = fopen(FICHIER_CATEGORIES, "rb");
    if (!f) return 0;
    int nb = 0;
    while (fread(&tab[nb], sizeof(Categorie), 1, f) == 1) nb++;
    fclose(f);
    return nb;
}

static void sauvegarder_categories(Categorie tab[], int nb) {
    FILE *f = fopen(FICHIER_CATEGORIES, "wb");
    if (!f) return;
    for (int i = 0; i < nb; i++) fwrite(&tab[i], sizeof(Categorie), 1, f);
    fclose(f);
}

void ajouter_categorie(void) {
    Categorie c;
    c.id = utils_prochainId(FICHIER_CATEGORIES, sizeof(Categorie));
    printf("Libelle : ");
    fgets(c.libelle, NAME_LEN, stdin);
    c.libelle[strcspn(c.libelle, "\n")] = 0;
    printf("Description : ");
    fgets(c.description, DESC_LEN, stdin);
    c.description[strcspn(c.description, "\n")] = 0;
    utils_dateActuelleCourte(c.dateCreation);

    FILE *f = fopen(FICHIER_CATEGORIES, "ab");
    if (f) {
        fwrite(&c, sizeof(Categorie), 1, f);
        fclose(f);
        printf("Categorie ajoutee (ID %d).\n", c.id);
        utils_journaliser("Ajout categorie");
    } else {
        printf("Erreur d'ouverture du fichier.\n");
    }
}

void lister_categories(void) {
    Categorie tab[1000];
    int nb = charger_categories(tab);
    printf("Liste des categories :\n");
    for (int i = 0; i < nb; i++) {
        printf("[%d] %s - %s\n", tab[i].id, tab[i].libelle, tab[i].description);
    }
}

int categorie_existe(int id) {
    Categorie tab[1000];
    int nb = charger_categories(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == id) return 1;
    }
    return 0;
}

void menu_categories(void) {
    int choix;
    do {
        printf("\n--- GESTION DES CATEGORIES ---\n");
        printf("1. Ajouter une categorie\n");
        printf("2. Lister les categories\n");
        printf("0. Retour\n");
        choix = utils_saisirEntier("Votre choix : ");
        switch (choix) {
            case 1: ajouter_categorie(); break;
            case 2: lister_categories(); break;
        }
    } while (choix != 0);
}
