#include <allegro.h>
#include <list>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

bool cga_mode = false;
std::string save_name = "save.sav";


std::string binary(unsigned int i)
{
    if (i == 0)
        return "0";

    std::string rval;
    while (i > 0)
    {
        rval.push_back('0' + (i & 1));
        i >>= 1;
    }
//    std::reverse(rval.begin(), rval.end());
    return rval;
}


class Object;
typedef std::list<Object *> ObjectList;

enum ObjectType { O_STATIC, O_PLAYER, O_ENEMY, O_EGG, O_SPAWNER };
std::string object_names[] = { "STATIC", "PLAYER", "ENEMY", "EGG", "SPAWNER" };

class Point
{
  public:
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point &p)
    {
        return ((x == p.x) && (y == p.y));
    }
    int x, y;
};

class Line
{
  public:
    Line (Point a, Point b, int col) : a(a), b(b), col(col) {}
    bool operator==(const Line &l)
    {
        return ((a == l.a) && (b == l.b));
    }
    Point a, b;
    int col;
};

int *colors;

void cga_line(BITMAP *buffer, float x1, float y1, float x2, float y2, int col)
{
    if (cga_mode)
        line(buffer, x1, y1 / 2, x2, y2 / 2, col);
    else
        line(buffer, x1, y1, x2, y2, col);
}

int cga_pos(int number)
{
    return SCREEN_H - number;
}

class Sprite
{
  public:
    Sprite(std::string f) : max_x(0), max_y(0)
    {
        std::ifstream fin(f.c_str());
        if (!fin.fail())
        {
            while (true)
            {
                int ax, ay, bx, by, c;
                fin >> ax >> ay >> bx >> by >> c;

                if (ax > max_x) max_x = ax;
                if (bx > max_x) max_x = bx;
                if (ay > max_y) max_y = ay;
                if (by > max_y) max_y = by;

                lines.push_back(Line(Point(ax, ay), Point(bx, by), c));
                if (fin.eof())
                    break;
            }
            fin.close();
        }
    }

    void render(BITMAP *buffer, float x, float y, float width, float height, bool flip)
    {
        float x_mult = width / max_x;
        float y_mult = height / max_y;

        if (flip)
        {
            x_mult *= -1;
            for(std::list<Line>::iterator i = lines.begin(); i != lines.end(); ++i)
                cga_line(buffer, x + width + i->a.x * x_mult, y + i->a.y * y_mult, x + width + i->b.x * x_mult, y + i->b.y * y_mult, colors[i->col]);
        }
        else
            for(std::list<Line>::iterator i = lines.begin(); i != lines.end(); ++i)
                cga_line(buffer, x + i->a.x * x_mult, y + i->a.y * y_mult, x + i->b.x * x_mult, y + i->b.y * y_mult, colors[i->col]);
    }

    float max_x, max_y;
    std::list<Line> lines;
};


class Sprites
{

  public:
    Sprites() : s_player("player.spr"), s_playerf("playerf.spr"), s_spawner("spawner.spr"), s_enemy("enemy.spr"), s_enemyf("enemyf.spr"), s_egg("egg.spr"), s_platform("platform.spr")
    {
    }
    Sprite *get(ObjectType ot, bool alternative)
    {
        if (alternative)
        {
            if (ot == O_PLAYER)
                return &s_playerf;
            if (ot == O_ENEMY)
                return &s_enemyf;
        }
        switch (ot)
        {
          case O_STATIC: return &s_platform;
          case O_PLAYER: return &s_player;
          case O_ENEMY: return &s_enemy;
          case O_EGG: return &s_egg;
          case O_SPAWNER: return &s_spawner;
        }
        return &s_platform;
    }
    Sprite s_player, s_playerf, s_spawner, s_enemy, s_enemyf, s_egg, s_platform;
};

Sprites sprites;



class Object
{
  public:
    ObjectType ot;
    float x, y, width, height;
    bool dead;
    bool facing_right;
    bool alt_sprite;

    Object(ObjectType ot, float x, float y, float w, float h) : ot(ot), x(x), y(y), width(w), height(h), dead(false), facing_right(false), alt_sprite(false)
    {
    }

    void kill()
    {
        dead = true;
    }
    bool isDead()
    {
        return dead;
    }


    void render(BITMAP *buffer)
    {
        float _x = x + 4 - (x / float(SCREEN_W) * 4);
        float _y = y + 4 - (y / float(SCREEN_H) * 4);
        float _width = width - 4;
        float _height = height - 4;
        sprites.get(ot, alt_sprite)->render(buffer, x, y, width, height, facing_right);
        sprites.get(ot, alt_sprite)->render(buffer, _x, _y, _width, _height, facing_right);
    }

    virtual void update(ObjectList) = 0;
    virtual std::string serialise()
    {
        std::stringstream ss;
        ss << x << " " << y << " " << width << " " << height << " ";
        return ss.str();
    }
};

bool below_screen(Object *o)
{
    return (o->y > SCREEN_H);
}

int enemy_count(ObjectList ol)
{
    int rval = 0;
    for (ObjectList::iterator i = ol.begin(); i != ol.end(); ++i)
        if (((*i)->ot == O_ENEMY) || ((*i)->ot == O_EGG))
            ++rval;
    return rval;
}


unsigned int colide(Object *a, Object *b)
{
    float Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
    Ax = a->x;
    Bx = a->x + a->width;
    Cx = b->x;
    Dx = b->x + b->width;

    Ay = a->y;
    By = a->y + a->height;
    Cy = b->y;
    Dy = b->y + b->height;

    if ((Ax > Dx) || (Bx < Cx) || (Ay > Dy) || (By < Cy))
        return 0;

    By += 2;

    unsigned int result = 0;
    if ((Ax > Cx) && (Ax < Dx)) result |= (1 << 0);
    if ((Bx > Cx) && (Bx < Dx)) result |= (1 << 1);
    if ((Ay > Cy) && (Ay < Dy)) result |= (1 << 2);
    if ((By > Cy) && (By < Dy)) result |= (1 << 3);
    return result;
}


class StaticObject : public Object
{
  public:
    StaticObject(float x, float y, float w, float h) : Object(O_STATIC, x, y, w, h)
    {
    }
    void update(ObjectList)
    {
    }
};

class PhysicsObject : public Object
{
  public:
    PhysicsObject(ObjectType ot, float x, float y, float w, float h, float g) : Object(ot, x, y, w, h), dx(0.0), dy(0.0), gravity(g), gx(gravity)
    {
    }

    void physics(ObjectList ol)
    {
        float ox = x;
        float oy = y;
        x += dx;
        y += dy;
        dy += gx;
        dx *= 0.95;

        if (dx > 0)
            facing_right = true;
        if (dx < 0)
            facing_right = false;

        if (x < -width)
            x += SCREEN_W + width;

        if (x > SCREEN_W)
            x -= (SCREEN_W + width);


        for (ObjectList::iterator i = ol.begin(); i != ol.end(); ++i)
        {
            if (*i == this)
                continue;

            unsigned int col = colide(this, *i);

            if ((dy > 0) && (col & (1 << 3)))
            {
                y = (*i)->y - height;
                dy = 0;
                gx = 0;
            }

            if (!(col & (1 << 3)))
            {
                gx = gravity;
            }

            if ((dy < 0) && (col & (1 << 2)))
            {
                y = oy;
                dy = gravity;
            }

            if (gx != 0)
            {
                if ((dx > 0) && (col & (1 << 1)))
                    x = ox;
                if ((dx < 0) && (col & (1 << 0)))
                    x = ox;
            }
        }
    }
    virtual std::string serialise()
    {
        std::stringstream ss;
        ss << Object::serialise() << " " << gravity << " ";
        return ss.str();
    }

    float dx, dy, gravity, gx;
};

class PlayerObject : public PhysicsObject
{
  public:
    PlayerObject(float x, float y, float w, float h, float g) : PhysicsObject(O_PLAYER, x, y, w, h, g), flap(false)
    {
    }

    void update(ObjectList ol)
    {

        if (key[KEY_LEFT]) dx = -4;
        if (key[KEY_RIGHT]) dx = 4;

        if (key[KEY_UP])
        {
            if (!flap)
            {
                dy -= 8;
                gx = gravity;
                flap = true;
                alt_sprite = !alt_sprite;
            }
        }
        else
            flap = false;
        physics(ol);


        for (ObjectList::iterator i = ol.begin(); i != ol.end(); ++i)
        {
            if ((*i)->ot == O_ENEMY)
            {
                unsigned int col = colide(*i, this);
                if (col & (1 << 3))
                    kill();
            }
        }

        if (below_screen(this))
            kill();
    }

    bool flap;

};

class EnemyObject : public PhysicsObject
{
  public:
    EnemyObject(float x, float y, float w, float h, float g) : PhysicsObject(O_ENEMY, x, y, w, h, g), flap_count(0)
    {
    }

    void update(ObjectList ol)
    {
        Object *p = NULL;

        for (ObjectList::iterator i = ol.begin(); i != ol.end(); ++i)
        {
            if (*i == this)
                continue;

            if ((*i)->ot == O_PLAYER)
            {
                p = *i;
                break;
            }
        }

        if (p != NULL)
        {
            if (y > p->y)
            {
                if (flap_count == 0)
                {
                    dy -= 8;
                    gx = gravity;
                    flap_count = 6;
                    alt_sprite = !alt_sprite;
                }
                else
                    --flap_count;
            }

            if (x > p->x) dx = -3;
            if (x < p->x) dx = 3;

            unsigned int col = colide(p, this);
            if (col & (1 << 3))
            {
                kill();
            }

        }

        physics(ol);
    }

    int flap_count;

};

class EggObject : public PhysicsObject
{
  public:
    EggObject(float x, float y, float w, float h, float g) : PhysicsObject(O_EGG, x, y, w, h, g), hatch_count(75)
    {
    }

    void update(ObjectList ol)
    {
        Object *p = NULL;

        for (ObjectList::iterator i = ol.begin(); i != ol.end(); ++i)
        {
            if (*i == this)
                continue;

            if ((*i)->ot == O_PLAYER)
            {
                p = *i;
                break;
            }
        }

        if (p != NULL)
        {
            unsigned int col = colide(p, this);
            if (col & (1 << 0))
                dx -= 10;
            if (col & (1 << 1))
                dx += 10;
        }
        if (--hatch_count == 0)
            kill();

        physics(ol);
    }

    int hatch_count;

};

class EggSpawnerObject : public Object
{
  public:
    EggSpawnerObject(float x, float y, float w, float h) : Object(O_SPAWNER, x, y, w, h)
    {
    }

    void update(ObjectList ol)
    {
    }

};

void write(std::ofstream &f, int level, int high_score, int score, ObjectList ol)
{
    f << level << std::endl;
    f << high_score << std::endl;
    f << score << std::endl;
    for (ObjectList::iterator i = ol.begin(); i != ol.end(); ++i)
    {
        f << object_names[(*i)->ot] << std::endl;
        f << (*i)->serialise() << std::endl;
    }
}

void read(std::ifstream &f, int &level, int &high_score, int &score, ObjectList &ol)
{
    std::string name;
    f >> level;
    f >> high_score;
    f >> score;
    float one, two, three, four, five;
    while (!f.eof())
    {
        f >> name;
        std::cout << name << std::endl;
        f >> one >> two >> three >> four;
        if (name == "STATIC")
        {
            ol.push_back(new StaticObject(one, two, three, four));
        }
        else if (name == "PLAYER")
        {
            f >> five;
            ol.push_back(new PlayerObject(one, two, three, four, five));
        }
        else if (name == "ENEMY")
        {
            f >> five;
            ol.push_back(new EnemyObject(one, two, three, four, five));
        }
        else if (name == "EGG")
        {
            f >> five;
            ol.push_back(new EggObject(one, two, three, four, five));
        }
        else if (name == "SPAWNER")
        {
            ol.push_back(new EggSpawnerObject(one, two, three, four));
        }
    }
}



volatile int ticker = 0;
void ticker_handler()
{
    ++ticker;
}
END_OF_FUNCTION(ticker_handler)

unsigned int primes[] = { 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127 };
unsigned int primes_count = 32;

void init(int &level, int &high_score, int &score, ObjectList &objects)
{
    level = -1;
    score = 0;
    objects.erase(objects.begin(), objects.end());


    bool restart = false;


    std::ifstream fin(save_name.c_str());
    if (!fin.fail())
    {
        read(fin, level, high_score, score, objects);
        fin.close();
        unlink(save_name.c_str());
    }
    else
        restart = true;


    if (restart == true)
    {
        objects.push_back(new StaticObject(60, cga_pos(20), 520, 20));
        objects.push_back(new EggSpawnerObject(120, cga_pos(19), 30, 10));
        objects.push_back(new EggSpawnerObject(500, cga_pos(19), 30, 10));

        objects.push_back(new StaticObject(-300, cga_pos(140), 400, 20));
        objects.push_back(new EggSpawnerObject(40, cga_pos(139), 30, 10));

        objects.push_back(new StaticObject(540, cga_pos(140), 400, 20));
        objects.push_back(new EggSpawnerObject(570, cga_pos(139), 30, 10));

        objects.push_back(new StaticObject(80, cga_pos(260), 140, 20));
        objects.push_back(new EggSpawnerObject(135, cga_pos(259), 30, 10));

        objects.push_back(new StaticObject(420, cga_pos(260), 140, 20));
        objects.push_back(new EggSpawnerObject(475, cga_pos(259), 30, 10));

        objects.push_back(new StaticObject(220, cga_pos(380), 200, 20));
        objects.push_back(new EggSpawnerObject(305, cga_pos(379), 30, 10));

        objects.push_back(new PlayerObject(SCREEN_W / 2, SCREEN_H / 2, 20, 40, 1));
    }
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        cga_mode = true;
        save_name = "save_cga.sav";
    }
    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();


    set_color_depth(32);
    if (!cga_mode)
    {
        set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
    }
    else
    {
        set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 400, 0, 0);
    }


    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *buffer_cga = create_bitmap(SCREEN_W, 200);


    int color_table[] = {
        makecol(0,0,255),
        makecol(0,255,0),
        makecol(0,255,255),
        makecol(255,0,0),
        makecol(255,0,255),
        makecol(255,255,0),
        makecol(255,255,255),
        makecol(128,128,128)
    };
    colors = color_table;


    bool pause = true;
    bool reset = false;

    LOCK_VARIABLE(ticker);
    LOCK_FUNCTION(ticker_handler);

    int level = -1;
    int score = 0;
    int high_score = 0;
    ObjectList objects;
    init(level, high_score, score, objects);

    install_int_ex(ticker_handler, BPS_TO_TIMER(25));

    while(true)
    {

        clear(buffer);
        clear(buffer_cga);
        if (!pause)
        {

            if (key[KEY_F3] || reset)
            {
                init(level, high_score, score, objects);
                pause = true;
                reset = false;
            }

            while(ticker > 0)
            {
                int ec = enemy_count(objects);
                if (ec == 0)
                {
                    ++level;
                    if (level == 32)
                        level = 0;
                    std::string l = binary(primes[level]);
                    for(ObjectList::iterator i = objects.begin(); i != objects.end(); ++i)
                    {
                        if ((*i)->ot == O_SPAWNER)
                        {
                            if (l[0] == '1')
                                objects.push_back(new EggObject((*i)->x + 10, (*i)->y - 10, 10, 10, 1));
                            l.erase(l.begin());
                            if (l.size() == 0)
                                break;
                        }
                    }
                }

                for(ObjectList::iterator i = objects.begin(); i != objects.end(); ++i)
                    (*i)->update(objects);

                for(ObjectList::iterator i = objects.begin(); i != objects.end(); ++i)
                {
                    if ((*i)->isDead())
                    {
                        if ((*i)->ot == O_EGG)
                        {
                            Object *o = *i;
                            *i = new EnemyObject((*i)->x, (*i)->y - 30, 20, 40, 1);
                            delete o;
                        }
                        else if ((*i)->ot == O_ENEMY)
                        {
                            Object *o = *i;
                            *i = new EggObject((*i)->x, (*i)->y, 10, 10, 1);
                            delete o;
                            score += 3;
                        }
                        else if ((*i)->ot == O_PLAYER)
                        {
                            pause = true;
                            reset = true;
                            clear_keybuf();
                        }
                    }
                    if (below_screen(*i))
                    {
                        if ((*i)->ot == O_EGG)
                            score += 7;
                        if ((*i)->ot == O_ENEMY)
                            score += 11;
                    }
                }

                ObjectList::iterator b = std::remove_if(objects.begin(), objects.end(), below_screen);

    //          should use smart pointers, instead we will just leak memory. :D
    //            for(ObjectList::iterator i = b; i != objects.end(); ++i) delete *i;
                objects.erase(b, objects.end());

                if (score > high_score)
                    high_score = score;

                --ticker;
            }
        }
        else
        {
            ticker = 0;
            if (keypressed())
                pause = false;
        }

        if(!cga_mode)
        {
            for(ObjectList::iterator i = objects.begin(); i != objects.end(); ++i)
                (*i)->render(buffer);
        }
        else
        {
            for(ObjectList::iterator i = objects.begin(); i != objects.end(); ++i)
                (*i)->render(buffer_cga);
            stretch_blit(buffer_cga, buffer, 0, 0, SCREEN_W, 200, 0, 0, SCREEN_W, SCREEN_H);
        }

        textprintf_ex(buffer, font, 0, 0, makecol(255, 255, 255), -1, "High Score: %d", high_score);
        textprintf_ex(buffer, font, 0, 10, makecol(255, 255, 255), -1, "Level: %d", level + 1);
        textprintf_ex(buffer, font, 0, 20, makecol(255, 255, 255), -1, "Score: %d", score);

        if (pause)
            textout_centre_ex(buffer, font, "Paused - Press any key to continue", SCREEN_W / 2, SCREEN_H / 2, makecol(255,0, 0), -1);


        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        if (key[KEY_F4])
        {
            save_bitmap("screen.bmp", buffer, NULL);
        }


        if (key[KEY_ESC])
            break;
    }

    std::ofstream fout(save_name.c_str());
    write(fout, level, high_score, score, objects);
    fout.close();

    destroy_bitmap(buffer_cga);
    destroy_bitmap(buffer);


    return 0;
}
END_OF_MAIN()

