//#include "hud.hpp"
//ALLEGRO_COLOR hud::player1Color_;
//ALLEGRO_COLOR hud::player2Color_;
//player * hud::player1_;
//player * hud::player2_;
//int hud::height_;
//int hud::y_;
//void hud::initHud(std::string iniFile,std::string hudSection, map& tempMap)
//{
//    std::vector<player *> players = tempMap.getPlayers();
//    player1_ = players.at(0);
//    player2_ = players.at(1);
//    std::vector<std::string>  intsToGet;
//    int player1R, player1G, player1B;
//    int player2R, player2G, player2B;
//    intsToGet.push_back("player1R");
//    intsToGet.push_back("player1G");
//    intsToGet.push_back("player1B");
//    intsToGet.push_back("player2R");
//    intsToGet.push_back("player2G");
//    intsToGet.push_back("player2B");
//    intsToGet.push_back("height");
//    intsToGet.push_back("y");
//    std::vector<int *> intsToAssign;
//    intsToAssign.push_back(&player1R);
//    intsToAssign.push_back(&player1G);
//    intsToAssign.push_back(&player1B);
//    intsToAssign.push_back(&player2R);
//    intsToAssign.push_back(&player2G);
//    intsToAssign.push_back(&player2B);
//    intsToAssign.push_back(&height_);
//    intsToAssign.push_back(&y_);
//    if(intsToAssign.size() != intsToGet.size())
//    {
//        fprintf(stderr, "Dumbass, you forgot to fill in intsToGet/Assign properly. Go kill yourself.\n");
//        exit(EXIT_FAILURE);
//    }
//    getValue value;
//    for(int i = 0;i<intsToGet.size();++i)
//    {
//        value.update(intsToGet.at(i),hudSection,iniFile);
//        if(!value.isNumber)
//        {
//            fprintf(stderr, "Recieved string value where number was expected: Name: %s\n",intsToGet.at(i).data());
//            exit(EXIT_FAILURE);
//        }
//        *intsToAssign.at(i) = value.numValue;
//    }
//    player1Color_ = al_map_rgb(player1R,player1G,player1B);
//    player2Color_ = al_map_rgb(player2R,player2G,player2B);
//}
//void hud::draw(int screenWidth)
//{
//    al_draw_filled_rectangle(0, y_, player1_->getHealth()*screenWidth/2, y_ + height_, player1Color_);
//    al_draw_filled_rectangle((-player2_->getHealth()+2)*screenWidth/2, y_, screenWidth, y_ + height_, player2Color_);
//}
