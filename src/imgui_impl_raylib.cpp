#include "imgui_impl_raylib.h"
#include <raylib.h>
#include <rlgl.h>
#include <memory>

typedef uint32_t Uint32;
typedef uint8_t  Uint8;
typedef uint16_t Uint16;

static double g_Time = 0.0;
static bool g_UnloadAtlas = false;
static int g_AtlasTexID = 0;
static Texture2D *g_FontTexture=nullptr;



static const char* ImGui_ImplRaylib_GetClipboardText(void*)
{
    return GetClipboardText();
}

static void ImGui_ImplRaylib_SetClipboardText(void*, const char* text)
{
    SetClipboardText(text);
}

bool ImGui_ImplRaylib_Init()
{



    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
  //  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();




    io.BackendPlatformName = "imgui_impl_raylib";

    io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_Home] = KEY_HOME;
    io.KeyMap[ImGuiKey_End] = KEY_END;
    io.KeyMap[ImGuiKey_Insert] = KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = KEY_A;
    io.KeyMap[ImGuiKey_C] = KEY_C;
    io.KeyMap[ImGuiKey_V] = KEY_V;
    io.KeyMap[ImGuiKey_X] = KEY_X;
    io.KeyMap[ImGuiKey_Y] = KEY_Y;
    io.KeyMap[ImGuiKey_Z] = KEY_Z;

    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    io.SetClipboardTextFn = ImGui_ImplRaylib_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplRaylib_GetClipboardText;
    io.ClipboardUserData = NULL;


    ImGui_ImplRaylib_LoadDefaultFontAtlas();
//

    return true;
}

void ImGui_ImplRaylib_Shutdown()
{
     if (g_FontTexture)
     {
     UnloadTexture(*g_FontTexture);
     delete g_FontTexture;
     }

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->ClearTexData();
        g_Time = 0.0;
    ImGui::DestroyContext();
}

static void ImGui_ImplRaylib_UpdateMousePosAndButtons()
{
    ImGuiIO& io = ImGui::GetIO();

    // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io.WantSetMousePos)
        SetMousePosition(io.MousePos.x, io.MousePos.y);
    else
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    io.MouseDown[0] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    io.MouseDown[1] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
    io.MouseDown[2] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

    if (!IsWindowMinimized())
        io.MousePos = ImVec2(GetMouseX(), GetMouseY());
}

static void ImGui_ImplRaylib_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        HideCursor();
    }
    else
    {
        // Show OS mouse cursor
        ShowCursor();
    }
}

void ImGui_ImplRaylib_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2((float)GetScreenWidth(), (float)GetScreenHeight());

    double current_time = GetTime();
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    io.KeyCtrl = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    io.KeyShift = IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
    io.KeyAlt = IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT);
    io.KeySuper = IsKeyDown(KEY_RIGHT_SUPER) || IsKeyDown(KEY_LEFT_SUPER);

    ImGui_ImplRaylib_UpdateMousePosAndButtons();
    ImGui_ImplRaylib_UpdateMouseCursor();
    ImGui_ImplRaylib_ProcessEvent();

    if (GetMouseWheelMove() > 0)
        io.MouseWheel += 1;
    else if (GetMouseWheelMove() < 0)
        io.MouseWheel -= 1;
    ImGui::NewFrame();
}

#define FOR_ALL_KEYS(X) \
    X(KEY_APOSTROPHE); \
    X(KEY_COMMA); \
    X(KEY_MINUS); \
    X(KEY_PERIOD); \
    X(KEY_SLASH); \
    X(KEY_ZERO); \
    X(KEY_ONE); \
    X(KEY_TWO); \
    X(KEY_THREE); \
    X(KEY_FOUR); \
    X(KEY_FIVE); \
    X(KEY_SIX); \
    X(KEY_SEVEN); \
    X(KEY_EIGHT); \
    X(KEY_NINE); \
    X(KEY_SEMICOLON); \
    X(KEY_EQUAL); \
    X(KEY_A); \
    X(KEY_B); \
    X(KEY_C); \
    X(KEY_D); \
    X(KEY_E); \
    X(KEY_F); \
    X(KEY_G); \
    X(KEY_H); \
    X(KEY_I); \
    X(KEY_J); \
    X(KEY_K); \
    X(KEY_L); \
    X(KEY_M); \
    X(KEY_N); \
    X(KEY_O); \
    X(KEY_P); \
    X(KEY_Q); \
    X(KEY_R); \
    X(KEY_S); \
    X(KEY_T); \
    X(KEY_U); \
    X(KEY_V); \
    X(KEY_W); \
    X(KEY_X); \
    X(KEY_Y); \
    X(KEY_Z); \
    X(KEY_SPACE); \
    X(KEY_ESCAPE); \
    X(KEY_ENTER); \
    X(KEY_TAB); \
    X(KEY_BACKSPACE); \
    X(KEY_INSERT); \
    X(KEY_DELETE); \
    X(KEY_RIGHT); \
    X(KEY_LEFT); \
    X(KEY_DOWN); \
    X(KEY_UP); \
    X(KEY_PAGE_UP); \
    X(KEY_PAGE_DOWN); \
    X(KEY_HOME); \
    X(KEY_END); \
    X(KEY_CAPS_LOCK); \
    X(KEY_SCROLL_LOCK); \
    X(KEY_NUM_LOCK); \
    X(KEY_PRINT_SCREEN); \
    X(KEY_PAUSE); \
    X(KEY_F1); \
    X(KEY_F2); \
    X(KEY_F3); \
    X(KEY_F4); \
    X(KEY_F5); \
    X(KEY_F6); \
    X(KEY_F7); \
    X(KEY_F8); \
    X(KEY_F9); \
    X(KEY_F10); \
    X(KEY_F11); \
    X(KEY_F12); \
    X(KEY_LEFT_SHIFT); \
    X(KEY_LEFT_CONTROL); \
    X(KEY_LEFT_ALT); \
    X(KEY_LEFT_SUPER); \
    X(KEY_RIGHT_SHIFT); \
    X(KEY_RIGHT_CONTROL); \
    X(KEY_RIGHT_ALT); \
    X(KEY_RIGHT_SUPER); \
    X(KEY_KB_MENU); \
    X(KEY_LEFT_BRACKET); \
    X(KEY_BACKSLASH); \
    X(KEY_RIGHT_BRACKET); \
    X(KEY_GRAVE); \
    X(KEY_KP_0); \
    X(KEY_KP_1); \
    X(KEY_KP_2); \
    X(KEY_KP_3); \
    X(KEY_KP_4); \
    X(KEY_KP_5); \
    X(KEY_KP_6); \
    X(KEY_KP_7); \
    X(KEY_KP_8); \
    X(KEY_KP_9); \
    X(KEY_KP_DECIMAL); \
    X(KEY_KP_DIVIDE); \
    X(KEY_KP_MULTIPLY); \
    X(KEY_KP_SUBTRACT); \
    X(KEY_KP_ADD); \
    X(KEY_KP_ENTER); \
    X(KEY_KP_EQUAL);

#define SET_KEY_DOWN(KEY) io.KeysDown[KEY] = IsKeyDown(KEY)

bool ImGui_ImplRaylib_ProcessEvent()
{
    ImGuiIO& io = ImGui::GetIO();

    FOR_ALL_KEYS(SET_KEY_DOWN);

    if (GetKeyPressed())
    {
    // Uncomment the three lines below if using raylib earlier than version 3.
    //if (GetKeyPressed() != -1)
    //{
//#ifdef ENABLE_SCODETOUTF8
    int length;     //  Length was only ever created to be passed to CodepointToUtf8(), since it doesn't check for nullptrs.
    io.AddInputCharactersUTF8(CodepointToUTF8(GetCharPressed(), &length));
   // (void)length;   //  Silencing the compiler warnings.
//#else
   // io.AddInputCharacter(GetKeyPressed());
//#endif
    //}
    }

    return true;
}


void ImGui_ImplRaylib_LoadDefaultFontAtlas()
{

    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
    g_FontTexture = new Texture2D();
    g_FontTexture->width = width;
    g_FontTexture->height = height;
    g_FontTexture->mipmaps = 1;
    g_FontTexture->format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    g_FontTexture->id= rlLoadTexture(pixels,width,height,PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,1);
    io.Fonts->TexID = (void*)g_FontTexture;
/*
    if (!g_UnloadAtlas)
    {
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels = NULL;
        int width, height, bpp;
        Image image;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bpp);

        image = GenImageColor(width,height,BLACK);


        unsigned int size = GetPixelDataSize(width, height, 7);
        image.data = malloc(size);
        memcpy(image.data, pixels, size);
        image.width = width;
        image.height = height;
        image.mipmaps = 1;
        image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

      //  ExportImage(image,"font.png");



        Texture2D tex = LoadTextureFromImage(image);
        //UpdateTexture(tex,pixels);

		g_AtlasTexID = tex.id;
		//io.Fonts->TexID = (void*)&g_AtlasTexID;

        io.Fonts->SetTexID((ImTextureID)(intptr_t)g_AtlasTexID);

        free(pixels);
        free(image.data);
        g_UnloadAtlas = true;
    }


*/

};


int RenderTriGeometry(const float *xy, int xy_stride,
                         const Color *color, int color_stride,
                         const float *uv, int uv_stride,
                         int num_vertices,
                         const void *indices, int num_indices, int size_indices)
{
int count = indices ? num_indices : num_vertices;
rlPushMatrix();
rlBegin(RL_TRIANGLES);
rlSetTexture(g_FontTexture->id);

    for (int i=0;i< count; i+=3)
    {

           int k0, k1, k2;

            k0 = ((const Uint16 *)indices)[i + 0];
            k1 = ((const Uint16 *)indices)[i + 1];
            k2 = ((const Uint16 *)indices)[i + 2];


            const float *xy0, *xy1, *xy2;
            const float *uv0, *uv1, *uv2;


            Color col0 = *(const Color *)((const char*)color + k0 * color_stride);
            Color col1 = *(const Color *)((const char*)color + k1 * color_stride);
            Color col2 = *(const Color *)((const char*)color + k2 * color_stride);

            xy0 = (const float *)((const char*)xy + k0 * xy_stride);
            xy1 = (const float *)((const char*)xy + k1 * xy_stride);
            xy2 = (const float *)((const char*)xy + k2 * xy_stride);

            uv0 = (const float *)((const char*)uv + k0 * uv_stride);
            uv1 = (const float *)((const char*)uv + k1 * uv_stride);
            uv2 = (const float *)((const char*)uv + k2 * uv_stride);




        rlNormal3f(0.0f, 0.0f, 1.0f);


        rlColor4ub(col0.r, col0.g, col0.b, col0.a);
        rlTexCoord2f(uv0[0],uv0[1]);
        rlVertex2f(xy0[0],xy0[1]);

        rlColor4ub(col1.r, col1.g, col1.b, col1.a);
        rlTexCoord2f(uv1[0],uv1[1]);
        rlVertex2f(xy1[0],xy1[1]);

        rlColor4ub(col2.r, col2.g, col2.b, col2.a);
        rlTexCoord2f(uv2[0],uv2[1]);
        rlVertex2f(xy2[0],xy2[1]);
}

rlEnd();
rlPopMatrix();
}




void ImGui_ImplRaylib_Render()
{
 ImGui::Render();
 ImDrawData* draw_data=ImGui::GetDrawData();

//rlEnableScissorTest();
rlDisableBackfaceCulling();
ImGuiIO& io = ImGui::GetIO();

    float rsx = 1.0f;
	float rsy = 1.0f;
    ImVec2 render_scale;
	render_scale.x = (rsx == 1.0f) ? draw_data->FramebufferScale.x : 1.0f;
	render_scale.y = (rsy == 1.0f) ? draw_data->FramebufferScale.y : 1.0f;

	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	int fb_width = (int)(draw_data->DisplaySize.x * render_scale.x);
	int fb_height = (int)(draw_data->DisplaySize.y * render_scale.y);
	if (fb_width == 0 || fb_height == 0)
		return;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    {
                    }
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                const bool isWrappedTexture = pcmd->TextureId == io.Fonts->TexID;



                ImVec2 pos = draw_data->DisplayPos;
                int rectX = (int)(pcmd->ClipRect.x - pos.x);
                int rectY = (int)(pcmd->ClipRect.y - pos.y);
                int rectW = (int)(pcmd->ClipRect.z - rectX);
                int rectH = (int)(pcmd->ClipRect.w - rectY);
                BeginScissorMode(rectX, rectY, rectW, rectH);





                const float* xy = (const float*)((const char*)(vtx_buffer + pcmd->VtxOffset) + IM_OFFSETOF(ImDrawVert, pos));
                const float* uv = (const float*)((const char*)(vtx_buffer + pcmd->VtxOffset) + IM_OFFSETOF(ImDrawVert, uv));
                const Color* color = (const Color*)((const char*)(vtx_buffer + pcmd->VtxOffset) + IM_OFFSETOF(ImDrawVert, col));


                RenderTriGeometry(                   xy, (int)sizeof(ImDrawVert),
                    color, (int)sizeof(ImDrawVert),
                    uv, (int)sizeof(ImDrawVert),
                    cmd_list->VtxBuffer.Size - pcmd->VtxOffset,
                    idx_buffer + pcmd->IdxOffset, pcmd->ElemCount, sizeof(ImDrawIdx));

            }
        }
    }
EndScissorMode();
//rlDisableScissorTest();
rlEnableBackfaceCulling();
rlSetTexture(0);


}

