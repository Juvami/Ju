#include <SDL2/SDL.h>
#include <unistd.h>
#include "function.h"

#define WINDOW_WIDTH 704 //700
#define WINDOW_HEIGHT 800 //920
#define FPS 60

/*constantes de bornage*/
#define CIEL 0
#define SOL WINDOW_HEIGHT
#define FEUVERT 0
#define FEUROUGE WINDOW_WIDTH

#define ROW 5 //nbr lignes
#define COL 11 //nbr colonnes **WINDOW_WIDTH divisée par .longueur = COL

//int holding_key;
int press_key = 0;
double holdKey();

struct Paddle {
	int x; 
	int y;
	int x2; //Px + longueur
	int y2; //Py + hauteur
	
	double speedx; //#define PSYHORIZON 20
	/*int speedy;*/ 

	char *texture; //chemin vers sprite_sheet Psyduck

	int flip; //gère le sens du sprite « Psyduck » //int flip = 1;
	int turbo; //dit combien d'acceleration il y a eu
	int life;
};
struct Paddle psyduck;
#define TPSYDUCK 96

struct Ball {
	int x;
	int y; 
	int x2; //Bx + rayon
	int y2; //By + rayon

	double speedx; //double speedBx = -3
	double speedy; //double speedBy = -5

	char *texture; //chemin vers sprite_sheet Pokeball

	int turbo; //dit combien d'acceleration il y a eu
};
struct Ball pokeball;
#define TPOKEBALL 36
#define DEGAT 2

struct Brick {
	int x;
	int y; 
	int x2; //Kx + longueur
	int y2; //Ky + hauteur

	//int longueur; int hauteur; > CONSTANTES 

    int status;

	char *texture;
};
#define LGRASS 64
#define HGRASS 47

struct Brick level[ROW][COL]; //déclaration de tableau. Avant : int tab[ROW][COL];

/*Déclaration de fonctions*/
int liven_upBrick();
int liven_upPaddle();
int liven_upBall();

double goRight();
double goLeft();

double moveBall();

double collideBrick();
double detectImpact (double x, double xbis, double y, double ybis);

int GameOver();


void init_game(){
    psyduck.x = 300; psyduck.y = 704; //initialisation position
    psyduck.speedx = 10; //initialisation vitesse
    psyduck.flip = 1; psyduck.turbo = 0; psyduck.life = 10;

    pokeball.x = (psyduck.x + (TPSYDUCK/2)); pokeball.y = (psyduck.y - (TPSYDUCK + TPOKEBALL));
    pokeball.speedx = -3; pokeball.speedy = -5;
	pokeball.turbo = 0;

    int i; int j;

	for (i = 0; i <= (ROW - 1); i++) {
		for (j = 0; j <= (COL - 1); j++){
			level[i][j].x = j*LGRASS; 
        	level[i][j].x2 = level[i][j].x + LGRASS; // = x + longueur      
        	level[i][j].y = i*HGRASS;
       		level[i][j].y2 = level[i][j].y + HGRASS;
			
            level[i][j].status = 2;
		}
	}
    //printf("tableau initialisé\n");
}

void drawGame(){
    clear();
    
    sprite(0, 0, "assets/plaine_background.bmp");
    liven_upBrick();
    liven_upPaddle();
    liven_upBall();

    GameOver();

    actualize();
    usleep(1000000 / FPS); // 60 images par seconde | 1000000 = 1 seconde
}
void KeyPressed(SDL_Keycode touche){
    switch (touche) {
        case SDLK_SPACE:
            break;
        case SDLK_RIGHT:
			press_key = press_key + 1;
			holdKey();
            goRight();
            break;
        case SDLK_LEFT:
			press_key = press_key + 1;
			holdKey();
            goLeft();
            break;
        case SDLK_ESCAPE:
            freeAndTerminate();
            break;
        default:
            break;
    }
}
void joyButtonPressed(){
    /*switch (touche) {
        case SDLK_SPACE:
            break;
        case SDLK_RIGHT:
            goRight;
            break;
        case SDLK_LEFT:
            goLeft;
            break;
        case SDLK_ESCAPE:
            freeAndTerminate();
            break;
        default:
            break;
    }*/
}

void gameLoop() {
    int programLaunched = 1;
    while (programLaunched == 1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                /* https://wiki.libsdl.org/SDL_EventType */
                case SDL_QUIT:
                    programLaunched = 0;
                    break;
                case SDL_MOUSEBUTTONUP:
                    //printf("position de la souris x : %d , y : %d\n", event.motion.x, event.motion.y);
                    break;
                case SDL_KEYDOWN:
                    KeyPressed(event.key.keysym.sym);
					moveBall();
                    break;
				case SDL_KEYUP:
					press_key = 0;
					psyduck.speedx = 10; //ré-initialisation
					psyduck.turbo = 0;//ré-initialisation
					break;
                case SDL_JOYBUTTONDOWN:
                    break;
                default:
                    break;
            }
        }
        /* coder ici pour que le code s'execute après chaque évenement */
        //moveBall();
        collideBrick();
        drawGame();
    }
}

int main(){
    init(WINDOW_WIDTH, WINDOW_HEIGHT);
    init_game();
    gameLoop();
    printf("Fin du programme\n");
    freeAndTerminate();
}

//SPRITES & ANIMATIONS
int liven_upBrick()
{
    //sprite_sheet pour Pokémons
    //éventuellement sprice_sheet pour Grass (vague, mouvement vent dans herbe)

    int i; int j;
	//int roll_the_dice; /*variable pour gérer des évènements aléatoires*/

	for (i = 0; i <= (ROW - 1); i++) {
		for (j = 0; j <= (COL - 1); j++) {
			if (level[i][j].status == 2) {
				changeColor(255, 0, 0);
				drawRect(level[i][j].x, level[i][j].y, LGRASS, HGRASS);
				//level[i][j].texture = "assets/grass_brick.bmp"; //brique herbe
			}
			if (level[i][j].status == 1) {
				changeColor(125, 0, 0);
				drawRect(level[i][j].x, level[i][j].y, LGRASS, HGRASS);
				//level[i][j].texture = "assets/licki_brick.bmp"; //brique pokémon > sprite_sheet (future)
			}
			if (level[i][j].status <= 0) {
				//drawRect(int posX, int posY, int longueur, int hauteur)
				//level[i][j].texture = "assets/no_brick.bmp"; //brique morte
			}
			//sprite(level[i][j].x, level[i][j].y, level[i][j].texture);
		}
	}			
}

int liven_upBall()
{
    pokeball.texture = "assets/36px_pokeball.bmp";
	sprite(pokeball.x, pokeball.y, pokeball.texture);
}

int liven_upPaddle()
{
    //sprite_sheet Psyduck (future)

	if (psyduck.flip < 0) {
        psyduck.texture = "assets/psyduckG.bmp";
		sprite(psyduck.x, psyduck.y, psyduck.texture); //psyduck G

		if (psyduck.x <= FEUVERT) {
            psyduck.texture = "assets/red_psyduckG.bmp";
			//printf("Outch ! \n");
			sprite(psyduck.x, psyduck.y, psyduck.texture); //red psyduck G
		}
	}

	if (psyduck.flip> 0) {
        psyduck.texture = "assets/psyduckD.bmp";
		sprite(psyduck.x, psyduck.y, psyduck.texture); //psyduck D

		if (psyduck.x2 >= FEUROUGE) {
            psyduck.texture = "assets/red_psyduckD.bmp";
			//printf("Outch ! \n");
			sprite(psyduck.x, psyduck.y, psyduck.texture); //red psyduck D
		}
	}
}

// MVT PADDLE
double holdKey() {
	//printf("holdKey\n");

	if (press_key > 1) {
		if (psyduck.turbo < 3) { //au delà de 3, devient injouable
			psyduck.speedx = psyduck.speedx*1.5;
			psyduck.turbo = psyduck.turbo + 1;
		}
	}
}
double goRight() {
	//printf("Right HERE\n");
	psyduck.flip = 1;
	if (psyduck.x2 < FEUROUGE) {
		psyduck.x = psyduck.x + psyduck.speedx;
		liven_upPaddle();
	}
	else {
		psyduck.x2 = FEUROUGE; 
		liven_upPaddle();
		psyduck.x = psyduck.x - psyduck.speedx;
		liven_upPaddle();
		psyduck.speedx = 10; //ré-initialisée
	}
	psyduck.x2 = psyduck.x + TPSYDUCK;
}

double goLeft() {
	//printf("Left HERE\n");
	psyduck.flip = (-1);
	
	if (psyduck.x > FEUVERT) {
		psyduck.x = psyduck.x - psyduck.speedx;
		liven_upPaddle();
	}
	else {
		psyduck.x = FEUVERT;
		liven_upPaddle();
		psyduck.x = psyduck.x + psyduck.speedx;
		liven_upPaddle();
		psyduck.speedx = 10; //ré-initialisée
	}
	psyduck.x2 = psyduck.x + TPSYDUCK;
}

// MVT BALL
double moveBall()
{
	//printf("moveBall\n");
	if (pokeball.x <= FEUVERT) {
		//printf("MUR FV\n");
		pokeball.speedx = pokeball.speedx*(-1);
	}
	if (pokeball.x2 >= FEUROUGE ) {
		//printf("MUR FR\n");
		pokeball.speedx = pokeball.speedx*(-1);
	}
	if (pokeball.y <= CIEL) {
		//printf("PLAFOND\n");
		pokeball.speedy = pokeball.speedy*(-1);
	} 
	if (pokeball.speedy > 0) {
		if (((pokeball.y + (TPOKEBALL/2)) >= psyduck.y) && ((pokeball.y + (TPOKEBALL/2)) <= (psyduck.y + pokeball.speedy))) {
			if ((pokeball.x2 >= psyduck.x) && (pokeball.x <= psyduck.x2)) {
				//printf("Gotcha !\n");
				pokeball.speedy = pokeball.speedy*(-1);
				if (psyduck.turbo > 0) {
					if (pokeball.turbo < 5) { //limite de jouabilité. 5 peut-être un peu rapide
						int i;
						for (i = 1; i <= psyduck.turbo; i++) {
							pokeball.speedy = pokeball.speedy*1.5;
							pokeball.speedx = pokeball.speedx*1.5;
							pokeball.turbo = pokeball.turbo + 1;
						}
					}
				}
			}
		}
	}
	pokeball.x = pokeball.x + pokeball.speedx;
	pokeball.x2 = pokeball.x + TPOKEBALL;
	pokeball.y = pokeball.y + pokeball.speedy;
	pokeball.y2 = pokeball.y + TPOKEBALL;
	liven_upBall();
}

// GESTION BRIQUES

double collideBrick()
{
	int i; int j;
	int roll_the_dice; //variable pour gérer des évènements aléatoires
	double old_speed_x; double old_speed_y; //vitesses x et y initiales

	for (i = 0; i <= (ROW - 1); i++) {
		for (j = 0; j <= (COL -1); j++) {
			if (level[i][j].status > 0) {
				old_speed_y = pokeball.speedy; old_speed_x = pokeball.speedx;
				detectImpact(level[i][j].x, level[i][j].x2, level[i][j].y, level[i][j].y2);
				if ((old_speed_y != pokeball.speedy) || (old_speed_x != pokeball.speedx)) {
				//if (detectImpact(level[i][j].x, level[i][j].x2, level[i][j].y, level[i][j].y2) == 1){
					if (pokeball.turbo > 0) {
						pokeball.speedx = pokeball.speedx/1.5;
						pokeball.speedy = pokeball.speedy/1.5;
						pokeball.turbo = pokeball.turbo - 1; //une décélération par impact avec les briques
					}
					roll_the_dice = rand() % 6; // "un dé 6" ;)
					if (roll_the_dice >= 4) { //ou %x, etc
						level[i][j].status = level[i][j].status - (DEGAT/2);
					}
					else {
						level[i][j].status = level[i][j].status - DEGAT;
					}
				}
			}
		}
	}
}

double detectImpact (double x1, double x2, double y1, double y2)
/*x = bord gauche, xbis = bord droit*/
/*y = bord sup, ybis = bord inf*/
{
	if ((pokeball.x >= x1) && (pokeball.x2 <= x2)){
		if ((pokeball.y <= y2) && (pokeball.y >= y1)) { // - pokeball.speedy
			printf("Aïe, mes pieds\n");
			pokeball.speedy = pokeball.speedy*(-1);
		}
		if ((pokeball.y2 >= y1) && (pokeball.y2 <= y2)) { //(pokeball.y2 <= y2) //(y1 + pokeball.speedy)
			printf("Aïe, ma tête\n");
			pokeball.speedy = pokeball.speedy*(-1);
		}
	}
	else if ((pokeball.y >= y1) && (pokeball.y <= y2)) {
		if ((pokeball.x2 >= x1) && (pokeball.x2 <= x2)) { //(x1 + pokeball.speedx)
			printf("Ouille, ma gauche\n");
			pokeball.speedx = pokeball.speedx*(-1);
		}
		if ((pokeball.x <= x2) && (pokeball.x >= x1)) { //(x2 + pokeball.speedx)
			printf("OUILLE, ma droite\n");
			pokeball.speedx = pokeball.speedx*(-1);
		}
	}
}


// ANNEXES
int GameOver()
{
	if (pokeball.y2 >= SOL) { //si la balle tombe au sol
		//printf("Game Over\n");
		clear();
		sprite(20, 420, "assets/gameover.bmp");

		pokeball.speedx = 0;
		pokeball.speedy = 0;
	} 

	if (psyduck.life <= 0) { //si le paddle n'a plus PV
		clear();
		sprite(300, 200, "assets/pika_balloon.bmp");
	}
}

/*int staying_alivePaddle() {
	//psyduck.life init à 10PV
	if (impact balle/paddle) {
		psyduck.life = psyduck.life - 1;
	}
		
	if (attrape bonus baie) {
		psyduck.life = psyduck.life + 1; //modif valeur en fonction baie ?
	}
	
	if (attrape malus pokemon haute herbe) {
		psyduck.life = psyduck.life - 1; //constante dégat pv ? 
	}
}*/

/*double boostThis() {}*/

