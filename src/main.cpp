#include "mbed.h"
#include "C12832.h"

C12832 lcd(p5, p7, p6, p8, p11);

DigitalIn joy_up(p15);
DigitalIn joy_down(p12);
DigitalIn joy_left(p13);
DigitalIn joy_right(p16);
DigitalIn joy_press(p14);


DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

PwmOut red(p23);
PwmOut green(p24);
PwmOut blue(p25);

// potentiometre
AnalogIn pot1(p19);
AnalogIn pot2(p20);

// --- AJOUT AUDIO ---
PwmOut speaker(p26); 

DigitalOut* leds[] = { &led1, &led2, &led3, &led4 };


// VARIABLES

// Plateau de 6 lignes
// Hauteur d'une ligne = env 5 pixels.
int frog_x = 3; // 0 à 7 (Horizontal)
int frog_y = 0; // 0 à 5 (Vertical)

int life = 15; // Pour les vies du joueur
int compteur_vie = 3; // Pour le compteur de vie dans la lgoique de tkt t'as compris
float tour_compt = 5; // pour initialisé le déplcament des voitures


// Obstacles
uint8_t row1 = 0b10100000; // Ligne 1 (Bas)
uint8_t row2 = 0b00011000; // Ligne 2
uint8_t row3 = 0b10001000; // Ligne 3
uint8_t row4 = 0b01110000; // Ligne 4 (Haut)
// --- NOUVELLES RANGÉES pour niveau 2 et 3 ---
uint8_t row5 = 0b11001100; // Nouvelle ligne
uint8_t row6 = 0b00100010; // Nouvelle ligne (Haut)


// --- VARIABLES MUSIQUE ---

float MI4  = 329.63;
float FA4  = 349.23;
float SOL4 = 392.00;
float LA4  = 440.00;
float SI4  = 493.88;
float DO5  = 523.25;
float RE5  = 587.33;
float MI5  = 659.25;


float melodie_jeu[] = { 
    // Mesure 1 
    LA4, 0,    LA4, SI4, 
    DO5, DO5,  SI4, LA4,
    
    // Mesure 2 
    DO5, 0,    DO5, RE5,
    MI5, MI5,  RE5, DO5,

    // Mesure 3 
    SOL4, 0,   SOL4, LA4,
    SI4,  SI4, LA4,  SOL4,

    // Mesure 4 
    FA4, 0,    FA4, SOL4,
    LA4, LA4,  SOL4, FA4
};

int index_note = 0;




int puissance(int x, int p){
    int total = 1;
    for (int i = 0; i < p; i++){
        total *= x;
    }
    return total;
}

void DisplayIntLeds(int i) {
    for (int bit = 0; bit < 4; bit++) {
        *leds[bit] = (i >> bit) & 1;
    }
}


void Menu(){
    lcd.cls();
    while (!joy_press){
        lcd.locate(0,0);
        lcd.printf("Menu");
        lcd.locate(0,10);
        lcd.printf("Appuyer sur le joystick pour jouer");
        lcd.copy_to_lcd();
    }
    return;
}


int main() {

    speaker = 0;
    int level = 1;
    int vitesse_jeu;
    int max_hauteur;
    
    Menu();

    while(1) {

        // 0. Mise en place des valeurs en fonction du niveau
        if (level == 1) {max_hauteur = 5; vitesse_jeu = 200; green = 0; blue = 1; red = 1;}
        else if (level == 2) {max_hauteur = 7; vitesse_jeu = 200; green = 0; blue = 1; red = 0;}
        else {max_hauteur = 7; vitesse_jeu = 100; green = 1; blue = 1; red = 0;}

        float volume_general = pot1.read() * 0.5;
        float pause = pot2.read();

        // 1. DEPLACEMENT GRENOUILLE 
        if (joy_up)    { if(frog_y < max_hauteur) frog_y++; }
        if (joy_down)  { if(frog_y > 0) frog_y--; }
        if (joy_left)  { if(frog_x < 7) frog_x++; } 
        if (joy_right) { if(frog_x > 0) frog_x--; }
        if (pause < 0.1) {
            speaker = 0;

            while(pause < 0.1){
                pause = pot2.read();
                lcd.cls();
                lcd.locate(0,10);
                lcd.printf("Pause");
                wait_ms(1000);
                lcd.copy_to_lcd();
            }
        }


        if (tour_compt/5 == 1){
            // 2. DEPLACEMENT OBSTACLES
            row1 = (row1 << 1) | (row1 >> 7); // Vers la gauche
            row2 = (row2 >> 1) | (row2 << 7); // Vers la droite
            row3 = (row3 << 1) | (row3 >> 7); // Vers la gauche
            row4 = (row4 >> 1) | (row4 << 7); // Vers la droite

            if (level > 1) {
                row5 = (row5 << 1) | (row5 >> 7); // Gauche
                row6 = (row6 >> 1) | (row6 << 7); // Droite
            }

            tour_compt = 0;
        }
        else{tour_compt++;}


        // 3. DESSIN DU JEU
        lcd.cls(); 
        
        // BOUCLE DESSIN OBSTACLES
        for(int i=0; i<8; i++) {
            int x_pos_start = (7-i)*16 + 2;
            int x_pos_end   = (7-i)*16 + 14;

            if (level > 1){
                // Ligne 1 (Y environ 24)
                if ( (row1 >> i) & 1 ) lcd.fillrect(x_pos_start, 24, x_pos_end, 26, 1);
                // Ligne 2 (Y environ 20)
                if ( (row2 >> i) & 1 ) lcd.fillrect(x_pos_start, 20, x_pos_end, 22, 1);
                // Ligne 3 (Y environ 16)
                if ( (row3 >> i) & 1 ) lcd.fillrect(x_pos_start, 16, x_pos_end, 18, 1);
                // Ligne 4 (Y environ 12)
                if ( (row4 >> i) & 1 ) lcd.fillrect(x_pos_start, 12, x_pos_end, 14, 1);
                // Ligne 5 (Y environ 8) -- AJOUT
                if ( (row5 >> i) & 1 ) lcd.fillrect(x_pos_start, 8,  x_pos_end, 10, 1);
                // Ligne 6 (Y environ 4) -- AJOUT
                if ( (row6 >> i) & 1 ) lcd.fillrect(x_pos_start, 4,  x_pos_end, 6,  1);
            }

            else {
                // Ligne 1 
                if ( (row1 >> i) & 1 ) lcd.fillrect(x_pos_start, 21, x_pos_end, 24, 1);

                // Ligne 2
                if ( (row2 >> i) & 1 ) lcd.fillrect(x_pos_start, 16, x_pos_end, 19, 1);

                // Ligne 3 
                if ( (row3 >> i) & 1 ) lcd.fillrect(x_pos_start, 11, x_pos_end, 14, 1);

                // Ligne 4 
                if ( (row4 >> i) & 1 ) lcd.fillrect(x_pos_start, 6,  x_pos_end, 9,  1);
            }
        }

        if (level > 1){
        // DESSIN: Lignes Départ/Arrivée
        lcd.line(0, 2, 128, 2, 1);   // Ligne Arrivée 
        lcd.line(0, 28, 128, 28, 1); // Ligne Départ

        // DESSIN: Grenouille
        int draw_y = 30 - (frog_y * 4); 
        int draw_x = (7 - frog_x) * 16 + 8;
        
        // On dessine un pixel ou un tout petit cercle car c'est serré
        lcd.fillrect(draw_x-1, draw_y-1, draw_x+1, draw_y+1, 1);
        lcd.copy_to_lcd();
        }
        else {
            // DESSIN: Lignes Départ/Arrivée
            lcd.line(0, 5, 128, 5, 1);   // Ligne Arrivée 
            lcd.line(0, 26, 128, 26, 1); // Ligne Départ 

            // DESSIN: Grenouille
            int draw_y = 28 - (frog_y * 5); 
            int draw_x = (7 - frog_x) * 16 + 8;
            
            // Rayon plus petit (2) car les lignes sont moins hautes
            lcd.fillcircle(draw_x, draw_y, 2, 1);
        }

        
        // Gestion des vies
        DisplayIntLeds(life);


        // --- GESTION AUDIO DE FOND ---
        if (melodie_jeu[index_note] > 0) {
            speaker.period(1.0 / melodie_jeu[index_note]);
            speaker = volume_general*0.2; 
        } else {
            speaker = 0.0; 
        }
        
        // On passe à la note suivante
        index_note++;
        if (index_note >= 32) index_note = 0; 




        if (life == 0){
            speaker = 0;
            lcd.cls();
            lcd.locate(0,10);
            lcd.printf("Plus de vies !");
            lcd.copy_to_lcd();

            // Son triste 
            speaker.period(1.0/400.0); speaker = volume_general; wait(0.4);
            speaker.period(1.0/350.0); speaker = volume_general; wait(0.4);
            speaker.period(1.0/300.0); speaker = volume_general; wait(0.8);
            speaker = 0; 

            wait_ms(1000);

            lcd.cls();
            lcd.locate(0,10);
            lcd.printf("Retour au menu");
            lcd.copy_to_lcd();
            wait_ms(1000);

            frog_y = 0; 
            life = 15;
            compteur_vie = 3;
            level = 1;
            Menu();
        }


        // 4. COLLISION & VICTOIRE
        bool crash = false;
        
        if (frog_y == 1) { if ( (row1 >> frog_x) & 1 ) crash = true; }
        if (frog_y == 2) { if ( (row2 >> frog_x) & 1 ) crash = true; }
        if (frog_y == 3) { if ( (row3 >> frog_x) & 1 ) crash = true; }
        if (frog_y == 4) { if ( (row4 >> frog_x) & 1 ) crash = true; } 

        if (level > 1){
            if (frog_y == 5) { if ( (row5 >> frog_x) & 1 ) crash = true; }
            if (frog_y == 6) { if ( (row6 >> frog_x) & 1 ) crash = true; }
        }

        if (crash) {
            speaker = 0;
            lcd.cls();
            lcd.locate(0,10);
            lcd.printf("BOOM !");
            lcd.copy_to_lcd();

            // Bruit bizarre pour le crash
            for(int k=0; k < 60; k++) {
                speaker.period(1.0 / (1000.0 - (k*15)));
                speaker = volume_general;
                wait_ms(10);
            }
            speaker = 0;

            wait_ms(1000);
            frog_y = 0; 
            life = life - puissance(2,compteur_vie);
            compteur_vie--;
        }


        // Cas victoire
        if (frog_y == max_hauteur) {
            wait_ms(500);
            lcd.cls();
            lcd.locate(0,10);
            lcd.printf("WIN !");
            lcd.copy_to_lcd();

            // Musique Joyeux 
            speaker.period(1.0/523.25); speaker = volume_general; wait(0.1); 
            speaker.period(1.0/659.25); speaker = volume_general; wait(0.1);
            speaker.period(1.0/783.99); speaker = volume_general; wait(0.1); 
            speaker.period(1.0/1046.5); speaker = volume_general; wait(0.3); 
            speaker = 0;

            wait_ms(1000);
            frog_y = 0; 

            if (level == 3) {
                lcd.cls();
                lcd.locate(0,10);
                lcd.printf("Vous avez gagner tout tout tout !");
                lcd.copy_to_lcd();

                // Musique Joyeux 
                speaker.period(1.0/523.25); speaker = volume_general; wait(0.1); 
                speaker.period(1.0/659.25); speaker = volume_general; wait(0.1);
                speaker.period(1.0/783.99); speaker = volume_general; wait(0.1); 
                speaker.period(1.0/1046.5); speaker = volume_general; wait(0.3); 

                speaker.period(1.0/523.25); speaker = volume_general; wait(0.1); 
                speaker.period(1.0/659.25); speaker = volume_general; wait(0.1);
                speaker.period(1.0/783.99); speaker = volume_general; wait(0.1); 
                speaker.period(1.0/1046.5); speaker = volume_general; wait(0.3); 

                speaker.period(1.0/523.25); speaker = volume_general; wait(0.1); 
                speaker.period(1.0/659.25); speaker = volume_general; wait(0.1);
                speaker.period(1.0/783.99); speaker = volume_general; wait(0.1); 
                speaker.period(1.0/1046.5); speaker = volume_general; wait(0.3); 
                speaker = 0;

                wait_ms(2000);
                lcd.cls();
                lcd.locate(0,10);
                lcd.printf("Retour au menu");
                lcd.copy_to_lcd();
                wait_ms(1000);

                level = 1;
                Menu();
            }
            else {
                lcd.cls();
                lcd.locate(0,10);
                lcd.printf("Niveau %d termine !", level);
                lcd.copy_to_lcd();

                wait_ms(1000);

                lcd.cls();
                lcd.locate(0,10);
                lcd.printf("Debut du niveau %d !", level+1);
                lcd.copy_to_lcd();

                speaker = 0;

                wait_ms(1000);
                frog_y = 0; 

                level++;
            }
        }

        wait_ms(vitesse_jeu);
    }
}
