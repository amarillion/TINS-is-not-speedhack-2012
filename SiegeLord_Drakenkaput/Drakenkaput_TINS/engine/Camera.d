/*
Copyright 2012 Pavel Sountsov

This file is part of TINSEngine.

TINSEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TINSEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TINSEngine.  If not, see <http://www.gnu.org/licenses/>.
*/
module engine.Camera;

import engine.MathTypes;
import allegro5.allegro;

class CCamera
{
	this(SVector2D position = SVector2D(0, 0))
	{
		Position = position;
	}
	
	void Update(SVector2D screen_size)
	{
		al_identity_transform(&Transform);
		al_translate_transform(&Transform, -Position.X, -Position.Y);
		al_translate_transform(&Transform, screen_size.X / 2, screen_size.Y / 2);
		UseTransform();
	}
	
	void UseTransform()
	{
		al_use_transform(&Transform);
	}
	
	SVector2D ToWorld(SVector2D view_pos)
	{
		al_transform_coordinates(&Transform, &view_pos.X, &view_pos.Y);
		return view_pos;
	}
	
	SVector2D ToView(SVector2D world_pos)
	{
		ALLEGRO_TRANSFORM inverse;
		al_copy_transform(&inverse, &Transform);
		al_invert_transform(&inverse);
		al_transform_coordinates(&inverse, &world_pos.X, &world_pos.Y);
		return world_pos;
	}
	
	SVector2D Position;
protected:
	ALLEGRO_TRANSFORM Transform;
}
