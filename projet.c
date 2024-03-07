#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

char getch()
{
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

// Structure pour stocker les informations de connexion
typedef struct
{
    char login[MAX_USERNAME_LENGTH];
    char motDePasse[MAX_PASSWORD_LENGTH];
} Identifiants;

// Fonction pour vérifier les identifiants de connexion
int verifierIdentifiants(Identifiants *identifiants, int nombreIdentifiants, char *login, char *motDePasse)
{
    for (int i = 0; i < nombreIdentifiants; i++)
    {
        if (strcmp(identifiants[i].login, login) == 0 && strcmp(identifiants[i].motDePasse, motDePasse) == 0)
        {
            return 1; // Identifiants valides
        }
    }
    return 0; // Identifiants invalides
}

// Structure pour représenter un étudiant
struct Etudiant
{
    int id;
    char nom[50];
    char prenom[50];
    char matricule[20];
};

// Fonction pour ajouter un étudiant au fichier
void ajouterEtudiant(FILE *fichier, struct Etudiant etudiant)
{
    fprintf(fichier, "%d %s %s %s\n", etudiant.id, etudiant.nom, etudiant.prenom, etudiant.matricule);
}

// Fonction pour marquer la présence d'un étudiant
void marquerPresence(FILE *fichierPresence, int id, char nomEtudiant[50], char prenomEtudiant[50])
{
    time_t t;
    time(&t);
    fprintf(fichierPresence, "%d %s %s %s", id, nomEtudiant, prenomEtudiant, ctime(&t));
}

int main()
{
    // Création des fichiers pour stocker les identifiants
    FILE *fichierAdmin = fopen("admin.txt", "r");
    FILE *fichierEtudiant = fopen("etudiant.txt", "r");

    if (fichierAdmin == NULL || fichierEtudiant == NULL)
    {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return 1;
    }

    // Lecture des identifiants de l'admin
    Identifiants identifiantsAdmin;
    fscanf(fichierAdmin, "%s %s", identifiantsAdmin.login, identifiantsAdmin.motDePasse);
    fclose(fichierAdmin);

    // Lecture des identifiants de l'étudiant
    Identifiants identifiantsEtudiant;
    fscanf(fichierEtudiant, "%s %s", identifiantsEtudiant.login, identifiantsEtudiant.motDePasse);
    fclose(fichierEtudiant);

    // Variables pour l'authentification
    char saisieLogin[MAX_USERNAME_LENGTH];
    char saisieMotDePasse[MAX_PASSWORD_LENGTH];
    char c;
    int i = 0;

    // Authentification
    do
    {
        printf("Connexion :\n");

        do
        {
            printf("Entrer votre username : ");
            fgets(saisieLogin, MAX_USERNAME_LENGTH, stdin);
            saisieLogin[strcspn(saisieLogin, "\n")] = 0; // Supprimer le retour à la ligne
        } while (strcmp(saisieLogin, "") == 0);          // Demander à nouveau si le login est vide

        printf("Entrer votre mot de passe : ");

        i = 0;
        while (i < MAX_PASSWORD_LENGTH - 1 && (c = getch()) != '\n')
        {
            if (c == 127)
            { // ASCII value for backspace
                if (i > 0)
                {
                    printf("\b \b"); // Effacer le caractère précédent
                    i--;
                }
            }
            else
            {
                saisieMotDePasse[i++] = c;
                printf("*");
            }
        }

        saisieMotDePasse[i] = '\0';

        // Vérification des identifiants
        if (verifierIdentifiants(&identifiantsAdmin, 1, saisieLogin, saisieMotDePasse))
        {
            printf("\nAuthentification réussie en tant qu'admin.\n");
            // Menu pour l'admin
            int choix;
            do
            {
                printf("\nMenu Admin :\n");
                printf("1. Gestion des étudiants\n");
                printf("2. Gestion de fichier\n");
                printf("3. Marquer les présences\n");
                printf("4. Envoyer un message\n");
                printf("5. Paramètre\n");
                printf("6. Quitter\n");
                printf("Choix : ");
                scanf("%d", &choix);
                int sousChoix;
                do
                {
                    if (choix == 3)
                    {
                        // Afficher le menu principal
                        printf("\t \t \t----------------------------------------------------------------------------\n");
                        printf("\t \t \t1-------------------> Marquer présence <------------------------------------\n");
                        printf("\t \t \t2. Liste présence\n");
                        printf("\t \t \t0. Quitter\n");
                        printf("\t \tChoix : ");
                        scanf("%d", &sousChoix);
                        if (sousChoix > 3 || sousChoix < 0)
                        {
                            printf("Choix invalide. Veuillez réessayer.\n");
                        }

                        // Marquer la présence d'un étudiant
                        int matricule;
                        printf("Matricule de l'etudiant : ");
                        scanf("%d", &matricule);

                                            // Ouvrir le fichier des étudiants
                        FILE *fichierEtudiants = fopen("etudiants.txt", "a+");
                        if (fichierEtudiants == NULL)
                        {
                            perror("Erreur lors de l'ouverture du fichier des étudiants");
                            exit(EXIT_FAILURE);
                        }

                        // Ouvrir le fichier des présences
                        FILE *fichierPresence = fopen("presence.txt", "a+");
                        if (fichierPresence == NULL)
                        {
                            perror("Erreur lors de l'ouverture du fichier des présences");
                            exit(EXIT_FAILURE);
                        }

                        // Rechercher l'étudiant dans le fichier
                        rewind(fichierEtudiants);
                        struct Etudiant etudiant;
                        int etudiantTrouve = 0;

                        while (fscanf(fichierEtudiants, "%d %s %s %s", &etudiant.id, etudiant.nom, etudiant.prenom, etudiant.matricule) == 4)
                        {
                            if (etudiant.id == matricule)
                            {
                                etudiantTrouve = 1;
                                printf("%s %s (Matricule : %s) est present.\n", etudiant.prenom, etudiant.nom, etudiant.matricule);

                                // Marquer la présence dans le fichier
                                marquerPresence(fichierPresence, etudiant.id, etudiant.nom, etudiant.prenom);
                                break;
                            }
                        }

                        if (!etudiantTrouve)
                        {
                            printf("Le matricule %d n'existe pas.\n", matricule);
                        }

                        // Demander à l'admin de taper Q pour revenir au menu principal
                        char reponse;
                        printf("Taper Q pour revenir au menu principal : ");
                        scanf(" %c", &reponse);

                        if (reponse == 'Q' || reponse == 'q')
                        {
                            // Demander le mot de passe pour revenir au menu principal
                            char mdp[50];
                            printf("Saisir le mot de passe : ");
                            scanf("%s", mdp);
                            // Vérifier le mot de passe pour revenir au menu principal

                            // if (strcmp(mdp, adminPassword) != 0)
                            // {
                            //     printf("Mot de passe incorrect. Fin du programme.\n");
                            //     exit(EXIT_SUCCESS);
                            // }
                            
                        }
                    }

                } while (sousChoix > 3 || sousChoix < 0);

            } while (choix != 6);
        }
        else if (verifierIdentifiants(&identifiantsEtudiant, 1, saisieLogin, saisieMotDePasse))
        {
            printf("\nAuthentification réussie en tant qu'étudiant.\n");
            // Menu pour l'étudiant
            int choix;
            do
            {
                printf("\nMenu Étudiant :\n");
                printf("1. Marquer ma présence\n");
                printf("2. Nombre de minutes d'absences\n");
                printf("3. Mes messages\n");
                printf("4. Quitter\n");
                printf("Choix : ");
                scanf("%d", &choix);

                // Ajoutez ici les fonctionnalités correspondantes aux choix du menu étudiant

            } while (choix != 4);
        }
        else
        {
            printf("\nIdentifiants invalides. Veuillez réessayer.\n");
        }
    } while (1);

    return 0;
}
