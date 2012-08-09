#ifndef LEVELDATA_H
#define LEVELDATA_H

class LevelData {
public:
	LevelData() {};
	void init(int level, int dhealth, int knights, int stars, int planets) {
		this->level = level;
		this->dhealth = dhealth;
		this->knights = knights;
		this->stars = stars;
		this->planets = planets;
	}

	int getLevel() {return level;}
	int getDHealth() { return dhealth; }
	int getKnights() { return knights; }
	int getStars() { return stars; }
	int getPlanets() { return planets; }

private:
	int level, dhealth, knights, stars, planets;
};


#endif