--
-- Notes: 
--   + All game API functions are exported within a module named 'game'
--   + Objects can be added in one of two ways: 
--      (i)  create in ObjectMap and lookup from script, or, 
--      (ii) create in script. 
--      This script demonstrates both, (i) for player and (ii) for enemies.
--      Method (i) is preferred since it is easier to configure custom shapes for sprites.
--   + For printf variants: varargs are not supported at this time
--

cloud2_update = function(name, data_ptr)
	cloud = game.om_get_object_by_name(name)
        cloud.x = cloud.x - 2
        if(cloud.x + cloud.w < 0) then
		cloud.x = 1800
        end
end

cloud1_update = function(name, data_ptr)
	cloud = game.om_get_object_by_name(name)
        cloud.x = cloud.x - 1.25
        if(cloud.x + cloud.w < 0) then
		cloud.x = 1200
        end
end

cloud0_update = function(name, data_ptr)
	cloud = game.om_get_object_by_name(name)
        cloud.x = cloud.x - 0.25
        if(cloud.x + cloud.w < 0) then
		cloud.x = 600
        end
end

enemy_update = function(name, data_ptr) 
	enemy = game.om_get_object_by_name(name)
	if (enemy.health <= 0) then
		return enemy
	end
	hero = game.om_get_object_by_name("hero")
	if (hero == nil) then
		game.action_idle(enemy, 0)
		return enemy
	end
	dx = hero.x - enemy.x
        vxmul = 0.2
	if(math.abs(dx)>80) then
		if(dx > 0) then
	        	enemy.vx_desired = math.abs(dx) * vxmul
			game.action_direction_right(enemy, 0)
			enemy.facing = game.OBJECT_FACING_W
		else
	        	enemy.vx_desired = math.abs(dx) * vxmul
			game.action_direction_left(enemy, 0)
			enemy.facing = game.OBJECT_FACING_E
		end
	else
		game.action_attack(enemy, 0)
	end
        return enemy
end

hero_update = function(name, data_ptr) 
	hero = game.om_get_object_by_name("hero")
	if (hero ~= nil) then
		if(hero.health < 20) then
			hero.sprinstance = game.om_create_sprite_instance(hero.omanager,"Hero")
		elseif(hero.health < 60) then
			hero.sprinstance = game.om_create_sprite_instance(hero.omanager,"TRex")
		else
			hero.sprinstance = game.om_create_sprite_instance(hero.omanager,"Dragon")
		end
	end
end

enemy_destroy = function(name, data_ptr)
	hero = game.om_get_object_by_name("hero")
	if (hero ~= nil) then
		hero.health = hero.health + 29
		if(hero.health > hero.health_max) then
			hero.health = hero.health_max	
		end
   		game.score = game.score + 1
        	game.log_printf("[HERO] " .. " Points: " .. game.score)
	end
	return enemy
end

interact_hero_enemy = function(a, b, data)
   hero = game.om_get_object_by_name("hero")
   hero_state = game.om_get_object_state(hero)
   if hero_state == game.SPRITESET_ATTACK then
  	enemy = game.om_get_object_by_name(b)
        enemy.health = enemy.health - 23
        game.log_printf("[ENEMY] " .. enemy.name .. " Life: " .. enemy.health)
   else
   	hero.health = hero.health - 5.0
        game.log_printf("[HERO] Life: " .. hero.health)
   end
   return data
end

script_init = function()
        -- from "appinfo.h"
        mesg = "Dragon01: " .. game.PRODNAME .. " v" .. game.PRODVER .. ": " .. game.PRODTITLE
        -- from "log.[h|c]" 
        game.log_printf(mesg)
        
        -- from "object.h" and "script.c"
        om = game.get_scriptload_objectmanager()

	-- Hero
        hero = game.om_get_object_by_name("hero")
	hero.vx_desired = 100.0
        game.om_center_camera_on(hero)
	-- update_hero = hero_update

        -- GUI
	uibox = game.om_create_sprite_instance(om,"uibox")
	uibup = game.om_create_sprite_instance(om,"uibup")
	uibdown = game.om_create_sprite_instance(om,"uibdown")
	uiscroll = game.om_create_sprite_instance(om,"uiscroll")
	uislider = game.om_create_sprite_instance(om,"uislider")
	uiedit = game.om_create_sprite_instance(om,"uiedit")
	game.gui_update_skin_from_sprites(uibox,uibup,uibdown,uiscroll,uislider,uiedit)
	game.gui_set_theme_color(game.GUI_THEME_SKINNED,game.GUI_COLOR_TINT,0.9,0.9,0.9,1.0)
	game.gui_set_theme_color(game.GUI_THEME_SKINNED,game.GUI_COLOR_FONT,0.1,0.1,0.1,1.0)

        -- Enemy Updates
	update_enemy0 = enemy_update
	update_enemy1 = enemy_update
	update_enemy2 = enemy_update
	update_enemy3 = enemy_update
	destroy_enemy0 = enemy_destroy
	destroy_enemy1 = enemy_destroy
	destroy_enemy2 = enemy_destroy
	destroy_enemy3 = enemy_destroy

        -- Enemy Interactions
	interact_hero_enemy0 = interact_hero_enemy
	interact_hero_enemy1 = interact_hero_enemy
	interact_hero_enemy2 = interact_hero_enemy
	interact_hero_enemy3 = interact_hero_enemy
	interact_enemy0_hero = interact_hero_enemy
	interact_enemy1_hero = interact_hero_enemy
	interact_enemy2_hero = interact_hero_enemy
	interact_enemy3_hero = interact_hero_enemy

	-- Animations
	update_Cloud0_0 = cloud0_update
	update_Cloud0_1 = cloud0_update
	update_Cloud0_1 = cloud0_update
	update_Cloud1_0 = cloud1_update
	update_Cloud1_1 = cloud1_update
	update_Cloud1_1 = cloud1_update
	update_Cloud2_0 = cloud2_update
	update_Cloud2_1 = cloud2_update
	update_Cloud2_2 = cloud2_update
end

hero_landed = function(name, data_ptr)
  hero = game.om_get_object_by_name(name)
  game.landed_player(hero, nil)
  return hero
end
