#include "special.h"
void special(Actor * a) {
    if (! a) {
        return;
    }
    if (a->special < 900) {
        return;
    }
    a->special = 0;
    if (a->kind->id == AT_HEALER) {
        {LandArrayIterator __iter0__ = LandArrayIterator_first(app->actors->array) ; for (Actor * b = LandArrayIterator_item(app->actors->array, & __iter0__) ; LandArrayIterator_next(app->actors->array, & __iter0__) ; b = LandArrayIterator_item(app->actors->array, & __iter0__)) {
            if (b->dead) {
                continue;
            }
            if (! b->kind->enemy) {
                b->hp = b->max_hp;
            }
        }
        }
    }
    if (a->kind->id == AT_KNIGHT) {
        {LandArrayIterator __iter1__ = LandArrayIterator_first(app->actors->array) ; for (Actor * b = LandArrayIterator_item(app->actors->array, & __iter1__) ; LandArrayIterator_next(app->actors->array, & __iter1__) ; b = LandArrayIterator_item(app->actors->array, & __iter1__)) {
            if (b->dead) {
                continue;
            }
            if (b->kind->enemy) {
                b->target = a;
            }
        }
        }
    }
    if (a->kind->id == AT_DOG) {
        {LandArrayIterator __iter2__ = LandArrayIterator_first(app->actors->array) ; for (Actor * b = LandArrayIterator_item(app->actors->array, & __iter2__) ; LandArrayIterator_next(app->actors->array, & __iter2__) ; b = LandArrayIterator_item(app->actors->array, & __iter2__)) {
            if (b->dead) {
                continue;
            }
            if (b->kind->enemy) {
                b->target = NULL;
                b->tx = (a->x + b->x) / 2;
                b->ty = (a->y + b->y) / 2;
                b->walking = 1;
            }
        }
        }
    }
    if (a->kind->id == AT_HUNTER) {
        {LandArrayIterator __iter3__ = LandArrayIterator_first(app->actors->array) ; for (Actor * b = LandArrayIterator_item(app->actors->array, & __iter3__) ; LandArrayIterator_next(app->actors->array, & __iter3__) ; b = LandArrayIterator_item(app->actors->array, & __iter3__)) {
            if (b->dead) {
                continue;
            }
            if (b->kind->enemy) {
                b->frozen = 300;
            }
        }
        }
    }
    if (a->kind->id == AT_MAGE) {
        {LandArrayIterator __iter4__ = LandArrayIterator_first(app->actors->array) ; for (Actor * b = LandArrayIterator_item(app->actors->array, & __iter4__) ; LandArrayIterator_next(app->actors->array, & __iter4__) ; b = LandArrayIterator_item(app->actors->array, & __iter4__)) {
            if (b->dead) {
                continue;
            }
            if (b->kind->enemy) {
                b->hp -= 7;
                if (b->hp < 1) {
                    b->hp = 1;
                }
                b->frozen = 60;
            }
        }
        }
    }
    if (a->kind->id == AT_PAPAGROLL) {
        {LandArrayIterator __iter5__ = LandArrayIterator_first(app->actors->array) ; for (Actor * b = LandArrayIterator_item(app->actors->array, & __iter5__) ; LandArrayIterator_next(app->actors->array, & __iter5__) ; b = LandArrayIterator_item(app->actors->array, & __iter5__)) {
            if (b->dead) {
                continue;
            }
            if (b->kind->id == AT_GROLL) {
                b->hp = b->max_hp;
            }
        }
        }
    }
}
