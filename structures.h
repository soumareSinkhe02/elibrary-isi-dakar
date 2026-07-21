#ifndef STRUCTURES_H
#define STRUCTURES_H

#define LOGIN_LEN        7
#define PASS_LEN         51
#define NAME_LEN         51
#define EMAIL_LEN        101
#define PHONE_LEN        21
#define ADDR_LEN         101
#define DATE_LEN         20
#define ISBN_LEN         21
#define TITLE_LEN        151
#define BIO_LEN          501
#define DESC_LEN         301
#define EMPRUNT_NUM_LEN  25
#define EMPLACEMENT_LEN  50
#define LANGUE_LEN       30

typedef enum { ROLE_ADMIN, ROLE_USER } Role;
typedef enum { ETAT_ACTIF, ETAT_BLOQUE } EtatCompte;

typedef struct {
    int  id;
    char nom[NAME_LEN];
    char prenom[NAME_LEN];
    char telephone[PHONE_LEN];
    char adresse[ADDR_LEN];
    char email[EMAIL_LEN];
    char login[LOGIN_LEN];
    char motDePasse[PASS_LEN];
    Role role;
    EtatCompte etat;
    char dateCreation[DATE_LEN];
    char dateDerniereConnexion[DATE_LEN];
    int  doitChangerMdp;
} Utilisateur;

typedef struct {
    int  id;
    char nomComplet[NAME_LEN];
    char nationalite[NAME_LEN];
    char dateNaissance[DATE_LEN];
    char biographie[BIO_LEN];
    int  nombreLivresPublies;
} Auteur;

typedef struct {
    int  id;
    char libelle[NAME_LEN];
    char description[DESC_LEN];
    char dateCreation[DATE_LEN];
} Categorie;

typedef struct {
    int  id;
    char isbn[ISBN_LEN];
    char titre[TITLE_LEN];
    int  idAuteur;
    int  idCategorie;
    char editeur[NAME_LEN];
    int  anneePublication;
    char langue[LANGUE_LEN];
    int  nombrePages;
    int  nombreExemplaires;
    int  nombreDisponibles;
    char emplacement[EMPLACEMENT_LEN];
    char resume[DESC_LEN];
    char dateAjout[DATE_LEN];
} Livre;

typedef enum { EMPRUNT_EN_COURS, EMPRUNT_RETOURNE, EMPRUNT_EN_RETARD } EtatEmprunt;

typedef struct {
    int  id;
    char numeroEmprunt[EMPRUNT_NUM_LEN];
    int  idUtilisateur;
    int  idLivre;
    char dateEmprunt[DATE_LEN];
    char dateRetourPrevue[DATE_LEN];
    EtatEmprunt etat;
} Emprunt;

typedef struct {
    int    id;
    int    idEmprunt;
    char   dateRetourReelle[DATE_LEN];
    int    joursRetard;
    double montantPenalite;
} Retour;

typedef struct {
    int    id;
    int    idUtilisateur;
    int    idEmprunt;
    int    joursRetard;
    double montant;
    char   date[DATE_LEN];
} Penalite;

typedef enum { RES_EN_ATTENTE, RES_DISPONIBLE, RES_ANNULEE } EtatReservation;

typedef struct {
    int  id;
    int  idUtilisateur;
    int  idLivre;
    char dateReservation[DATE_LEN];
    EtatReservation etat;
} Reservation;

#endif
