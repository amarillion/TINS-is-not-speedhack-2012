#include <allegro.h>
#include <list>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>


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

#define GRADE   20

int main()
{



    allegro_init();
    install_keyboard();

    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

    std::list<Line> lines;


    std::ifstream fin("picture.txt");
    if (!fin.fail())
    {
        while (true)
        {
            int ax, ay, bx, by, c;
            fin >> ax >> ay >> bx >> by >> c;
            if (fin.eof())
                break;
            lines.push_back(Line(Point(ax, ay), Point(bx, by), c));
        }
        fin.close();
    }


    int colors[] = {
        makecol(0,0,255),
        makecol(0,255,0),
        makecol(0,255,255),
        makecol(255,0,0),
        makecol(255,0,255),
        makecol(255,255,0),
        makecol(255,255,255),
        makecol(128,128,128)
    };

    int col = 0;
    while(true)
    {
        if (key[KEY_1]) col = 0;
        if (key[KEY_2]) col = 1;
        if (key[KEY_3]) col = 2;
        if (key[KEY_4]) col = 3;
        if (key[KEY_5]) col = 4;
        if (key[KEY_6]) col = 5;
        if (key[KEY_7]) col = 6;
        if (key[KEY_8]) col = 7;


        clear(buffer);
        for (int y = 0; y < SCREEN_H; y += GRADE)
        {
            for (int x = 0; x < SCREEN_W; x += GRADE)
            {
                putpixel(buffer, x, y, makecol(255,255,255));
            }
        }

        Point mouse((mouse_x / GRADE) * GRADE, (mouse_y / GRADE) * GRADE);
        Point horizontal(((mouse_x / GRADE) + 1) * GRADE, (mouse_y / GRADE) * GRADE);
        Point vertical((mouse_x / GRADE) * GRADE, ((mouse_y / GRADE) + 1) * GRADE);

        circle(buffer, mouse.x, mouse.y, 3, colors[col]);


        for(std::list<Line>::iterator i = lines.begin(); i != lines.end(); ++i)
        {
            line(buffer, i->a.x, i->a.y, i->b.x, i->b.y, colors[i->col]);
        }

        show_mouse(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        show_mouse(NULL);


        if (mouse_b & 1)
        {
            Line l(mouse, horizontal, col);
            lines.remove(l);
            lines.push_back(l);
        }
        if (mouse_b & 2)
        {
            Line l(mouse, vertical, col);
            lines.remove(l);
            lines.push_back(l);
        }
        if (mouse_b & 4)
        {
            lines.remove(Line(mouse, horizontal, col));
            lines.remove(Line(mouse, vertical, col));
        }

        if (key[KEY_N])
        {

            int left = 10000, top = 10000;
            for(std::list<Line>::iterator i = lines.begin(); i != lines.end(); ++i)
            {
                if (i->a.x < left) left = i->a.x;
                if (i->a.y < top) top = i->a.y;
            }
            for(std::list<Line>::iterator i = lines.begin(); i != lines.end(); ++i)
            {
                i->a.x -= left;
                i->b.x -= left;
                i->a.y -= top;
                i->b.y -= top;
            }


        }

        if (key[KEY_ESC])
            break;
    }

    std::ofstream fout("picture.txt");
    if (!fout.fail())
    {
        for(std::list<Line>::iterator i = lines.begin(); i != lines.end(); ++i)
        {
            fout << i->a.x << " " << i->a.y << " " << i->b.x << " " << i->b.y << " " << i->col << std::endl;
        }
        fin.close();
    }


    return 0;
}
END_OF_MAIN()
