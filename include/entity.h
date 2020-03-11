#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gf2d_sprite.h"

typedef struct Entity_S
{
    Uint8       _inuse;         /**<check if this entity in memory is active or not>*/
    Sprite      *sprite;        /**<a pointer to the sprite that is used by this entity>*/
    float        frame;         /**<current frame for the sprite>*/

    Vector2D     position;      /**<where the entity is in 2D space>*/
    Vector3D     *rotation;      /**<rotation of entity (mainly for player)>*/

    void         *typeOfEnt;    /**<void pointer to be set to what type of entity I want>*/

    void (*think)(struct Entity_S *self);       /**<called when an entity draws>*/
}Entity;

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
 * @brief update every active entity
 */

void entity_update_all();

/**
 * @brief draw every active entity
 */

void entity_draw_all();

#endif