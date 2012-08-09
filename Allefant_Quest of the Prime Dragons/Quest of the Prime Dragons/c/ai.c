#include "ai.h"
void ai_tick(Actor * self) {
    if (self->special == 900) {
        special(self);
    }
    if (! self->target) {
        float mind = 0;
        Actor * closest = NULL;
        {LandArrayIterator __iter0__ = LandArrayIterator_first(app->actors->array) ; for (Actor * a = LandArrayIterator_item(app->actors->array, & __iter0__) ; LandArrayIterator_next(app->actors->array, & __iter0__) ; a = LandArrayIterator_item(app->actors->array, & __iter0__)) {
            if (a->kind->enemy) {
                continue;
            }
            float d = actor_distance(self, a);
            if (! closest || d < mind) {
                mind = d;
                closest = a;
            }
        }
        }
        self->target = closest;
    }
    if (! self->target) {
        return;
    }
}
