#include "Planetarium.h"
#include "../../include/rapidjson/document.h"
#include "../../include/rapidjson/writer.h"
#include "../../include/rapidjson/stringbuffer.h"

using namespace rapidjson;

void Planetarium::render_celestials() {

    if (focused_vessel == nullptr) { printf("E 218754: No Focused Vessel!\n");return;}
    if (celestials.size() == 0) { printf("E 128585: Cannot render planets!\n");return;}

    //Should i do distance from CAMERA instead??????

    for (CelestialBody& c : celestials) {
        auto vp = planet_to_universe(focused_vessel->orbit.POS,focused_vessel->home_body);

        auto pp = planet_to_universe({0,0,0},find_body_by_name(c.name));

        //Vessel coordinate in planet space, per planet
        float v_x = pp.x-vp.x;
        float v_z = pp.y-vp.y;
        float v_y = pp.z-vp.z;

        //Get draw length of body
        auto delta = pp - vp;
        float len = linalg::length(delta);
        float altitude = len - (c.radius);

        
        //printf("LEN %f\n",len);
        
        
        int mode = 0;

        //Check if vessel is low enough to render in nearby mode
        if (altitude < c.radius*1.1f) {
            mode = 1;
        }

        //Horrific altitudes
        if ((int)abs(altitude) > 104780001000) {
            if (focused_vessel != nullptr)
                focused_vessel->protoVessel.altitude = 9999999; //Fucked
            return;
        }

        glPushMatrix();
        if (mode) {
            //Mode 1 PQS
            printf("Mode 1 not implemented!\n");

        } else {
            auto obj = c.me;
            if (obj != nullptr)
            {
                //Mode 0 Distant
                //Planet renderer works by scaling the glscale3f, and keeping the planet at a fixed distance
                float fixed_bubble = 20000.0;

                glTranslatef(
                    -(v_x  / len)* fixed_bubble        * 1,
                    -(v_y  / len)* fixed_bubble        * 1,
                    -(v_z   / len)* fixed_bubble       * 1
                );


                float angle = fmod(fmod(c.angle, 360.0f) + 360.0f, 360.0f);
                nglRotateY(360.0f - angle);

                float angular_diameter = 2.0f * (c.radius / len);
                float render_radius = angular_diameter * fixed_bubble;



                glScale3f(render_radius, render_radius, render_radius);


                if (obj->texture != nullptr && obj->positions != nullptr) {
                    glBindTexture(obj->texture);

                    nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);
                }
        
                //reformat
                if (focused_vessel != nullptr) {
                    if (c.name == "Earth") {
                        focused_vessel->protoVessel.altitude = altitude; //SL alt

                    }

                }



            }
        }
        glPopMatrix();
    }


}




//Get index of celestial by name
int Planetarium::find_body_by_name(std::string name) {
    int i = 0;
    for (CelestialBody &c : celestials) {
        if (c.name == name) {
            return i;
        }
        i++;
    }
    return -1;
}

//get the SOI of a celestial
double Planetarium::get_soi(int index) {
    double soi = INFINITY;
    //Sun
    if (celestials[index].parent == "") {
        return soi;
    }
    //Other planets
    CelestialBody* me;
    CelestialBody* parent;
    me = &celestials[index];

    //Optimization: save time and dont do repeated recalcs
    if (me->_SOI != -1)
        return me->_SOI;

    parent = &celestials[find_body_by_name(me->parent)];

    //Solve
    //rSOI = a(m/M)^(2/5)
    double sma = me->orbit.semi_major_axis;
    double num1 = (me->mass / parent->mass);
    double num2 = 2.0/5.0;
    soi = linalg::pow(num1,num2);
    soi *= sma;
    me->_SOI = soi;

    return soi;
}

//Find closest attractor to a vessel
int Planetarium::get_attractor(Vessel *v) {
    int best = -1;
    double best_score = INFINITY;

    for (size_t i = 0; i < celestials.size(); ++i) {
        const CelestialBody& c = celestials[i];
        if (c.parent.empty()) continue; //Sun

        double dist_to_planet = linalg::length(v->orbit.POS - c.orbit.POS);
        double soi = get_soi(static_cast<int>(i));
        double score = dist_to_planet / soi;

        if (score < best_score) {
            best_score = score;
            best = static_cast<int>(i);
        }
    }
    return best;
}

void Planetarium::update_planet_positions(double universal_time) {
    for (CelestialBody &c : celestials) {
        if (c.parent != "") {
            c.orbit.calculate_state_from_keplers(universal_time);
        }
    }
}

linalg::vec<double,3> Planetarium::planet_to_universe(linalg::vec<double,3> local,int home) {
    linalg::vec<double,3> pos = local;
    std::string parent = celestials[home].parent;

    pos += celestials[home].orbit.POS;
    while (parent != "") {
        CelestialBody *cb = &celestials[find_body_by_name(parent)];
        pos += cb->orbit.POS;
        parent = cb->parent;
    }
    return pos;
}

//Clear out the heaps of texture and model any loaded celestials eat up.
void Planetarium::clear_celestial_models() {
    for (CelestialBody &c : celestials)
        c.clear_model();
}

int Planetarium::load_celestial_bodies(Bundle* resources) {
    celestials.clear();

    printf("LOADING BODIES FROM JSON\n");
    std::vector<uint8_t> raw = resources->load_raw_data("resources/system/system.json");
    std::string json(raw.begin(),raw.end());

    Document d;
    d.Parse(json.c_str(),json.size());

    if (!d.HasMember("Bodies") || !d["Bodies"].IsArray()) {
        printf("E 13418: Missing celestials!\n");
        return 2;
    }

    const rapidjson::Value& bodies = d["Bodies"];

    //Iterate bodies
    for (rapidjson::SizeType i = 0; i < bodies.Size(); i++) {
        const rapidjson::Value& body = bodies[i];
        if (!body.IsObject()) continue;

        if (!body.HasMember("Orbit")) {
            printf("E 13418: Missing orbit!\n");
            return 3;
        }
        //Read Orbit data
        const rapidjson::Value& orbit = body["Orbit"];

        CelestialBody cb;

        //You should really rewrite this and keep the style of PartLoader idiot
        //Grab values
        const char* s_name      = body["name"].GetString();
        const char* s_parent    = body["parent"].GetString();
        const char* s_radius    = body["radius"].GetString();
        const char* s_mass      = body["mass"].GetString();
        const char* s_rr        = body["rotation_rate"].GetString();
        const char* s_ha        = body["has_atmosphere"].GetString();
        const char* s_atmh      = body["atmosphere_height"].GetString();
        const char* s_slp       = body["sea_level_pressure"].GetString();
        const char* s_mu        = orbit["mu"].GetString();
        const char* s_sma       = orbit["semi_major_axis"].GetString();
        const char* s_ecc       = orbit["eccentricity"].GetString();
        const char* s_inc       = orbit["inclination"].GetString();
        const char* s_lan       = orbit["long_ascending_node"].GetString();
        const char* s_ap        = orbit["argument_of_periapsis"].GetString();
        const char* s_ma        = orbit["mean_anomaly_at_epoch"].GetString();
        const char* s_ep        = orbit["epoch"].GetString();

        //Check
        if (
            s_name   == nullptr ||
            s_parent == nullptr ||
            s_radius == nullptr ||
            s_mass   == nullptr ||
            s_rr     == nullptr ||
            s_ha     == nullptr ||
            s_atmh   == nullptr ||
            s_slp    == nullptr
        ) { printf("E 15123: Error parsing object!\n"); return 1; }
        if (
            s_mu     == nullptr ||
            s_sma    == nullptr ||
            s_ecc    == nullptr ||
            s_inc    == nullptr ||
            s_lan    == nullptr ||
            s_ap     == nullptr ||
            s_ma     == nullptr ||
            s_ep     == nullptr
        ) { printf("E 15123: Error parsing orbit!\n"); return 1; }



        //If these values cant parse ittl just crash the app which, ig whatever
        //better than the entire calc...... ^ [Like putting a "+" for a double]
        //Do i even bother? I woulda just had it close the whole app either way

        if (std::strlen(s_name) < 64)
            cb.name = std::string(s_name);
        if (std::strlen(s_parent) < 64)
            cb.parent = std::string(s_parent);
        if (std::strlen(s_radius) < 64)
            cb.radius = std::stod(s_radius);
        if (std::strlen(s_mass) < 64)
            cb.mass = std::stod(s_mass);
        if (std::strlen(s_rr) < 64)
            cb.rotation_rate = std::stof(s_rr);
        if (std::strlen(s_ha) < 64)
            cb.atmosphere = std::stoi(s_ha);
        if (std::strlen(s_atmh) < 64)
            cb.atmosphere_height = std::stoi(s_atmh);
        if (std::strlen(s_slp) < 64)
            cb.sea_level_pressure = std::stof(s_slp);
        if (std::strlen(s_mu) < 64)
            cb.orbit.mu = std::stod(s_mu);
        if (std::strlen(s_sma) < 64)
            cb.orbit.semi_major_axis = std::stod(s_sma);
        if (std::strlen(s_ecc) < 64)
            cb.orbit.eccentricity = std::stod(s_ecc);
        if (std::strlen(s_inc) < 64)
            cb.orbit.inclination = std::stod(s_inc);
        if (std::strlen(s_lan) < 64)
            cb.orbit.long_ascending_node = std::stod(s_lan);
        if (std::strlen(s_ap) < 64)
            cb.orbit.argument_of_periapsis = std::stod(s_ap);
        if (std::strlen(s_ma) < 64)
            cb.orbit.mean_anomaly = std::stod(s_ma);
        if (std::strlen(s_ep) < 64)
            cb.orbit.epoch = std::stod(s_ep);

        cb.orbit.POS = {0,0,0};
        cb.orbit.VEL = {0,0,0};

        celestials.push_back(cb);
        printf("Added body %s.\n",cb.name.c_str());

    }
    
    //Set mu of each body
    for (CelestialBody &c : celestials) {
        if (c.parent == "") continue;

        CelestialBody *parent = &celestials[find_body_by_name(c.parent)];
        //double calc_mu = c.mass + parent->mass;
        //calc_mu *= 6.6743E-11;
        double calc_mu = parent->mass * 6.6743E-11;
        c.orbit.mu = calc_mu;
    }

    return 0;
}
