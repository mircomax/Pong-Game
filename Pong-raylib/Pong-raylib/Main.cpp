#include "raylib.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
class Ball {
    private:
        float x, y;
        float speed_x, speed_y, radius;
    public:
        Ball() { //costruttore in modo che ogni palla creata abbia gli stessi parametri
            x = 1280 / 2;
            y = 800 / 2;
            speed_x = -6.5;
            speed_y = -6.5;
            radius = 20.0;
        }
        void DrawBall() {
            DrawCircle(x, y, radius, GetColor(0xFFD700FF)); //Disegna la palla
        }
        void UpdateBall() { //far muovere la palla
            x += speed_x;
            y += speed_y;
            if (y + radius >= GetScreenHeight() || y - radius <= 0) { 
                speed_y *= -1;  //se colpisce il lato destro  o sinistro dello schermo inverte la velocita su x
            }
            if (x + radius >= GetScreenWidth() || x - radius <= 0 ) {
                speed_x *= -1; //stessa cosa ma con il lato inferiore e superiore
            }
        }
        void ResetPartita() {
            // Riposiziona la palla al centro
            x = GetScreenWidth() / 2; 
            y = GetScreenHeight() / 2;
        } 
        Vector2 GetCD() {
            return Vector2{ x,y };
        }

        void InvertiVelocitaX() {
             speed_x *= -1;

             // Aumenta la velocità della palla per rendere il gioco più veloce
             speed_x += (speed_x > 0) ? 0.15f : -0.15f;
             speed_y += (speed_y > 0) ? 0.15f : -0.15f;

             // Genera una variazione casuale nella velocità verticale
             float variazione = (static_cast<float>(rand() % 41) - 20) / 100.0f;
             speed_y += variazione;
        }

        int GetX() {
            return x;
        }
        void setspeedY(float speedY) {
            speed_y = speedY;
        }
        void setspeedX(float speedX) {
            speed_x = speedX;
        }
};

void CheckMovimentoPaddle(int &Paddle_y) { //paddle necessario per il movimento
    if (IsKeyDown(KEY_DOWN) && Paddle_y + 70 < GetScreenHeight()) {
        Paddle_y += 6.5; //sposta il paddle verso il basso
    }
    else if (IsKeyDown(KEY_UP) && Paddle_y - 70 > 0) {
        Paddle_y -= 6.5; //sposta il paddle verso l'alto
    }
}

void MovimentoPaddleAI(float& PaddleAI_y, float PaddleAI_speed, Ball& palla) {
    // Se la palla è sopra il paddle, muovi il paddle verso l'alto
    if (palla.GetCD().y < PaddleAI_y) {
        PaddleAI_y -= PaddleAI_speed;
    }
    // Se la palla è sotto il paddle, muovi il paddle verso il basso
    else if (palla.GetCD().y > PaddleAI_y) {
        PaddleAI_y += PaddleAI_speed;
    }

    // Mantieni il paddle all'interno dei limiti dello schermo
    if (PaddleAI_y - 70 < 0) {
        PaddleAI_y = 70;
    }
    else if (PaddleAI_y + 70 > GetScreenHeight()) {
        PaddleAI_y = GetScreenHeight() - 70;
    }
}

float IncrementoDifficolta(int& PunteggioAI, int& PunteggioUser) {
    if (PunteggioUser - PunteggioAI >= 2) {
        return 0.5f;
    }
    else if (PunteggioUser - PunteggioAI == 1) {
        return 0.2f;
    }
    else if (PunteggioAI > PunteggioUser) {
        return -0.5f;
    }
    else {
        return 0.0f;
    }
}

void LightEffect(Rectangle rett, Color lightcolor, int showLightEffect, Ball& palla) {
    if (showLightEffect > 1 && showLightEffect % 2 == 0) {
        DrawRectangleRec(rett, lightcolor);
    }
    if (showLightEffect > 1) palla.ResetPartita(); // Ferma la palla al centro
}

void MostraTestoGo(bool& showGoText, int& goTextFrames, const int goTextDurationFrames, int showLightEffect) {
    if (showLightEffect < 5) {// Mostriamo il testo solo quando la palla sta per ripartire
        if (showGoText) {
            // Disegna il testo al centro dello schermo
            DrawText("GO!", (GetScreenWidth() / 2) - (MeasureText("GO!", 100) / 2), (GetScreenHeight() / 2) - (MeasureText("GO!", 100) / 2), 100, BLACK);

            // Decrementa il contatore dei frame
            goTextFrames--;

            // Disattiva il flag quando il contatore dei frame raggiunge 0
            if (goTextFrames <= 0) {
                showGoText = false;
            }
        }
    }
}

Ball palla1;
int main(void)
{
    srand(time(NULL));
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 800;
    int Paddle_y = screenHeight / 2;
    float PaddleAI_y = screenHeight / 2;
    float PaddleAI_speed = 5.8;
    int PunteggioUser = 0, PunteggioAI = 0;
    SetConfigFlags(FLAG_VSYNC_HINT); // Rende tutto più fluido
    InitWindow(screenWidth, screenHeight, "Pong game!");
    int counter = 0;
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    //Colori
    Color cornflowerBlue = GetColor(0x6495EDFF); // CornflowerBlue con alfa pieno
    Color PaddleColor = LIGHTGRAY; // Colore paddle
    Color TextColor = GetColor(0xFFD700FF); // Gold Color

    //Variabili per gli effetti luminosi
    Color lightColor = LIME; // Colore 
    int showLightEffect = 0; //Essenziale per mostrare effetto luminoso
    Rectangle lightRect = { 0, 0, 0, 0 }; // Rettangolo che verrà poi colorato

    // Variabili per il testo "GO!"
    bool showGoText = false;
    int goTextFrames = 0;
    const int goTextDurationFrames = 30; // Mostra il testo per 30 frame

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //update
        palla1.UpdateBall(); //muovere la palla 
        CheckMovimentoPaddle(Paddle_y); //per muovere il paddle 
        Rectangle PaddleGiocatore = { (float)(screenWidth - 35), (float)(Paddle_y - 60), (float)25, (float)120 };
        Rectangle PaddleAI = {(float)10,(float)(PaddleAI_y - 60),(float)25,(float)120 };
        MovimentoPaddleAI(PaddleAI_y, PaddleAI_speed, palla1);

        //Controllo se uno dei due giocatori fa punto
        if (palla1.GetX() + 20 >= screenWidth) { 
            palla1.ResetPartita(); //viene resettata la palla al centro dello schermo
            PunteggioAI++; //incremento punteggio

            showGoText = true;
            goTextFrames = goTextDurationFrames;

            //Effetti illuminazione
            showLightEffect = 30;
            lightRect = { screenWidth - 50, 10, 45, screenHeight - 20 };

            //Cambia il lato in cui andrà la palla
            palla1.setspeedX(6.5);
            palla1.setspeedY((PunteggioAI % 2 == 0) ? -6.5 : 6.5);
        }
        else if (palla1.GetX() - 20 <= 0) {
            palla1.ResetPartita();
            PunteggioUser++;

            showGoText = true;
            goTextFrames = goTextDurationFrames;

            //effetti illuminazione
            showLightEffect = 30;
            lightRect = { 10, 10, 50, screenHeight - 20 };

            //Cambia il lato in cui andrà la palla
            palla1.setspeedX(-6.5);
            palla1.setspeedY((PunteggioUser % 2 == 0) ? -6.5 : 6.5);
        }
        else {
            showLightEffect--;
        }

        //Aumentare o diminuire velocità AI
        if ((PunteggioAI + PunteggioUser == 3 || PunteggioAI + PunteggioUser == 6) && counter == 0) {
            PaddleAI_speed += IncrementoDifficolta(PunteggioAI, PunteggioUser); 
            counter = 1; // Blocca ulteriori incrementi finché non viene resettato
        }
        else if (PunteggioAI + PunteggioUser != 3 && PunteggioAI + PunteggioUser != 6) {
            counter = 0; // Resetta il counter per il prossimo incremento
        }

        BeginDrawing(); 
        
        ClearBackground(cornflowerBlue);
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, TextColor); // Mostra gli FPS a schermo
        
        if ((CheckCollisionCircleRec(palla1.GetCD(), (float)20, PaddleGiocatore)) || (CheckCollisionCircleRec(palla1.GetCD(), (float)20, PaddleAI))){
            palla1.InvertiVelocitaX(); // Inverte la velocità della palla se c'è una collisione con un paddle
            palla1.UpdateBall(); // Aggiorna la posizione della palla per evitare sovrapposizioni 
        }
        //drawing objects
        palla1.DrawBall();
        DrawRectangleRec(PaddleAI, PaddleColor); // Disegna Paddle giocatore 1 
        DrawRectangleRec(PaddleGiocatore, PaddleColor); // Paddle giocatore 2
        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, BLACK);
        DrawText("YOUR SIDE", (screenWidth/2) + 25, 50, 25, TextColor);
        DrawText("CPU SIDE", ((screenWidth / 2) - (MeasureText("CPU SIDE", 25) + 25)), 50, 25, TextColor);
        DrawText(TextFormat("%d", PunteggioUser), (screenWidth / 2) + (MeasureText("CPU SIDE", 25) / 2), 10, 30, BLACK);
        DrawText(TextFormat("%d", PunteggioAI), (screenWidth / 2) - ((MeasureText("CPU SIDE", 25) / 2) + MeasureText("0",30)), 10, 30, BLACK);
        MostraTestoGo(showGoText, goTextFrames, goTextDurationFrames, showLightEffect); //Mostrare il testo GO!
        // Disegna l'effetto di illuminazione se necessario
        LightEffect(lightRect, lightColor, showLightEffect, palla1);
        EndDrawing(); 
    }

    // De-Initialization
    //---------------------------------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //---------------------------------------------------------------------------------------------------------------

    return 0;
}

//DrawText("GO!", (screenWidth / 2) - (MeasureText("GO!", 60) / 2), (screenHeight / 2) - (MeasureText("GO!", 60)), 60, BLACK);