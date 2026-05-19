# Questions pour un Tekien - Gestionnaire de QCM
CY Tech - preING1 - 2025-2026

## Compilation et lancement


make
./qcm_tekien


## Utilisation

### Menu principal
`1` Mode Etudiant  : passer un QCM
`2` Mode Enseignant : creer un QCM (mot de passe : `cytech2026`)
`3` Quitter

### Mode Etudiant
Choisir un QCM dans la liste
Repondre par lettre (A, B, C...) ou par o/n selon le type de QCM
Taper `p` pour passer une question (si le mode sequentiel est desactive)
La note sur 20 s affiche avec la corection a la fin

### Mode Enseignant
Mot de passe : `cytech2025`
Donner un nom au QCM (les espaces sont remplacés par des _)
Choisir les parametres : points negatifs, reponses multiple, mode sequentiel
Saisir les questions et les proposition
Le QCM est sauvegarde automatiquement dans `qcm/<nom>.txt`
Son nom est ajoute dans `qcm/index.txt` pour qu il apparaisse dans la liste

## Structure des fichiers


qcm_project/
─ Makefile
─ README.md
  ─ src/
        main.c          -> menu principal
        utils.h / .c    -> constantes, structures, fonctions partagees
        enseignant.h / .c -> creation de QCM
        etudiant.h / .c   -> passation de QCM
  ─ qcm/
        index.txt                  -> liste des QCM disponibles
        Informatique_Generale.txt
        Mathematiques_Niveau1.txt
        Langage_C_Bases.txt


## QCM fourni

| QCM                      | Pts negatifs | Reps multiples | Sequentiel |
|--------------------------|:------------:|:--------------:|:----------:|
| Informatique_Generale    | Non          | Non            | Non        |
| Mathematiques_Niveau1    | Oui          | Oui            | Non        |
| Langage_C_Bases          | Non          | Non            | Oui        |
