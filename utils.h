#ifndef UTILS_H
#define UTILS_H

#include "structures.h"

/* ============================================================
   FICHIER PARTAGE - fonctions "boîte à outils" utilisées par
   tous les modules. Ecrit une fois pour toute l'équipe.
   ============================================================ */

/* --- Dates --- */
void utils_dateActuelle(char *buffer);              /* "JJ/MM/AAAA HH:MM:SS" */
void utils_dateActuelleCourte(char *buffer);         /* "JJ/MM/AAAA" */
void utils_genererNumeroEmprunt(char *buffer);        /* EMP_AAAAMMJJHHMMSS */
void utils_genererNomFichierAAAAMMJJ(char *buffer);    /* AAAAMMJJ pour les rapports */
int  utils_diffJours(const char *dateDebutJJMMAAAA, const char *dateFinJJMMAAAA);
void utils_ajouterJours(const char *dateJJMMAAAA, int nbJours, char *resultat);

/* --- Dossiers --- */
void utils_creerDossiers(void); /* crée DATABASE/ REPORTS/ REPORTS/BORROWS/ ... */

/* --- Mot de passe (chiffrement simple pédagogique, PAS pour la prod) --- */
void utils_crypterMotDePasse(const char *clair, char *sortieCryptee);
int  utils_verifierMotDePasse(const char *clairSaisi, const char *crypteStocke);

/* --- Journal des opérations (REPORTS/HISTORY.txt) --- */
void utils_journaliser(const char *login, const char *operation);

/* --- Génération d'id auto-incrémenté générique ---
   Relit tout le fichier binaire et retourne (max id existant)+1.
   tailleRecord = sizeof(la struct), offsetId = offset du champ id (toujours 0 si id est le 1er champ) */
int utils_prochainId(const char *cheminFichier, size_t tailleRecord);

#endif /* UTILS_H */
