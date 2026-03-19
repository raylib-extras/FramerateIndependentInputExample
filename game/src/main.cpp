#include "raylib.h"
#include "raymath.h"

#include <vector>
#include <set>

// structures for storing input events
struct KeyboardEvent
{
    int Key = KEY_NULL;
    bool Pressed = false;
    double Time = 0;
};

struct MouseButtonEvent
{
    int Button = MOUSE_BUTTON_LEFT;
	bool Pressed = false;
    double Time = 0;
};

// current event queues
std::vector<KeyboardEvent> KeyEvents;
std::vector<MouseButtonEvent> MouseButtonEvents;

// sets to know what keys are down to check for up events
std::set<int> KeysDown;

// the current agrigate mouse delta
Vector2 MouseDelta = { 0.0f, 0.0f };


// something to draw
Matrix CubeTransform = MatrixIdentity();
Mesh CubeMesh = { 0 };
Material CubeMaterial = { 0 };

Camera3D ViewCamera = { 0 };


// Drawing accumulator at the render FPS
double Accumulator = 0.0f;
double FPSTime = 0.0f;

// double precision delta time for high FPS loop
double HighResLastTime = 0.0;
double HighResDeltaTime = 0.0;
double FrameStartTime = 0.0;

// FPS of the input loop
double InputFrameTime = 0.0f;

// should we draw this frame
bool DrawThisFrame = false;

// how many fixed tick updates are we drawing (low FPS can make this more than one tick)
double RenderFrameDelta = 0;


// High Res FPS timer functions
void UpdateFrameTime()
{
    FrameStartTime = GetTime();
	HighResDeltaTime = FrameStartTime - HighResLastTime;
	HighResLastTime = FrameStartTime;
}

double GetHighResDeltaTime()
{
    return HighResDeltaTime;
}

void GameInit()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 800, "Example");

    // Your Time Starts Now
    HighResLastTime = GetTime();

    // compute the nominal FPS of the current monitor
    FPSTime = 1.0 / double(GetMonitorRefreshRate(GetCurrentMonitor()));
	Accumulator = FPSTime;

    // camera for drawing
    ViewCamera.fovy = 45.0f;
    ViewCamera.position = { 3.0f, 3.0f, 3.0f };
    ViewCamera.up = { 0.0f, 1.0f, 0.0f };
    ViewCamera.target = { 0.0f, 0.0f, 0.0f };

    // load geometry and textures
    CubeMesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    CubeMaterial = LoadMaterialDefault();
    CubeMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("resources/texture_01.png");

    GenTextureMipmaps(&CubeMaterial.maps[MATERIAL_MAP_DIFFUSE].texture);
    SetTextureFilter(CubeMaterial.maps[MATERIAL_MAP_DIFFUSE].texture, TEXTURE_FILTER_TRILINEAR);
}

void GameCleanup()
{
    // unload resources
    UnloadMesh(CubeMesh);
    UnloadTexture(CubeMaterial.maps[MATERIAL_MAP_DIFFUSE].texture);

    CloseWindow();
}

// what direction to rotate
static float RotationDir = 1;

void ProcessFixedTickUpdate(float deltaTime)
{
    // walk the keyboard events, for every keydown event reverse direction, even if there are more than one for this fixed tick
    for (auto& key : KeyEvents)
    {
        switch (key.Key)
        {
        case KEY_SPACE:
            RotationDir *= -1;
            break;
        }
    }

    // update the cube only on fixed ticks
    Matrix rotation = MatrixRotateXYZ({ 0.0f, deltaTime * RotationDir, 0.0f });
    CubeTransform = MatrixMultiply(CubeTransform, rotation);
}

bool GameUpdate()
{
    // called every real frame (as fast as possible)

    DrawThisFrame = false;
    RenderFrameDelta = 0;

    // update any input and OS events
    PollInputEvents();

    // see if any pressed keys are now up
    for (auto itr = KeysDown.begin(); itr != KeysDown.end();)
    {
        if (IsKeyUp(*itr))
        {
            // record a key up event
            KeyEvents.push_back({ *itr, false, FrameStartTime });
            itr = KeysDown.erase(itr);
        }
        else
        {
            itr++;
        }
    }

    // find out what keys are pressed this frame and set events
    while (true)
	{
        auto key = GetKeyPressed();
        if (key == 0)
            break;

        KeyEvents.push_back({ key, true, FrameStartTime });
        KeysDown.insert(key);
    }

    // see process mouse button events
    for (int i = MOUSE_BUTTON_LEFT; i <= MOUSE_BUTTON_BACK; i++)
    {
        if (IsMouseButtonPressed(i))
        {
            MouseButtonEvents.push_back({ i, true, FrameStartTime });
        }
        else if (IsMouseButtonReleased(i))
        {
            MouseButtonEvents.push_back({ i, false, FrameStartTime });
        }
    }

    // accumulate mouse motion for the next fixed tick
    MouseDelta += GetMouseDelta();

    // increment accumulator
	Accumulator += GetHighResDeltaTime();

	while (Accumulator >= FPSTime)
	{
        // we have to do a tick, so process it
		RenderFrameDelta += FPSTime;
		Accumulator -= FPSTime;
        ProcessFixedTickUpdate(float(FPSTime));

        // assume the first tick will process all accumulated input events, very low FPS means that you can tick more than ones per loop
		KeyEvents.clear();
		MouseButtonEvents.clear();
        MouseDelta = Vector2Zeros;

        // we ticked this frame so update the rendering
		DrawThisFrame = true;
	}

    return true;
}

void GameDraw()
{
    // If we did not tick, then no need to render
    if (!DrawThisFrame)
        return;

    // process drawing
    BeginDrawing();
    ClearBackground(GRAY);

    BeginMode3D(ViewCamera);
    DrawMesh(CubeMesh, CubeMaterial, CubeTransform);
    EndMode3D();

    DrawText(TextFormat("Render FPS %0.1f", 1.0f / float(RenderFrameDelta)), 10, 10, 20, GREEN);
    DrawText(TextFormat("Input FPS %0.1f", float(1.0f / InputFrameTime)), 10, 30, 20, GREEN);
    EndDrawing();

    SwapScreenBuffer(); // flush the draw calls and display this buffer (will wait for vsync)
}

int main()
{
    GameInit();

    // store a frame rate timer for the actual high speed loop
    double lastFrameTime = GetTime();

    while (!WindowShouldClose())
    {
		UpdateFrameTime();

        double now = GetTime();
		InputFrameTime = now - lastFrameTime;
        lastFrameTime = now;

        if (!GameUpdate())
            break;

        GameDraw();
    }
    GameCleanup();

    return 0;
}