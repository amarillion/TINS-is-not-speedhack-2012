module tilesizes;

const int FINE=50;	//WARNING: this can't change any more without jiggery pokery. Level files have been made with coordinates (scrollx,scrolly) scaled by it!
const int MAXSPEED_BASIC=10;
const int MAXSPEED_TURBO=25;
const int TIME_TO_MAXSPEED_TURBO=100;

const int PLAYER_SIZE_FOR_COLLISIONS=50;
const int PLAYER_SIZE_FOR_COLLECTION=20;
const int PLAYER_SIZE_FOR_DRAWING=50;

const int TERMINAL_SIZE=30;

const int TORCH_SIZE_FOR_BURNING=0;

const float FIRE_OVAL_NEAR_END=0.2;
const float FIRE_OVAL_FAR_END=1.8;
const float FIRE_OVAL_WIDTH=1;
