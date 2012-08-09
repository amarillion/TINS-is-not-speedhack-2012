#ifndef jumpTheGun_entity_hpp
#define jumpTheGun_entity_hpp
#include "vector.hpp"
namespace game
{

class entity
{
    public:
    virtual void update();
    private:
    game::vector movementVector_;
    game::polygon hitBox_;
    std::vector<bool> flags_;
    std::vector<int> values_;
};

}
#endif

