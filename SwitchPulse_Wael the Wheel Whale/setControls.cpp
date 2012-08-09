#include "setControls.hpp"
void setControls(std::string iniFile, std::string player1Section, std::string player2Section, ALLEGRO_FONT *font, int x, int y)
{
    std::vector<std::string> names;
    names.push_back("up");
    names.push_back("down");
    names.push_back("left");
    names.push_back("right");
    names.push_back("jump");
    names.push_back("fire");
    std::vector<std::string> sections;
    sections.push_back(player1Section);
    sections.push_back(player2Section);
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    for(int i = 0;i < sections.size();++i)
    {
        for(int j = 0;j < names.size();++j)
        {
            bool done = false;
            al_clear_to_color(al_map_rgb(0,0,0));
            al_draw_textf(font, al_map_rgb(255,255,255),x,y, ALLEGRO_ALIGN_CENTRE,"Press the button for Player %i's %s key", i + 1,names.at(j).data());
            al_flip_display();
            while(!done)
            {
                ALLEGRO_EVENT ev;
                al_wait_for_event(event_queue, &ev);
                if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
                {
                    done = true;
                    if(ev.keyboard.keycode != ALLEGRO_KEY_ESCAPE)
                    {
                        saveValue(ev.keyboard.keycode,names.at(j),sections.at(i),iniFile);
                    }
                }
            }
        }
    }
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_destroy_event_queue(event_queue);
}
