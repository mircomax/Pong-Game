#include "raylib.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <filesystem>
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
            DrawCircleLines(x, y, radius, BLACK);
        }
        void UpdateBall() { //far muovere la palla
            x += speed_x;
            y += speed_y;
            if (y + radius >= GetScreenHeight() || y - radius <= 0) { 
                speed_y *= -1;  //stessa cosa ma con il lato inferiore e superiore
            }
            if (x + radius >= GetScreenWidth() || x - radius <= 0 ) {
                speed_x *= -1; 
                //se colpisce il lato destro  o sinistro dello schermo inverte la velocita su x
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
        int getY() {
            return y;
        }
};

enum GameState{
    MENU,
    GAMEPLAY,
    PAUSE
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
        DrawRectangleRec(rett, lightcolor); // L'effetto luminoso compare una volta ogni 2 frame per 30 frame
    }
    if (showLightEffect > 1) palla.ResetPartita(); // Ferma la palla al centro
}

void MostraTestoGo(bool& showGoText, int& goTextFrames, const int goTextDurationFrames, int showLightEffect, bool& Iniziopartita) {
    if (showLightEffect < 5) {// Mostriamo il testo solo quando la palla sta per ripartire
        if (showGoText || Iniziopartita) {
            // Disegna il testo al centro dello schermo
            DrawText("GO!", (GetScreenWidth() / 2) - (MeasureText("GO!", 100) / 2), (GetScreenHeight() / 2) - (50), 100, BLACK);

            // Decrementa il contatore dei frame
            goTextFrames--;

            // Disattiva il flag quando il contatore dei frame raggiunge 0
            if (goTextFrames <= 0) {
                showGoText = false;
                Iniziopartita = false;
            }
        }
    }
}

void DisegnaPaddle(Rectangle PaddleAI, Rectangle PaddleGiocatore, Color PaddleColor){
    // Disegna Paddle giocatore 1 
    DrawRectangleRounded(PaddleAI, 0.8, 0, PaddleColor); 
    DrawRectangleRoundedLines(PaddleAI, 0.8, 0, 1.0, BLACK);

    // Paddle giocatore 2
    DrawRectangleRounded(PaddleGiocatore, 0.8, 0, PaddleColor); 
    DrawRectangleRoundedLines(PaddleGiocatore, 0.8, 0, 1.0, BLACK);
}

void DisegnaMenuPausa(Rectangle Pausa[], Color lightColor) { // Disegnare menu di pausa (Rettangoli e scritte)
    DrawRectangleRec(Pausa[0], lightColor);
    DrawRectangleRec(Pausa[1], lightColor);
    DrawRectangleRec(Pausa[2], lightColor);
    DrawText("RIPRENDI", Pausa[0].x + (Pausa[0].width - MeasureText("RIPRENDI", 60)) / 2, Pausa[0].y + (Pausa[0].height - 60) / 2, 60, WHITE);
    DrawText("MENU", Pausa[1].x + (Pausa[1].width - MeasureText("MENU", 60)) / 2, Pausa[1].y + (Pausa[1].height - 60) / 2, 60, WHITE);
    DrawText("ESCI", Pausa[2].x + (Pausa[2].width - MeasureText("ESCI", 60)) / 2, Pausa[2].y + (Pausa[2].height - 60) / 2, 60, WHITE);
}

void PlayWallHit(Sound& wallhit, Ball& palla, bool suonoattivo) {
    if ((palla.getY() + 20 >= GetScreenHeight() || palla.getY() - 20 <= 0) && suonoattivo) {
        PlaySound(wallhit);  
    }
}

Ball palla1;
int main(void)
{
    srand(time(NULL));
    // Initialization
    //--------------------------------------------------------------------------------------
    ChangeDirectory("../..");
    const int screenWidth = 1280;
    const int screenHeight = 800;
    int Paddle_y = screenHeight / 2;
    float PaddleAI_y = screenHeight / 2;
    float PaddleAI_speed = 5.9;
    int PunteggioUser = 0, PunteggioAI = 0;
    SetConfigFlags(FLAG_VSYNC_HINT); // Rende tutto più fluido
    InitAudioDevice();
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
    const int goTextDurationFrames = 40; // Mostra il testo per 40 frame

    bool MusicaAttiva = true;
    bool SuonoAttivo = true; // Per attivare e disattivare i suoni
    Rectangle MusicButton = { screenWidth / 2 - 320, screenHeight / 2 + 75, 170, 60 };
    Rectangle SoundButton = { screenWidth / 2 + 150, screenHeight / 2 + 75, 170, 60 };
    Rectangle pauseButton = { screenWidth - 60, 10, 50, 50 }; // Bottone di pausa
    Rectangle ButtonsPause[3];
    const int ButtonsPauseWidth = 400, ButtonsPauseHeight = 150; //Definisce altezza e larghezza dei bottoni di pausa
    Rectangle startButton = { screenWidth / 2 - 125, screenHeight / 2 + 65, 250, 80}; // Bottone di avvio
    GameState stato = MENU; //Il programma inizia con il menu 

    bool iniziopartita = true; // Per mostrare il testo GO! appena si preme start
    Color Rossoscuro = GetColor(0x8B0000FF); // Rosso scuro per il testo dei bottoni nel menu principale
    // Colori dei bottoni principali
    Color BackgroundMenuSound = GREEN, BackgroundMenuMusic = GREEN;
    Color TextMenuSound = DARKGREEN, TextMenuMusic = DARKGREEN;

    //Musica
    std::string workingdirectory = GetWorkingDirectory();

    // Costruisci il percorso completo del file audio
    std::string musicPath = workingdirectory + "\\x64\\Debug\\Musicadisottofondo.ogg";
    std::string Paddlehitsound = workingdirectory + "\\x64\\Debug\\PaddleHit.wav";
    std::string Wallhit = workingdirectory + "\\x64\\Debug\\HitMuro.wav";
    
    // Carica e riproduci la musica
    Music MusicaSottofondo = LoadMusicStream(musicPath.c_str());
    Sound WallHit = LoadSound(Wallhit.c_str());
    Sound PaddleHitSound = LoadSound(Paddlehitsound.c_str());
    PlayMusicStream(MusicaSottofondo);
    SetMusicVolume(MusicaSottofondo, 0.6);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        Rectangle PaddleGiocatore = { (float)(screenWidth - 35), (float)(Paddle_y - 60), (float)25, (float)120 };
        Rectangle PaddleAI = { (float)10,(float)(PaddleAI_y - 60),(float)25,(float)120 };
        UpdateMusicStream(MusicaSottofondo); // Aggiorna la musica di sottofondo
        if (stato == MENU) {
            // Controllo se il giocatore clicca sul bottone "Start Game"
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition(); //Posizione del mouse 

                if (CheckCollisionPointRec(mousePos, startButton)) { // Controlla se lo user preme start
                    stato = GAMEPLAY; // Avvia il gioco
                }
                if (CheckCollisionPointRec(mousePos, MusicButton)) {
                    //Se l'utente schiaccia sul pulsante la musica si attiva e disattiva
                    if (IsMusicStreamPlaying(MusicaSottofondo)) {
                        PauseMusicStream(MusicaSottofondo);
                        MusicaAttiva = false;
                        // Cambia i colori del bottone
                        BackgroundMenuMusic = RED;
                        TextMenuMusic = Rossoscuro;
                    }
                    else{
                        ResumeMusicStream(MusicaSottofondo);
                        MusicaAttiva = true;
                        BackgroundMenuMusic = GREEN;
                        TextMenuMusic = DARKGREEN;
                    }                    
                } // Se l'utente schiaccia sul pulsante l'audio si attiva e disattiva
                if (CheckCollisionPointRec(mousePos, SoundButton)) {  
                    if (SuonoAttivo) {
                        SuonoAttivo = false;
                        // Cambia i colori del bottone
                        BackgroundMenuSound = RED;
                        TextMenuSound = Rossoscuro;
                    }
                    else {
                        SuonoAttivo = true;
                        // Cambia i colori del bottone
                        BackgroundMenuSound = GREEN;
                        TextMenuSound = DARKGREEN;
                    }
                }
            }
        }
        else if (stato == GAMEPLAY) {
            //update
            palla1.UpdateBall(); //muovere la palla 
            CheckMovimentoPaddle(Paddle_y); //per muovere il paddle 
            
            MovimentoPaddleAI(PaddleAI_y, PaddleAI_speed, palla1);

            // Controllo se il giocatore clicca sul bottone "PAUSA"
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mousePos = GetMousePosition(); // Posizione del mouse 

                if (CheckCollisionPointRec(mousePos, pauseButton)) { // Controlla se lo user preme Pausa
                    stato = PAUSE; // Mette in pausa il gioco
                }
            }
            PlayWallHit(WallHit, palla1, SuonoAttivo);
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
            
        }
        else if(stato == PAUSE){
            ButtonsPause[0] = { (screenWidth / 2) - (ButtonsPauseWidth / 2), (screenHeight / 3 + 100) - (ButtonsPauseHeight * 2 ), ButtonsPauseWidth, ButtonsPauseHeight};
            ButtonsPause[1] = { (screenWidth / 2) - (ButtonsPauseWidth / 2), ((screenHeight / 3) * 2 + 100)  - (ButtonsPauseHeight * 2 ), ButtonsPauseWidth, ButtonsPauseHeight };
            ButtonsPause[2] = { (screenWidth / 2) - (ButtonsPauseWidth / 2), (screenHeight + 100) - (ButtonsPauseHeight * 2), ButtonsPauseWidth, ButtonsPauseHeight };
        }

        BeginDrawing();

        ClearBackground(cornflowerBlue);

            if (stato == MENU) {
                DrawRectangleRec(MusicButton, BackgroundMenuMusic);
                DrawRectangleRec(SoundButton, BackgroundMenuSound);
                DrawRectangleRec(startButton, RAYWHITE); // Disegna il bottone "Start"
                //Scrivere il testo all'interno di ogni bottone
                DrawText(TextFormat("SOUND: %s", (SuonoAttivo) ? "ON" : "OFF"), SoundButton.x + (SoundButton.width - MeasureText(TextFormat("SOUND: %s", (SuonoAttivo) ? "ON" : "OFF"), 25)) / 2, SoundButton.y + (SoundButton.height - 25) / 2, 25, TextMenuSound);
                DrawText(TextFormat("MUSIC: %s", MusicaAttiva ? "ON" : "OFF"), MusicButton.x + (MusicButton.width - MeasureText(TextFormat("MUSIC: %s", MusicaAttiva ? "ON" : "OFF"), 25)) / 2, MusicButton.y + (MusicButton.height - 25) / 2, 25, TextMenuMusic);
                DrawText("PONG GAME", screenWidth / 2 - MeasureText("PONG GAME", 100) / 2, screenHeight / 2 - 250, 100, DARKBLUE);
                DrawText("Click to Start", (screenWidth / 2) - MeasureText("Click to Start", 30) / 2, screenHeight / 2 + 90, 30, DARKGRAY);               
            }
            else if (stato == GAMEPLAY) {

                DrawRectangle(0, 0, screenWidth / 2, screenHeight, GetColor(0x5077C0FF));
                DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, TextColor); // Mostra gli FPS a schermo

                if ((CheckCollisionCircleRec(palla1.GetCD(), (float)20, PaddleGiocatore)) || (CheckCollisionCircleRec(palla1.GetCD(), (float)20, PaddleAI))) {
                    if (SuonoAttivo) PlaySound(PaddleHitSound);
                    palla1.InvertiVelocitaX(); // Inverte la velocità della palla se c'è una collisione con un paddle
                    palla1.UpdateBall(); // Aggiorna la posizione della palla per evitare sovrapposizioni 
                }
                //drawing objects
                DrawCircle(screenWidth / 2, screenHeight / 2, 105, GetColor(0xAFCBFFFF)); // Cerchio centrale
                DrawCircleLines(screenWidth / 2, screenHeight / 2, 105, BLACK);
                palla1.DrawBall();
                DisegnaPaddle(PaddleAI, PaddleGiocatore, PaddleColor);
                DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, BLACK); // Linea centrale    
                DrawText("YOUR SIDE", (screenWidth / 2) + 25, 50, 25, TextColor);
                DrawText("CPU SIDE", ((screenWidth / 2) - (MeasureText("CPU SIDE", 25) + 25)), 50, 25, TextColor);
                DrawText(TextFormat("%d", PunteggioUser), (screenWidth / 2) + (MeasureText("CPU SIDE", 25) / 2), 10, 30, BLACK);
                DrawText(TextFormat("%d", PunteggioAI), (screenWidth / 2) - ((MeasureText("CPU SIDE", 25) / 2) + MeasureText("0", 30)), 10, 30, BLACK);
                DrawRectangleRec(pauseButton, GetColor(0xAFCBFFFF)); DrawRectangleLinesEx(pauseButton, 0.8, BLACK);
                DrawText("| |", pauseButton.x + (pauseButton.width - MeasureText("| |", 30)) / 2, pauseButton.y + (pauseButton.height - 25) / 2, 30, BLACK);
                MostraTestoGo(showGoText, goTextFrames, goTextDurationFrames, showLightEffect, iniziopartita); //Mostrare il testo GO!
                // Disegna l'effetto di illuminazione se necessario
                LightEffect(lightRect, lightColor, showLightEffect, palla1);
                
            }
            else if (stato == PAUSE) {
                DisegnaMenuPausa(ButtonsPause, GetColor(0x5077C0FF));
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    Vector2 mousePos = GetMousePosition(); //Posizione del mouse 

                    if (CheckCollisionPointRec(mousePos, ButtonsPause[0])) { 
                        stato = GAMEPLAY; // Riprende il gioco
                    }
                    else if (CheckCollisionPointRec(mousePos, ButtonsPause[1])) {
                        stato = MENU; // Torna al menu
                        PunteggioAI = 0; PunteggioUser = 0; // Porta a 0 i punteggi
                        palla1.ResetPartita(); // Resetta la palla al centro
                    }
                    else if (CheckCollisionPointRec(mousePos, ButtonsPause[2])) {
                        CloseWindow(); // Chiude la finestra
                        return 0;
                    }
                }
            }

         EndDrawing();
    }
    UnloadMusicStream(MusicaSottofondo);
    UnloadSound(PaddleHitSound);
    UnloadSound(WallHit);
    CloseAudioDevice();
    // De-Initialization
    //---------------------------------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //---------------------------------------------------------------------------------------------------------------

    return 0;
}
