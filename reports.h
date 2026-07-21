#ifndef REPORTS_H
#define REPORTS_H

#include "structures.h"

void reports_genererRecuEmprunt(Emprunt e);
void reports_genererRecuRetour(Retour r, Emprunt e);
void reports_genererEtatJournalier(void);

#endif
