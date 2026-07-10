#ifndef USERS_H
#define USERS_H

#include "structures.h"

#define FICHIER_USERS "DATABASE/USERS.dat"

/* Crée le premier compte ADMIN si le fichier est vide (login ADMIN par défaut) */
void users_initialiserSiVide(void);

/* Retourne 1 si succès, 0 si login déjà pris ou format invalide (doit faire
   exactement 6 lettres majuscules, ex: "MATTOI") */
int users_ajouter(const char *nom, const char *prenom, const char *telephone,
                   const char *adresse, const char *email, const char *login,
                   Role role, Utilisateur *resultat);

/* Authentifie : retourne 1 si login+mdp corrects (et remplit *u), 0 sinon.
   Met aussi à jour dateDerniereConnexion. */
int users_authentifier(const char *login, const char *motDePasseClair, Utilisateur *u);

/* Change le mot de passe (à utiliser obligatoirement après 1ère connexion) */
int users_changerMotDePasse(int idUtilisateur, const char *nouveauMdpClair);

int users_rechercherParId(int id, Utilisateur *resultat);
int users_rechercherParLogin(const char *login, Utilisateur *resultat);

int users_bloquer(int id);
int users_debloquer(int id);

/* Retourne le nombre d'utilisateurs, remplit le tableau (déjà alloué, taille max) */
int users_listerTous(Utilisateur *tableau, int tailleMax);

int users_loginEstValide(const char *login); /* 6 lettres majuscules */
int users_loginExiste(const char *login);

#endif /* USERS_H */
