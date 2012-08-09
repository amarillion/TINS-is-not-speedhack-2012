#include "actor.h"
int const tile_w = 20;
int const tile_h = 10;
Actor * actor_new(Actors * actors, float x, float y, int type_id) {
    if (! actor_type[type_id]) {
        return NULL;
    }
    Actor * self;
    land_alloc(self);
    self->kind = actor_type[type_id];
    self->hp = self->max_hp = self->kind->initial_hp;
    self->damage = self->kind->initial_damage;
    self->angle = land_rnd(0, 2 * pi);
    self->x = x;
    self->y = y;
    self->tx = x;
    self->ty = y;
    self->id = 1 + land_array_count(actors->array);
    land_array_add(actors->array, self);
    actor_place(self);
    return self;
}
Actors * actors_new(void) {
    Actors * self;
    land_alloc(self);
    self->array = land_array_new();
    self->w = 64;
    self->h = 12;
    self->tilemap = land_calloc(self->w * self->h * sizeof * self->tilemap);
    return self;
}
void actor_draw(Actor * self) {
    int f = self->walk_tick >> 2;
    if (f > 4) {
        f = 8 - f;
    }
    int d = round(self->angle * 8 / (2 * pi) - 2);
    d &= 7;
    LandImage * im = self->kind->images[d + f * 8];
    int w = land_image_width(im);
    int h = land_image_height(im);
    if (self->dead) {
        int y = self->anim_tick * h / 120;
        land_image_draw_partial(im, self->x - w / 2, self->y - h + y, 0, 0, w, h - y);
    }
    else {
        land_image_draw(im, self->x - w / 2, self->y - h);
    }
    if (self->selected) {
        land_color(1, 1, 0, 1);
        land_rectangle(self->x - w / 2, self->y - h, self->x + w / 2, self->y);
    }
    if (self->projectile) {
        land_image_draw(item_types[self->kind->projectile] ->images[d] , self->px, self->py - 32);
    }
    float hp = self->hp / self->max_hp;
    if (hp > 0.8) {
        land_color(0, 1, 0, 1);
    }
    else if(hp > 0.5) {
        land_color(1, 1, 0, 1);
    }
    else if(hp > 0.2) {
        land_color(1, 0, 0, 1);
    }
    else {
        land_color(1, 1, 1, 1);
    }
    hp *= w;
    land_filled_rectangle(self->x - w / 2, self->y - h - 5, self->x - w / 2 + hp, self->y - h - 2);
}
void actor_draw_shadow(Actor * self) {
    int f = self->walk_tick >> 2;
    if (f > 4) {
        f = 8 - f;
    }
    int d = round(self->angle * 8 / (2 * pi) - 2);
    d &= 7;
    LandImage * im = self->kind->shadows[d + f * 8];
    int w = land_image_width(im);
    int h = land_image_height(im);
    land_image_draw_scaled(im, self->x - w / 2, self->y - h / 4 - h / 8, 1, 0.5);
    if (self->projectile) {
        land_image_draw(item_types[self->kind->projectile] ->shadows[d] , self->px, self->py - 16);
    }
}
static float wrap(float dx) {
    if (dx > app->w / 2) {
        dx -= app->w;
    }
    if (dx < app->w / - 2) {
        dx += app->w;
    }
    return dx;
}
void actor_unplace(Actor * a) {
    int x = a->x / tile_w;
    int y = a->y / tile_h;
    x %= app->actors->w;
    LandArray * arr = app->actors->tilemap[y * app->actors->w + x] .actors;
    if (! arr) {
        return;
    }
    int i = land_array_find(arr, a);
    if (i < 0) {
        return;
    }
    land_array_replace_nth(arr, i, land_array_pop(arr));
}
bool actor_place(Actor * a) {
    int x = a->x / tile_w;
    int y = a->y / tile_h;
    x %= app->actors->w;
    LandArray * arr = app->actors->tilemap[y * app->actors->w + x] .actors;
    if (! arr) {
        arr = land_array_new();
        app->actors->tilemap[y * app->actors->w + x] .actors = arr;
    }
    land_array_add(arr, a);
    return 1;
}
void actor_hp(Actor * self, Actor * bywho, float amount) {
    if (self->kind->enemy && bywho && amount < 0) {
        if (self->target) {
            float td = actor_distance(self, self->target);
            if (actor_distance(self, bywho) < td) {
                self->target = bywho;
            }
        }
        else {
            self->target = bywho;
        }
    }
    if (! self->kind->enemy && bywho && amount < 0) {
        if (! self->target && ! self->walking) {
            self->target = bywho;
        }
    }
    if (bywho && bywho->kind->id == AT_BAT && amount < 0) {
        bywho->hp -= amount / 2;
        if (bywho->hp > bywho->max_hp) {
            bywho->hp = bywho->max_hp;
        }
    }
    self->hp += amount;
    if (self->hp < 0) {
        self->hp = 0;
    }
    if (self->hp > self->max_hp) {
        self->hp = self->max_hp;
    }
}
static void check_pos(Actor * self) {
    if (self->x < 0) {
        self->x += app->w;
    }
    self->x = fmod(self->x, app->w);
    if (self->y < 0) {
        self->y = 0;
    }
    if (self->y > app->h) {
        self->y = app->h;
    }
}
void actor_avoid(Actor * self) {
    int tx = self->x / tile_w;
    int ty = self->y / tile_h;
    if (land_array_count(app->actors->tilemap[tx + app->actors->w * ty] .actors) > 1) {
        actor_unplace(self);
        self->x += land_rand( - 1, 1);
        self->y += land_rand( - 1, 1);
        check_pos(self);
        actor_place(self);
    }
}
void actor_tick(Actor * self) {
    if (self->dead) {
        self->anim_tick++;
        return;
    }
    if (self->special < 900) {
        self->special++;
    }
    if (self->hp <= 0) {
        self->dead = 1;
        self->anim_tick = 0;
    }
    if (self->frozen > 0) {
        self->frozen--;
        return;
    }
    if (! self->target) {
        self->projectile = 0;
    }
    else if(self->target->dead) {
        self->target = NULL;
        self->projectile = 0;
    }
    if (self->projectile) {
        float dx = wrap(self->target->x - self->px);
        float dy = self->target->y - self->py;
        float d = sqrt(dx * dx + dy * dy);
        float s = self->kind->projectile_speed;
        if (d > s) {
            self->px += s * dx / d;
            self->py += s * dy / d;
        }
        else {
            if (self->kind->projectile == IT_HEAL) {
                actor_hp(self->target, self, + 10);
            }
            if (self->kind->projectile == IT_ARROW) {
                actor_hp(self->target, self, - 3);
            }
            if (self->kind->projectile == IT_MAGIC_MISSILE) {
                actor_hp(self->target, self, - 6);
            }
            if (self->kind->projectile == IT_FIREBALL) {
                actor_hp(self->target, self, - 5);
            }
            self->projectile = 0;
        }
    }
    if (self->walking) {
        float dx = wrap(self->tx - self->x);
        float dy = self->ty - self->y;
        float d = sqrt(dx * dx + dy * dy);
        if (d > 1) {
            bool col = 0;
            actor_unplace(self);
            float ox = self->x;
            float oy = self->y;
            self->x += dx / d;
            check_pos(self);
            if (! actor_place(self)) {
                self->x = ox;
                col = 1;
            }
            else {
                actor_unplace(self);
            }
            self->y += dy / d;
            check_pos(self);
            if (! actor_place(self)) {
                self->y = oy;
                actor_place(self);
                col = 1;
            }
            self->angle = atan2(dy, dx);
            self->walk_tick++;
            self->walk_tick &= 31;
            if (col) {
                if (d <= 30) {
                    self->tx = self->x;
                    self->ty = self->y;
                }
            }
            return;
        }
        else {
            self->walking = 0;
            actor_items(self);
        }
    }
    if ((self->walk_tick & 15) != 8) {
        self->walk_tick++;
        self->walk_tick &= 31;
    }
    actor_avoid(self);
    if (self->target) {
        float dx = wrap(self->target->x - self->x);
        float dy = self->target->y - self->y;
        float d = sqrt(dx * dx + dy * dy);
        if (d > 1) {
            self->angle = atan2(dy, dx);
        }
        actor_target_action(self);
    }
    else {
        if (self->kind->enemy) {
            ai_tick(self);
        }
    }
}
float actor_distance(Actor * self, Actor * other) {
    float dx = other->x - self->x;
    float dy = other->y - self->y;
    return sqrtf(dx * dx + dy * dy);
}
bool actor_attack(Actor * self) {
    if (self->attack_tick == 0) {
        self->attack_tick = 60;
        return 1;
    }
    self->attack_tick--;
    return 0;
}
void actor_projectile(Actor * self) {
    assert(self->kind->projectile);
    self->projectile = 1;
    self->px = self->x;
    self->py = self->y;
}
void actor_walk_within(Actor * self, Actor * other, float distance) {
    float dx = other->x - self->x;
    float dy = other->y - self->y;
    float d = sqrtf(dx * dx + dy * dy);
    self->tx = self->x + dx * (d - distance) / d;
    self->ty = self->y + dy * (d - distance) / d;
    self->walking = 1;
}
void actor_ranged_attack(Actor * self) {
    if (actor_attack(self)) {
        float ad = self->kind->projectile_speed * 60;
        if (actor_distance(self, self->target) < ad - 30) {
            actor_projectile(self);
        }
        else {
            actor_walk_within(self, self->target, ad - 40);
        }
    }
}
void actor_melee_attack(Actor * self) {
    if (actor_distance(self, self->target) > 30) {
        actor_walk_within(self, self->target, 20);
    }
    else {
        if (actor_attack(self)) {
            actor_hp(self->target, self, - self->damage);
        }
    }
}
void actor_target_action(Actor * self) {
    if (self->kind->id == AT_DRAGON) {
        actor_ranged_attack(self);
    }
    if (self->kind->id == AT_DRAKE) {
        actor_ranged_attack(self);
    }
    if (self->kind->id == AT_HUNTER) {
        actor_ranged_attack(self);
    }
    if (self->kind->id == AT_HEALER) {
        actor_ranged_attack(self);
    }
    if (self->kind->id == AT_MAGE) {
        actor_ranged_attack(self);
    }
    if (self->kind->id == AT_KNIGHT) {
        actor_melee_attack(self);
    }
    if (self->kind->id == AT_DOG) {
        actor_melee_attack(self);
    }
    if (self->kind->id == AT_GROLL) {
        actor_melee_attack(self);
    }
    if (self->kind->id == AT_PAPAGROLL) {
        actor_melee_attack(self);
    }
    if (self->kind->id == AT_WOLF) {
        actor_melee_attack(self);
    }
    if (self->kind->id == AT_BAT) {
        actor_melee_attack(self);
    }
}
void actor_item(Actor * self, Item * it) {
    if (it->kind == IT_CHEESE) {
        if (self->kind->enemy) {
            return;
        }
        self->max_hp += 5;
        self->hp = self->max_hp;
        item_remove(it);
    }
}
void actor_items(Actor * self) {
    int tx = self->x / tile_w;
    int ty = self->y / tile_h;
    for (int iy = ty - 1; iy < ty + 2; iy += 1) {
        for (int ix = tx - 1; ix < tx + 2; ix += 1) {
            int x = ix;
            int y = iy;
            if (y < 0) {
                continue;
            }
            if (y > app->actors->h - 1) {
                continue;
            }
            if (x < 0) {
                x += app->actors->w;
            }
            x %= app->actors->w;
            Item * i = app->actors->tilemap[x + app->actors->w * y] .item;
            if (i) {
                float dx = i->x - self->x;
                float dy = i->y - self->y;
                if (dx * dx + dy * dy < 30 * 30) {
                    actor_item(self, i);
                }
            }
        }
    }
}
void actors_draw(Actors * self) {
    for (int y = 0; y < self->h; y += 1) {
        for (int x = 0; x < self->w; x += 1) {
            Item * i = self->tilemap[x + self->w * y] .item;
            if (i) {
                item_draw_shadow(i);
            }
            LandArray * arr = self->tilemap[x + self->w * y] .actors;
            if (arr) {
                {LandArrayIterator __iter0__ = LandArrayIterator_first(arr) ; for (Actor * a = LandArrayIterator_item(arr, & __iter0__) ; LandArrayIterator_next(arr, & __iter0__) ; a = LandArrayIterator_item(arr, & __iter0__)) {
                    actor_draw_shadow(a);
                }
                }
            }
        }
    }
    for (int y = 0; y < self->h; y += 1) {
        for (int x = 0; x < self->w; x += 1) {
            Item * i = self->tilemap[x + self->w * y] .item;
            if (i) {
                item_draw(i);
            }
            LandArray * arr = self->tilemap[x + self->w * y] .actors;
            if (arr) {
                {LandArrayIterator __iter1__ = LandArrayIterator_first(arr) ; for (Actor * a = LandArrayIterator_item(arr, & __iter1__) ; LandArrayIterator_next(arr, & __iter1__) ; a = LandArrayIterator_item(arr, & __iter1__)) {
                    actor_draw(a);
                }
                }
            }
        }
    }
}
void actors_unselect_all(Actors * self) {
    {LandArrayIterator __iter2__ = LandArrayIterator_first(self->array) ; for (Actor * a = LandArrayIterator_item(self->array, & __iter2__) ; LandArrayIterator_next(self->array, & __iter2__) ; a = LandArrayIterator_item(self->array, & __iter2__)) {
        a->selected = 0;
    }
    }
}
void actor_stop(Actor * a) {
    a->walking = 0;
}
void actors_tick(Actors * self) {
    Actor * under_mouse_ally = NULL;
    Actor * under_mouse_enemy = NULL;
    Actor * dead = NULL;
    self->selected = NULL;
    {LandArrayIterator __iter3__ = LandArrayIterator_first(self->array) ; for (Actor * a = LandArrayIterator_item(self->array, & __iter3__) ; LandArrayIterator_next(self->array, & __iter3__) ; a = LandArrayIterator_item(self->array, & __iter3__)) {
        if (a->dead && a->anim_tick >= 120) {
            dead = a;
        }
        if (! a->kind) {
            continue;
        }
        if (a->selected) {
            self->selected = a;
        }
        float dx = wrap(a->x - app->mx);
        int w = a->kind->w;
        int h = a->kind->h;
        if (dx >= - w / 2 && dx <= w / 2 && app->my >= a->y - h && app->my <= a->y) {
            if (a->kind->enemy) {
                under_mouse_enemy = a;
            }
            else {
                under_mouse_ally = a;
            }
        }
    }
    }
    if (dead) {
        int i = land_array_find(self->array, dead);
        actor_unplace(dead);
        land_array_replace_nth(self->array, i, land_array_pop(self->array));
    }
    if (land_mouse_button_clicked(0)) {
        if (under_mouse_ally) {
            actors_unselect_all(self);
            under_mouse_ally->selected = 1;
        }
    }
    if (land_mouse_button_clicked(1)) {
        bool clicked = 0;
        if (under_mouse_enemy) {
            if (self->selected && self->selected->kind->id != AT_HEALER) {
                self->selected->target = under_mouse_enemy;
                actor_stop(self->selected);
                clicked = 1;
            }
        }
        if (under_mouse_ally && ! clicked) {
            if (self->selected && self->selected->kind->id == AT_HEALER) {
                self->selected->target = under_mouse_ally;
                actor_stop(self->selected);
                clicked = 1;
            }
        }
        if (! clicked) {
            if (self->selected) {
                self->selected->tx = app->mx;
                self->selected->ty = app->my;
                self->selected->target = NULL;
                self->selected->walking = 1;
            }
        }
    }
    {LandArrayIterator __iter4__ = LandArrayIterator_first(self->array) ; for (Actor * a = LandArrayIterator_item(self->array, & __iter4__) ; LandArrayIterator_next(self->array, & __iter4__) ; a = LandArrayIterator_item(self->array, & __iter4__)) {
        if (! a->kind) {
            continue;
        }
        actor_tick(a);
    }
    }
}
void actors_reset(Actors * self) {
    land_array_destroy(self->array);
    self->array = land_array_new();
    land_free(self->tilemap);
    self->tilemap = land_calloc(self->w * self->h * sizeof * self->tilemap);
}
