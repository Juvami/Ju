#include <stdio.h>
/*librairie standard : opérationes d'entrées et sorties*/
#include <stdlib.h>
/*librairie standard : conversion chaine carac en format numérique, tirages aléatoires, ...*/
#include <time.h>
/*librairie standard : manipulation de dates, temps et intervals*/

struct Word {
    char letter;
};

int selectKeyword();
#define SIZE_MAX 255 // taille max des tableaux de caractère
struct Word answer[SIZE_MAX];
#define FIRST_LINE 0 //première ligne du doc = ligne 0
#define LAST_LINE 834 //doc de 835 lignes, donc 0 à 834 

int measureKeyword();
int size_keyword;

int initGame();
int attempt; //nombre de tentatives utilisées
struct Word riddle[SIZE_MAX];

int initHangman();
char hangman[8][8]; //tableau pour l'affichage graphique

int drawHangman();
int showDejavu();
int showRiddle();

int suggestLetter();
char proposal;

int stockLetter();
struct Word dejavu[SIZE_MAX]; //stockage des lettres déjà proposées
int alphabet; //variable pour parcourir le tableau "déjà-vu", comme size_keyword

int matchLetter();

int takeRegister();
int missing_letter; //comptabilise les lettres non-trouvées

int createAccount();
struct Account {
    char username[30]; //pseudo joueur de 30 carac max
    int score;
};
struct Account player[10]; //tableau pouvant sauvegarder 10 comptes différents
int account_id = 1; //numéro de compte utilisateur //1er compte utilisateur

int retryGame();
int power_button;

int youWin();
int drawDancingPrisoner();
int youLoose();
int drawVulture();
int wasAnwer();


void emptyBuffer() {
//pour vider le tampon de getchar()
    char c;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}

int main() {
    power_button = 1;

    printf("Que voulez-vous faire ?\n");
    printf("1 - Jouer\n");
    printf("2 - Afficher les comptes\n");

    //takePill(); //faire un choix
    //showAccount();
    createAccount();

    while (power_button > 0) {
        selectKeyword(); //récupération du mot à deviner
        measureKeyword(); //calculer sa longueur
        initGame(); //préparer la partie à partir du mot-clé
        initHangman(); //préparer les graphismes
        system("clear"); //effacer la console
        showRiddle(); //affichage initial du mot à deviner
        
        while ((missing_letter > 0) && (attempt < 8)) {
            suggestLetter(); //gestion du caractère
            system("clear"); //efface console avant l'actualisation
            drawHangman(); //affiche le pendu
            showRiddle(); //actualise l'état de la devinette
            showDejavu(); //affiche les lettres déjà dites
            takeRegister(); //vérifie s'il reste des lettres à trouver
        }

        if (missing_letter <= 0) { //sortie while car victoire (plus de lettres à trouver)
            youWin();
        }
        else { //sinon sortie while forcément pour game over
            youLoose();
        }
        retryGame();
    }
}

/*Initialisation de la partie*/
int selectKeyword() {
//pour récupérer mot-clé dans fichier
    char c;
    int i; int j;
    int alea_jacta_est;

    srand(time(NULL));
    alea_jacta_est = (rand() % (LAST_LINE - FIRST_LINE + 1)) + FIRST_LINE;


    FILE* fichier = NULL; //pointeur = objet qui contient adresse-mémoire d'une donnée ou fonction

    fichier = fopen("words.txt", "r"); //ouvrir le fichier en mode "lecture seule"
 
    if (fichier != NULL)
    {
        for (i = 0; i <= alea_jacta_est; i++) { //répéter boucle X fois -> seul le dernier mot lu sera conservé
            
            for (j = 0; j <= SIZE_MAX; j++) { //écraser le mot précédent 
                answer[j].letter = '\0';
            }

            j = 0; //repartir depuis le début du tableau
            
            do
            {
                c = fgetc(fichier); // lecture caractère par caractère
                if (c != '\n') { //stockage carac par carac, mais sans stocker le carac de retour à la ligne
                    answer[j].letter = c;
                }
                j++;
            } while (c != '\n');
        }
        fclose(fichier); //fermer le fichier
    }
    return(0);
}
int measureKeyword() { 
//pour mesurer la taille du mot clé     --> naviguer dans tableaux
    int i = 0;
    size_keyword = 0;

    while (answer[i].letter != '\0') {
        size_keyword = size_keyword + 1;
        i++;
    }
}
int initGame() {
//pour initialiser la partie : assigner lettres dans tab, attribuer valeurs départ à variabels de suivi
    attempt = 0; //aucune tentative utilisée
    alphabet = 0; //aucune lettre "déjà vu"
    missing_letter = size_keyword; //mot complet à trouver
    
    int i;

    for (i = 0; i <= size_keyword; i++) {
        if ((answer[i].letter >= 'a') && (answer[i].letter <= 'z')) { //si solution = lettre, devinette affiche tiret
            riddle[i].letter = '_';
        }
        else { //si réponse comporte symboles, espaces ou chiffres = caractères lisibles --> faire deviner des phrases
            riddle[i].letter = answer[i].letter; 
        }
    }
}


/*Boucle de jeu*/
int suggestLetter() {
//pour récupérer et vérifier carac en entrée
    printf("Propose une lettre\n");
    
    proposal = getchar(); //récupère le caractère donné par le joueur
    emptyBuffer(); //vide le tampon

    char maj;
    if ((proposal >= 'A') && (proposal <= 'Z')) { //conversion les majuscules en minuscule
        maj = proposal;
        proposal = maj + 32;
    }
    
    if ((proposal >= 'a') && (proposal <= 'z')) { //si c'est une lettre, demande à stockLetter de la comparer
        stockLetter();
    }
    else { //sinon redemande une proposition
        printf("J'avais demandé une lettre...\n");
        suggestLetter(); 
    }
}
int stockLetter() {
//pour comparer avec les lettres déjà dites et stocker les nouvelles
    int i;

    for (i = 0; i <= alphabet; i++) { //vérifie si la proposition est déjà en mémoire
        if (proposal == dejavu[i].letter) {
            printf("Tu l'as déjà dit\n");
            suggestLetter();
            return(0); //pour sortir fct sans modifier tableau (si lettre déjà dite). Valeur pas importante.
        }
    }
    alphabet = alphabet + 1; /*au prochain appel de stockLetter(), regardes le tableau dejavu[i] une case plus loin*/
    dejavu[alphabet].letter = proposal; /*remplis cette nouvelle case avec la lettre proposée par le joueur sur cette manche*/
    
    matchLetter(); /*si lettre n'est pas dans tableau, utilise-la pour jouer*/
}
int matchLetter() {
//pour détecter concordance
    int i; int change;
    change = 0; //variable de suivi des modifications

    for (i = 0; i <= size_keyword; i++) { /*si concordance, fais apparaitre la lettre et prend note de la modification*/
        if (proposal == answer[i].letter) {
            riddle[i].letter = proposal;
            change = change + 1;
        }
    }

    if (change <= 0) { //si mauvaise proposition, joueur perd une tentative
        attempt = attempt + 1;
    }
}
int takeRegister() {
//pour vérifier s'il reste des lettres à trouver
    int i;
    missing_letter = 0; //ré-initialise missing_letter à chaque entrée dans fonction

    for (i = 0; i <= size_keyword; i++) {//parcours le tableau et comptabilises les lettres non-trouvées
        if (riddle[i].letter == '_') {
            missing_letter = missing_letter + 1;
        }
    }
}


/*Fonctions d'affichage*/
int initHangman() {
//pour initialiser l'affichage du pendu
    int i; int j;

    i = 0;
    for (j = 0; j < 8; j++) {
        if ((j < 2) || (j >= 7)) {
            hangman[i][j] = ' '; //carac "espace"
        }
        if ((j >= 2) && (j < 7)) {
            hangman[i][j] = '_'; //underscore
        }
    }

    i = 1; 
    for (j = 0; j < 8; j++) {
        if ((j == 2) || (j == 6)) {
            hangman[i][j] = '|';
        }
        else {
            hangman[i][j] = ' '; //espace
        }
    }

    i = 2; 
    for (j = 0; j < 8; j++) {
        if ((j == 0) || (j == 1)) {
            hangman[i][j] = ' ';
        }
        if (j == 2) {
            hangman[i][j] = '|';
        }
        if ((j == 3) || (j == 4)) {
            hangman[i][j] = ' ';
        }
        if (j == 5) {
            hangman[i][j] = '(';
        }
        if (j == 6) {
            hangman[i][j] = '-'; //tiret
        }
        if (j == 7) {
            hangman[i][j] = ')';
        }
    }

    i = 3; 
    for (j = 0; j < 8; j++) {
        if ((j == 0) || (j == 1)) {
            hangman[i][j] = ' ';
        }
        if ((j == 2) || (j == 6)) {
            hangman[i][j] = '|';
        }
        if ((j == 3) || (j == 4)) {
            hangman[i][j] = ' ';
        }
        if (j == 5) {
            hangman[i][j] = '/';
        }
        if (j == 7) {
            hangman[i][j] = '\\';
        }
    }

    i = 4; 
    for (j = 0; j < 8; j++) {
        if ((j == 2) || (j == 6)) {
            hangman[i][j] = '|';
        }
        else {
            hangman[i][j] = ' ';
        }
    }

    i = 5; 
    for (j = 0; j < 8; j++) {
        if ((j == 0) || (j == 1)) {
            hangman[i][j] = ' ';
        }
        if (j == 2) {
            hangman[i][j] = '|';
        }
        if ((j == 3) || (j == 4) || (j == 6)) {
            hangman[i][j] = ' ';
        }
        if (j == 5) {
            hangman[i][j] = '/';
        }
        if (j == 7) {
            hangman[i][j] = '\\';
        }
    }

    i = 6; 
    for (j = 0; j < 8; j++) {
        if (j == 2) {
            hangman[i][j] = '|';
        }

        else {
            hangman[i][j] = ' ';
        }
    }

    i = 7; 
    for (j = 0; j < 8; j++) {
        if ((j == 0) || (j == 1) || (j == 3) || (j == 4)) {
            hangman[i][j] = '_';
        }
        if (j == 2) {
            hangman[i][j] = '|';
        }

        if ((j == 5) || (j == 6) || (j == 7)) {
            hangman[i][j] = ' ';
        }

    }
}
int drawHangman() { 
//pour updater l'affichage du pendu
    int i; int j;

    for (i = 0; i < attempt; i++) {
        for (j = 0; j < 8; j++) {
            printf("%c", hangman[i][j]);
        }
        printf("\n");
    }
}
int showDejavu() {
//pour montrer les lettres déjà proposées
    int i;

    printf("Lettres déjà proposées : ");
    for (i = 0; i <= alphabet; i++) {
        if ((dejavu[i].letter >= 'a') && (dejavu[i].letter <= 'z')) {
            printf("%c, ", dejavu[i].letter);
        }
    }
    printf("\n");
}
int showRiddle() {
//pour montrer l'état du mot à deviner
    int i;

    printf("Mot à deviner : ");
    for (i = 0; i <= size_keyword; i++) {
        printf("%c ", riddle[i].letter); //affiche les tirets ou lettres du mot à trouver
    }
    printf("\n");

    printf("Tentative(s) ratée(s) : %d / 8\n", attempt);
}


//AUTRES

/*Gestion des comptes utilisateurs*/
int createAccount() {
//pour créer son compte joueur
    int size_username;
    size_username = 0;

    int c; int i; 
    i = 0; //départ du tableau username

    printf("entrez votre pseudo\n");
    c = getchar();

    while ((c != '\n') && (c != EOF)) {
        
        player[account_id].username[i] = c; //assignation de la lettre donnée par getchar à l'username
        size_username = size_username + 1; //calcule la taille de l'username
        c = getchar(); //valeur suivante
        i++; //case suivante du tableau username
    }
    player[account_id].score = 0; //score initialiser à 0 lors de la création de compte

    printf("salut ");
    for (i = 0; i <= size_username; i++) {
        printf("%c", player[account_id].username[i]);
    }
    printf(" !\n");
    printf("C'est parti ? Appuie sur entrée pour jouer\n");
    emptyBuffer(); //vide le buffer mais bloque jeu si pas d'appui sur entrée
}

/*Retry*/
int retryGame() {
//pour relancer une partie
    char c;

    printf("Rejouer ?\n");
    printf("Oui : appuyer sur entrée\n");
    printf("Non : taper 'exit' puis valider\n");

    c = getchar();
    if (c != '\n') {
        power_button = 0;
    }
}

int youWin() {
//pour afficher une victoire
    system("clear");
    drawDancingPrisoner();
    printf("GAGNÉ !");
    wasAnwer();
    player[account_id].score = player[account_id].score + 1;
    printf("score : %d\n", player[account_id].score);
}
int drawDancingPrisoner() {
//pour illustrer la victoire
    printf("\\('-')\n");
    printf(" (__(z\n");
    printf(" /   \\\n");
}

int youLoose() {
//pour afficher une défaite
    system("clear");
    drawVulture();
    printf("PERDU !");
    wasAnwer();
    printf("score : %d\n", player[account_id].score);
}
int drawVulture() {
//pour illustrer la défaite
    printf("    .-'\'\\-,/^\\ .-.\n");
    printf("   /    l  \\  ( ee\\   __\n");
    printf("  l     l  |__/,--.\'\"\'  \',\n");
    printf("  l    /   .__/    \'\"\"\"\",/\n");
    printf("  l   /    /  |\n");
    printf(" .'.-'    /__/\n");
    printf("\'\"\'| |';-;_\'\n");
    printf("   |/ /-))))))\n");
}

int wasAnwer() {
//pour donner la réponse
    printf("Le mot à trouver était ");

    int i;

    for (i = 0; i <= size_keyword; i++) {
        printf("%c", answer[i].letter);
    }
    printf("\n");
}