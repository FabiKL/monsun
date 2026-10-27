#include "monsun_registry.h"

#include <string.h>

static monsun_entity_t s_entities[MONSUN_REGISTRY_MAX];
static size_t s_count;

monsun_entity_t *monsun_registry_find(const char *id)
{
    for (size_t i = 0; i < s_count; i++) {
        if (strcmp(s_entities[i].id, id) == 0) {
            return &s_entities[i];
        }
    }
    return NULL;
}

monsun_entity_t *monsun_registry_add(const monsun_entity_t *entity)
{
    if (s_count >= MONSUN_REGISTRY_MAX || monsun_registry_find(entity->id) != NULL) {
        return NULL;
    }
    s_entities[s_count] = *entity;
    return &s_entities[s_count++];
}

size_t monsun_registry_count(void)
{
    return s_count;
}

monsun_entity_t *monsun_registry_at(size_t index)
{
    return index < s_count ? &s_entities[index] : NULL;
}

void monsun_registry_set(const char *id, float value, bool valid)
{
    monsun_entity_t *e = monsun_registry_find(id);
    if (e != NULL) {
        e->value = value;
        e->valid = valid;
    }
}
