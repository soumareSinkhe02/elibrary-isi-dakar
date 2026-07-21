#ifndef USERS_H
#define USERS_H

#include "structures.h"

void menu_utilisateurs(void);

void users_initialiserSiVide(void);

int  users_connecter(Utilisateur *u);

int  users_ajouter(const char *nom, const char *prenom, const char *tel,
                   const char *adresse, const char *email, const char *login,
                   Role role, Utilisateur *resultat);
int  users_bloquer(int id);
int  users_debloquer(int id);
int  users_changerMotDePasse(int id, const char *nouveauMdp);
int  users_rechercherParId(int id, Utilisateur *u);
int  users_listerTous(Utilisateur *tableau, int tailleMax);

#endif
