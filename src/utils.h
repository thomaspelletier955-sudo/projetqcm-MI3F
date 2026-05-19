#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

/* Constantes */
#define MOT_DE_PASSE     "cytech2026"
#define DOSSIER_QCM      "qcm/"
#define FICHIER_INDEX    "qcm/index.txt"
#define MAX_NOM          64
#define MAX_QUESTION     128
#define MAX_PROPOSITIONS 6
#define MAX_QUESTIONS    50
#define MAX_QCM          20
#define NOTE_MAX         20

/* Couleur terminal */
#define RESET  "\033[0m"
#define ROUGE  "\033[31m"
#define VERT   "\033[32m"
#define JAUNE  "\033[33m"
#define BLEU   "\033[34m"
#define CYAN   "\033[36m"
#define BLANC  "\033[37m"
#define GRAS   "\033[1m"
#define FBLEU  "\033[44m"
#define FMAG   "\033[45m"

/* Structure d une question */
typedef struct {
    char intitule[MAX_QUESTION];
    char propositions[MAX_PROPOSITIONS][MAX_QUESTION];
    int  nb_propositions;
    int  bonnes_reponses[MAX_PROPOSITIONS];
    int  nb_bonnes_reponses;
} Question;

/* Structure d un QCM complet */
typedef struct {
    char     nom[MAX_NOM];
    int      nb_questions;
    Question questions[MAX_QUESTIONS];
    int      points_negatifs;
    int      reponses_multiples;
    int      mode_sequentiel;
} QCM;

/* Fonctions utilitaires */
void vider_buffer(void);
void sep(void);
void titre(void);
void erreur(const char *msg);
void ok(const char *msg);
void attendre(void);
/* Li une ligne et enleve le \n final */
void lire(char *buf, int taille);
/* Lit un entier et retourne sa valeur (-1 si erreur) */
int  lire_int(void);

#endif
