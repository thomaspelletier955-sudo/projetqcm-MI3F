#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "etudiant.h"
#include "utils.h"

/* Lit la liste des QCM depuis qcm/index.txt
 * Retourne le nombre de QCM trouves */
static int lister_qcm(char noms[MAX_QCM][MAX_NOM]) {
    int  nb = 0;
    FILE *f  = fopen(FICHIER_INDEX, "r");
    if (f == NULL) {
        erreur("Aucun QCM disponible.");
        return 0;
    }
    while (nb < MAX_QCM && fscanf(f, "%63s", noms[nb]) == 1)
        nb++;
    fclose(f);
    return nb;
}

/* Charge un QCM depuis son fichier */
static int charger_qcm(const char *nom, QCM *qcm) {
    int   i, j;
    char  chemin[MAX_NOM * 2];
    char  tampon[MAX_QUESTION + 8];
    char *sep;
    FILE *f;

    snprintf(chemin, sizeof(chemin), "%s%s.txt", DOSSIER_QCM, nom);
    f = fopen(chemin, "r");
    if (f == NULL) { erreur("Fichier introuvable."); return 0; }

    memset(qcm, 0, sizeof(QCM));

    /* Lecture en-tete */
    fscanf(f, " %63[^\n]\n", qcm->nom);
    fscanf(f, "%d %d %d\n", &qcm->points_negatifs,
                             &qcm->reponses_multiples,
                             &qcm->mode_sequentiel);
    fscanf(f, "%d\n", &qcm->nb_questions);

    /* Lecture des questions */
    for (i = 0; i < qcm->nb_questions; i++) {
        Question *q = &qcm->questions[i];
        fscanf(f, " %127[^\n]\n", q->intitule);
        fscanf(f, "%d\n", &q->nb_propositions);
        fscanf(f, "%f\n", &q->nb_point_question);
        for (j = 0; j < q->nb_propositions; j++) {
            fgets(tampon, sizeof(tampon), f);
            tampon[strcspn(tampon, "\n")] = '\0';
            /* La ligne est : texte<TAB>0_ou_1 */
            sep = strrchr(tampon, '\t');
            *sep = '\0';
            strncpy(q->propositions[j], tampon, MAX_QUESTION - 1);
            q->bonnes_reponses[j] = atoi(sep + 1);
            if (q->bonnes_reponses[j]) q->nb_bonnes_reponses++;
        }
    }
    fclose(f);
    return 1;
}

static void passer_qcm(const QCM *qcm) {
    int    i, j;
    int    rep[MAX_QUESTIONS][MAX_PROPOSITIONS];
    int    repondu[MAX_QUESTIONS];
    char   buf[8];
    int    index, coche;
    double pts, note = 0.0;

    memset(rep,     0, sizeof(rep));
    memset(repondu, 0, sizeof(repondu));

    printf("\n");
    sep();
    printf(FBLEU BLANC GRAS " QCM : %s " RESET "\n", qcm->nom);
    printf(BLEU "  Pts negatifs:%s | Reps multiples:%s | Sequentiel:%s\n" RESET,
           qcm->points_negatifs    ? "OUI" : "NON",
           qcm->reponses_multiples ? "OUI" : "NON",
           qcm->mode_sequentiel    ? "OUI" : "NON");
    sep();

    for (i = 0; i < qcm->nb_questions; i++) {
        const Question *q = &qcm->questions[i];

        /* Affichage de la question */
        printf(BLEU GRAS "\nQuestion %d : " RESET GRAS "%s\n" RESET, i+1, q->intitule);
        printf(BLEU GRAS "\nNb point : " RESET GRAS "%f\n" RESET, q->nb_point_question);
        for (j = 0; j < q->nb_propositions; j++)
            printf(CYAN "  [%c] " RESET "%s\n", 'A'+j, q->propositions[j]);

        if (qcm->reponses_multiples) {
            coche = 0;
            for (j = 0; j < q->nb_propositions; j++) {
                do {
                    printf(JAUNE "  %c (o/n) : " RESET, 'A'+j);
                    lire(buf, sizeof(buf));
                } while (buf[0]!='o' && buf[0]!='O' && buf[0]!='n' && buf[0]!='N');
                if (buf[0]=='o' || buf[0]=='O') { rep[i][j]=1; coche++; }
            }
            /* Mode sequentiel : doit repondre */
            if (qcm->mode_sequentiel && coche == 0) {
                erreur("Vous devez choisir au moins une reponse.");
                i--; continue;
            }
            repondu[i] = 1;

        } else {
            while (1) {
                printf(JAUNE "  Reponse (A-%c%s) : " RESET,
                       'A'+q->nb_propositions-1,
                       qcm->mode_sequentiel ? "" : ", p=passer");
                lire(buf, sizeof(buf));

                if ((buf[0]=='p'||buf[0]=='P') && !qcm->mode_sequentiel) {
                    printf(JAUNE "  Passee.\n" RESET); break;
                }
                if (buf[0]>='a') buf[0] = buf[0]-'a'+'A';
                index = buf[0]-'A';
                if (index < 0 || index >= q->nb_propositions) {
                    erreur("Lettre incorrecte."); continue;
                }
                rep[i][index]=1; repondu[i]=1; break;
            }
        }
    }

/* Calcul et affichage de la note */
    pts = (double)NOTE_MAX / qcm->nb_questions;
    printf("\n"); sep();
    printf(FBLEU BLANC GRAS " CORRECTION " RESET "\n"); sep();
    for (i = 0; i < qcm->nb_questions; i++) {
        const Question *q = &qcm->questions[i];
        int correct = 1, mauvaise = 0;
        int nb_correctes = 0;

        if (!qcm->reponses_multiples) {
            /* Mode reponse unique : on cherche l index coche */
            int idx = -1;
            for (j = 0; j < q->nb_propositions; j++)
                if (rep[i][j] == 1) { idx = j; break; }
            if (idx == -1) {
                correct = 0;
            } else if (q->bonnes_reponses[idx]) {
                correct = 1;
                nb_correctes = 1;
            } else {
                correct = 0;
                mauvaise = 1;
            }
        } else {
            /* Mode reponses multiples : comparaison complete */
            for (j = 0; j < q->nb_propositions; j++) {
                if (rep[i][j] != q->bonnes_reponses[j]) {
                    correct = 0;
                    if (rep[i][j]==1 && q->bonnes_reponses[j]==0) mauvaise = 1;
                }
                if (rep[i][j]==1 && q->bonnes_reponses[j]==1) nb_correctes++;
            }
        }

        float gain = (q->nb_bonnes_reponses > 0)
                   ? q->nb_point_question * ((float)nb_correctes / q->nb_bonnes_reponses)
                   : 0.0f;

        float penalite = 0.0f;
        if (qcm->points_negatifs && mauvaise)
            penalite = q->nb_point_question / 2.0f;

        gain -= penalite;
        if (gain < 0.0f) gain = 0.0f;

        printf(BLEU "Q%d : " RESET, i+1);

        if (!repondu[i]) {
            printf(JAUNE "Passee\n" RESET);
        } else if (correct) {
            note += gain;
            printf(VERT "Correcte  (+%.2f pts)\n" RESET, gain);
        } else if (qcm->points_negatifs && mauvaise) {
            note += gain;
            printf(ROUGE "Incorrecte (+%.2f pts, penalite -%.2f pts)\n" RESET,
                   gain + penalite, penalite);
        } else if (nb_correctes > 0) {
            note += gain;
            printf(JAUNE "Partielle (+%.2f / %.0f pts)\n" RESET, gain, q->nb_point_question);
        } else {
            printf(ROUGE "Incorrecte (0 pt)\n" RESET);
        }
    }

    if (note < 0.0) note = 0.0;

    float note_max = 0.0f;
    for (i = 0; i < qcm->nb_questions; i++)
        note_max += qcm->questions[i].nb_point_question;

    float note_sur_20 = (note_max > 0.0f) ? (note / note_max) * NOTE_MAX : 0.0f;

    sep();
    printf(GRAS "  NOTE FINALE : ");
    printf("%s%.2f / %d\n" RESET, note_sur_20 >= 10.0f ? VERT : ROUGE, note_sur_20, NOTE_MAX);
    sep();
    attendre();
}
void menu_etudiant(void) {
    char noms[MAX_QCM][MAX_NOM];
    int  nb, choix, i;
    QCM  qcm;

    printf("\n"); sep();
    printf(FBLEU BLANC GRAS " MODE ETUDIANT " RESET "\n"); sep();

    nb = lister_qcm(noms);
    if (nb == 0) { attendre(); return; }

    printf(JAUNE "  QCM disponibles :\n" RESET);
    for (i = 0; i < nb; i++)
        printf(BLEU "  %d." RESET " %s\n", i+1, noms[i]);
    printf(BLEU "  0." RESET " Retour\n"); sep();

    do {
        printf(JAUNE "Choix (0-%d) : " RESET, nb);
        choix = lire_int();
        if (choix < 0 || choix > nb)
            erreur("Entrez un numero entre 0 et %d.");
    } while (choix < 0 || choix > nb);

    if (choix == 0) return;

    if (charger_qcm(noms[choix-1], &qcm))
        passer_qcm(&qcm);
    else
        attendre();
}
