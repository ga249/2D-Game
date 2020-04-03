#include <SDL.h>
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "entity.h"
#include "player.h"
#include "collisions.h"
#include "menu.h"
#include "level.h"




int main(int argc, char * argv[])
{
    /*variable declarations*/
    //Entity *closeEnt;
    //Sprite *menuExitSprite;
    Level *level;
    //int paused = 0;
    float pauseBuffer;
    float startBoostTime;
    int savedAnimals = 0;
    const Uint8 * keys;
    SDL_Rect waterBar;
    SDL_Rect timeBar;
    Menu *menuExit;
    Menu *menuStart;
    Menu *menuEasy;
    Menu *menuHard;
    SDL_Rect menuExitBox = {LEVEL_WIDTH - 256,
                            LEVEL_HEIGHT - 84,
                            256,
                            84};                        //{408,100,384,128}
    SDL_Rect menuStartBox = {408,100,384,128};
    SDL_Rect menuEasyBox = {344,228,256,84};
    SDL_Rect menuHardBox = {600,228,256,84};
    //EntityManager entity_manager = entity_manager_get_active();
    Sprite *sprite;
    Entity *playerEnt;
    Entity *ambulance;
    //Entity *fire1;
    //Entity *fire2;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1084);
    SDL_ShowCursor(SDL_DISABLE);
    entity_manager_init(1084);
    menu_manager_init(64);

    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_grass.png");
    
    //menuExitSprite = gf2d_sprite_load_image("images/exitMenuSmaller.png");
    //closeEnt = doneEnt();
    level = level_new(sprite, DIFF_EASY);
    level->paused = 1;
    level->winLose = 0;
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
    gfc_rect_set(waterBar, 10, 10, 500, 20);
    gfc_rect_set(timeBar,100, LEVEL_HEIGHT - 20, 1000, 20);
    playerEnt = player_new_ent("images/player1.png", vector2d(200, 300));
    Player *p = (Player *)playerEnt->typeOfEnt;     p->waterAmmo = 500;
    SDL_GameController *c = p->controller;
    ambulance = entity_spawn_ambulance(vector2d(10.0,300.0));
    //fire1 = entity_spawn_fire(vector2d(200.0,300.0));
    //fire2 = entity_spawn_fire(tree1->position);
    menuExit = menu_generic(menuExitBox,vector2d(menuExitBox.x,menuExitBox.y),
                            gf2d_sprite_load_image("images/exitMenu.png"),button_exit_think);
    menuStart = menu_generic(menuStartBox,vector2d(menuStartBox.x,menuStartBox.y),
                            gf2d_sprite_load_image("images/startMenu.png"),button_start_think);
    menuEasy = menu_generic(menuEasyBox,vector2d(menuEasyBox.x,menuEasyBox.y),
                            gf2d_sprite_load_image("images/easyMenu.png"),button_easy_think);
    menuHard = menu_generic(menuHardBox,vector2d(menuHardBox.x,menuHardBox.y),
                            gf2d_sprite_load_image("images/hardMenu.png"),button_hard_think);
    //level_spawn_ents(level_get_active());

    /*main game loop*/
    while(level_get_active()->done == 0)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        if(level_get_active()->paused == 1)
        {
            menu_update_all();
            
        }else{
            entity_update_all();
        }
        

        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(level_get_active()->background,vector2d(0,0));
            entity_draw_all();
            if (level_get_active()->paused == 1)
            {
                menu_draw_all();
                //slog("drawing");
            }


            
            //UI elements last
            if(level_get_active()->paused == 1)
            {
                gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);
            }
            

        gf2d_draw_rect(waterBar, vector4d(255,0,255,255));
        waterBar.w = p->waterAmmo;

        gf2d_draw_rect(timeBar, vector4d(255,0,255,255));
        timeBar.w = 1000 * (level_get_active()->timeLeft/30);

       // gf2d_draw_rect(fire1->hitBox, vector4d(600,50, 30,550));

        //if (get_water_ent())
        //{
            //gf2d_draw_rect(get_water_ent()->hitBox, vector4d(600,50, 30,550));
        //}

        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if(level_get_active()->paused != 1 && level_get_active()->winLose == 0)
        {
            level_get_active()->timeLeft -= .01;
            //slog("timeLeft: %f",level_get_active()->timeLeft);
            //slog("timebar w: %f",timeBar.w);
        }

        if (level_get_active()->timeLeft > 0 && isFireLeft() == 0 && !isKoalaLeft() == 0 && level_get_active()->paused == 0)
        {
            if (level_get_active()->winLose == 0)
            {
                entity_spawn_win(vector2d(408,100));
                level_get_active()->winLose = 1;
            }
            
        }
        if(level_get_active()->timeLeft <= 0)
        {
            if (level_get_active()->winLose == 0)
            {
                entity_spawn_lose(vector2d(408,100));
                level_get_active()->winLose = 1;
            }
        }

        //Runs through all entities and checks for and deals with all collisions------------------------------------------------
        for (int i = 0; i < entity_manager_get_active().maxEnts; i++)
        {
             //animals---------------------------------------------------------------------------------------------------------------------
            if(entity_manager_get_active().entityList[i].tag == ENT_ANIMAL && entity_manager_get_active().entityList[i]._inuse == 1)
            {
                //Grabbing
                if (collide_rect(playerEnt->hitBox, entity_manager_get_active().entityList[i].hitBox))
                {
                    if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_B))
                    {
                        entity_manager_get_active().entityList[i].position = playerEnt->position;
                        entity_manager_get_active().entityList[i].position.x += 40;
                    }
                }
                //returning animals
                if (collide_rect(entity_manager_get_active().entityList[i].hitBox, ambulance->hitBox))
                {
                    entity_manager_get_active().entityList[i]._inuse = 0;
                    entity_free(&entity_manager_get_active().entityList[i]);
                    savedAnimals += 1;
                    slog("saved animals: %i", savedAnimals);
                }
            }
            //buckets-----------------------------------------------------------------------------------------------------------------------
            if(entity_manager_get_active().entityList[i].tag == ENT_BUCKET && entity_manager_get_active().entityList[i]._inuse == 1) 
            {
                //water pickup
                if (p->waterAmmo < 500 && collide_rect(playerEnt->hitBox, entity_manager_get_active().entityList[i].hitBox))
                {
                    entity_free(&entity_manager_get_active().entityList[i]);
                    if (p->waterAmmo > 250)
                    {
                        p->waterAmmo = 500;
                    }else{
                        p->waterAmmo += 250;
                    }
                    slog("Water Ammo: %i", p->waterAmmo);

                }
            }
            //boots-------------------------------------------------------------------------------------------------------------------------
            if(entity_manager_get_active().entityList[i].tag == ENT_BOOTS && entity_manager_get_active().entityList[i]._inuse == 1) 
            {
                if (collide_rect(playerEnt->hitBox, entity_manager_get_active().entityList[i].hitBox))
                {
                    entity_free(&entity_manager_get_active().entityList[i]);
                    p->isSprint = 1;
                    startBoostTime = SDL_GetTicks();
                }
                
            }
            //fire---------------------------------------------------------------------------------------------------------------------------
            if(entity_manager_get_active().entityList[i].tag == ENT_FIRE && entity_manager_get_active().entityList[i]._inuse == 1) 
            {
                if(get_water_ent())
                {
                    if (collide_rect(entity_manager_get_active().entityList[i].hitBox, get_water_ent()->hitBox))
                    {
                        entity_manager_get_active().entityList[i].health -= 1;
                        slog("hitting");
                    }
                    if (collide_rect(entity_manager_get_active().entityList[i].hitBox, playerEnt->hitBox))
                    {
                        slog("fire hitting player");
                    }
                }
                
            }
        }

        if (p->isSprint == 1 && SDL_GetTicks() - startBoostTime >= 4000)
        {
            p->isSprint = 0;
        }

        /*
        if (collide_rect(playerEnt->hitBox, boots1->hitBox))
        {
            entity_free(boots1);
            p->isSprint = 1;
            startBoostTime = SDL_GetTicks();
        }
        if (p->isSprint == 1 && SDL_GetTicks() - startBoostTime >= 2000000)
        {
            p->isSprint = 0;
        }*/
        
        if (SDL_GameControllerGetButton(c,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) && p->waterAmmo >= 1)
        {
            if(p->isShooting == 0)
            {
                water_shoot(playerEnt);
                p->isShooting = 1;
                
            }
            if(!get_water_ent())
            {
                p->isShooting = 0;
            }
            
        }
        
        if (keys[SDL_SCANCODE_F])
        {
            killAllFire();
        }

        if (keys[SDL_SCANCODE_ESCAPE])
        {
            entity_free(playerEnt);// exit condition
            menu_free(menuEasy);
            menu_free(menuHard);
            menu_free(menuStart);
            menu_free(menuExit);
            level_free(level_get_active());
            level_get_active()->done = 1; // exit condition
        }
        
        if (keys[SDL_SCANCODE_SPACE])
        { 
            entity_free(playerEnt);// exit condition
        }

        if (SDL_GameControllerGetButton(c,SDL_CONTROLLER_BUTTON_START) || keys[SDL_SCANCODE_P])
        {
            if(SDL_GetTicks() - pauseBuffer >= 200)
            {
                pauseBuffer = SDL_GetTicks();
                if(level_get_active()->paused == 0)
                {
                    level_get_active()->paused = 1;
                }else{
                    level_get_active()->paused = 0;
                } 
            }
            
        }

        if (SDL_GameControllerGetButton(c,SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
        {
            float rx = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_RIGHTX)/10000;
            float ry = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_RIGHTY)/10000;

            Vector3D *rot = playerEnt->rotation;
            rot->x = 65;
            rot->y = 55;
            rot->z = vector_angle(rx,ry) + 90;
            //slog("rotx: %f,  roty: %f,  rotz: %f",rot->x,rot->y,rot->z);
        }

//      slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/