#include "planet-lod.h"


void Terrain::calculate_chunk(Chunk* c) 
{
	

}


//3D position of chunk on planet
linalg::vec<double,3> Terrain::map_to_pos(linalg::vec<double,2> coordinate) {
    double lat_r = coordinate.x * M_PI / 180.0;
    double lon_r = coordinate.y * M_PI / 180.0;

    double x = radius * cos(lat_r) * cos(lon_r);
    double y = radius * cos(lat_r) * sin(lon_r);
    double z = radius * sin(lat_r);

    return {x, y, z};
}

//Eats lat/long/alt
void Terrain::recalculate_chunks(linalg::vec<double,2> coordinate,double altitude) 
{
	for (unsigned int i = 0; i < chunks.size(); i++) {
		if (chunks[i].delete_next) chunks.erase(chunks.begin()+i);
	}

	
	//Have to calculate the visible surface area,. will save that for later.
	//static value for now: HINT COMES FROM ALTITUDE!!
	//double distance_to_horizon = 163840; 
	
	
	printf("Altitude: %f\n",altitude);
}


void Terrain::render_terrain(linalg::vec<double,2> coordinate,double altitude) 
{
	for (Chunk &c : chunks) {
		if (c.visible) {

		}
	}
	return;
}
