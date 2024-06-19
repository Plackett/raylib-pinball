//MIT License
//
//Copyright(c) 2024 Plackett
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "pinball.h"
#include "raylib.h"
#include "raymath.h"

int main()
{
    InitWindow(800, 600, "Pinball Game :D");
    SetTargetFPS(60);

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = Vector3{ 0.0f, 10.0f, 10.0f };  // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type


    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

    Model flipper_L = LoadModel("./assets/flipper_L.glb");
    Model flipper_R = LoadModel("./assets/flipper_R.glb");

    flipper_L.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90,0.0f,0.0f));
    flipper_R.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90, 0.0f, 0.0f));

    while (!WindowShouldClose())
    {

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            flipper_L.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90, 0.0f, DEG2RAD * -45.0f));
        }
        else
        {
            flipper_L.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90, 0.0f, 0.0f));
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            flipper_R.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90, 0.0f, DEG2RAD * 45.0f));
        }
        else
        {
            flipper_R.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90, 0.0f, 0.0f));
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(flipper_L, Vector3(-4.0f, 0.0f,0.0f),1,WHITE);
        DrawModel(flipper_R, Vector3(4.0f, 0.0f, 0.0f), 1, WHITE);

        EndMode3D();

        DrawText("Welcome to the pinball dimension!", 10, 40, 20, DARKGRAY);

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
