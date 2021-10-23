#include "raylib.h"

struct AnimData
{
    Rectangle Rec;
    Vector2 Pos;
    float Frame;
    float UpdateTime;
    float RunningTime;
};

bool IsOnGround(AnimData Data, int WindowHeight)
{
    return Data.Pos.y >= WindowHeight - Data.Rec.height;
}

AnimData UpdateAnimData(AnimData data, float DeltaTime, int MaxFrame)
{
    //Update Running time
    data.RunningTime += DeltaTime;
    if (data.RunningTime >= data.UpdateTime)
    {
        data.RunningTime = 0.0;
        //Update animation frame
        data.Rec.x = data.Frame * data.Rec.width;
        data.Frame++;
        if (data.Frame > MaxFrame)
        {
            data.Frame = 0;
        }
        
    }
    return data; 
}

int main()
{
    //Window dimensions
    int WindowDimensions [2];
    WindowDimensions [0] = 512;
    WindowDimensions [1] = 380;
    //Initialize/draw window
    InitWindow(WindowDimensions[0], WindowDimensions[1], "Dapper Dasher");

    //Nebula hazard
    Texture2D Nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int SizeOfNebulae{10};

    //Nebula Animdata
    AnimData Nebulae[SizeOfNebulae]{};

    for (int i = 0; i < SizeOfNebulae; i++)
    {
        Nebulae[i].Rec.x = 0.0;
        Nebulae[i].Rec.y = 0.0;
        Nebulae[i].Rec.width = Nebula.width/8;
        Nebulae[i].Rec.height = Nebula.height/8;
        Nebulae[i].Pos.y = WindowDimensions[1] - Nebula.width/8;
        Nebulae[i].Frame = 0;
        Nebulae[i].RunningTime = 0.0;
        Nebulae[i].UpdateTime = 0.0;
        Nebulae[i].Pos.x = WindowDimensions[0] + i * 300;
    }

    //Finish line condition 
    float FinishLine{Nebulae[SizeOfNebulae - 1].Pos.x};

    //Nebula x velocity (pixels/s)
    int NebVel{-200};

    //Character  
    Texture2D Scarfy = LoadTexture("textures/scarfy.png");
    AnimData ScarfyData;
    ScarfyData.Rec.width = Scarfy.width/6;
    ScarfyData.Rec.height = Scarfy.height;
    ScarfyData.Rec.x = 0;
    ScarfyData.Rec.y = 0;
    ScarfyData.Pos.x = WindowDimensions[0] /2 - ScarfyData.Rec.width/2;
    ScarfyData.Pos.y = WindowDimensions[1] - ScarfyData.Rec.height;
    ScarfyData.Frame = 0;
    ScarfyData.UpdateTime = 1.0/12.0;
    ScarfyData.RunningTime = 0.0;


    //Background textures
    Texture2D Background = LoadTexture("textures/far-buildings.png"); 
    float BgX{0};
    Texture2D MidGround = LoadTexture("textures/back-buildings.png");
    float MgX{0};
    Texture2D ForeGround = LoadTexture("textures/foreground.png");
    float FgX{0};
    
    //Physics mechanics
    int Velocity{0};
    //Accelerations due to gravity (pixels/Second)/Second
    const int Gravity{1'000};
    //Is the character in the air? 
    bool IsInAir{};
    //Pixels/Second
    const int JumpVelocity{-600};
    //Collision check 
    bool Collision{};
    //Target framerate
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        //Delta time (Time since last frame)
        const float dT{GetFrameTime()};
        //Start drawing 
        BeginDrawing();
        ClearBackground(WHITE);

        //Scroll Background
        BgX -= 20 * dT;
        if (BgX <= -Background.width*2)
        {
            BgX = 0.0;
        }
        //Scroll Midground
        MgX -= 40 * dT;
        if (MgX <= -MidGround.width*2)
        {
            MgX = 0.0;
        }
        //Scroll foreground
        FgX -= 80 * dT;
        if (FgX <= -ForeGround.width*2)
        {
            FgX = 0.0;
        }
        
        
        Vector2 BgPos{BgX, 0.0};
        DrawTextureEx(Background, BgPos, 0.0, 2.0, WHITE);
        Vector2 BgPos2{BgX + Background.width*2, 0.0};
        DrawTextureEx(Background, BgPos2, 0.0, 2.0, WHITE);

        Vector2 MgPos{MgX, 0.0};
        DrawTextureEx(MidGround, MgPos, 0.0, 2.0, WHITE);
        Vector2 MgPos2{MgX + MidGround.width*2, 0.0};
        DrawTextureEx(MidGround, MgPos2, 0.0, 2.0, WHITE);

        Vector2 FgPos{FgX, 0.0};
        DrawTextureEx(ForeGround, FgPos, 0.0, 2.0, WHITE);
        Vector2 FgPos2{FgX + ForeGround.width*2, 0.0};
        DrawTextureEx(ForeGround, FgPos2, 0.0, 2.0, WHITE);
           

        //Perform ground check
        if (IsOnGround(ScarfyData, WindowDimensions[1]))
        {
            //Rectangle is on the ground
            Velocity = 0;
            IsInAir = false;
        }

        else
        {
            //Rectangle is on the air
            Velocity += Gravity * dT;
            IsInAir = true;
        }
        
        //Jumping check
        if (IsKeyPressed(KEY_SPACE) && !IsInAir)
        {
            Velocity += JumpVelocity; 
        }

        for (int i = 0; i < SizeOfNebulae; i++)
        {   //Update position of each nebula
            Nebulae[i].Pos.x += NebVel * dT;
        }  

        //Update Finish line
        FinishLine += NebVel * dT;

         //Update character position 
        ScarfyData.Pos.y += Velocity * dT;
        //Update character animation frame 
        if (!IsInAir)
        {
            ScarfyData = UpdateAnimData(ScarfyData, dT, 5);
        }

        for (int i = 0; i < SizeOfNebulae; i++)
        {

            Nebulae[i] = UpdateAnimData(Nebulae[i], dT, 7);
        }
        
        for (AnimData Nebula : Nebulae) 
        {
            float Pad{50};
            Rectangle NebRec 
            {
                Nebula.Pos.x + Pad,
                Nebula.Pos.y + Pad,
                Nebula.Rec.width - 2*Pad,
                Nebula.Rec.height - 2*Pad
            };
            Rectangle ScarfyRec
            {
                ScarfyData.Pos.x,
                ScarfyData.Pos.y,
                ScarfyData.Rec.width,
                ScarfyData.Rec.height
            };
            if (CheckCollisionRecs(NebRec, ScarfyRec))
            {
                Collision = true;
            }
        }

        if (Collision)
        {
            //Loosing the game
            DrawText("GAME OVER", WindowDimensions[0]/4, WindowDimensions[1]/2, 40, RED);
        }
        else if (ScarfyData.Pos.x >= FinishLine)
        {
            //Win the game
            DrawText("YOU WIN", WindowDimensions[0]/3, WindowDimensions[1]/2, 40, YELLOW);
        }
        else 
        {
            //Draw Nebula
            for(int i = 0; i < SizeOfNebulae; i++)
            {
                DrawTextureRec(Nebula, Nebulae[i].Rec, Nebulae[i].Pos, WHITE);
            }
            //Draw Character 
            DrawTextureRec(Scarfy, ScarfyData.Rec, ScarfyData.Pos, WHITE);
        }

        //Stop drawing
        EndDrawing();
    }
    UnloadTexture(Scarfy);
    UnloadTexture(Nebula);
    UnloadTexture(Background);
    UnloadTexture(MidGround);
    UnloadTexture(ForeGround);
    CloseWindow();
}