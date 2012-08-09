#ifndef jumpTheGun_polygon_hpp
#define jumpTheGun_polygon_hpp
#include "vector.hpp"
namespace game
{

class polygon
{
    public:
    game::vector edge(int num);
    game::vector point(int num);
    void move(game::vector &movementVector) { originPoint_ = originPoint_ + movementVector;}
    game::vector getCenter();
    private:
    game::vector originPoint_;
    std::vector<game::vector> points_;
};

}
#endif
