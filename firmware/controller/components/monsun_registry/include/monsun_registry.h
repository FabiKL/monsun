#ifndef MONSUN_REGISTRY_H
#define MONSUN_REGISTRY_H

#include <stdbool.h>
#include <stddef.h>

#include "monsun_link.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MONSUN_REGISTRY_MAX 48

typedef struct {
    const char *id;            /* unique, used in MQTT topics */
    const char *name;          /* display name in Home Assistant */
    monsun_entity_type_t type;
    const char *device_class;  /* HA device class or NULL */
    const char *unit;          /* unit of measurement or NULL */
    bool writable;
    bool diagnostic;
    float value;
    bool valid;                /* false = unavailable/stale */
} monsun_entity_t;

/* Returns the entity or NULL when the registry is full or the id already exists. */
monsun_entity_t *monsun_registry_add(const monsun_entity_t *entity);
monsun_entity_t *monsun_registry_find(const char *id);
size_t monsun_registry_count(void);
monsun_entity_t *monsun_registry_at(size_t index);

void monsun_registry_set(const char *id, float value, bool valid);

#ifdef __cplusplus
}
#endif

#endif
