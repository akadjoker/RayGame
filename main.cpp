#include <iostream>
#include <fstream>
#include <streambuf>
#include "global.h"

#include "imgui.h"
#include "imgui_impl_raylib.h"
#include "TextEditor.h"

#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

const int screenWidth = 1624;
const int screenHeight = 868;



const char* EditorModeName[] = {"NORMAL", "RESIZE", "MOVE","PLACE","ROTATE"};
EditorMode mode = MODE_NORMAL;
Vector2 lastMousePosition = {0,0};

float c_angle = 0;
float q_angle = 0;


static int idCound=0;

class Entity
 {
public:
	Entity()
	{
	 position.x=0;
	 position.y=0;
	 rotation=0;
	 xscale=1;
	 yscale=1;
	 width=25;
	 height=25;
	 name ="Entity";
	 Id=idCound;
	 idCound++;
	 pivot.x=0;
	 pivot.y=0;
	 bbReset=false;
	 isPrefab=false;
	 isRigidBody=false;
	}
	virtual ~Entity()
	{
	}

	void UpdateBound()
	{
         x1=position.x;
         y1=position.y;
         x2=position.x+(width*xscale);
         y2=position.y+(height*yscale);
	}

	void Update()
	{
	//DrawRectangle((int)position.x,(int)position.y,(int)width*xscale,(int)height*yscale,WHITE);

        bbReset=true;

	    const auto cost = cosf(rotation*DEG2RAD);
        const auto sint = sinf(rotation*DEG2RAD);



        const auto tx1 = -pivot.x * xscale;
        const auto ty1 = -pivot.y * yscale;
        const auto tx2 = (width - pivot.x) * xscale;
        const auto ty2 = (height - pivot.y) * yscale;


        float x1= tx1 * cost - ty1 * sint + position.x;
        float y1= tx1 * sint + ty1 * cost + position.y;

        float x2=tx2 * cost - ty1 * sint + position.x;
        float y2=tx2 * sint + ty1 * cost + position.y;

        float x3= tx2 * cost - ty2 * sint + position.x;
        float y3= tx2 * sint + ty2 * cost + position.y;

        float x4=tx1 * cost - ty2 * sint + position.x;
        float y4=tx1 * sint + ty2 * cost + position.y;





        Encapsulate(tx1 * cost - ty1 * sint + position.x, tx1 * sint + ty1 * cost + position.y);
        Encapsulate(tx2 * cost - ty1 * sint + position.x, tx2 * sint + ty1 * cost + position.y);
        Encapsulate(tx2 * cost - ty2 * sint + position.x, tx2 * sint + ty2 * cost + position.y);
        Encapsulate(tx1 * cost - ty2 * sint + position.x, tx1 * sint + ty2 * cost + position.y);




        DrawLine(x1,y1,x2,y2,RED);
        DrawLine(x1,y1,x4,y4,RED);
        DrawLine(x3,y3,x4,y4,RED);
        DrawLine(x2,y2,x3,y3,RED);
        DrawCircleLines(x1,y1,2,RED);
        DrawCircleLines(x2,y2,2,RED);
        DrawCircleLines(x3,y3,2,RED);
        DrawCircleLines(x4,y4,2,RED);

        DrawLine(x1,y2/2,x2,y1,LIME);



      //   DrawRectangleLinesEx(bound,1,RED);



	}

	void Encapsulate(float x, float y)
    {
        if(bbReset)
        {
            x1=x2=x;
            y1=y2=y;
            bbReset=false;
        }
        else
        {
            if(x<x1) x1=x;
            if(x>x2) x2=x;
            if(y<y1) y1=y;
            if(y>y2) y2=y;

         bound.x= x1;
         bound.y= y1;
         bound.width =(x2-x1);
         bound.height=(y2-y1);
        }
    }

	bool	bbReset;
    Vector2 position;
    float rotation;
    float xscale;
    float yscale;
    float width;
    float height;
    Vector2 pivot;
    Rectangle bound;
    int Id;
    std::string name;
    float	x1, y1, x2, y2;

    bool isPrefab;
    bool isRigidBody;



private:

};

std::vector<Entity*> m_entities;


void AddEntity(Entity* e)
{
m_entities.emplace_back(e);
}

void RemoveEntity(Entity* e)
{
//auto end_iter = std::remove(m_entities.begin(), m_entities.end(), e);
m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), e), m_entities.end());
delete e;

}

bool EntityEists(Entity* n)
{
if (!n) return false;
    if (std::find(m_entities.begin(), m_entities.end(), n) != m_entities.end())
       {
          return true;
       }
    else
    {
       return false;
    }
}

void UpdateEntitys()
{
    for(auto& child : m_entities)
    {
        if(child)
        {
            child->Update();
        }
    }
}
Entity* getEntityByName(std::string name)
{
        for(int j=0; j < m_entities.size(); j++)
{
         if (m_entities[j]->name.compare(name))
         {
             return m_entities[j];
         }
}
    return nullptr;
}


Entity* getEntityByID(int id)
{
 for(int j=0; j < m_entities.size(); j++)
{
         if (m_entities[j]->Id==id)
         {
             return m_entities[j];
         }
}
    return nullptr;
}

Entity* PickEntity(Vector2 mouse)
{
 for(int j=0; j < m_entities.size(); j++)
{
         if ( CheckCollisionPointRec(mouse,m_entities[j]->bound))
         {
             return m_entities[j];
         }
}
    return nullptr;
}


void ClearEntitys()
{
    while (!m_entities.empty())
          {
            Entity* t = m_entities.back();
            m_entities.pop_back();
            delete t;
        }
}
Rectangle viewBond;
void createObject(int x, int y)
{
Entity* e = new  Entity();
e->position.x= x;
e->position.y= y;

e->width  = 50;
e->height = 50;
e->pivot.x =25;
e->pivot.y =25;
e->UpdateBound();

AddEntity(e);
}

void DrawVec3Control(const std::string& label, Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label.c_str());
	ImGui::NextColumn();

	//ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
//	ImGui::PushItemsWidths(3, ImGui::CalcItemWidth());
//	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = 2;//ImGui->Font->FontSize + ImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	//ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

}

int snapDistance = 5;
bool snap = true;
Texture2D texture;



const Color gridLineColor[2] =
{
	(Color){ 120, 120, 120, 25 },
	(Color){ 120, 120, 120, 50 }
};
 void DrawSnapGrid(int size, Color c)
{

	int sq = (screenWidth>screenHeight) ? screenWidth/size : screenHeight/size;
	for(int i=0; i<sq; ++i)
	{
		DrawLine(0, i*size, screenWidth, i*size,  c);
		DrawLine(i*size, 0, i*size, screenHeight, c);
	}
}


int main()
{
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - raylib logo using shapes");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second



    ImGui_ImplRaylib_Init();


    bool  showFps=true;
    Vector2 lastMouse;
    Vector2 mouse;
    int current_node_idx =0;
    int current_prefabe_idx =0;
    Entity *selectEntity=nullptr;
    bool editMode=false;
    int gameWidth = 640;
    int gameHeight=480;
    bool usePyshics=false;
    bool showLines=true;
    bool showPhysicsLines=true;

    std::string selectName;
    char namedef[] = "Entity";

    viewBond=(Rectangle){220,100,screenWidth-500,screenHeight-200};

    createObject(viewBond.x+10,viewBond.y+100);
    createObject(viewBond.x+100,viewBond.y+300);


float rotation;
float x;
float y;

 bool isMove=false;
 bool isRotate=false;
 bool isScale=false;
 bool isSelect=false;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

            viewBond=(Rectangle){220,100,screenWidth-500,screenHeight-200};

        BeginDrawing();

            ClearBackground(BLACK);


	        lastMouse = GetMousePosition();
	        if (CheckCollisionPointRec(lastMouse,viewBond))
	        {
                mouse.x=lastMouse.x;
                mouse.y=lastMouse.y;
                editMode = true;
	        } else
	        {
	        editMode = false;
	        }



        ImGui_ImplRaylib_NewFrame();



ImGui::SetNextWindowPos(ImVec2(220, 35));
ImGui::SetNextWindowSize(ImVec2(screenWidth-500, 52));
ImGui::Begin("Tool", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus );

ImGui::End();

ImGui::SetNextWindowPos(ImVec2(20, 740));
ImGui::SetNextWindowSize(ImVec2(150, 82));
ImGui::Begin("Snap", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus );
ImGui::Checkbox("Enable",&snap);
ImGui::SliderInt("Size",&snapDistance,1,15,"%d");
ImGui::End();

{//MENU***************************************************************************************************************************************
//ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar  | ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_MenuBar);
ImGui::SetNextWindowPos(ImVec2(0, 0));
ImGui::SetNextWindowSize(ImVec2(screenWidth, 1));
ImGui::Begin("MainMenu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);
if (ImGui::BeginMenuBar())
{
    if (ImGui::BeginMenu("File"))
    {
            ImGui::MenuItem("New");
            ImGui::MenuItem("Save");
            ImGui::MenuItem("Load");
            ImGui::MenuItem("Exit");
            ImGui::EndMenu();
    }

     if (ImGui::BeginMenu("Scene"))
    {
            ImGui::MenuItem("GameObject");
            if (ImGui::MenuItem("Deselect"))
            {
                current_node_idx =0;
                selectEntity=nullptr;
            }
            ImGui::EndMenu();
    }

     if (ImGui::BeginMenu("GameObject"))
    {
            ImGui::MenuItem("test1_1");
            ImGui::MenuItem("test1_2");
            ImGui::EndMenu();
    }
       if (ImGui::BeginMenu("Windows"))
    {
            if (showFps)
            {
             if (ImGui::MenuItem("Hide Fps"))
                showFps = false;
            } else
            {
             if (ImGui::MenuItem("Show Fps"))
                showFps = true;
            }


            ImGui::MenuItem("test1_2");
            ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
}

ImGui::End();
}

{//SCENE***************************************************************************************************************************************
ImGui::SetNextWindowPos(ImVec2(4, 50));
ImGui::SetNextWindowSize(ImVec2(200, (screenHeight)-200));
ImGui::Begin("Scene", nullptr  ,   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus  );

if (ImGui::CollapsingHeader("Entitys"))
{


ImGui::BeginChild("#Game Entitys");
ImGui::SameLine(0,50);
if (ImGui::Button(" + "))
{
    createObject(screenWidth/2,screenHeight/2);
}
ImGui::SameLine(0,20);

if (ImGui::Button(" - "))
{

}
ImGui::Separator();
//ImGui::EndChild();
//ImGui::BeginChild("SceneList");
 // const auto child_size = ImVec2(310, 280);
 ImVec2 wSize = ImGui::GetWindowSize();
            const auto  draw_list_size = ImVec2(wSize.x-2, wSize.y-30);
            if (ImGui::BeginListBox("##draw_list", draw_list_size))
            {
                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
               {
                    if (current_node_idx < m_entities.size() - 1) { ++current_node_idx; }
                }
                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
                {
                    if (current_node_idx > 0) { --current_node_idx; }
                }

                for (int n = 0; n <  m_entities.size() ; ++n)
                {
                    const bool is_selected = (current_node_idx == n);
                    if (ImGui::Selectable(m_entities[n]->name.c_str(), is_selected,ImGuiSelectableFlags_AllowDoubleClick))
                     {
                        current_node_idx = n;
                        selectEntity=m_entities[n];
                        selectName=std::string(selectEntity->name);
                     }


                    if (is_selected) { ImGui::SetItemDefaultFocus(); }
                }
                ImGui::EndListBox();
            }
ImGui::EndChild();
}
//****
if (ImGui::CollapsingHeader("Prefabs"))
{
ImGui::BeginChild("#Game Prefabs");
            ImVec2 wSize = ImGui::GetWindowSize();
            const auto  draw_list_size = ImVec2(wSize.x-2, wSize.y-30);
            if (ImGui::BeginListBox("##draw_list_prefabs", draw_list_size))
            {

                ImGui::EndListBox();
            }
ImGui::EndChild();
}



ImGui::End();
}



{//INSPECTOR***************************************************************************************************************************************
ImGui::SetNextWindowPos(ImVec2(screenWidth-240, 50));
ImGui::SetNextWindowSize(ImVec2(220, screenHeight-100));
ImGui::Begin("Inspector", nullptr  , ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoCollapse |ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus  );


if (ImGui::CollapsingHeader("Game"))
{
ImGui::InputInt("Width",&gameWidth);
ImGui::InputInt("Heigth",&gameHeight);
ImGui::Checkbox("Use Physics",&usePyshics);
if (usePyshics)
    ImGui::Checkbox("Show Physics Lines",&showPhysicsLines);
ImGui::Checkbox("Show Debug Lines",&showLines);

}
//******


if (ImGui::CollapsingHeader("Entitys"))
{

if (m_entities.size()>0  &&  selectEntity!=nullptr)
{
ImGui::Text("Id %d ",selectEntity->Id);
ImGui::Text("Width  %d ",(int)selectEntity->width);
ImGui::Text("Heigth %d ",(int)selectEntity->height);


if (ImGui::InputText("Name",(char*)selectName.c_str(), selectName.size()+1),ImGuiInputTextFlags_CharsNoBlank)
{

if ( strlen(selectName.c_str())>1)
{
if (IsKeyReleased(KEY_ENTER))
    selectEntity->name=std::string(selectName);
}


}


ImGui::Separator();

static Vector3 pos = (Vector3){0,0,0};
//DrawVec3Control("Position",pos);


if (ImGui::CollapsingHeader("Transform"))
{
//ImGui::SliderFloat("X",   &selectEntity->x,  0,screenWidth,   "%f");
//ImGui::SliderFloat("Y",   &selectEntity->y,  0,screenHeight,   "%f");

//ImGui::DragScalar("X",ImGuiDataType_Float, &selectEntity->x,1);
ImGui::DragFloat("X", &selectEntity->position.x, 1.0, 0.0f, 0.0f, "%.2f");
ImGui::DragFloat("Y", &selectEntity->position.y, 1.0, 0.0f, 0.0f, "%.2f");
ImGui::DragFloat("X Scale", &selectEntity->xscale, 0.1f, 0.1f, 100.0f, "%.2f");
ImGui::DragFloat("Y Scale", &selectEntity->yscale, 0.1f, 0.1f, 100.0f, "%.2f");

float maxWidth = selectEntity->width*selectEntity->xscale;
float maxHeigth = selectEntity->height*selectEntity->yscale;

ImGui::DragFloat("X Pivot", &selectEntity->pivot.x, 1, -maxWidth, maxWidth, "%.2f");
ImGui::DragFloat("Y Pivot", &selectEntity->pivot.y, 1, -maxHeigth, maxHeigth, "%.2f");
ImGui::DragFloat("Rotation ", &selectEntity->rotation, 1.0f, -360, 360, "%.2f");
}

ImGui::Separator();

if (ImGui::CollapsingHeader("Options"))
{
ImGui::Checkbox("Rigid Body",&selectEntity->isRigidBody);
ImGui::Checkbox("Prefab",&selectEntity->isPrefab);
}
//ImGui::BeginChild("Trasform", nullptr  , ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus  );
//ImGui::EndChild();


}//obj slect
}// header entitys



ImGui::End();
}//inspector
//******************************

if (showFps)
{
//ImGui::SetNextWindowPos(ImVec2(screenWidth-465, screenHeight-42));
//ImGui::SetNextWindowSize(ImVec2(460, 20));
ImGui::SetNextWindowPos(ImVec2(20, screenHeight-42));
ImGui::SetNextWindowSize(ImVec2(screenWidth-60, 20));

ImGui::Begin("WindowSystem",&showFps,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar );
    ImGui::Text("Mouse ( %f | %f ) | Selected (%d) | Mode:%s | Entitys %d |  average %.3f ms/frame (%.1f FPS)",(mouse.x-viewBond.x),(mouse.y-viewBond.y),current_node_idx,EditorModeName[mode],m_entities.size(), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
ImGui::End();
}



//     DrawRectangleGradientV(0,0,screenWidth, 10, (Color){0,0,0,80}, (Color){0,0,0,0});
//    DrawRectangleGradientV(0,screenHeight-10,screenWidth, 10, (Color){0,0,0,0}, (Color){0,0,0,80});

BeginScissorMode(viewBond.x,viewBond.y,viewBond.width,viewBond.height);

 if(snap)
    {
		DrawSnapGrid(snapDistance*4, gridLineColor[1]);
		DrawSnapGrid(snapDistance, gridLineColor[0]);
	}

UpdateEntitys();

DrawRectangleLinesEx(viewBond,1,WHITE);

//   DrawText(TextFormat(" %d ",showFps),10,screenHeight-50,12,WHITE);


if (m_entities.size()>0  &&  selectEntity!=nullptr)
{
DrawText(TextFormat("name %s  %d",selectEntity->name.c_str(),selectEntity->Id),230,110,10,RED);
DrawRectangleLinesEx(selectEntity->bound,1.2f,LIME);
} else
{
DrawText("None",230,110,10,RED);
}
DrawFPS(2,2);
EndScissorMode();

//  	DrawText(TextFormat(" SELECT:%d | X:%f Y:%f | ROTATION %f | BOUNDS:[%i %i %i %i] | MODE:%s",  resizerPointActive,x,y,rotation,(int)bounds.x, (int)bounds.y, (int)bounds.width, (int)bounds.height,EditorModeName[mode]), 80, 4, 10, WHITE);

if (editMode)
{

        if (IsKeyDown(KEY_M))
        {
	       mode = MODE_MOVE_WIDGET;
        } else
        if (IsKeyDown(KEY_E))
        {
          mode = MODE_RESIZE_WIDGET;
	    } else
	     if (IsKeyDown(KEY_R))
        {
          mode = MODE_ROTATE_WIDGET;
	    } else
        if (IsKeyDown(KEY_P))
        {
	       mode = MODE_PLACE_WIDGET;
        } else
        {
        //mode = MODE_NORMAL;
        }


    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      //  if (IsKeyDown(KEY_LEFT_SHIFT))
        {
         selectEntity=PickEntity(mouse);
        }

}

if (selectEntity)
{

            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	        {



                        x = selectEntity->position.x;
                        y = selectEntity->position.y;
                     //   resizerPointActive = selectNode->CheckCollisionWithResizerPoints();
                       // rotatePointActive  = selectNode->CheckCollisionWithRotate();


                        if(mode == MODE_MOVE_WIDGET)
                        {
                                if(snap)
                                {
                                    selectEntity->position.x = ((int)(selectEntity->position.x/snapDistance))*snapDistance;
                                    selectEntity->position.y = ((int)(selectEntity->position.y/snapDistance))*snapDistance;
                                    //selectNode->width = ((int)(selectNode->width/snapDistance))*snapDistance;
                                    //selectNode->height = ((int)(selectNode->height/snapDistance))*snapDistance;
                                }
                                lastMousePosition = mouse;
                        }

                         if( mode == MODE_ROTATE_WIDGET)
                        {

                            int deltaX = mouse.x - (selectEntity->position.x + selectEntity->width  / 2);
                            int deltaY = mouse.y - (selectEntity->position.y + selectEntity->height / 2);
                            selectEntity->rotation = atan2(deltaY, deltaX)* RAD2DEG;
                        }



            }


			  if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	        {



                        if(mode == MODE_MOVE_WIDGET)
                        {


                               if(snap)
                                {
                                    mouse.x = ((int)(mouse.x/snapDistance))*snapDistance;
                                    mouse.y = ((int)(mouse.y/snapDistance))*snapDistance;
                                    lastMousePosition.x = ((int)(lastMousePosition.x/snapDistance))*snapDistance;
                                    lastMousePosition.y = ((int)(lastMousePosition.y/snapDistance))*snapDistance;
                                }
                                selectEntity->position.x += mouse.x - lastMousePosition.x;
                                selectEntity->position.y += mouse.y - lastMousePosition.y;
                                x = selectEntity->position.x;
                                y = selectEntity->position.y;
                                lastMousePosition = mouse;



                        }

                        if (mode == MODE_PLACE_WIDGET)
                        {
                                selectEntity->position.x = mouse.x;
                                selectEntity->position.y = mouse.y;
                                x = selectEntity->position.x;
                                y = selectEntity->position.y;
                        }

                         if(mode == MODE_RESIZE_WIDGET)
                        {

                             //   ResizeWidget(selectNode);

                        }
                        if (mode ==MODE_ROTATE_WIDGET)
                        {
                                int deltaX = mouse.x - (selectEntity->position.x + selectEntity->width / 2);// Rectangle Center X
                                int deltaY = mouse.y - (selectEntity->position.y + selectEntity->height / 2);// Rectangle Center Y
                                selectEntity->rotation = atan2(deltaY, deltaX)* RAD2DEG;

                        }



			} else
			if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
			{

				// mode = MODE_NORMAL;
				 //if (selectNode) selectNode->selectColor = {200,200,200,100};
            }
}

}




        ImGui_ImplRaylib_Render();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

     ClearEntitys();
     ImGui_ImplRaylib_Shutdown();

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
