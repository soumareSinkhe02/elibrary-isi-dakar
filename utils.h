#ifndef UTILS_H
#define UTILS_H

extern char loginCourant[7];

void utils_dateActuelle(char *buffer);
void utils_dateActuelleCourte(char *buffer);
void utils_genererNumeroEmprunt(char *buffer);
int  utils_diffJours(const char *dateDebut, const char *dateFin);
void utils_ajouterJours(const char *date, int nbJours, char *resultat);
void utils_creerDossiers(void);
void utils_crypterMotDePasse(const char *clair, char *sortieCryptee);
int  utils_verifierMotDePasse(const char *clair, const char *crypte);
void utils_journaliser(const char *operation);

int  utils_prochainId(const char *cheminFichier, size_t tailleRecord);
int  utils_saisirEntier(const char *invite);

#endif
