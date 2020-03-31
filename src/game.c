#include <SDL.h>
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "entity.h"
#include "player.h"
#include "collisions.h"

/*
Entity *newTestEntity()
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_all(
        "images/space_bug.png",
        128,
        128,
        16);
    return self;
}*/

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    Entity *playerEnt;
    Entity *tree1;
    Entity *bush1;
    Entity *koala1;
    Entity *bucket1;
    Entity *boots1;
    
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
    gf2d_sprite_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    entity_manager_init(1024);

    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
    playerEnt = player_new_ent("images/player1.png", vector2d(0, 0));
    Player *p = (Player *)playerEnt->typeOfEnt;
    SDL_GameController *c = p->controller;
    tree1 = entity_spawn_tree(vector2d(40.0,30.0));
    bush1 = entity_spawn_bush(vector2d(200.0,60.0));
    koala1 = entity_spawn_koala(vector2d(500.0,200.0));
    bucket1 = entity_spawn_waterPickUp(vector2d(700.0, 300.0));
    boots1 = entity_spawn_speedBoots(vector2d(40.0,600.0));
    

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
        entity_update_all();
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            entity_draw_all();
            //UI elements last
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);

        gf2d_draw_rect(boots1->hitBox, vector4d(600,50, 30,550));

        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        
        if (collide_rect(playerEnt->hitBox, tree1->hitBox))
        {
            slog("hitting tree1");
        }

        if (collide_rect(playerEnt->hitBox, bush1->hitBox))
        {
            slog("hitting bush1");
        }

        //Grabbing
        if (collide_rect(playerEnt->hitBox, koala1->hitBox))
        {
            if (SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_B))
            {
                koala1->position = playerEnt->position;
                koala1->position.x += 40;
            }
        }

        if (collide_rect(playerEnt->hitBox, bucket1->hitBox))
        {
            entity_free(bucket1);
            p->waterAmmo += 20;
            
        }
        
        slog("water: %f", p->waterAmmo);

        if (keys[SDL_SCANCODE_ESCAPE])
        {
            entity_free(playerEnt);// exit condition
            done = 1; // exit condition
        }
        
        if (keys[SDL_SCANCODE_SPACE] || SDL_GameControllerGetButton(c,SDL_CONTROLLER_BUTTON_START))
        { 
            entity_free(playerEnt);// exit condition
            entity_free(tree1);// exit condition
            entity_free(bush1);// exit condition
        }

        if (SDL_GameControllerGetButton(c,SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
        {
            float rx = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_RIGHTX)/10000;
            float ry = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_RIGHTY)/10000;

            Vector3D *rot = playerEnt->rotation;
            rot->x = 65;
            rot->y = 55;
            rot->z = vector_angle(rx,ry) + 90;
            //slog("rot: %f", rot->z);
        }

//      slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/