#include <stdlib.h>
#include "simple_logger.h"
#include "entity.h"
#include "collisions.h"
#include "player.h"
#include "gf2d_draw.h"

static EntityManager entity_manager = {0};

EntityManager entity_manager_get_active()
{
    return entity_manager;
}

void entity_manager_close()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (entity_manager.entityList[i]._inuse)
        {
            entity_free(&entity_manager.entityList[i]);
        }
    }
    entity_manager.maxEnts = 0;
    free(entity_manager.entityList);
    entity_manager.entityList = NULL;
    slog("entity manager closed");
}

void entity_manager_init(Uint32 maxEnts)
{
    if (entity_manager.entityList != NULL)
    {
        //TODO: cleanup
    }
    if (!maxEnts)
    {
        slog("cannot initialize a zero size entity list!");
        return;
    }
    entity_manager.entityList = malloc(sizeof(Entity) * maxEnts);
    if (entity_manager.entityList == NULL)
    {
        slog("failed to allocate %i entities for the entity manager", maxEnts);
        return;
    }
    entity_manager.maxEnts = maxEnts;
    memset(entity_manager.entityList,0,sizeof(Entity)*maxEnts);
    slog("entity manager initialized");
    atexit(entity_manager_close);

}

Entity *entity_new()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (entity_manager.entityList[i]._inuse)continue;
        entity_manager.entityList[i]._inuse = 1;
        return &entity_manager.entityList[i];
    }
    slog("out of open entity slots in memory!");
    return NULL;
}

void free_all_ents()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (entity_manager.entityList[i]._inuse && entity_manager.entityList[i].tag != ENT_PLAYER)
        {
            entity_free(&entity_manager.entityList[i]);
        }
    }
}

void entity_free(Entity *self)
{
    if (!self)return;
    gf2d_sprite_free(self->sprite);
    self->_inuse = 0;
}

void entity_update(Entity *self)
{
    if (!self)return;
    //self->frame = self->frame + 0.1;
    //if (self->frame > 10)self->frame = 0;
    if (self->think)self->think(self);
    if (self->waterThink)self->waterThink(self,get_player_ent());
}

void entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (!entity_manager.entityList[i]._inuse)continue;
        entity_update(&entity_manager.entityList[i]);
    }
}

void entity_draw(Entity *self)
{
    if (self == NULL)
    {
        slog("cannot draw sprite, NULL entity provided!");
        return;
    }
    gf2d_sprite_draw(
    self->sprite,
    self->position,
    NULL,
    NULL,
    self->rotation,
    NULL,
    NULL,
    (Uint32)self->frame);
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.maxEnts; i++)
    {
        if (!entity_manager.entityList[i]._inuse)continue;
        entity_draw(&entity_manager.entityList[i]);
    }
}

void fire_think(Entity *self)
{
    if (self->health < 1)
    {
        entity_free(self);
    }
}

Entity *entity_spawn_fire(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/fire.png");
    self->position = pos;
    self->health = 100;
    self->tag = ENT_FIRE;
    SDL_Rect hb;            //hitbox
    hb.x = pos.x + 30;
    hb.y = pos.y + 20;
    hb.h = 75;
    hb.w = 50;
    self->hitBox = hb;
    self->think = fire_think;
    return self;
}

int    isFireLeft()
{
    for (int i = 0; i < entity_manager_get_active().maxEnts; i++)
    {
        if(entity_manager_get_active().entityList[i].tag == ENT_FIRE && entity_manager_get_active().entityList[i]._inuse == 1)
        {
            return 1;
        }
    }
    return 0;
}

int    isKoalaLeft()
{
    for (int i = 0; i < entity_manager_get_active().maxEnts; i++)
    {
        if(entity_manager_get_active().entityList[i].tag == ENT_ANIMAL && entity_manager_get_active().entityList[i]._inuse == 1)
        {
            return 1;
        }
    }
    return 0;
}

void   killAllFire()
{
    for (int i = 0; i < entity_manager_get_active().maxEnts; i++)
    {
        if(entity_manager_get_active().entityList[i].tag == ENT_FIRE && entity_manager_get_active().entityList[i]._inuse == 1)
        {
            entity_free(&entity_manager_get_active().entityList[i]);
        }
    }
    
}

Entity *entity_spawn_win(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/yay.png");
    self->position = pos;
    return self;
}

Entity *entity_spawn_lose(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/boo.png");
    self->position = pos;
    return self;
}

Entity *entity_spawn_tree(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/tree.png");
    self->position = pos;
    SDL_Rect hb;            //hitbox
    hb.x = pos.x + 28;  //28
    hb.y = pos.y + 10;  //10
    hb.h = 100;         //100
    hb.w = 72;          //72
    self->hitBox = hb;
    entity_spawn_fire(pos);
    return self;
}

Entity *entity_spawn_bush(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/bush.png");
    self->position = pos;
    SDL_Rect hb;            //hitbox
    hb.x = pos.x + 20;
    hb.y = pos.y + 45;
    hb.h = 40;
    hb.w = 80;
    self->hitBox = hb;
    entity_spawn_fire(pos);
    return self;
}

Entity *entity_spawn_ambulance(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/ambulance.png");
    self->position = pos;
    SDL_Rect hb;            //hitbox
    hb.x = pos.x + 5;
    hb.y = pos.y + 20;
    hb.h = 80;
    hb.w = 110;
    self->hitBox = hb;
    return self;
}

void koala_think(Entity *self)
{
    SDL_Rect hb;            //hitbox
    hb.x = self->position.x + 40;
    hb.y = self->position.y + 35;
    hb.h = 60;
    hb.w = 50;
    self->hitBox = hb;
}

Entity *entity_spawn_koala(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/koala.png");
    self->position = pos;
    self->think = koala_think;
    self->tag = 2;
    entity_spawn_fire(pos);
    return self;
}


Entity *entity_spawn_waterPickUp(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/WaterBucket.png");
    self->position = pos;
    self->tag = 1;
    SDL_Rect hb;            //hitbox
    hb.x = pos.x + 40;
    hb.y = pos.y + 20;
    hb.h = 65;
    hb.w = 50;
    self->hitBox = hb;
    return self;
}

Entity *entity_spawn_speedBoots(Vector2D pos)
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->sprite = gf2d_sprite_load_image("images/speedBoots.png");
    self->position = pos;
    self->tag = 3;
    SDL_Rect hb;            //hitbox
    hb.x = pos.x + 5;
    hb.y = pos.y + 10;
    hb.h = 30;
    hb.w = 55;
    self->hitBox = hb;
    return self;
}

void water_shoot_think(Entity *self, Entity *owner)
{
    Player *player = (Player *)owner->typeOfEnt;
    //Vector3D rot = vector3d(0, 0, owner->rotation->z);
    //self->rotation = &rot;
    self->rotation = owner->rotation;
    //self->rotation->x = 40;
    //self->rotation->y = 40;

    self->position.x = owner->position.x;
    self->position.y = owner->position.y;
    
    SDL_Rect hb;            //hitbox
    hb.x = self->position.x + 43;  
    hb.y = self->position.y + 5;  
    hb.h = 70;         
    hb.w = 40;          
    self->hitBox = hb;

    player->waterAmmo -= 1;
    slog("Water Ammo: %i", player->waterAmmo);
    //slog("rotx: %f,  roty: %f,  rotz: %f",self->rotation->x,self->rotation->y,self->rotation->z);
    //slog("rotz: %f",self->rotation->z);

    //gf2d_draw_rect(self->hitBox, vector4d(0,0,0,255));

    if (!SDL_GameControllerGetButton(player->controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) || player->waterAmmo < 1)
    {
        //slog("not shooting");
        player->isShooting = 0;
        entity_free(self);
        //self->_inuse = 0;

    }
    
}

Entity *water_shoot(Entity *player)
{
    Player *p = (Player *)player->typeOfEnt;
    if(p->isShooting == 0)
    {
        Entity *self;
        self = entity_new();
        if (!self)return NULL;
        self->sprite = gf2d_sprite_load_image("images/WaterShoot.png");
        self->position = player->position;
        self->waterThink = water_shoot_think;
        self->tag = ENT_WATER;
        SDL_Rect hb;            //hitbox
        hb.x = self->position.x + 43;  
        hb.y = self->position.y + 5;  
        hb.h = 70;         
        hb.w = 40;          
        self->hitBox = hb;
        //slog("shooting");
        return self;
    }
    return NULL;
}

Entity *get_water_ent()
{
    for (int i = 0; i < entity_manager_get_active().maxEnts; i++)
    {
        if(entity_manager_get_active().entityList[i].tag == ENT_WATER && entity_manager_get_active().entityList[i]._inuse == 1)
        {
            return &entity_manager_get_active().entityList[i];
        }
    }
    return NULL;
}

Entity *doneEnt()
{
    Entity *self;
    self = entity_new();
    if (!self)return NULL;
    self->done = 0;
    self->tag = ENT_DONE;
    return self;
}
Entity *get_done_ent()
{
    for (int i = 0; i < entity_manager_get_active().maxEnts; i++)
    {
        if(entity_manager_get_active().entityList[i].tag == ENT_DONE)
        {
            return &entity_manager_get_active().entityList[i];
        }
    }
    return NULL;
}