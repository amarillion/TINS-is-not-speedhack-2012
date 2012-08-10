#include "actortype.h"
char const * actor_names[AT_COUNT] = {"Hunter", "Dog", "Healer", "Drake", "Dragon", "Groll", "Papa Groll", "Knight", "Mage", "Bat", "Wolf", };
int levels[AT_COUNT] = {4, 1, 4, 19, 29, 5, 23, 6, 4, 3, 2};
ActorType * actor_type[AT_COUNT];
static ActorType * global_current;
static int thread_data[8];
static LandThread * thread[8];
static void cb(void * data) {
    ActorType * self = global_current;
    int * ip = data;
    int i = * ip;
    Land4x4Matrix t = land_4x4_matrix_identity();
    t = land_4x4_matrix_mul(land_4x4_matrix_rotate(1, 0, 0, pi * - 0.333) , t);
    t = land_4x4_matrix_mul(t, land_4x4_matrix_rotate(0, 0, 1, 2 * pi * (i & 7) / 8));
    t = land_4x4_matrix_mul(t, land_4x4_matrix_scale(32, 32, 32));
    self->traces[i] = graphics_parse(self->graphics, self->w * 4, self->h * 4, t, (i >> 3) / 2.0 - 1.0);
    t = land_4x4_matrix_identity();
    t = land_4x4_matrix_mul(t, land_4x4_matrix_rotate(0, 0, 1, 2 * pi * (i & 7) / 8));
    t = land_4x4_matrix_mul(t, land_4x4_matrix_scale(32, 32, 32));
    self->shadow_traces[i] = graphics_parse(self->graphics, self->w * 4, self->h * 4, t, (i >> 3) / 2.0 - 1.0);
}
void actor_render(ActorType * self) {
    global_current = self;
    for (int j = 0; j < 5; j += 1) {
        for (int i = 0; i < 8; i += 1) {
            thread_data[i] = j * 8 + i;
            thread[i] = land_thread_new(cb, thread_data + i);
        }
        for (int i = 0; i < 8; i += 1) {
            land_thread_destroy(thread[i]);
        }
        for (int i = 0; i < 8; i += 1) {
            self->images[j * 8 + i] = land_image_new(self->w, self->h);
            land_image_set_rgba_data(self->images[j * 8 + i] , self->traces[j * 8 + i] ->rgba);
            unsigned char * p = self->shadow_traces[j * 8 + i] ->rgba;
            for (int y = 0; y < self->h; y += 1) {
                for (int x = 0; x < self->w; x += 1) {
                    p[0] = p[1] = p[2] = 0;
                    p[3] /= 4;
                    p += 4;
                }
            }
            self->shadows[j * 8 + i] = land_image_new(self->w, self->h);
            land_image_set_rgba_data(self->shadows[j * 8 + i] , self->shadow_traces[j * 8 + i] ->rgba);
        }
    }
}
ActorType * actor_type_new(int w, int h, int kind) {
    ActorType * self;
    land_alloc(self);
    self->w = w;
    self->h = h;
    self->level = levels[kind];
    self->initial_hp = 10 * pow(2, (self->level - 1) / 5.0);
    self->initial_damage = pow(2, (self->level - 1) / 5.0);
    if (self->initial_damage > 7) {
        self->initial_damage = 7;
    }
    self->id = kind;
    actor_type[kind] = self;
    land_clear(1, 1.0 * kind / AT_COUNT, 1, 1);
    land_flip();
    char s[100];
    sprintf(s, "raytracing %d/%d (%s)", kind, AT_COUNT, actor_names[kind]);
    land_display_title(s);
    return self;
}
ActorType * actor_hunter_new(void) {
    ActorType * self = actor_type_new(32, 32, AT_HUNTER);
    self->projectile = IT_ARROW;
    self->projectile_speed = 3;
    self->graphics = "\n"
        "/turquoise\n"
        ".5z\n"
        ".75S\n"
        "(.1trz\n"
        "(x .5rx .25s2sz-.5zR)\n"
        "(-x .5rx .25s2sz-.5zR))\n"
        "(z  .5x -.5z .2trx .25s2sz.5zR)\n"
        "(z -.5x -.5z -.2trx .25s2sz.5zR)\n"
        "-z\n"
        "/pink S\n"
        "/sienna\n"
        "(-.1z-.1y S)\n"
        ".75y\n"
        "(-.5x.4/whiteS .25y.2/blackS)\n"
        "( .5x.4/whiteS .25y.2/blackS)";
    actor_render(self);
    return self;
}
ActorType * actor_knight_new(void) {
    ActorType * self = actor_type_new(32, 32, AT_KNIGHT);
    self->graphics = "\n"
        "/silver\n"
        ".5z\n"
        ".75S\n"
        "(.1trz\n"
        "(x .5rx -.25z (/silver.25s2szR)/palegreen-.5z.05szR)\n"
        "(-x .5rx -.25z(/silver2sz.25R)/lightcyan-.5z-.4y8sy.1B))\n"
        "/silver\n"
        "(z  .5x -.5z .2trx .25s2sz.5zR)\n"
        "(z -.5x -.5z -.2trx .25s2sz.5zR)\n"
        "-z\n"
        "/pink S\n"
        "/silver\n"
        "(-.1z-.1y S)\n"
        ".75y\n"
        "(-.5x.4/whiteS .25y.2/blackS)\n"
        "( .5x.4/whiteS .25y.2/blackS)";
    actor_render(self);
    return self;
}
ActorType * actor_healer_new(void) {
    ActorType * self = actor_type_new(32, 32, AT_HEALER);
    self->projectile = IT_HEAL;
    self->projectile_speed = 5;
    self->graphics = "\n"
        "/white\n"
        ".5z\n"
        ".75S\n"
        "(.1trz\n"
        "(x .5rx .25s2sz-.5zR)\n"
        "(-x .5rx .25s2sz-.5zR))\n"
        "(z  .5x -.5z .2trx .25s2sz.5zR)\n"
        "(z -.5x -.5z -.2trx .25s2sz.5zR)\n"
        "-z\n"
        "/pink S\n"
        "/black\n"
        "(\n"
        "-.1y(rzH) (-.5z.2szR) /white -.9z.2szR\n"
        ") #hair\n"
        ".75y\n"
        "(-.5x.4/whiteS .25y.2/blackS)\n"
        "( .5x.4/whiteS .25y.2/blackS)";
    actor_render(self);
    return self;
}
ActorType * actor_mage_new(void) {
    ActorType * self = actor_type_new(32, 32, AT_MAGE);
    self->projectile = IT_MAGIC_MISSILE;
    self->projectile_speed = 4;
    self->graphics = "\n"
        "/orchid\n"
        ".5z\n"
        ".75S\n"
        "(.1trz\n"
        "(x .5rx .25s2sz-.5zR)\n"
        "(-x .5rx -.25z(2sz.25R)/yellow-.5z-.4y8sy.1R))\n"
        "/orchid\n"
        "(z  .5x -.5z .2trx .25s2sz.5zR)\n"
        "(z -.5x -.5z -.2trx .25s2sz.5zR)\n"
        "-z\n"
        "/pink S\n"
        "/maroon\n"
        "(\n"
        "-.1y(.75szrzH) (-.5z.2szR) /plum -z.4szrx.5zC\n"
        ") #hair\n"
        ".75y\n"
        "(-.5x.4/whiteS .25y.2/blackS)\n"
        "( .5x.4/whiteS .25y.2/blackS)";
    actor_render(self);
    return self;
}
ActorType * actor_drake_new(void) {
    ActorType * self = actor_type_new(64, 64, AT_DRAKE);
    self->projectile = IT_FIREBALL;
    self->projectile_speed = 5;
    self->enemy = 1;
    self->graphics = "\n"
        "1.7s\n"
        "-.2y\n"
        "/chartreuse\n"
        ".75S #body\n"
        "(-y2sy.5rx.3C) # tail\n"
        "# wings\n"
        "(.5x .2try x .5rx .7s.25sy B)\n"
        "(-.5x -.2try -x .5rx .7s.25sy B)\n"
        "# feet\n"
        "(-.5z .3y\n"
        "(z  .5x -.2trx .25s2sz.5zR)\n"
        "(z -.5x -.2trx .25s2sz.5zR))\n"
        "(-.5z -.3y\n"
        "(z -.5x .2trx .25s2sz.5zR)\n"
        "(z .5x .2trx .25s2sz.5zR))\n"
        "-z\n"
        ".5y\n"
        "S\n"
        "(1.2rx(1.2z.2C) 8{-.1rx(1.2z.2C)} )\n"
        "(.2zy1.5sy.7S)\n"
        "# eyes\n"
        ".8y\n"
        "-.4z\n"
        "(-.6x.3/whiteS .25y.2/blackS)\n"
        "( .6x.3/whiteS .25y.2/blackS)\n"
        "";
    actor_render(self);
    return self;
}
ActorType * actor_dragon_new(void) {
    ActorType * self = actor_type_new(112, 112, AT_DRAGON);
    self->projectile = IT_FIREBALL;
    self->projectile_speed = 5;
    self->enemy = 1;
    self->graphics = "\n"
        "2.5s\n"
        "-.5y\n"
        "/deeppink\n"
        ".75S #body\n"
        "(-y3sy.5rx.3C) # tail\n"
        "# wings\n"
        "(.5x .2try .2rz 3{(x.5ry4sz.25C)-.2rz})\n"
        "(-.5x -.2try .2rz 3{(-x-.5ry4sz.25C)-.2rz})\n"
        "# feet\n"
        "(-.5z .3y\n"
        "(z  .5x -.2trx .25s2sz.5zC)\n"
        "(z -.5x -.2trx .25s2sz.5zC))\n"
        "(-.5z -.3y\n"
        "(z -.5x .2trx .25s2sz.5zC)\n"
        "(z .5x .2trx .25s2sz.5zC))\n"
        "-z\n"
        ".5y\n"
        "S\n"
        "(1.2rx(1.2z.2C) 8{-.1rx(1.2z.2C)} )\n"
        "(.2zy1.5syS) #nose\n"
        "# eyes\n"
        ".8y\n"
        "-.4z\n"
        "(-.6x.3/whiteS .25y.2/blackS)\n"
        "( .6x.3/whiteS .25y.2/blackS)\n"
        "";
    actor_render(self);
    return self;
}
ActorType * actor_dog_new(void) {
    ActorType * self = actor_type_new(20, 20, AT_DOG);
    self->graphics = "\n"
        "0.5s\n"
        "-.2y.7z\n"
        "/peru\n"
        ".75S #body\n"
        "(-y2sy.5rx.3S) # tail\n"
        "# feet\n"
        "(-.5z .3y\n"
        "(z  .5x -.2trx .25s2sz.5zR)\n"
        "(z -.5x -.2trx .25s2sz.5zR))\n"
        "(-.5z -.3y\n"
        "(z -.5x .2trx .25s2sz.5zR)\n"
        "(z .5x .2trx .25s2sz.5zR))\n"
        "-z\n"
        ".5y\n"
        "S # head\n"
        "(.2zy1.5sy.7S) # nose\n"
        "\n"
        "\n"
        "# ears\n"
        "(-.3y-.6z\n"
        "(-.6x.4S)\n"
        "(.6x.4S)\n"
        ")\n"
        "# eyes\n"
        ".8y\n"
        "-.4z\n"
        "(-.6x.3/whiteS .25y.2/blackS)\n"
        "( .6x.3/whiteS .25y.2/blackS)\n"
        "";
    actor_render(self);
    return self;
}
ActorType * actor_groll_new(void) {
    ActorType * self = actor_type_new(32, 32, AT_GROLL);
    self->enemy = 1;
    self->graphics = "\n"
        ".2tz\n"
        "S\n"
        "/red\n"
        ".8y\n"
        "(-.5x.3S)\n"
        "(.5x.3S)\n"
        "";
    actor_render(self);
    return self;
}
ActorType * actor_papagroll_new(void) {
    ActorType * self = actor_type_new(64, 64, AT_PAPAGROLL);
    self->enemy = 1;
    self->graphics = "\n"
        "2s\n"
        ".2tz\n"
        "S\n"
        "/red\n"
        ".8y\n"
        "(-.5x.3S)\n"
        "(.5x.3S)\n"
        "";
    actor_render(self);
    return self;
}
ActorType * actor_bat_new(void) {
    ActorType * self = actor_type_new(32, 32, AT_BAT);
    self->enemy = 1;
    self->graphics = "\n"
        "/maroon\n"
        ".2tz\n"
        "S\n"
        "(-.5y\n"
        "(x.3try.1szryH)\n"
        "(-x-.3try.1szryH))\n"
        "/yellow\n"
        ".8y\n"
        "(-.5x.3S)\n"
        "(.5x.3S)\n"
        "";
    actor_render(self);
    return self;
}
ActorType * actor_wolf_new(void) {
    ActorType * self = actor_type_new(32, 32, AT_WOLF);
    self->enemy = 1;
    self->graphics = "\n"
        "0.8s\n"
        "-.2y.7z\n"
        "/wheat\n"
        ".75S #body\n"
        "(-y2sy.5rx.3S) # tail\n"
        "# feet\n"
        "(-.5z .3y\n"
        "(z  .5x -.2trx .25s2sz.5zR)\n"
        "(z -.5x -.2trx .25s2sz.5zR))\n"
        "(-.5z -.3y\n"
        "(z -.5x .2trx .25s2sz.5zR)\n"
        "(z .5x .2trx .25s2sz.5zR))\n"
        "-z\n"
        ".5y\n"
        "S # head\n"
        "(.2zy-.5rx.7C) # nose\n"
        "\n"
        "\n"
        "# ears\n"
        "(-.3y-z\n"
        "(-.3xry.5C)\n"
        "(.3xry.5C)\n"
        ")\n"
        "# eyes\n"
        ".8y\n"
        "-.4z\n"
        "(-.6x.3/tomato S .25y.2/blackS)\n"
        "( .6x.3/springgreen S .25y.2/blackS)\n"
        "";
    actor_render(self);
    return self;
}
void actor_types_init(void) {
    actor_papagroll_new();
    actor_dragon_new();
    actor_wolf_new();
    actor_bat_new();
    actor_mage_new();
    actor_hunter_new();
    actor_knight_new();
    actor_healer_new();
    actor_dog_new();
    actor_groll_new();
    actor_drake_new();
}
