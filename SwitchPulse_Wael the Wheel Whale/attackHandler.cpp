//#include "attackHandler.hpp"
//ALLEGRO_BITMAP * attackHandler::bulletBitmap;
//ALLEGRO_BITMAP * attackHandler::mineBitmap;
//animation * attackHandler::explosionAnimation;
//std::vector<bullet> attackHandler::bullets;
//std::vector<bullet> attackHandler::mines;
//std::vector<ALLEGRO_SAMPLE *> attackHandler::sounds_;
//std::vector<float> attackHandler::explosionX;
//std::vector<float> attackHandler::explosionY;
//std::vector<animHandler> attackHandler::explosions;
//void attackHandler::reset(player &playerToClear)
//{
//    playerToClear.numMines = 0;
//    playerToClear.numBullets = 0;
//}
//void attackHandler::reset()
//{
//    bullets.clear();
//    mines.clear();
//}
//void attackHandler::initHandler(animation * tempAnim,ALLEGRO_BITMAP * bitmap,ALLEGRO_BITMAP * bitmap2,std::string iniFile,std::string attackSection)
//{
//    explosionAnimation = tempAnim;
//    getValue shootText("shoot",attackSection,iniFile);
//    if(shootText.isNumber or !shootText.exists)
//    {
//        fprintf(stderr, "error loading 'shoot' from section: %s\n", attackSection.data());
//        exit(EXIT_FAILURE);
//    }
//    sounds_.push_back(al_load_sample(shootText.stringValue.data()));
//    getValue bulletHitText("bulletHit",attackSection,iniFile);
//    if(bulletHitText.isNumber or !bulletHitText.exists)
//    {
//        fprintf(stderr, "error loading 'bulletHit' from section: %s\n", attackSection.data());
//        exit(EXIT_FAILURE);
//    }
//    sounds_.push_back(al_load_sample(bulletHitText.stringValue.data()));
//    getValue knife("knife",attackSection,iniFile);
//    if(knife.isNumber or !knife.exists)
//    {
//        fprintf(stderr, "error loading 'knife' from section: %s\n", attackSection.data());
//        exit(EXIT_FAILURE);
//    }
//    sounds_.push_back(al_load_sample(knife.stringValue.data()));
//    getValue mineSet("mineSet",attackSection,iniFile);
//    if(mineSet.isNumber or !mineSet.exists)
//    {
//        fprintf(stderr, "error loading 'mineSet' from section: %s\n", attackSection.data());
//        exit(EXIT_FAILURE);
//    }
//    sounds_.push_back(al_load_sample(mineSet.stringValue.data()));
//    getValue mineExplode("mineExplode",attackSection,iniFile);
//    if(mineExplode.isNumber or !mineExplode.exists)
//    {
//        fprintf(stderr, "error loading 'mineExplode' from section: %s\n", attackSection.data());
//        exit(EXIT_FAILURE);
//    }
//    sounds_.push_back(al_load_sample(mineExplode.stringValue.data()));
//    getValue bump("bump",attackSection,iniFile);
//    if(bump.isNumber or !bump.exists)
//    {
//        fprintf(stderr, "error loading 'bump' from section: %s\n", attackSection.data());
//        exit(EXIT_FAILURE);
//    }
//    sounds_.push_back(al_load_sample(bump.stringValue.data()));
//    bulletBitmap = bitmap;
//    mineBitmap = bitmap2;
//}
//void attackHandler::draw()
//{
//    for(int i = 0; i < bullets.size();++i)
//    {
//        bullets.at(i).draw();
//    }
//    for(int i = 0; i < mines.size();++i)
//    {
//        mines.at(i).draw();
//    }
//    for(int i = 0; i < explosions.size(); ++i)
//    {
//        explosions.at(i).draw(explosionX.at(i),explosionY.at(i),1,0,1);
//    }
//}
//void attackHandler::updateAttacks(map & currentMap)
//{
//    for(int i = 0; i < explosions.size(); ++i)
//    {
//        if(explosions.at(i).update())
//        {
//            explosionX.erase(explosionX.begin() + i);
//            explosionY.erase(explosionY.begin() + i);
//            explosions.erase(explosions.begin() + i--);
//        }
//    }
//    std::vector<player *> players = currentMap.getPlayers();
//    bool flag = false;
//    for(int i = 0; i < bullets.size();++i)
//    {
//        for(int j = 0; j < mines.size();++j)
//        {
//            if(bullets.at(i).extendCollide(mines.at(j), 3) and bullets.at(i).playerNumber != mines.at(j).playerNumber)
//            {
//                --players.at(bullets.at(i).playerNumber)->numBullets;
//                --players.at(mines.at(j).playerNumber)->numMines;
//                bullets.erase(bullets.begin() + i);
//                mines.erase(mines.begin() + j);
//                al_play_sample(sounds_.at(SOUNDS_MINE_EXPLODE),1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
//                flag = true;
//                break;
//            }
//        }
//    }
//    if(!flag)
//    {
//        for(int i = 0; i < bullets.size();++i)
//        {
//            bullets.at(i).update();
//            if(currentMap.collide(bullets.at(i)))
//            {
//                --players.at(bullets.at(i).playerNumber)->numBullets;
//                bullets.erase(bullets.begin() + i);
//            }
//        }
//    }
//    std::vector<int> knifeTargets;
//    std::vector<int> knifingPlayers;
//    for(int i = 0; i < players.size(); ++i)
//    {
//        if(!players.at(i)->isBumped())
//        {
//            if(players.at(i)->isShooting)
//            {
//                bool knifing = false;
//                for(int j = 0; j < players.size(); ++j)
//                {
//                    if(i != j and !players.at(j)->isBumped())
//                    {
//                        float tempFloatX = -players.at(i)->facing*(players.at(i)->getX() - players.at(j)->getX()) - players.at(i)->getWidth();
//                        float tempFloatY = players.at(i)->getY() - players.at(j)->getY();
//                        if((tempFloatX > 0 and tempFloatX < players.at(i)->knifeX) and (tempFloatY > -players.at(i)->knifeY and tempFloatY < players.at(i)->knifeY))
//                        {
//    //                        bool clank = false;
//    //                        for(int k = 0; k < knifeTargets.size(); ++k)
//    //                        {
//    //                            if(knifeTargets.at(k) == i)
//    //                            {
//    //                                clank = true;
//    //                                knifeTargets.erase(knifeTargets.begin() + k);
//    //                                knifingPlayers.erase(knifeTargets.begin() + k);
//    //                            }
//    //                        }
//    //                        if(!clank)
//    //                        {
//                            knifeTargets.push_back(j);
//                            knifingPlayers.push_back(i);
//                            knifing = true;
//                            //}
//                        }
//                    }
//                }
//                if(!knifing)// and !players.at(i)->crouching)
//                {
//                    bullet newBullet(players.at(i)->getBulletX(),players.at(i)->getBulletY(),players.at(i)->bulletDimensions,players.at(i)->bulletDimensions, bulletBitmap);
//                    newBullet.playerNumber = i;
//                    int tempIntX = players.at(i)->horizontal;
//                    int tempIntY = players.at(i)->vertical;
//                    float tempFloatX, tempFloatY,tempFloatX2,tempFloatY2;
//                    if(players.at(i)->numBullets < players.at(i)->maxBullets)
//                    {
//                        ++players.at(i)->numBullets;
//                        if(tempIntX and tempIntY)
//                        {
//                            if(!players.at(i)->isOnWall())
//                            {
//                                tempFloatX = players.at(i)->bulletSpeedDiagonal*tempIntX + players.at(i)->getXMove();
//                                tempFloatX2 = players.at(i)->bulletSpeedDiagonal*tempIntX;
//                                if((tempIntX < 0 and tempFloatX > tempFloatX2) or (tempIntX > 0 and tempFloatX < tempFloatX2))
//                                {
//                                    tempFloatX = tempFloatX2;
//                                }
//                            }else
//                            {
//                                tempFloatX = players.at(i)->bulletSpeedDiagonal*tempIntX;
//                            }
//                            tempFloatY = players.at(i)->bulletSpeedDiagonal*tempIntY + players.at(i)->getYMove();
//                            tempFloatY2 = players.at(i)->bulletSpeedDiagonal*tempIntY;
//                            if((tempIntY < 0 and tempFloatY > tempFloatY2) or (tempIntY > 0 and tempFloatY < tempFloatY2))
//                            {
//                                tempFloatY = tempFloatY2;
//                            }
//                            newBullet.setMove(tempFloatX,tempFloatY);
//                        }else
//                        {
//                            if(!players.at(i)->isOnWall())
//                            {
//                                tempFloatX = players.at(i)->bulletSpeed*tempIntX + players.at(i)->getXMove()*players.at(i)->movingInfluence;
//                                tempFloatX2 = players.at(i)->bulletSpeed*tempIntX;
//                                if((tempIntX < 0 and tempFloatX > tempFloatX2) or (tempIntX > 0 and tempFloatX < tempFloatX2))
//                                {
//                                    tempFloatX = tempFloatX2;
//                                }
//                            }else
//                            {
//                                tempFloatX = players.at(i)->bulletSpeed*tempIntX;
//                            }
//                            tempFloatY = players.at(i)->bulletSpeed*tempIntY + players.at(i)->getYMove()*players.at(i)->movingInfluence;
//                            tempFloatY2 = players.at(i)->bulletSpeed*tempIntY;
//                            if((tempIntY < 0 and tempFloatY > tempFloatY2) or (tempIntY > 0 and tempFloatY < tempFloatY2))
//                            {
//                                tempFloatY = tempFloatY2;
//                            }
//                            newBullet.setMove(tempFloatX,tempFloatY);
//                        }
//                        bullets.push_back(newBullet);
//                        al_play_sample(sounds_.at(SOUNDS_BULLET_SHOOT),1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
//                    }
//                }
//                players.at(i)->isShooting = false;
//            }else if(players.at(i)->plantingMine)
//            {
//                al_play_sample(sounds_.at(SOUNDS_MINE_SET),1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
//                if(players.at(i)->numMines >= players.at(i)->maxMines)
//                {
//                    bool done = false;
//                    int j = 0;
//                    while(!done)
//                    {
//                        if(mines.at(j).playerNumber == i)
//                        {
//                            addExplosion(mines.at(j).getX(),mines.at(j).getY());
//                            mines.erase(mines.begin() + j);
//                            done = true;
//                            al_play_sample(sounds_.at(SOUNDS_MINE_EXPLODE),1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
//                        }
//                        ++j;
//                    }
//                    --players.at(i)->numMines;
//                }
//                float tempFloatX = players.at(i)->getX() + players.at(i)->getWidth()/2 - players.at(i)->bulletDimensions/2;
//                float tempFloatY = players.at(i)->getY() + players.at(i)->getHeight() - players.at(i)->mineHeight;
//                bullet newBullet(tempFloatX,tempFloatY,players.at(i)->mineWidth,players.at(i)->mineHeight, mineBitmap);
//                newBullet.playerNumber = i;
//                mines.push_back(newBullet);
//                players.at(i)->plantingMine=false;
//                ++players.at(i)->numMines;
//            }
//        }
//        for(int j = 0; j < bullets.size();++j)
//        {
//            if(i != bullets.at(j).playerNumber and !players.at(i)->isBumped())
//            {
//                if(players.at(i)->getBoundingBox().collide(bullets.at(j)))
//                {
//                    --players.at(bullets.at(j).playerNumber)->numBullets;
//                    players.at(i)->hit(players.at(bullets.at(j).playerNumber)->bulletDamage);
//                    bullets.erase(bullets.begin() + j);
//                    al_play_sample(sounds_.at(SOUNDS_BULLET_HIT),1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
//                }
//            }
//        }
//        for(int j = 0; j < mines.size();++j)
//        {
//            if(i != mines.at(j).playerNumber)
//            {
//                if(players.at(i)->getBoundingBox().collide(mines.at(j)))
//                {
//                    --players.at(mines.at(j).playerNumber)->numMines;
//                    players.at(i)->hit(players.at(mines.at(j).playerNumber)->mineDamage);
//                    if(players.at(i)->getX() + ((float)players.at(i)->getWidth())/2 < mines.at(j).getX() + 2)
//                    {
//                        players.at(i)->bump(true,false);
//                    }else
//                    {
//                        players.at(i)->bump(true,true);
//                    }
//                    players.at(i)->bump(false,false);
//                    addExplosion(mines.at(j).getX(),mines.at(j).getY());
//                    mines.erase(mines.begin() + j);
//                    al_play_sample(sounds_.at(SOUNDS_MINE_EXPLODE),1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
//                }
//            }
//        }
//        for(int j = 0; j < explosions.size(); ++j)
//        {
//            if(!players.at(i)->isBumped())
//            {
//                block tempBlock(explosionX.at(j),explosionY.at(j),explosionAnimation->getWidth(),explosionAnimation->getHeight());
//                if(tempBlock.collide(players.at(i)->getBoundingBox()))
//                {
//                    players.at(i)->hit(.085);
//                    if(players.at(i)->getX() + ((float)players.at(i)->getWidth())/2 < tempBlock.getX() + ((float)tempBlock.getWidth())/2)
//                    {
//                        players.at(i)->bump(true,false);
//                    }else
//                    {
//                        players.at(i)->bump(true,true);
//                    }
//                }
//            }
//        }
//    }
//    for(int i = 0; i < knifeTargets.size(); ++i)
//    {
//        if(players.at(knifeTargets.at(i))->getX() < players.at(knifingPlayers.at(i))->getX())
//        {
//            players.at(knifeTargets.at(i))->bump(true,false);
//        }else
//        {
//            players.at(knifeTargets.at(i))->bump(true,true);
//        }
//        players.at(knifeTargets.at(i))->hit(players.at(knifingPlayers.at(i))->knifeDamage);
//        players.at(knifingPlayers.at(i))->setKnifing();
//        al_play_sample(sounds_.at(SOUNDS_KNIFE),1,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
//    }
//}
//
//void attackHandler::addExplosion(float x, float y)
//{
//    animHandler tempHandler;
//    tempHandler.setHandler(*explosionAnimation);
//    explosions.push_back(tempHandler);
//    explosionX.push_back(x);
//    explosionY.push_back(y);
//}
