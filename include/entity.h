#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#define ENT_PLAYER      0
#define ENT_BUCKET      1
#define ENT_ANIMAL      2
#define ENT_BOOTS       3
#define ENT_WATER       4
#define ENT_FIRE        5
#define ENT_DONE        6

typedef struct Entity_S
{
    Uint8       _inuse;         /**<check if this entity in memory is active or not>*/
    Sprite      *sprite;        /**<a pointer to the sprite that is used by this entity>*/
    float        frame;         /**<current frame for the sprite>*/

    SDL_Rect    hitBox;         /**<rect used for collisions>*/
    int         tag;            /**<type of entity:player - 0,bucket - 1, animal - 2,boots - 3>*/
    int         health;

    Vector2D     position;      /**<where the entity is in 2D space>*/
    Vector3D     *rotation;      /**<rotation of entity (mainly for player)>*/

    void         *typeOfEnt;    /**<void pointer to be set to what type of entity I want>*/
    int          done;
    void (*think)(struct Entity_S *self);       /**<called when an entity draws>*/
    void (*waterThink)(struct Entity_S *self, struct Entity_S *other);  /**<specific think for shooting water>*/
}Entity;

typedef struct 
{
    Uint32  maxEnts;         /**<Maximum number of entities*/
    Entity  *entityList;     /**<List of entities*/
}EntityManager;

/**
 * @brief Returns a pointer to the entity_manager
 * */
EntityManager entity_manager_get_active();


/**
 * @brief get a pointer to a new entity
 * @return NULL on out of memory or error, a pointer to a blank entity otherwise
 */

Entity *entity_new();

/**
 * @brief initialize the entity resource manager
 * @param maxEnts upper bound of maximum concurrent entities to be supported
 * @note must be called before creating a new entity
 */

void entity_manager_init(Uint32 maxEnts);

/**
 * @brief free a previously allocated entity
 * @param self a pointer to the entity to free
 */

void entity_free(Entity *self);

/**
 * @brief free all ents exept player
 */
void free_all_ents();

/**
 * @brief update every active entity
 */

void entity_update_all();

/**
 * @brief draw every active entity
 */

void entity_draw_all();

Entity *entity_spawn_tree(Vector2D pos);

Entity *entity_spawn_bush(Vector2D pos);

Entity *entity_spawn_ambulance(Vector2D pos);

Entity *entity_spawn_koala(Vector2D pos);

Entity *entity_spawn_fire(Vector2D pos);

int    isFireLeft();

int    isKoalaLeft();

void   killAllFire();

Entity *entity_spawn_waterPickUp(Vector2D pos);

Entity *entity_spawn_speedBoots(Vector2D pos);

Entity *entity_spawn_win(Vector2D pos);

Entity *entity_spawn_lose(Vector2D pos);

Entity *water_shoot(Entity *player);

Entity *get_water_ent();

Entity *doneEnt();

Entity *get_done_ent();

#endif