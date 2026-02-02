#include "../include/angel/angel.h"
#include "physics.h"

inline float clamp(float x, float min, float max) {
    if (x < min) {return min;}
    if (x > max) {return max;}
    return x;
}

Phys physics;

struct Camera {
    float pitch,roll,yaw = 0;
    linalg::vec<float,3> pos;
};

Camera cam;

int main()
{
    physics.step();
    Bundle assets;

    if (assets.load_asset_bundle("mars.tar.gz.tns")) {
        printf("Asset load error!!");
        return 1;
    }

    //All assets in a file can be viewed this way. This includes every file in the tarball.
    printf("Assets:\n");
    assets.debug_list_assets();
    printf("----\n");

    //ModelGroups contain multiple objects. A house would have a foundation, walls, and a roof. They would all be contained in this group as objects.
    ModelGroup group_mars;
    ModelGroup group_cap;

    //Define the name of the obj file to load into your group. This can be modified in a program like blender, or in the .obj itself.
    //notice: .obj extension is ommited! house.obj becomes house, as house.mtl and house.png are all likely to coexist. and make it easier to keep track in my head
    //Objects and groups in the .obj file are treated as *the same* (for now).


    if (group_mars.load_group(&assets,"mars")) return 1;
    if (group_cap.load_group(&assets,"capsule")) return 1;


    printf("Objects in group:\n");
    group_cap.debug_list_objects();
    printf("----\n");

        //Load our objects from the group
    ngl_object* planet = group_mars.get_object("Sphere");
    ngl_object* capsule = group_cap.get_object("MK1");


    //Neat way to store all ngl_objects.
    const ngl_object *scene[] = {
        planet,
        capsule,
        //cloud
    };
    for(auto &&obj : scene) {if (obj == nullptr) { return 1;} } //Exit if any objects are missing



    /* VOGTINATOR CODE */

    // Initialize nGL first
    nglInit();
    // Allocate the framebuffer
    TEXTURE *screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT, 0, false);
    nglSetBuffer(screen->bitmap);

    GLFix angle = 0;
    float planet_angle = 0;

    size_t max_pos = 0;
    for(auto &&obj : scene)
        if(obj->count_positions > max_pos)
            max_pos = obj->count_positions;

    ProcessedPosition *processed = new ProcessedPosition[max_pos];

    int big_counter = 0;

    #ifdef _TINSPIRE
    while(!isKeyPressed(KEY_NSPIRE_ESC))
    #else
    for(unsigned int i = 1300;--i;)
    #endif
    {
        angle += 1;

        glPushMatrix(); //World inverse
		glColor3f(0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Camera rotation
        nglRotateX(-cam.pitch);
        nglRotateY(-cam.yaw);
        nglRotateZ(-cam.roll);

        //Move physics to vessel
        cam.pos.x = 3;
        cam.pos.y = 3;
        cam.pos.z = 3;
        glTranslatef(-cam.pos.x, -cam.pos.y, -cam.pos.z);


        //PLANET
        glPushMatrix();
        //nglRotateX(30);
        //nglRotateZ(40);
		//nglRotateY(((GLFix)planet_angle).normaliseAngle());


        // glTranslatef(
        //     -physics.POS.x  / 100,
        //     -physics.POS.y  / 100,
        //     -physics.POS.z   / 100
        // );


        if (isKeyPressed(KEY_NSPIRE_W)) {
            //altitude_multiplier += 1;
        }
        if (isKeyPressed(KEY_NSPIRE_S)) {
            //altitude_multiplier -= 1;
        }
        
        //Planet renderer works by scaling the glscale3f, and keeping the planet at a fixed distance

        float distance_from_planet = sqrtf(
             ((physics.POS.x)*
              (physics.POS.x))+
             ((physics.POS.y)*
              (physics.POS.y))+
             ((physics.POS.z)*
              (physics.POS.z))
        );
        //distance_from_planet *= altitude_multiplier;
        //3000 meter bubble
        float fixed_bubble = 3000;
        glTranslatef(
            -(physics.POS.x  / distance_from_planet)* fixed_bubble        * 0,
            -(physics.POS.y  / distance_from_planet)* fixed_bubble        * 0,
            -(physics.POS.z   / distance_from_planet)* fixed_bubble       * 0
        );
        glTranslatef(0,0,fixed_bubble);


        //Debug rotation
        GLFix normd = planet_angle;
        nglRotateY(normd.normaliseAngle());

        float mars_radius = 3389500;
        float angularRadius = mars_radius / distance_from_planet;
        float renderRadius  = angularRadius * fixed_bubble;

        glScale3f(renderRadius, renderRadius, renderRadius);


        //glScale3f(20, 20, 20);
        //would do planet pos here

        auto obj = scene[0];
        glBindTexture(obj->texture);
        nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);

        glPopMatrix();


        //CAPSULE
        glPushMatrix();

		glTranslatef(0, -0, 200);

        nglRotateX(200);
        nglRotateZ(300);
		nglRotateY(270);
        nglRotateX(angle.normaliseAngle());
        nglRotateY(angle.normaliseAngle());


        glScale3f(20, 20, 20);


        obj = scene[1];
        glBindTexture(obj->texture);
        nglDrawArray(obj->vertices, obj->count_vertices, obj->positions, obj->count_positions, processed, obj->draw_mode);



        glPopMatrix();


        glPopMatrix();

        nglDisplay();
        big_counter ++;
        planet_angle += 1.1f;

        physics.step();
    }

    group_mars.free_group();

    delete[] processed;
    // Deinitialize nGL
    nglUninit();
    deleteTexture(screen);

    return 0;
}