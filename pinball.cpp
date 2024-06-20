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

#define RLIGHTS_IMPLEMENTATION
#include "pinball.h"
#include "raylib.h"
#include "raymath.h"
#include "out/build/x64-debug/_deps/raylib-src/examples/shaders/rlights.h"
#include <vector>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

#define vec3addeq(v,w) { v.x += w.x; v.y += w.y; v.z += w.z; }
#define vec3mulfeq(v,w) { v.x *= w; v.y *= w; v.z *= w; }
#define vec3divfeq(v,w) { v.x /= w; v.y /= w; v.z /= w; }

int main()
{
    time_t beginTime = std::clock();
    SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)
    InitWindow(800, 600, "Pinball Game :D");

    SetTargetFPS(60);

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = Vector3{ 0.0f, 30.0f, 30.0f };  // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    // set board tilt
    float BOARD_TILT = 6.5f;

    // Load basic lighting shader
    Shader shader = LoadShader(TextFormat("./_deps/raylib-src/examples/shaders/resources/shaders/glsl%i/lighting.vs", GLSL_VERSION),
        TextFormat("./_deps/raylib-src/examples/shaders/resources/shaders/glsl%i/lighting.fs", GLSL_VERSION));
    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4])( 0.1f,0.1f,0.1f, 1.0f ), SHADER_UNIFORM_VEC4);

    // Create lights
    Light lights[MAX_LIGHTS];
    lights[0] = CreateLight(LIGHT_POINT, Vector3( - 2, 1, -2 ), Vector3Zero(), YELLOW, shader);
    lights[1] = CreateLight(LIGHT_POINT, Vector3( 2, 1, 2 ), Vector3Zero(), RED, shader);
    lights[2] = CreateLight(LIGHT_POINT, Vector3( - 2, 1, 2 ), Vector3Zero(), GREEN, shader);
    lights[3] = CreateLight(LIGHT_POINT, Vector3( 2, 1, -2 ), Vector3Zero(), BLUE, shader);

    Model flipper_L = LoadModel("./assets/flipper_L.glb");
    Model flipper_R = LoadModel("./assets/flipper_R.glb");
    Mesh boardMesh = GenMeshCube(40, 20, 1);
    Mesh ballMesh = GenMeshSphere(0.4f, 32, 32);

    Model board = LoadModelFromMesh(boardMesh);
    Model ball = LoadModelFromMesh(ballMesh);
    Vector3 ballPosition = Vector3(0, 25, 0);
    Vector3 ballVelocity = Vector3Zero();
    Vector3 ballAcceleration = Vector3(0, -1.0f, 0);
    board.transform = MatrixRotateXYZ(Vector3(DEG2RAD * (90 + BOARD_TILT), 0.0f, 90*DEG2RAD));
    for (size_t i = 0; i < board.meshes[0].vertexCount; i += 3)
    {
        printf("Vert: X:%f, Y:%f, Z:%f\n", board.meshes[0].vertices[i], board.meshes[0].vertices[i + 1], board.meshes[0].vertices[i + 2]);
    }

    flipper_L.transform = MatrixRotateXYZ(Vector3(DEG2RAD * (90 + BOARD_TILT), 0.0f, 0.0f));
    flipper_R.transform = MatrixRotateXYZ(Vector3(DEG2RAD * (90 + BOARD_TILT), 0.0f, 0.0f));

    time_t startTime = 0;
    time_t endTime = 0;
    time_t deltaTime = 0;

    Ray groundRay{};
    groundRay.direction = Vector3(0, -1, 0);
    RayCollision hitLevel{};
    time_t dropTime = 0;
    bool collided = false;
    time_t collTime = 0;

    while (!WindowShouldClose())
    {

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            flipper_L.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90 + (BOARD_TILT * DEG2RAD), 0.0f, DEG2RAD * -45.0f));
        }
        else
        {
            flipper_L.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90 + (BOARD_TILT * DEG2RAD), 0.0f, 0.0f));
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            flipper_R.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90 + (BOARD_TILT * DEG2RAD), 0.0f, DEG2RAD * 45.0f));
        }
        else
        {
            flipper_R.transform = MatrixRotateXYZ(Vector3(DEG2RAD * 90 + (BOARD_TILT * DEG2RAD), 0.0f, 0.0f));
        }

        camera.target = ballPosition;

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        BeginShaderMode(shader);
        DrawModel(board, Vector3(0,0,-5), 1.0f, BROWN);
        DrawModel(flipper_L, Vector3(-3.0f, 1.0f, 12.0f),1,WHITE);
        DrawModel(flipper_R, Vector3(3.0f, 1.0f, 12.0f), 1, WHITE);
        DrawModel(ball, ballPosition, 1, GRAY);
        EndShaderMode();

        EndMode3D();

        DrawText("Welcome to the pinball dimension!", 10, 40, 20, DARKGRAY);
        DrawText(TextFormat("Current Board Tilt: %f", BOARD_TILT), 10, 60, 20, GREEN);

        DrawFPS(10, 10);

        EndDrawing();
        
        // DELTA TIME PART 2
        // -----------------
        endTime = std::clock();
        deltaTime = endTime - startTime;

        // PHYSICS
        // -------
        printf("e");
        groundRay.position = ballPosition;
        hitLevel = GetRayCollisionMesh(groundRay, boardMesh, board.transform);
        if (hitLevel.hit)
        {
            // 0.4 is radius of the ball
            if (hitLevel.distance <= 1.0f)
            {
                printf("Collided: %d", std::clock() - collTime);
                // hit ground
                ballVelocity = Vector3(0, 0.11320321*0.5, 0.99357186*0.5);
                vec3divfeq(ballVelocity, deltaTime/128);
                vec3addeq(ballPosition, ballVelocity);
            }
            else
            {
                if (!collided)
                {
                    collTime = std::clock();
                    collided = true;
                }
                vec3addeq(ballVelocity, ballAcceleration);
                vec3divfeq(ballVelocity, deltaTime/128);
                vec3addeq(ballPosition, ballVelocity);
            }

        }
        if (ballVelocity.x > 10)
        {
            ballVelocity.x = 10;
        }
        if (ballVelocity.y > 10)
        {
            ballVelocity.y = 10;
        }
        if (ballVelocity.z > 10)
        {
            ballVelocity.z = 10;
        }

    }

    UnloadShader(shader);   // Unload shader

    CloseWindow();
    return 0;
}
