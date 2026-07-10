#ifndef CATEGORIES_H
#define CATEGORIES_H

#include "structures.h"

#define FICHIER_CATEGORIES "DATABASE/CATEGORIES.dat"

int categories_ajouter(const char *libelle, const char *description, Categorie *resultat);
int categories_rechercherParId(int id, Categorie *resultat);
int categories_existe(int id); /* utilisé par le module Livres pour vérifier la contrainte */
int categories_listerToutes(Categorie *tableau, int tailleMax);
int categories_supprimer(int id); /* à utiliser uniquement si aucun livre ne la référence */

#endif /* CATEGORIES_H */
