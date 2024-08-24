#include "raylib.h"
#include <random>
#include <cstdlib>
#include <ctime>
class Ball {
    private:
        float x, y;
        int speed_x, speed_y, radius;
    public:
        Ball() { //costruttore in modo che ogni palla creata abbia gli stessi parametri
            x = 1280 / 2;
            y = 800 / 2;
            speed_x = 7;
            speed_y = 7;
            radius = 20;
        }
        void DrawBall() {
            DrawCircle(x, y, radius, WHITE); //Disegna la palla
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
            x = GetScreenWidth() / 2;
            y = GetScreenHeight() / 2;
        } 
        Vector2 GetCD() {
            return Vector2{ x,y };
        }
        void InvertiVelocitaX() {
            speed_x *= -1;
            speed_x += 0.17;
            
        }
        int GetX() {
            return x;
        }
        void setspeedY(int speedY) {
            speed_y = speedY;
        }
};

void CheckMovimentoPaddle(int &Paddle_y) { //paddle necessario per il movimento
    if (IsKeyDown(KEY_DOWN) && Paddle_y + 70 < GetScreenHeight()) {
        Paddle_y += 7; //sposta il paddle verso il basso
    }
    else if (IsKeyDown(KEY_UP) && Paddle_y - 70 > 0) {
        Paddle_y -= 7; //sposta il paddle verso l'alto
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
    float PaddleAI_speed = 5.85;
    int PunteggioUser = 0, PunteggioAI = 0;
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Pong game!");
    
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

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
        }
        else if (palla1.GetX() - 20 <= 0) {
            palla1.ResetPartita();
            PunteggioUser++;
        }

        BeginDrawing(); //disegna il canva dove possiamo disegnare gli oggetti
        
        ClearBackground(BLACK);
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, GREEN);
        
        if ((CheckCollisionCircleRec(palla1.GetCD(), (float)20, PaddleGiocatore)) || (CheckCollisionCircleRec(palla1.GetCD(), (float)20, PaddleAI))){
            palla1.InvertiVelocitaX(); //inverte la velocità della palla se c'è una collisione con un paddle
            palla1.UpdateBall(); //aggiorna la posizione della palla per evitare sovrapposizioni 
        }
        //drawing objects
        palla1.DrawBall();
        DrawRectangleRec(PaddleAI, WHITE); //Disegna Paddle giocatore 1 
        DrawRectangleRec(PaddleGiocatore, WHITE); //Paddle giocatore 2
        DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
        DrawText("YOUR SIDE", (screenWidth/2) + 25, 50, 25, GREEN);
        DrawText("CPU SIDE", ((screenWidth / 2) - (MeasureText("CPU SIDE", 25) + 25)), 50, 25, GREEN);
        DrawText(TextFormat("%d", PunteggioUser), (screenWidth / 2) + (MeasureText("CPU SIDE", 25) / 2), 10, 30, WHITE);
        DrawText(TextFormat("%d", PunteggioAI), (screenWidth / 2) - ((MeasureText("CPU SIDE", 25) / 2) + MeasureText("0",30)), 10, 30, WHITE);
        EndDrawing(); //finisce il disegno
    }

    // De-Initialization
    //---------------------------------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //---------------------------------------------------------------------------------------------------------------

    return 0;
}
