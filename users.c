#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "users.h"
#include "utils.h"

#define MDP_PAR_DEFAUT "Library123"

int users_loginEstValide(const char *login) {
    if (strlen(login) != 6) return 0;
    for (int i = 0; i < 6; i++) {
        if (!isupper((unsigned char)login[i])) return 0;
    }
    return 1;
}

int users_loginExiste(const char *login) {
    Utilisateur tmp;
    return users_rechercherParLogin(login, &tmp);
}

void users_initialiserSiVide(void) {
    utils_creerDossiers();
    FILE *f = fopen(FICHIER_USERS, "rb");
    int vide = 1;
    if (f) {
        Utilisateur u;
        if (fread(&u, sizeof(Utilisateur), 1, f) == 1) vide = 0;
        fclose(f);
    }
    if (vide) {
        Utilisateur admin;
        users_ajouter("Admin", "Systeme", "770000000", "ISI Dakar",
                      "admin@elibrary.sn", "ADMINI", ROLE_ADMIN, &admin);
        printf("[INFO] Compte admin cree automatiquement : login=ADMINI mot de passe=%s\n",
               MDP_PAR_DEFAUT);
    }
}

int users_ajouter(const char *nom, const char *prenom, const char *telephone,
                   const char *adresse, const char *email, const char *login,
                   Role role, Utilisateur *resultat) {
    if (!users_loginEstValide(login)) {
        printf("Erreur : le login doit comporter exactement 6 lettres majuscules.\n");
        return 0;
    }
    if (users_loginExiste(login)) {
        printf("Erreur : ce login existe deja.\n");
        return 0;
    }

    Utilisateur u = {0};
    u.id = utils_prochainId(FICHIER_USERS, sizeof(Utilisateur));
    strncpy(u.nom, nom, NAME_LEN - 1);
    strncpy(u.prenom, prenom, NAME_LEN - 1);
    strncpy(u.telephone, telephone, PHONE_LEN - 1);
    strncpy(u.adresse, adresse, ADDR_LEN - 1);
    strncpy(u.email, email, EMAIL_LEN - 1);
    strncpy(u.login, login, LOGIN_LEN - 1);
    utils_crypterMotDePasse(MDP_PAR_DEFAUT, u.motDePasse);
    u.role = role;
    u.etat = ETAT_ACTIF;
    utils_dateActuelle(u.dateCreation);
    strcpy(u.dateDerniereConnexion, "-");
    u.doitChangerMdp = 1;

    FILE *f = fopen(FICHIER_USERS, "ab");
    if (!f) { printf("Erreur ouverture fichier utilisateurs.\n"); return 0; }
    fwrite(&u, sizeof(Utilisateur), 1, f);
    fclose(f);

    utils_journaliser(login, "Creation de compte utilisateur");
    if (resultat) *resultat = u;
    return 1;
}

int users_rechercherParId(int id, Utilisateur *resultat) {
    FILE *f = fopen(FICHIER_USERS, "rb");
    if (!f) return 0;
    Utilisateur u;
    while (fread(&u, sizeof(Utilisateur), 1, f) == 1) {
        if (u.id == id) { *resultat = u; fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}

int users_rechercherParLogin(const char *login, Utilisateur *resultat) {
    FILE *f = fopen(FICHIER_USERS, "rb");
    if (!f) return 0;
    Utilisateur u;
    while (fread(&u, sizeof(Utilisateur), 1, f) == 1) {
        if (strcmp(u.login, login) == 0) { *resultat = u; fclose(f); return 1; }
    }
    fclose(f);
    return 0;
}

/* Réécrit tout le fichier avec l'utilisateur modifié (technique "lire tout,
   remplacer, réécrire tout" : la plus simple à expliquer en soutenance) */
static int users_sauvegarder(Utilisateur *u) {
    FILE *f = fopen(FICHIER_USERS, "rb");
    if (!f) return 0;
    Utilisateur tmp;
    Utilisateur *tous = malloc(sizeof(Utilisateur) * 10000);
    int n = 0;
    while (fread(&tmp, sizeof(Utilisateur), 1, f) == 1) {
        if (tmp.id == u->id) tous[n] = *u; else tous[n] = tmp;
        n++;
    }
    fclose(f);
    FILE *fw = fopen(FICHIER_USERS, "wb");
    fwrite(tous, sizeof(Utilisateur), n, fw);
    fclose(fw);
    free(tous);
    return 1;
}

int users_authentifier(const char *login, const char *motDePasseClair, Utilisateur *u) {
    if (!users_rechercherParLogin(login, u)) {
        printf("Login inconnu.\n");
        return 0;
    }
    if (u->etat == ETAT_BLOQUE) {
        printf("Compte bloque. Contactez un administrateur.\n");
        return 0;
    }
    if (!utils_verifierMotDePasse(motDePasseClair, u->motDePasse)) {
        printf("Mot de passe incorrect.\n");
        return 0;
    }
    utils_dateActuelle(u->dateDerniereConnexion);
    users_sauvegarder(u);
    utils_journaliser(login, "Connexion reussie");
    return 1;
}

int users_changerMotDePasse(int idUtilisateur, const char *nouveauMdpClair) {
    Utilisateur u;
    if (!users_rechercherParId(idUtilisateur, &u)) return 0;
    utils_crypterMotDePasse(nouveauMdpClair, u.motDePasse);
    u.doitChangerMdp = 0;
    users_sauvegarder(&u);
    utils_journaliser(u.login, "Changement de mot de passe");
    return 1;
}

int users_bloquer(int id) {
    Utilisateur u;
    if (!users_rechercherParId(id, &u)) return 0;
    u.etat = ETAT_BLOQUE;
    users_sauvegarder(&u);
    utils_journaliser(u.login, "Compte bloque par un administrateur");
    return 1;
}

int users_debloquer(int id) {
    Utilisateur u;
    if (!users_rechercherParId(id, &u)) return 0;
    u.etat = ETAT_ACTIF;
    users_sauvegarder(&u);
    utils_journaliser(u.login, "Compte debloque par un administrateur");
    return 1;
}

int users_listerTous(Utilisateur *tableau, int tailleMax) {
    FILE *f = fopen(FICHIER_USERS, "rb");
    if (!f) return 0;
    int n = 0;
    while (n < tailleMax && fread(&tableau[n], sizeof(Utilisateur), 1, f) == 1) {
        n++;
    }
    fclose(f);
    return n;
}

