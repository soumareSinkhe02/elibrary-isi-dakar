#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "utils.h"

int utils_saisirEntier(const char *invite) {
    int valeur;
    char buffer[50];
    while (1) {
        printf("%s", invite);
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &valeur) == 1) {
            char test;
            if (sscanf(buffer, "%d %c", &valeur, &test) == 1) {
                return valeur;
            }
        }
        printf("Veuillez entrer un nombre valide.\n");
    }
}

char loginCourant[7] = "";

void utils_dateActuelle(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, 20, "%d/%m/%Y %H:%M:%S", tm_info);
}

void utils_dateActuelleCourte(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, 11, "%d/%m/%Y", tm_info);
}

void utils_genererNumeroEmprunt(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, 25, "EMP_%Y%m%d%H%M%S", tm_info);
}

int utils_diffJours(const char *dateDebut, const char *dateFin) {
    int j1, m1, a1, j2, m2, a2;
    sscanf(dateDebut, "%d/%d/%d", &j1, &m1, &a1);
    sscanf(dateFin, "%d/%d/%d", &j2, &m2, &a2);
    struct tm d1 = {0}, d2 = {0};
    d1.tm_mday = j1; d1.tm_mon = m1 - 1; d1.tm_year = a1 - 1900;
    d2.tm_mday = j2; d2.tm_mon = m2 - 1; d2.tm_year = a2 - 1900;
    time_t t1 = mktime(&d1);
    time_t t2 = mktime(&d2);
    double diff_sec = difftime(t2, t1);
    return (int)(diff_sec / (60 * 60 * 24));
}

void utils_ajouterJours(const char *date, int nbJours, char *resultat) {
    int j, m, a;
    sscanf(date, "%d/%d/%d", &j, &m, &a);
    struct tm d = {0};
    d.tm_mday = j; d.tm_mon = m - 1; d.tm_year = a - 1900;
    d.tm_mday += nbJours;
    mktime(&d);
    strftime(resultat, 11, "%d/%m/%Y", &d);
}

void utils_creerDossiers(void) {
    #ifdef _WIN32
        mkdir("DATABASE");
        mkdir("REPORTS");
        mkdir("REPORTS/BORROWS");
        mkdir("REPORTS/RETURNS");
        mkdir("REPORTS/DAILY");
    #else
        mkdir("DATABASE", 0755);
        mkdir("REPORTS", 0755);
        mkdir("REPORTS/BORROWS", 0755);
        mkdir("REPORTS/RETURNS", 0755);
        mkdir("REPORTS/DAILY", 0755);
    #endif
}

void utils_crypterMotDePasse(const char *clair, char *sortieCryptee) {
    int i;
    for (i = 0; clair[i] != '\0'; i++) {
        sortieCryptee[i] = clair[i] ^ 'K';
    }
    sortieCryptee[i] = '\0';
}

int utils_verifierMotDePasse(const char *clair, const char *crypte) {
    char crypteSaisi[51];
    utils_crypterMotDePasse(clair, crypteSaisi);
    return strcmp(crypteSaisi, crypte) == 0;
}

void utils_journaliser(const char *operation) {
    if (loginCourant[0] == '\0') return;
    FILE *f = fopen("REPORTS/HISTORY.txt", "a");
    if (!f) return;
    char date[20];
    utils_dateActuelle(date);
    fprintf(f, "%s — %s : %s\n", date, loginCourant, operation);
    fclose(f);
}

int utils_prochainId(const char *cheminFichier, size_t tailleRecord) {
    FILE *f = fopen(cheminFichier, "rb");
    if (!f) return 1;
    fseek(f, 0, SEEK_END);
    long taille = ftell(f);
    if (taille < (long)tailleRecord) {
        fclose(f);
        return 1;
    }
    fseek(f, -tailleRecord, SEEK_END);
    char buffer[tailleRecord];
    fread(buffer, tailleRecord, 1, f);
    fclose(f);
    int id;
    memcpy(&id, buffer, sizeof(int));
    return id + 1;
}
