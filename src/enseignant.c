#include <stdio.h>
#include <string.h>
#include "enseignant.h"
#include "utils.h"

int verifier_mot_de_passe(void) {
    char saisie[MAX_NOM];
    printf(JAUNE "Mot de passe : " RESET);
    lire(saisie, sizeof(saisie));
    if (strcmp(saisie, MOT_DE_PASSE) == 0) {
        ok("Authentification reussie !");
        return 1;
    }
    erreur("Mot de passe incorrect.");
    return 0;
}

/* Enregistre le nom du QCM dans l'index */
static void ajouter_index(const char *nom) {
    FILE *f = fopen(FICHIER_INDEX, "a");
    if (f != NULL) {
        fprintf(f, "%s\n", nom);
        fclose(f);
    }
}

/* Sauvegarde le QCM dans qcm/<nom>.txt */
static int sauvegarder_qcm(const QCM *qcm) {
    int  i, j;
    char chemin[MAX_NOM * 2];
    FILE *f;

    snprintf(chemin, sizeof(chemin), "%s%s.txt", DOSSIER_QCM, qcm->nom);
    f = fopen(chemin, "w");
    if (f == NULL) {
        erreur("Impossible de creer le fichier.");
        return 0;
    }

    /* En-tete : nom, parametres, nombre de questions */
    fprintf(f, "%s\n%d %d %d\n%d\n",
            qcm->nom,
            qcm->points_negatifs,
            qcm->reponses_multiples,
            qcm->mode_sequentiel,
            qcm->nb_questions);

    /* Questions */
    for (i = 0; i < qcm->nb_questions; i++) {
        const Question *q = &qcm->questions[i];
        fprintf(f, "%s\n%d\n%f\n", q->intitule, q->nb_propositions,q->nb_point_question);
        for (j = 0; j < q->nb_propositions; j++)
            fprintf(f, "%s\t%d\n", q->propositions[j], q->bonnes_reponses[j]);
    }

    fclose(f);
    return 1;
}

/* Demande o ou n, retourne 1 pour oui */
static int oui_non(void) {
    char buf[8];
    do {
        lire(buf, sizeof(buf));
        if (buf[0]!='o' && buf[0]!='O' && buf[0]!='n' && buf[0]!='N')
            printf(ROUGE "Repondez par o ou n : " RESET);
    } while (buf[0]!='o' && buf[0]!='O' && buf[0]!='n' && buf[0]!='N');
    return (buf[0] == 'o' || buf[0] == 'O') ? 1 : 0;
}

static void saisir_question(Question *q, int reponses_multiples) {
    int i, index;
    char buf[8];

    printf(CYAN "  Intitule (sans espaces en debut) : " RESET);
    lire(q->intitule, sizeof(q->intitule));

    /* Nombre de propositions */
    do {
        printf(CYAN "  Nombre de propositions (2 a %d) : " RESET, MAX_PROPOSITIONS);
        q->nb_propositions = lire_int();
        if (q->nb_propositions < 2 || q->nb_propositions > MAX_PROPOSITIONS)
            erreur("Entrez un nombre entre 2 et 6.");
    } while (q->nb_propositions < 2 || q->nb_propositions > MAX_PROPOSITIONS);
    
    /* nombre de point par question */
    do {
        printf(CYAN "  Nombre de point pour cette question(0 a %d) : " RESET, MAX_POINT);
        q->nb_point_question = lire_int();
        if (q->nb_point_question < 0 || q->nb_point_question > MAX_POINT)
            erreur("Entrez un nombre entre 0 et 100.");
    } while (q->nb_point_question < 0 || q->nb_point_question > MAX_POINT);
    
    
    
    /* Texte de chaque proposition */
    for (i = 0; i < q->nb_propositions; i++) {
        printf(CYAN "  Proposition %c : " RESET, 'A' + i);
        lire(q->propositions[i], sizeof(q->propositions[i]));
        q->bonnes_reponses[i] = 0;
    }

    q->nb_bonnes_reponses = 0;

    if (reponses_multiples) {
        for (i = 0; i < q->nb_propositions; i++) {
            printf(CYAN "  Proposition %c correcte ? (o/n) : " RESET, 'A' + i);
            if (oui_non()) {
                q->bonnes_reponses[i] = 1;
                q->nb_bonnes_reponses++;
            }
        }
    } else {
        do {
            printf(CYAN "  Bonne reponse (A a %c) : " RESET, 'A' + q->nb_propositions - 1);
            lire(buf, sizeof(buf));
            if (buf[0] >= 'a') buf[0] = buf[0] - 'a' + 'A';
            index = buf[0] - 'A';
            if (index < 0 || index >= q->nb_propositions)
                erreur("Lettre incorrecte.");
        } while (index < 0 || index >= q->nb_propositions);

        q->bonnes_reponses[index] = 1;
        q->nb_bonnes_reponses     = 1;
    }
}

static void creer_qcm(void) {
    QCM qcm;
    int i;

    memset(&qcm, 0, sizeof(QCM));

    printf("\n");
    sep();
    printf(FMAG BLANC GRAS " CREATION D'UN NOUVEAU QCM " RESET "\n");
    sep();

    do {
        printf(JAUNE "Nom du QCM (sans espaces) : " RESET);
        lire(qcm.nom, sizeof(qcm.nom));
        if (qcm.nom[0] == '\0')
            erreur("Nom vide.");
        else if (strchr(qcm.nom, ' ') != NULL)
            erreur("Le nom ne doit pas contenir d espaces.");
    } while (qcm.nom[0] == '\0' || strchr(qcm.nom, ' ') != NULL);

    printf(JAUNE "Points negatifs ? (o/n) : " RESET);
    qcm.points_negatifs = oui_non();

    printf(JAUNE "Plusieurs bonnes reponses par question ? (o/n) : " RESET);
    qcm.reponses_multiples = oui_non();

    printf(JAUNE "Obliger a repondre avant de passer ? (o/n) : " RESET);
    qcm.mode_sequentiel = oui_non();

    do {
        printf(JAUNE "Nombre de questions (1 a %d) : " RESET, MAX_QUESTIONS);
        qcm.nb_questions = lire_int();
        if (qcm.nb_questions < 1 || qcm.nb_questions > MAX_QUESTIONS)
            erreur("Entrez un nombre entre 1 et 50.");
    } while (qcm.nb_questions < 1 || qcm.nb_questions > MAX_QUESTIONS);

    for (i = 0; i < qcm.nb_questions; i++) {
        printf(CYAN "\n-- Question %d/%d --\n" RESET, i + 1, qcm.nb_questions);
        saisir_question(&qcm.questions[i], qcm.reponses_multiples);
    }

    printf("\n");
    if (sauvegarder_qcm(&qcm)) {
        ajouter_index(qcm.nom);
        ok("QCM sauvegarde !");
        printf(VERT "Fichier : qcm/%s.txt\n" RESET, qcm.nom);
    }
    attendre();
}

void menu_enseignant(void) {
    int choix;
    while (1) {
        printf("\n");
        sep();
        printf(FMAG BLANC GRAS " MODE ENSEIGNANT " RESET "\n");
        sep();
        printf(CYAN "  1." RESET " Creer un QCM\n");
        printf(CYAN "  2." RESET " Retour\n");
        sep();
        printf(JAUNE "Choix : " RESET);
        choix = lire_int();
        if (choix == 1) creer_qcm();
        else if (choix == 2) break;
        else erreur("Choisissez 1 ou 2.");
    }
}
