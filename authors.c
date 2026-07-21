#include <stdio.h>
#include <string.h>
#include "authors.h"
#include "books.h"
#include "utils.h"

#define FICHIER_AUTHORS "DATABASE/AUTHORS.dat"

static int charger_auteurs(Auteur tab[]) {
    FILE *f = fopen(FICHIER_AUTHORS, "rb");
    if (!f) return 0;
    int nb = 0;
    while (fread(&tab[nb], sizeof(Auteur), 1, f) == 1) nb++;
    fclose(f);
    return nb;
}

static void sauvegarder_auteurs(Auteur tab[], int nb) {
    FILE *f = fopen(FICHIER_AUTHORS, "wb");
    if (!f) return;
    for (int i = 0; i < nb; i++) fwrite(&tab[i], sizeof(Auteur), 1, f);
    fclose(f);
}

static void ajouter_auteur(void) {
    Auteur a;
    a.id = utils_prochainId(FICHIER_AUTHORS, sizeof(Auteur));
    printf("Nom complet : ");
    fgets(a.nomComplet, NAME_LEN, stdin);
    a.nomComplet[strcspn(a.nomComplet, "\n")] = 0;
    printf("Nationalite : ");
    fgets(a.nationalite, NAME_LEN, stdin);
    a.nationalite[strcspn(a.nationalite, "\n")] = 0;
    printf("Date de naissance (JJ/MM/AAAA) : ");
    fgets(a.dateNaissance, DATE_LEN, stdin);
    a.dateNaissance[strcspn(a.dateNaissance, "\n")] = 0;
    printf("Biographie : ");
    fgets(a.biographie, BIO_LEN, stdin);
    a.biographie[strcspn(a.biographie, "\n")] = 0;
    a.nombreLivresPublies = utils_saisirEntier("Nombre de livres publies : ");

    FILE *f = fopen(FICHIER_AUTHORS, "ab");
    if (f) {
        fwrite(&a, sizeof(Auteur), 1, f);
        fclose(f);
        printf("Auteur ajoute (ID %d).\n", a.id);
        utils_journaliser("Ajout auteur");
    }
}

static void lister_auteurs(void) {
    Auteur tab[1000];
    int nb = charger_auteurs(tab);
    printf("Liste des auteurs :\n");
    for (int i = 0; i < nb; i++) {
        printf("[%d] %s (%s) - %d livre(s) publie(s)\n",
               tab[i].id, tab[i].nomComplet, tab[i].nationalite,
               tab[i].nombreLivresPublies);
    }
}

static void supprimer_auteur(void) {
    int id;
   id = utils_saisirEntier("ID de l'auteur a supprimer : ");

    if (!books_aucunLivrePourAuteur(id)) {
        printf("Impossible : des livres sont encore associes a cet auteur.\n");
        return;
    }

    Auteur tab[1000];
    int nb = charger_auteurs(tab);
    int trouve = 0;
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == id) {
            trouve = 1;
            for (int j = i; j < nb - 1; j++) tab[j] = tab[j + 1];
            nb--;
            break;
        }
    }
    if (trouve) {
        sauvegarder_auteurs(tab, nb);
        printf("Auteur supprime.\n");
        utils_journaliser("Suppression auteur");
    } else {
        printf("Auteur introuvable.\n");
    }
}

int auteur_existe(int id) {
    Auteur tab[1000];
    int nb = charger_auteurs(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == id) return 1;
    }
    return 0;
}

void menu_auteurs(void) {
    int choix;
    do {
        printf("\n--- GESTION DES AUTEURS ---\n");
        printf("1. Ajouter un auteur\n");
        printf("2. Lister les auteurs\n");
        printf("3. Supprimer un auteur\n");
        printf("0. Retour\n");
        choix = utils_saisirEntier("Votre choix : ");
        switch (choix) {
            case 1: ajouter_auteur(); break;
            case 2: lister_auteurs(); break;
            case 3: supprimer_auteur(); break;
        }
    } while (choix != 0);
}
