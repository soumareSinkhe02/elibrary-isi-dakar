#include <stdio.h>
#include <string.h>
#include "reports.h"
#include "utils.h"
#include "users.h"
#include "books.h"
#include "borrows.h"
#include "returns.h"
#include "reservations.h"
#include "penalties.h"

void reports_genererRecuEmprunt(Emprunt e) {
    char nomFichier[100];
    sprintf(nomFichier, "REPORTS/BORROWS/BORROW_%s.txt", e.numeroEmprunt);
    FILE *f = fopen(nomFichier, "w");
    if (!f) return;

    Livre l;
    int livreTrouve = 0;
    FILE *fl = fopen("DATABASE/BOOKS.dat", "rb");
    if (fl) {
        while (fread(&l, sizeof(Livre), 1, fl) == 1) {
            if (l.id == e.idLivre) { livreTrouve = 1; break; }
        }
        fclose(fl);
    }

    Utilisateur u;
    char nomUtilisateur[100] = "Inconnu";
    if (users_rechercherParId(e.idUtilisateur, &u)) {
        sprintf(nomUtilisateur, "%s %s", u.prenom, u.nom);
    }

    fprintf(f, "RECU D'EMPRUNT\n");
    fprintf(f, "Numero d'emprunt : %s\n", e.numeroEmprunt);
    fprintf(f, "Lecteur : %s\n", nomUtilisateur);
    fprintf(f, "Date d'emprunt : %s\n", e.dateEmprunt);
    fprintf(f, "Date prevue de retour : %s\n", e.dateRetourPrevue);
    if (livreTrouve) {
        fprintf(f, "Livre : %s\n", l.titre);
        fprintf(f, "Auteur ID : %d\n", l.idAuteur);
        fprintf(f, "ISBN : %s\n", l.isbn);
    }
    fclose(f);
}

void reports_genererRecuRetour(Retour r, Emprunt e) {
    char nomFichier[100];
    sprintf(nomFichier, "REPORTS/RETURNS/RETURN_%s.txt", e.numeroEmprunt);
    FILE *f = fopen(nomFichier, "w");
    if (!f) return;

    fprintf(f, "REÇU DE RETOUR\n");
    fprintf(f, "Numero d'emprunt : %s\n", e.numeroEmprunt);
    fprintf(f, "Date de retour : %s\n", r.dateRetourReelle);
    fprintf(f, "Retard : %d jour(s)\n", r.joursRetard);
    fprintf(f, "Penalite : %.0f FCFA\n", r.montantPenalite);
    fclose(f);
}

void reports_genererEtatJournalier(void) {
    char date[11];
    utils_dateActuelleCourte(date);
    char nomFichier[50];
    sprintf(nomFichier, "REPORTS/DAILY/REPORT_%s.txt", date);
    FILE *f = fopen(nomFichier, "w");
    if (!f) return;

    int nbEmprunts = 0;
    Emprunt emp[1000];
    FILE *fe = fopen("DATABASE/BORROWS.dat", "rb");
    if (fe) {
        while (fread(&emp[nbEmprunts], sizeof(Emprunt), 1, fe) == 1) nbEmprunts++;
        fclose(fe);
    }
    int empruntsJour = 0;
    for (int i = 0; i < nbEmprunts; i++) {
        if (strcmp(emp[i].dateEmprunt, date) == 0) empruntsJour++;
    }

    int nbRetours = 0;
    Retour ret[1000];
    FILE *fr = fopen("DATABASE/RETURNS.dat", "rb");
    if (fr) {
        while (fread(&ret[nbRetours], sizeof(Retour), 1, fr) == 1) nbRetours++;
        fclose(fr);
    }
    int retoursJour = 0;
    for (int i = 0; i < nbRetours; i++) {
        if (strcmp(ret[i].dateRetourReelle, date) == 0) retoursJour++;
    }

    int nbRes = 0;
    Reservation res[1000];
    FILE *frr = fopen("DATABASE/RESERVATIONS.dat", "rb");
    if (frr) {
        while (fread(&res[nbRes], sizeof(Reservation), 1, frr) == 1) nbRes++;
        fclose(frr);
    }
    int resJour = 0;
    for (int i = 0; i < nbRes; i++) {
        if (strcmp(res[i].dateReservation, date) == 0) resJour++;
    }

    int nbUsers = 0;
    Utilisateur users[1000];
    FILE *fu = fopen("DATABASE/USERS.dat", "rb");
    if (fu) {
        while (fread(&users[nbUsers], sizeof(Utilisateur), 1, fu) == 1) nbUsers++;
        fclose(fu);
    }
    int newUsers = 0;
    for (int i = 0; i < nbUsers; i++) {
        if (strncmp(users[i].dateCreation, date, 10) == 0) newUsers++;
    }

    int nbPen = 0;
    Penalite pen[1000];
    FILE *fp = fopen("DATABASE/PENALTIES.dat", "rb");
    if (fp) {
        while (fread(&pen[nbPen], sizeof(Penalite), 1, fp) == 1) nbPen++;
        fclose(fp);
    }
    int penJour = 0;
    double montantJour = 0;
    for (int i = 0; i < nbPen; i++) {
        if (strcmp(pen[i].date, date) == 0) {
            penJour++;
            montantJour += pen[i].montant;
        }
    }

    int maxEmprunts = 0;
    char titrePlusEmprunte[TITLE_LEN] = "Aucun";
    Livre tousLivres[1000];
    int nbLivres = 0;
    FILE *fl = fopen("DATABASE/BOOKS.dat", "rb");
    if (fl) {
        while (fread(&tousLivres[nbLivres], sizeof(Livre), 1, fl) == 1) nbLivres++;
        fclose(fl);
    }
    for (int i = 0; i < nbLivres; i++) {
        int compteur = 0;
        for (int j = 0; j < nbEmprunts; j++) {
            if (emp[j].idLivre == tousLivres[i].id) compteur++;
        }
        if (compteur > maxEmprunts) {
            maxEmprunts = compteur;
            strcpy(titrePlusEmprunte, tousLivres[i].titre);
        }
    }

    int maxEmpruntsUser = 0;
    char userPlusActif[100] = "Aucun";
    for (int i = 0; i < nbUsers; i++) {
        int compteur = 0;
        for (int j = 0; j < nbEmprunts; j++) {
            if (emp[j].idUtilisateur == users[i].id) compteur++;
        }
        if (compteur > maxEmpruntsUser) {
            maxEmpruntsUser = compteur;
            sprintf(userPlusActif, "%s %s (%s)", users[i].prenom, users[i].nom, users[i].login);
        }
    }

    fprintf(f, "ETAT JOURNALIER - %s\n\n", date);
    fprintf(f, "Nombre d'emprunts : %d\n", empruntsJour);
    fprintf(f, "Nombre de retours : %d\n", retoursJour);
    fprintf(f, "Nombre de reservations : %d\n", resJour);
    fprintf(f, "Nouveaux utilisateurs : %d\n", newUsers);
    fprintf(f, "Penalites infligees : %d\n", penJour);
    fprintf(f, "Montant total des penalites : %.0f FCFA\n", montantJour);
    fprintf(f, "Livre le plus emprunte : %s\n", titrePlusEmprunte);
    fprintf(f, "Utilisateur le plus actif : %s\n", userPlusActif);

    fclose(f);
    printf("Etat journalier genere dans %s\n", nomFichier);
}
