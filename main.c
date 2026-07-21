#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "users.h"
#include "authors.h"
#include "categories.h"
#include "books.h"
#include "borrows.h"
#include "returns.h"
#include "reservations.h"
#include "penalties.h"
#include "reports.h"

int main(void) {
    utils_creerDossiers();
    users_initialiserSiVide();
    borrows_actualiserRetards();

    Utilisateur utilisateur;
    if (!users_connecter(&utilisateur)) {
        printf("Au revoir.\n");
        return 0;
    }

    if (utilisateur.doitChangerMdp) {
        char nouveau[51], confirmation[51];
        printf("\n>>> Premiere connexion : vous devez changer votre mot de passe.\n");
        do {
            printf("Nouveau mot de passe : ");
            fgets(nouveau, sizeof(nouveau), stdin);
            nouveau[strcspn(nouveau, "\n")] = 0;
            printf("Confirmer : ");
            fgets(confirmation, sizeof(confirmation), stdin);
            confirmation[strcspn(confirmation, "\n")] = 0;
            if (strcmp(nouveau, confirmation) != 0)
                printf("Les mots de passe ne correspondent pas.\n");
        } while (strcmp(nouveau, confirmation) != 0);
        users_changerMotDePasse(utilisateur.id, nouveau);
        printf("Mot de passe change avec succes.\n");
    }

    int choix;
    do {
        printf("\n========== E-LIBRARY ISI DAKAR ==========\n");
        printf("Connecte en tant que : %s\n", utilisateur.login);
        printf("1. Auteurs\n");
        printf("2. Categories\n");
        printf("3. Livres\n");
        printf("4. Emprunts\n");
        printf("5. Retours\n");
        printf("6. Reservations\n");
        printf("7. Penalites\n");
        printf("8. Etat journalier\n");
        if (utilisateur.role == ROLE_ADMIN) {
            printf("9. Gestion des utilisateurs\n");
        }
        printf("0. Quitter\n");
        choix = utils_saisirEntier("Votre choix : ");

        switch (choix) {
            case 1: menu_auteurs(); break;
            case 2: menu_categories(); break;
            case 3: menu_livres(); break;
            case 4: menu_emprunts(); break;
            case 5: menu_retours(); break;
            case 6: menu_reservations(); break;
            case 7: menu_penalites(); break;
            case 8: reports_genererEtatJournalier(); break;
            case 9:
                if (utilisateur.role == ROLE_ADMIN) menu_utilisateurs();
                else printf("Option invalide.\n");
                break;
        }
    } while (choix != 0);

    printf("Au revoir.\n");
    return 0;
}
