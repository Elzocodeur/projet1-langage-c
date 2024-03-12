#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>



#define MAX_TAILLE_ETUDIANT 50
#define MAX_TAILLE_UTILISATEUR 50
#define MAX_TAILLE_PRESENCE 50
#define MAX_TAILLE_MESSAGE 100



typedef struct
{
    int j, m, a;
} Date;
typedef struct
{
    int h, m, s;
} Heure;



typedef struct
{
    int id;
    char nom[50];
    char prenom[50];
    char classe[20];
    char codeEtud[20];
} Etudiant;



typedef struct
{
    int codeEtud;
    Date date;
    Heure heure;
} Presence;



typedef struct
{
    int id;
    int estAdmin;
    char login[10];
    char password[10];
} Utilisateur;



typedef struct
{
    int id;
    char contenue[200];
    Etudiant dest;
    Date date;
    Heure heure;
    int status;
} Message;



// Déclaration de variable global pour stocker la liste des étudiants et uilisateurs
Etudiant LIST_ETUDIANTS[MAX_TAILLE_ETUDIANT];
Utilisateur LIST_UTILISATEUR[MAX_TAILLE_UTILISATEUR];
Presence LIST_PRESENCE[MAX_TAILLE_PRESENCE];
Message LIST_MESSAGES[MAX_TAILLE_MESSAGE];



// Déclaration des variable global pour stocker la taille des listes des étudiants
int tailleEtud = 0;
int tailleUtil = 0;
int taillePres = 0;
int tailleMsgs = 0;



// Fonction qui permet de lire la saisie d'un utilisateur
void saissir(char msg[], char *var)
{
    char c;
    int i;
    while (1)
    {
        memset(var, '\0', sizeof(var));
        printf("%s: ", msg);
        i = 0;
        c = getchar();
        while (c != '\n')
        {
            var[i++] = c;
            c = getchar();
        }
        if (i != 0)
            break;
    }
}



// Fonction qui permet de saissir un mot de passe
void saissirMDP(char *password)
{
    struct termios old_attr, new_attr;
    int i = 0, ch;

    tcgetattr(STDIN_FILENO, &old_attr);
    new_attr = old_attr;
    new_attr.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);

    printf("Enter votre mot de passe: ");
    while ((ch = getchar()) != '\n')
    {
        if (ch == 127 || ch == 8)
        { // Touche de retour arrière (backspace)
            if (i > 0)
            {
                i--;
                password[i] = '\0';
                printf("\b \b"); // Efface le dernier caractère affiché
            }
        }
        else if (ch != '\n')
        {
            password[i++] = ch;
            printf("*");
            fflush(stdout);
        }
        else
        {
            break;
        }
    }
    password[i] = '\0';

    puts("");

    new_attr.c_lflag |= (ECHO | ECHOE | ECHOK | ECHONL | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
}





// Fonction pour obtenir la date actuel
Date dateActuel()
{
    Date date;
    time_t dt = time(0);
    struct tm *dtA = localtime(&dt);

    date.j = dtA->tm_mday;
    date.m = dtA->tm_mon + 1;
    date.a = dtA->tm_year + 1900;

    return date;
}





// Fonction pour obtenir l'heure actuel
Heure heureActuel()
{
    Heure heure;
    time_t dt = time(0);
    struct tm *dtA = localtime(&dt);

    heure.h = dtA->tm_hour;
    heure.m = dtA->tm_min;
    heure.s = dtA->tm_sec;

    return heure;
}




// Fonction qui compare deux date
int compareDate(Date dt1, Date dt2)
{
    if (dt1.a == dt2.a && dt1.m == dt2.m && dt1.j == dt2.j)
        return 1;
    return 0;
}



// Fonction qui permet de charger la liste des étudaints
void chargerListEtudiant()
{
    FILE *fichier = fopen("etudiants.txt", "r");
    if (fichier == NULL)
        fichier = fopen("etudiant.txt", "w");
    char ligne[100];
    Etudiant etud;

    while (fscanf(fichier, "%d %s %s %s %s", &etud.id, etud.prenom, etud.nom, etud.classe, etud.codeEtud) != EOF)
    {
        LIST_ETUDIANTS[tailleEtud++] = etud;
    }

    fclose(fichier); // on ferme le fichier
}




// Fonction qui permet de charger la liste des utilisateurs
void chargerListUtilisateur()
{
    FILE *fichier = fopen("utilisateurs.txt", "r");
    if (fichier == NULL)
        fichier = fopen("utilisateurs.txt", "w");
    char ligne[100];
    Utilisateur util;
    while (fscanf(fichier, "%d %d %s %s", &util.id, &util.estAdmin, util.login, util.password) != EOF)
    {
        LIST_UTILISATEUR[tailleUtil++] = util;
    }

    fclose(fichier); // on ferme le fichier
}

// Fonction qui permet de charger la liste des presences
void chargerListPresence()
{
    FILE *ficher = fopen("presence.txt", "r");
    if (ficher == NULL)
        ficher = fopen("presence.txt", "w");
    char ligne[100];
    Presence pres;
    taillePres = 0; // On initialise la taille du tableau à 0
    while (
        fscanf(ficher, "%d %d/%d/%d %d:%d:%d", &pres.codeEtud, &pres.date.j, &pres.date.m, &pres.date.a, &pres.heure.h, &pres.heure.m, &pres.heure.s) != EOF)
    {
        // On stocke les données récupéredant le fichier
        LIST_PRESENCE[taillePres++] = pres;
    }

    fclose(ficher); // On ferme le fichier
}

// Fonction qui permet de charger la liste des messages envoyer
void chargerListeMessage()
{
    FILE *fichier = fopen("message.bin", "rb");
    if (fichier == NULL)
        fichier = fopen("message.bin", "ab");

    Message msg;
    while (fread(&msg, sizeof(Message), 1, fichier))
    {
        LIST_MESSAGES[tailleMsgs++] = msg;
    }
    fclose(fichier); // On ferme le fichier
}

// Fonction pour vérifie si quelqu'un est marquer présent ou pas
int estMarquerPresent(int codeEtud)
{

    Presence pres;
    for (int i = 0; i < taillePres; i++)
    {
        pres = LIST_PRESENCE[i];
        if (pres.codeEtud == codeEtud && compareDate(pres.date, dateActuel()))
            return 1;
    }
    return 0;
}

// Fonction pour marquer les présences
void marquerPresent(int codeEtud)
{
    chargerListPresence(); // On charge la liste des présences
    if (estMarquerPresent(codeEtud))
    {
        printf("\nL'étudiant avec l'id %d est déja marquer présent.\n", codeEtud);
    }
    else
    {
        Presence pres;
        pres.codeEtud = codeEtud;
        pres.date = dateActuel();
        pres.heure = heureActuel();

        FILE *fichier = fopen("presence.txt", "a");
        fprintf(fichier, "%d %d/%d/%d %d:%d:%d\n", pres.codeEtud, pres.date.j, pres.date.m, pres.date.a, pres.heure.h, pres.heure.m, pres.heure.s);
        fclose(fichier);
        LIST_PRESENCE[taillePres++] = pres;

        printf("\n✅ Code Valide, présence à %02d:%02d:%02d\n", pres.heure.h, pres.heure.m, pres.heure.s);
    }
}

// Fonction qui permet de récupérer la connexion d'un utilisateur
Utilisateur connexion()
{
    Utilisateur util;
    char c;
    int i;
    // struct termios term, oldterm;
    while (1)
    {

        // On récupére le login de l'utilisateur
        saissir("Entrer votre login", util.login);

        memset(util.password, '\0', sizeof(util.password));

        // On récupére le mot de passe de l'utilisateur
        saissirMDP(util.password);

        // On vérifie si le login est bon
        for (i = 0; i < tailleUtil; i++)
        {
            if (
                strcmp(LIST_UTILISATEUR[i].login, util.login) == 0 &&
                strcmp(LIST_UTILISATEUR[i].password, util.password) == 0)
                return LIST_UTILISATEUR[i];
        }
        puts("\n❌ Le login ou le mot de passe n'est pas correcte.\n");
    }
}

// Fonction pour afficher le menu de l'admin
int menuAdmin()
{
    int choix;

    do
    {
        printf("\n--------------------------------------------------------\n");
        printf("\t\t\t MENU DE L'ADMINISTRATEUR:\n");
        printf("--------------------------------------------------------\n");
        printf("1. --->GESTION DES ETUDIANTS\n");
        printf("2. --->GENERATION DE FICHIERS\n");
        printf("3. --->MARQUER PRESENCES\n");
        printf("4. --->ENVOYER MESSAGE\n");
        printf("5. --->LISTE DES ABSENCES PAR DATE\n");
        printf("5. --->PARAMETRES\n");
        printf("6. --->DECONNEXION\n");
        printf("\n--- Entrez votre choix : ");
        // S'il saisie des caractères alpha numérique
        if (scanf("%d", &choix) != 1)
            while (getchar() != '\n')
                ;        // On libére le tampon
        system("clear"); // On efface le terminale
    } while (choix > 5 || choix < 1);

    return choix;
}

// Fonction pour afficher le menue de l'étudiant
int menuEtudiant(int nb)
{
    int choix;
    do
    {

        printf("\n---------------------------------------------------------\n");
        printf("\t\t\t Menu de l'apprenant :\n");
        printf("---------------------------------------------------------\n");
        printf("1. --->Marque ma présence\n");
        printf("2. --->Nombre de minutes d'absence\n");
        printf("3. --->Mes messages(%d)\n", nb);
        printf("4. --->Déconnexion\n");
        printf("\n--- Entrez votre choix : ");

        // S'il saisie des caractères alpha numérique
        if (scanf("%d", &choix) != 1)
            while (getchar() != '\n')
                ;        // On libére le tampon
        system("clear"); // On efface le terminale
    } while (choix > 4 || choix < 1);
    return choix;
}

// Fonction qui permet de générer des fichier
void genererFichier(char file[], int mode, Date date)
{
    chargerListPresence();
    char heure[9];
    int i, maxL[4] = {0}, max = 0, idEtuds[taillePres], size = 0;
    maxL[3] = 8;

    Etudiant etud;

    // On récupere l'id des étudiants qui sont présent dans la date donner
    for (i = 0; i < taillePres; i++)
    {
        if (compareDate(LIST_PRESENCE[i].date, date))
        {
            idEtuds[size++] = LIST_PRESENCE[i].codeEtud;
        }
    }

    if (size == 0)
    {
        printf("\nIl n'y a pas de présence pour cette date\n");
    }
    else
    {

        // On parcourt le prenom, nom et classe des étudiants pour vérifieer la chaine la plus longue
        for (i = 0; i < size; i++)
        {
            etud = LIST_ETUDIANTS[idEtuds[i] - 1]; // On récupére l'étudiant avec l'id donner
            maxL[0] = (strlen(etud.prenom) > maxL[0]) ? strlen(etud.prenom) : maxL[0];
            maxL[1] = (strlen(etud.nom) > maxL[1]) ? strlen(etud.nom) : maxL[1];
            maxL[2] = (strlen(etud.classe) > maxL[2]) ? strlen(etud.classe) : maxL[2];
        }

        // On récupére la plus longue chaine de caractère à afficher
        for (i = 0; i < 4; i++)
            if (maxL[i] > max)
                max = maxL[i];

        // On récupére la larguer du tableau a afficher
        int taille = (max * 4) + 13;
        // On déclare une chaine de caractère nommée étoile
        char etoile[taille + 1];
        // On préremplie le tableau étoile par des caractères de fin de chaine
        memset(etoile, ' ', taille);
        // On remplie le tableau étoile par des étoiles
        for (i = 0; i < taille; i++)
            etoile[i] = '*';
        // On marque la fin du tableau de chaine de caractère
        etoile[taille] = '\00';

        FILE *fichier = fopen(file, mode ? "a" : "w"); // On ouvre le fichier a remplir
        fprintf(fichier, "\nLa liste des présences pour la date %02d/%02d/%d est:\n", date.j, date.m, date.a);
        // On affiche les en têt du tableau
        fprintf(fichier, "\n| %-*s | %-*s | %-*s | %-*s |\n", max, "Prenom", max, "Nom", max, "Classe", max, "Code");
        fprintf(fichier, "%s", etoile); // On affiche les étoiles

        for (i = 0; i < size; i++)
        {
            etud = LIST_ETUDIANTS[idEtuds[i] - 1];
            fprintf(fichier, "\n| %-*s | %-*s | %-*s | %-*s |", max, etud.prenom, max, etud.nom, max, etud.classe, max, etud.codeEtud);
        }
        fprintf(fichier, "\n%s\n", etoile);
        fclose(fichier);
    }
}

// Fonction qui permet de vérifier si une date est valide
int dateEstValide(Date date)
{
    int jour = date.j, mois = date.m, annee = date.a;

    if (mois < 1 || mois > 12 || annee < 1 || jour < 1)
        return 0;
    if (mois == 2)
    {
        if (annee % 400 == 0 || (annee % 4 == 0 && annee % 100 != 0))
        {
            return jour > 29 ? 0 : 1;
        }
        else
        {
            return jour > 28 ? 0 : 1;
        }
    }
    else if (mois == 6 || mois == 4 || mois == 9 || mois == 11)
        return jour > 30 ? 0 : 1;
    else
        return jour > 31 ? 0 : 1;
}

//  Fonction qui permet d'evoyer une message
void envoyerMessage(int mode)
{
    Message msg;
    Etudiant etud;
    char LIST_CLASS[3][10] = {"Dev-Web", "Ref-Dig", "Data"};
    chargerListeMessage(); // On charge la liste des messages
    msg.status = 1;
    msg.date = dateActuel();
    msg.heure = heureActuel();
    getchar(); // On liber le tampon
    saissir("Entrer votre message", msg.contenue);

    FILE *fichier = fopen("message.bin", "a");
    // On récupére l'id du dernier message
    int id = tailleMsgs == 0 ? 1 : LIST_MESSAGES[tailleMsgs].id + 1;

    if (mode == 0)
    { // Envoyer message à tout le monde
        int test = tailleEtud;
        int test1;
        for (int i = 0; i < tailleEtud; i++)
        {
            test1 = tailleMsgs;
            msg.dest = LIST_ETUDIANTS[i];
            msg.id = id + i;
            fwrite(&msg, sizeof(Message), 1, fichier);
            LIST_MESSAGES[tailleMsgs++] = msg;
        }

        puts("\nLe message a été envoyer à tout le monde.\n");
        puts("Tapez entrer pour continuer...");
        getchar();
    }
    else if (mode == 1)
    { // Envoyer message à tout une classe
        char classe[10];
        int quitter = 1;
        while (quitter)
        {
            saissir("Entrer la classe", classe);
            for (int i = 0; i < 3; i++)
            {
                if (strcmp(LIST_CLASS[i], classe) == 0)
                {
                    quitter = 0;
                    break;
                }
            }
            if (quitter)
            {
                system("clear");
                puts("❌ La classe que vous avez donner n'est valide");
            }
        }

        // On envoie le message a tout les étudiants d'une classe donner
        for (int i = 0, j = 0; i < tailleEtud; i++)
        {
            if (strcmp(LIST_ETUDIANTS[i].classe, classe) == 0)
            {
                msg.id = id + j++;
                msg.dest = LIST_ETUDIANTS[i];
                fwrite(&msg, sizeof(Message), 1, fichier);
                LIST_MESSAGES[tailleMsgs++] = msg;
            }
        }

        printf("\nLe message a été envoyer à tout les apprennants de la classe: %s\n", classe);
        printf("Taper entrer pour continuer.\n");
        getchar();
    }
    else
    { // Pour tout les autre valeur différent de 0 et 1 on envoie le message à des étudiants dont leur code est saisie
        char codes[tailleEtud][12];
        int nbC = 0, trouver, i, j;
        char nm;
        // On préremplie le tableau code de caractère de fin de chaine
        memset(codes, '\0', sizeof(codes));
        while (1)
        {
            i = nbC = 0;
            printf("Entrer le(s) code(s): ");
            nm = getchar();
            while (nm != '\n')
            {
                if (nm == ',')
                {
                    nbC++;
                    i = 0;
                }
                else
                {
                    if (nm != ' ')
                    {
                        codes[nbC][i++] = nm;
                    }
                }
                nm = getchar();
            }
            if (i != 0)
                break;
        }

        // On envoie le message a tout les étudaints dont leur code a été spécifier
        for (i = 0; i <= nbC; i++)
        {
            trouver = 0;
            for (j = 0; j < tailleEtud; j++)
            {
                if (strcmp(codes[i], LIST_ETUDIANTS[j].codeEtud) == 0)
                {
                    msg.dest = LIST_ETUDIANTS[j];
                    trouver = 1;
                    break; // Si on trouve le l'étudiant correspondant on l'envoie le message
                }
            }

            // On vérifie si le code donner est le code d'un des étudiants
            if (trouver)
            { // Si c'est le cas on lui envoi un message
                msg.id = id + i;
                fwrite(&msg, sizeof(Message), 1, fichier);
                printf("\n✅ Code Valide: %s !! Message envoyer\n", codes[i]);
            }
            else
            { // Sinon on affiche que le code n'est pas valide
                printf("\n❌ Code Invalide: %s !!! Message non envoyer.\n", codes[i]);
            }
        }
        puts("Tapez entrer pour continuer...");
        getchar();
    }

    fclose(fichier); // On ferme le fichier
}

// On récupere le nombre de message d'un utilisateur
int nombreDeMessage(int id)
{
    int nb = 0;
    // Si on n'a pas encore charger les messages, on le charge
    if (tailleMsgs == 0)
        chargerListeMessage();

    Message msg;

    for (int i = 0; i < tailleMsgs; i++)
    {
        msg = LIST_MESSAGES[i];
        if (msg.dest.id == id && msg.status == 1)
            nb++;
    }

    return nb;
}

// Fonction qui permet de lire une message
void lireMessage(int id)
{
    chargerListeMessage(); // On charge la liste des message
    Message msg;
    for (int i = 0, j = 1; i < tailleMsgs; i++)
    {
        msg = LIST_MESSAGES[i];
        if (msg.dest.id == id && msg.status == 1)
        {
            printf("\n\tMESSAGE N°%d \n", j);
            printf("Destinataire: %s %s classe: %s\n", msg.dest.prenom, msg.dest.nom, msg.dest.classe);
            printf("Expéditeur: Administration.\n");
            printf("Contenu: %s\n\n", msg.contenue);
            j++;
            LIST_MESSAGES[i].status = 0;
        }
    }

    // On met à jour les messages pour tenir en conte les messages lue
    FILE *fichier = fopen("message.bin", "wb");
    for (int i = 0; i < tailleMsgs; i++)
    {
        if (LIST_MESSAGES[i].status)
        {
            fwrite(&LIST_MESSAGES[i], sizeof(Message), 1, fichier);
        }
    }
    fclose(fichier);

    printf("Taper entrer pour continuer.\n");
    getchar();
}


// -----------------------------------------------------nombre de minutes d'absence------------------------------------------------------

// Fonction pour afficher le nombre de minutes d'absence

// Fonction pour calculer le nombre total de minutes d'absence d'un étudiant
// Fonction pour calculer le nombre total de minutes d'absence d'un étudiant
int calculerMinutesAbsence(int idUtilisateur) {
    int debutCoursHeure = 8; // Heure de début des cours
    int totalMinutesAbsence = 0;

    // Obtention de l'heure actuelle du système
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int heureActuelle = tm.tm_hour;
    int minuteActuelle = tm.tm_min;

    // Parcours de la liste des présences
    for (int i = 0; i < taillePres; i++) {
        Presence pres = LIST_PRESENCE[i];
        // Vérification si l'étudiant est concerné et si la date n'est pas celle d'aujourd'hui
        if (pres.codeEtud == idUtilisateur && !compareDate(pres.date, dateActuel())) {
            if (pres.heure.h >= debutCoursHeure) {
                // Calcul de la différence totale en minutes
                totalMinutesAbsence += (pres.heure.h - debutCoursHeure) * 60 + (pres.heure.m - 0);
            }
        }
    }

    // Si l'étudiant n'a pas été marqué présent, calculer l'absence jusqu'à l'heure actuelle
    if (totalMinutesAbsence == 0 && heureActuelle >= debutCoursHeure) {
        totalMinutesAbsence = (heureActuelle - debutCoursHeure) * 60 + (minuteActuelle - 0);
    }

    return totalMinutesAbsence;
}



void afficherMinutesAbsence(int idUtilisateur) {
    int minutesAbsence = calculerMinutesAbsence(idUtilisateur);
    printf("Nombre de minutes d'absence : %d minutes\n", minutesAbsence);
    printf("Appuyer sur la touche entrer pour continuer...\n");
    getchar();
}

// Le programme principal
int main()
{
    chargerListEtudiant();    // On charge la liste des étudiants
    chargerListUtilisateur(); // On charge la liste des utilisateurs

    Utilisateur util = connexion();
    int choix;

    if (util.estAdmin)
    {
        Etudiant etud;
        do
        {
            system("clear"); // On efface le terminal
            choix = menuAdmin();

            if (choix == 1)
            {
            }
            else if (choix == 2)
            {
                system("clear");
                int ch = 1;
                char fichier[30] = "listDesPresences.txt";
                Date date = LIST_PRESENCE[0].date;

                // On génére la liste des présence par date
                genererFichier(fichier, 0, date);

                // On parcour la liste des présences pour filtrer les dates et générer la liste
                for (int i = 1; i < taillePres; i++)
                {
                    if (!compareDate(date, LIST_PRESENCE[i].date))
                    {
                        date = LIST_PRESENCE[i].date;
                        genererFichier(fichier, 1, date);
                    }
                }

                while (ch != 2)
                {
                    system("clear");
                    puts("\nGénéreration de fichier de presence : \n");
                    puts("1) Générer fichier des presents.");
                    puts("2) Générer fichier par date.");
                    puts("3) Retour");
                    printf("Votre choix: ");
                    if (scanf("%d", &ch) != 1)
                    {
                        while (getchar() != '\n')
                            ;
                    }
                    else
                    {
                        if (ch == 1)
                        {
                            memset(fichier, '\0', sizeof(fichier));
                            Date date;
                            int i = 0;
                            do
                            {
                                if (i > 1)
                                {
                                    puts("\nLa date n'est pas valide. Recommencez.\n");
                                }
                                i++;

                                printf("Donner une date au format(jj-mm-aaaa): ");

                                if (scanf("%d-%d-%d", &date.j, &date.m, &date.a) != 3)
                                    while (getchar() != '\n')
                                        ;
                            } while (!dateEstValide(date));

                            sprintf(fichier, "listPresence_%02d_%02d_%04d.txt", date.j, date.m, date.a);

                            genererFichier(fichier, 0, date);
                        }
                    }
                }
            }
            else if (choix == 3)
            {
                char code[10], nm;
                int estMarquer, quitter = 1;
                while (quitter)
                {
                    getchar();
                    while (1)
                    {
                        system("clear"); // On efface le terminal
                        puts("------------------------------------------------------");
                        puts(" MARQUER LES PRESENCES ");
                        puts("----------------------------------------------------\n");
                        saissir("Entrer votre code [Q pour quitter]", code);

                        if (strlen(code) == 1 && (code[0] == 'q' || code[0] == 'Q'))
                        {
                            puts("\n\tVeuillez vous conecter !!!\n");
                            saissirMDP(code);
                            if (strcmp(util.password, code) == 0)
                            {
                                quitter = 0;
                                break;
                            }
                        }
                        else
                        {
                            estMarquer = 0;
                            for (int i = 0; i < tailleEtud; i++)
                            {
                                etud = LIST_ETUDIANTS[i];
                                if (strcmp(etud.codeEtud, code) == 0)
                                {
                                    marquerPresent(etud.id);
                                    estMarquer = 1;
                                    break;
                                }
                            }
                            if (!estMarquer)
                            {
                                puts("❌ Code invalide!!!");
                            }

                            puts("Appuyer sur la touche entrer pour continuer...");
                            getchar();
                        }
                    }
                }
            }
            else if (choix == 4)
            {
                int ch;
                do
                {
                    system("clear"); // On efface le terminal
                    putchar('\n');
                    puts("1) Envoyer message à tout le monde.");
                    puts("2) Envoyer message à une classe.");
                    puts("3) Envoyer message à un ou des étudiant(s).");
                    puts("4) Retour.");
                    printf("\nVotre choix: ");
                    if (scanf("%d", &ch) != 1)
                        while (getchar() != '\n')
                            ; // On liber le tampon

                    if (ch == 1)
                    {
                        // On envoie un message à tout le monde
                        envoyerMessage(0);
                    }
                    else if (ch == 2)
                    {
                        // On envoie un message à une classe
                        envoyerMessage(1);
                    }
                    else if (ch == 3)
                    {
                        // On envoie un message à des étudiants.
                        envoyerMessage(2);
                    }
                } while (ch != 4);
            }
            

        } while (choix != 5);
    }
    else
    {
        do
        {
            system("clear");
            choix = menuEtudiant(nombreDeMessage(util.id));
            getchar();
            if (choix == 1)
            {
                marquerPresent(util.id);
                puts("Appuyer sur la touche entrer pour continuer...");
                getchar();
            }
            else if (choix == 2)
            {
                afficherMinutesAbsence(1);
            }
            else if (choix == 3)
            {
                lireMessage(util.id);
            }
        } while (choix != 4);
    }

    return 0;
}
