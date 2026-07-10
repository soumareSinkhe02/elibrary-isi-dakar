#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "utils.h"

void utils_dateActuelle(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void utils_dateActuelleCourte(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(buffer, "%02d/%02d/%04d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
}

void utils_genererNumeroEmprunt(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(buffer, "EMP_%04d%02d%02d%02d%02d%02d",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void utils_genererNomFichierAAAAMMJJ(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(buffer, "%04d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

/* Convertit "JJ/MM/AAAA" en jours depuis une origine arbitraire (via mktime) */
static time_t dateVersTime(const char *dateJJMMAAAA) {
    struct tm tm = {0};
    int j, m, a;
    sscanf(dateJJMMAAAA, "%d/%d/%d", &j, &m, &a);
    tm.tm_mday = j;
    tm.tm_mon = m - 1;
    tm.tm_year = a - 1900;
    tm.tm_hour = 12; /* midi pour éviter les soucis d'heure d'été */
    return mktime(&tm);
}

int utils_diffJours(const char *dateDebutJJMMAAAA, const char *dateFinJJMMAAAA) {
    time_t t1 = dateVersTime(dateDebutJJMMAAAA);
    time_t t2 = dateVersTime(dateFinJJMMAAAA);
    double diff = difftime(t2, t1) / (60 * 60 * 24);
    return (int)(diff + 0.5);
}

void utils_ajouterJours(const char *dateJJMMAAAA, int nbJours, char *resultat) {
    time_t t = dateVersTime(dateJJMMAAAA);
    t += (time_t)nbJours * 24 * 60 * 60;
    struct tm *tm = localtime(&t);
    sprintf(resultat, "%02d/%02d/%04d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
}

void utils_creerDossiers(void) {
#if defined(_WIN32)
    system("mkdir DATABASE 2>nul");
    system("mkdir REPORTS 2>nul");
    system("mkdir REPORTS\\BORROWS 2>nul");
    system("mkdir REPORTS\\RETURNS 2>nul");
    system("mkdir REPORTS\\DAILY 2>nul");
#else
    mkdir("DATABASE", 0777);
    mkdir("REPORTS", 0777);
    mkdir("REPORTS/BORROWS", 0777);
    mkdir("REPORTS/RETURNS", 0777);
    mkdir("REPORTS/DAILY", 0777);
#endif
}

/* Chiffrement pédagogique très simple : décalage XOR + encodage hexadécimal.
   Objectif : le mot de passe n'apparaît pas en clair dans le fichier .dat.
   (Ce n'est PAS un algorithme de sécurité réel — à mentionner en soutenance !) */
#define CLE_XOR 0x5A

void utils_crypterMotDePasse(const char *clair, char *sortieCryptee) {
    size_t i, len = strlen(clair);
    char hex[3];
    sortieCryptee[0] = '\0';
    for (i = 0; i < len; i++) {
        unsigned char c = (unsigned char)clair[i] ^ CLE_XOR;
        sprintf(hex, "%02X", c);
        strcat(sortieCryptee, hex);
    }
}

int utils_verifierMotDePasse(const char *clairSaisi, const char *crypteStocke) {
    char tmp[PASS_LEN];
    utils_crypterMotDePasse(clairSaisi, tmp);
    return strcmp(tmp, crypteStocke) == 0;
}

void utils_journaliser(const char *login, const char *operation) {
    utils_creerDossiers();
    FILE *f = fopen("REPORTS/HISTORY.txt", "a");
    if (!f) return;
    char date[DATE_LEN];
    utils_dateActuelle(date);
    fprintf(f, "%s - %s - %s\n", date, login, operation);
    fclose(f);
}

int utils_prochainId(const char *cheminFichier, size_t tailleRecord) {
    FILE *f = fopen(cheminFichier, "rb");
    if (!f) return 1;
    int maxId = 0;
    unsigned char *buf = malloc(tailleRecord);
    while (fread(buf, tailleRecord, 1, f) == 1) {
        int id;
        memcpy(&id, buf, sizeof(int)); /* id est TOUJOURS le 1er champ de chaque struct */
        if (id > maxId) maxId = id;
    }
    free(buf);
    fclose(f);
    return maxId + 1;
}
