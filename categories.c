#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "categories.h"
#include "utils.h"

int categories_ajouter(const char *libelle, const char *description, Categorie *resultat) {
    Categorie c = {0};
    c.id = utils_prochainId(FICHIER_CATEGORIES, sizeof(Categorie));
    strncpy(c.libelle, libelle, NAME_LEN - 1);
    strncpy(c.description, description, DESC_LEN - 1);
    utils_dateActuelleCourte(c.dateCreation);

    FILE *f = fopen(FICHIER_CATEGORIES, "ab");
    if (!f) return 0;
    fwrite(&c, sizeof(Categorie), 1, f);
    fclose(f);

    utils_journaliser("SYSTEME", "Ajout d'une categorie");
    if (resultat) *resultat = c;
    return 1;
}

int categories_rechercherParId(int id, Categorie *resultat) {
    FILE *f = fopen(FICHIER_CATEGORIES, "rb");
    if (!f) return 0;
    Categorie c;
    while (fread(&c, sizeof(Categorie), 1, f) == 1) {
        if (c.id == id) { *resultat = c; fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}

int categories_existe(int id) {
    Categorie tmp;
    return categories_rechercherParId(id, &tmp);
}

int categories_listerToutes(Categorie *tableau, int tailleMax) {
    FILE *f = fopen(FICHIER_CATEGORIES, "rb");
    if (!f) return 0;
    int n = 0;
    while (n < tailleMax && fread(&tableau[n], sizeof(Categorie), 1, f) == 1) n++;
    fclose(f);
    return n;
}

int categories_supprimer(int id) {
    /* NOTE : la vérification "aucun livre associé" doit être faite par le
       module Livres (books_aucuneCategorieAssociee) AVANT d'appeler ceci. */
    FILE *f = fopen(FICHIER_CATEGORIES, "rb");
    if (!f) return 0;
    Categorie *tous = malloc(sizeof(Categorie) * 10000);
    Categorie tmp;
    int n = 0, trouve = 0;
    while (fread(&tmp, sizeof(Categorie), 1, f) == 1) {
        if (tmp.id == id) { trouve = 1; continue; }
        tous[n++] = tmp;
    }
    fclose(f);
    if (trouve) {
        FILE *fw = fopen(FICHIER_CATEGORIES, "wb");
        fwrite(tous, sizeof(Categorie), n, fw);
        fclose(fw);
    }
    free(tous);
    return trouve;
}
