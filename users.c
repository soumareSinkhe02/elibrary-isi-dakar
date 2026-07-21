#include <stdio.h>
#include <string.h>
#include "users.h"
#include "utils.h"

static int users_rechercherParLogin(const char *login, Utilisateur *u);

#define FICHIER_USERS "DATABASE/USERS.dat"

static int charger_utilisateurs(Utilisateur tab[]) {
    FILE *f = fopen(FICHIER_USERS, "rb");
    if (!f) return 0;
    int nb = 0;
    while (fread(&tab[nb], sizeof(Utilisateur), 1, f) == 1) nb++;
    fclose(f);
    return nb;
}

static void sauvegarder_utilisateurs(Utilisateur tab[], int nb) {
    FILE *f = fopen(FICHIER_USERS, "wb");
    if (!f) return;
    for (int i = 0; i < nb; i++) fwrite(&tab[i], sizeof(Utilisateur), 1, f);
    fclose(f);
}

void users_initialiserSiVide(void) {
    FILE *f = fopen(FICHIER_USERS, "rb");
    if (f) {
        fclose(f);
        return;
    }

    Utilisateur admin;
    admin.id = 1;
    strcpy(admin.nom, "Admin");
    strcpy(admin.prenom, "Super");
    strcpy(admin.telephone, "0000000000");
    strcpy(admin.adresse, "ISI Dakar");
    strcpy(admin.email, "admin@isi.sn");
    strcpy(admin.login, "ADMIN");
    utils_crypterMotDePasse("Library123", admin.motDePasse);
    admin.role = ROLE_ADMIN;
    admin.etat = ETAT_ACTIF;
    admin.doitChangerMdp = 1;
    utils_dateActuelle(admin.dateCreation);
    strcpy(admin.dateDerniereConnexion, "");

    f = fopen(FICHIER_USERS, "wb");
    fwrite(&admin, sizeof(Utilisateur), 1, f);
    fclose(f);
}

int users_connecter(Utilisateur *u) {
    char login[7], mdp[51];
    printf("Login (6 majuscules) : ");
    fgets(login, sizeof(login), stdin);
    login[strcspn(login, "\n")] = 0;
    printf("Mot de passe : ");
    fgets(mdp, sizeof(mdp), stdin);
    mdp[strcspn(mdp, "\n")] = 0;

    Utilisateur tab[1000];
    int nb = charger_utilisateurs(tab);
    for (int i = 0; i < nb; i++) {
        if (strcmp(tab[i].login, login) == 0 &&
            utils_verifierMotDePasse(mdp, tab[i].motDePasse) &&
            tab[i].etat == ETAT_ACTIF) {
            *u = tab[i];
            utils_dateActuelle(u->dateDerniereConnexion);

            tab[i].dateDerniereConnexion[0] = '\0';

            strcpy(tab[i].dateDerniereConnexion, u->dateDerniereConnexion);
            sauvegarder_utilisateurs(tab, nb);

            strcpy(loginCourant, login);
            return 1;
        }
    }
    printf("Connexion echouee.\n");
    return 0;
}

int users_ajouter(const char *nom, const char *prenom, const char *tel,
                   const char *adresse, const char *email, const char *login,
                   Role role, Utilisateur *resultat) {
    Utilisateur existant;
    if (users_rechercherParLogin(login, &existant)) {
        printf("Login deja utilise.\n");
        return 0;
    }
    Utilisateur u;
    u.id = utils_prochainId(FICHIER_USERS, sizeof(Utilisateur));
    strcpy(u.nom, nom);
    strcpy(u.prenom, prenom);
    strcpy(u.telephone, tel);
    strcpy(u.adresse, adresse);
    strcpy(u.email, email);
    strcpy(u.login, login);
    utils_crypterMotDePasse("Library123", u.motDePasse);
    u.role = role;
    u.etat = ETAT_ACTIF;
    u.doitChangerMdp = 1;
    utils_dateActuelle(u.dateCreation);
    strcpy(u.dateDerniereConnexion, "");

    FILE *f = fopen(FICHIER_USERS, "ab");
    if (!f) return 0;
    fwrite(&u, sizeof(Utilisateur), 1, f);
    fclose(f);
    if (resultat) *resultat = u;
    utils_journaliser("Ajout utilisateur");
    return 1;
}

int users_bloquer(int id) {
    Utilisateur tab[1000];
    int nb = charger_utilisateurs(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == id) {
            tab[i].etat = ETAT_BLOQUE;
            sauvegarder_utilisateurs(tab, nb);
            utils_journaliser("Compte bloque");
            return 1;
        }
    }
    return 0;
}

int users_debloquer(int id) {
    Utilisateur tab[1000];
    int nb = charger_utilisateurs(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == id) {
            tab[i].etat = ETAT_ACTIF;
            sauvegarder_utilisateurs(tab, nb);
            utils_journaliser("Compte debloque");
            return 1;
        }
    }
    return 0;
}

int users_changerMotDePasse(int id, const char *nouveauMdp) {
    Utilisateur tab[1000];
    int nb = charger_utilisateurs(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == id) {
            utils_crypterMotDePasse(nouveauMdp, tab[i].motDePasse);
            tab[i].doitChangerMdp = 0;
            sauvegarder_utilisateurs(tab, nb);
            return 1;
        }
    }
    return 0;
}

int users_rechercherParId(int id, Utilisateur *u) {
    Utilisateur tab[1000];
    int nb = charger_utilisateurs(tab);
    for (int i = 0; i < nb; i++) {
        if (tab[i].id == id) {
            *u = tab[i];
            return 1;
        }
    }
    return 0;
}

static int users_rechercherParLogin(const char *login, Utilisateur *u) {
    Utilisateur tab[1000];
    int nb = charger_utilisateurs(tab);
    for (int i = 0; i < nb; i++) {
        if (strcmp(tab[i].login, login) == 0) {
            if (u) *u = tab[i];
            return 1;
        }
    }
    return 0;
}

int users_listerTous(Utilisateur *tableau, int tailleMax) {
    return charger_utilisateurs(tableau);
}

void menu_utilisateurs(void) {
    int choix;
    do {
        printf("\n--- GESTION DES UTILISATEURS ---\n");
        printf("1. Ajouter un utilisateur\n");
        printf("2. Lister les utilisateurs\n");
        printf("3. Bloquer un compte\n");
        printf("4. Debloquer un compte\n");
        printf("0. Retour\n");
        choix = utils_saisirEntier("Votre choix : ");
        switch (choix) {
            case 1: {
                char nom[NAME_LEN], prenom[NAME_LEN], tel[PHONE_LEN], adr[ADDR_LEN],
                     mail[EMAIL_LEN], login[LOGIN_LEN];
                printf("Nom : "); fgets(nom, NAME_LEN, stdin); nom[strcspn(nom, "\n")] = 0;
                printf("Prenom : "); fgets(prenom, NAME_LEN, stdin); prenom[strcspn(prenom, "\n")] = 0;
                printf("Telephone : "); fgets(tel, PHONE_LEN, stdin); tel[strcspn(tel, "\n")] = 0;
                printf("Adresse : "); fgets(adr, ADDR_LEN, stdin); adr[strcspn(adr, "\n")] = 0;
                printf("Email : "); fgets(mail, EMAIL_LEN, stdin); mail[strcspn(mail, "\n")] = 0;
                printf("Login (6 majuscules) : "); fgets(login, LOGIN_LEN, stdin); login[strcspn(login, "\n")] = 0;
                int roleInt;
                roleInt = utils_saisirEntier("Role (0=ADMIN, 1=USER) : ");
                Utilisateur res;
                if (users_ajouter(nom, prenom, tel, adr, mail, login,
                                  roleInt == 0 ? ROLE_ADMIN : ROLE_USER, &res))
                    printf("Utilisateur cree (mot de passe par defaut : Library123).\n");
                break;
            }
            case 2: {
                Utilisateur tab[1000];
                int n = users_listerTous(tab, 1000);
                for (int i = 0; i < n; i++) {
                    printf("[%d] %s %s (%s) - %s - %s\n",
                           tab[i].id, tab[i].prenom, tab[i].nom, tab[i].login,
                           tab[i].role == ROLE_ADMIN ? "ADMIN" : "USER",
                           tab[i].etat == ETAT_ACTIF ? "ACTIF" : "BLOQUE");
                }
                break;
            }
            case 3: {
                int id;
                id = utils_saisirEntier("ID a bloquer : ");
                if (users_bloquer(id)) printf("Compte bloque.\n");
                else printf("Echec.\n");
                break;
            }
            case 4: {
                int id;
                id = utils_saisirEntier("ID a debloquer : ");
                if (users_debloquer(id)) printf("Compte debloque.\n");
                else printf("Echec.\n");
                break;
            }
        }
    } while (choix != 0);
}
