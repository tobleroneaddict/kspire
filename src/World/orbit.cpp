#include "orbit.h"


//Newton-Rhapson equation solver
double Orbit::Ecc_A(double M, double ecc, double maxError = 1E-07)
{
    double adjust = 1.0;
    double guess = M + ecc * linalg::sin(M) + 0.5 * ecc * ecc * linalg::sin(2.0 * M);
    int max_iterations = 100;
    int iterations = 0;
    while (linalg::abs(adjust) > maxError && iterations < max_iterations)
    {
        double ans = guess - ecc * linalg::sin(guess);
        adjust = (M - ans) / (1.0 - ecc * linalg::cos(guess));
        guess += adjust;
        iterations++;
        
        if (linalg::abs(guess) > 1000.0) {
            printf("Ecc_A Diverge!\n");
            return M;
        }
    }
    return guess;
}

double Orbit::Ecc_A_Hyp(double M, double ecc, double maxError = 1E-07)
{
    
	double adjust = 1.0;
    double guess = linalg::log(2.0 * M / ecc + 1.8);

    int i_max = 100;
    int i = 0;
    
    while (linalg::abs(adjust) > maxError && i < i_max)
    {
        double sinh_guess = linalg::sinh(guess);
        double cosh_guess = linalg::cosh(guess);
        
        adjust = (ecc * sinh_guess - guess - M) / (ecc * cosh_guess - 1.0);
        guess -= adjust;
        i++;

        if (linalg::abs(guess) > 100.0) {
            printf("Ecc_A_Hyp Diverge!!!!!\n");
            break;
        }
    }
    
    return guess;
}



void Orbit::calculate_state_from_keplers(double _UNIVERSAL_TIME) {
    universal_time = _UNIVERSAL_TIME;

    if (mu == 0) {printf("Mu undefined!\n"); return;}
    //Might i have to set mu to the parent object???s mu
    
    //Time along orbit after Epoch
    double ObT = universal_time - epoch;

    double radius = -1;
    double eccentric_anomaly = -1;
    double true_anomaly = -1;
    //Calc MA, Ecc-A TA, and Radius.
    if (eccentricity < 1.0) { //SOLVE ELLIPTICAL / CIRCULAR
        //if (eccentricity < 1E-05) { printf("ECC edge case\n");}
        //Calc MA
        //mean_anomaly = ObT / period * 2.0 * pi;
        double n = linalg::sqrt(mu / (semi_major_axis*semi_major_axis*semi_major_axis));
        mean_anomaly = mean_anomaly_at_epoch + n * ObT;
        mean_anomaly = fmod(mean_anomaly, 2.0*pi);

        //Calc EA
        if (eccentricity < 1E-05) { //Near-circular orbits
            eccentric_anomaly = mean_anomaly;
            true_anomaly = mean_anomaly;
        } else {    //Noncircular orbits
            if (eccentricity < 0.9) {
                eccentric_anomaly = Ecc_A(mean_anomaly,eccentricity);
            } else {
                
                //printf("ECC TOO HIGHH!!! SOLVE EXTREME CASE WITH OTHER FUNC\n");
            }
                //Calc TA
                true_anomaly = linalg::atan2(
                    linalg::sqrt(1 - eccentricity*eccentricity) * linalg::sin(eccentric_anomaly),
                    linalg::cos(eccentric_anomaly) - eccentricity
                );
        }

        radius = semi_major_axis * (1.0 - eccentricity * eccentricity)
        / (1.0 + eccentricity * linalg::cos(true_anomaly));

    } else if (eccentricity > 1.0) { //SOLVE HYPERBOLIC
        if (eccentricity == 1.0)    //Turn parabolic orbit edge case into hyperbolic
            eccentricity += 1E-10;
        mean_anomaly = 2.0 * pi * linalg::abs(ObT) / period;
        eccentric_anomaly = Ecc_A_Hyp(mean_anomaly,eccentricity);
        true_anomaly = linalg::atan2(linalg::sqrt(eccentricity * eccentricity - 1.0) * linalg::sinh(eccentric_anomaly), eccentricity - linalg::cosh(eccentric_anomaly));

        radius = (0.0 - semi_major_axis) * (eccentricity * eccentricity - 1.0) / (1.0 + eccentricity * linalg::cos(true_anomaly));
    }

    if (radius < 0) {
        printf("E 128385 : Something went terribly wrong %f\n",eccentricity);
        return;
    }

    double h = linalg::sqrt(mu * semi_major_axis * (1 - eccentricity*eccentricity));
    double v_r = mu / h * eccentricity * linalg::sin(true_anomaly);
    double v_theta = mu / h * (1 + eccentricity * linalg::cos(true_anomaly));

    linalg::vec<double,3> r_pf;
    linalg::vec<double,3> v_pf;

    //Now convert to perifocal plane
    //Describes a 2D plane along the plane of the orbit
    //POS
    r_pf.x = radius * linalg::cos(true_anomaly);
    r_pf.y = radius * linalg::sin(true_anomaly);
    r_pf.z = 0.0;

    //PROPER
    //vf_vec = np.array([-np.sqrt(mu/p)*np.sin(oe[5]), np.sqrt(mu/p)*(oe[1] + np.cos(oe[5])), 0])
    v_pf.x = v_r * linalg::cos(true_anomaly) - v_theta * linalg::sin(true_anomaly); //NEGT
    v_pf.y = v_r * linalg::sin(true_anomaly) + v_theta * linalg::cos(true_anomaly); //POST
    v_pf.z = 0.0;

    //Didnt fix
    //double p = (h_mag*h_mag)/mu ;
    //v_pf.x = -linalg::sqrt(mu/p)*sin(true_anomaly);
    //v_pf.y = linalg::sqrt(mu/p)*(eccentricity+cos(true_anomaly));
    //Rotation matrix
    linalg::mat<double,3,3> RM;

    //https://en.wikipedia.org/wiki/Perifocal_coordinate_system
    //Dont forget to make it Y up! if you end up converting to perifocal for some reason!
    //Precompute terms

    double cos_O = linalg::cos(long_ascending_node);
    double sin_O = linalg::sin(long_ascending_node);
    double cos_i = linalg::cos(inclination);
    double sin_i = linalg::sin(inclination);
    double cos_w = linalg::cos(argument_of_periapsis);
    double sin_w = linalg::sin(argument_of_periapsis);

    //Same mat as https://github.com/Keeby-Astro/Space-Sciences-and-Astrodynamics/blob/main/course_scripts/Coordinate%20Transformation.py#L57
    RM[0][0] =  cos_O*cos_w - sin_O*sin_w*cos_i;
    RM[0][1] = -cos_O*sin_w - sin_O*cos_w*cos_i;
    RM[0][2] =  sin_O*sin_i;

    RM[1][0] =  sin_O*cos_w + cos_O*sin_w*cos_i;
    RM[1][1] = -sin_O*sin_w + cos_O*cos_w*cos_i;
    RM[1][2] = -cos_O*sin_i;

    RM[2][0] =  sin_w*sin_i;
    RM[2][1] =  cos_w*sin_i;
    RM[2][2] =  cos_i;

    //Matrix multiplication

    linalg::vec<double,3> pos_c = linalg::mul(RM, r_pf);
    linalg::vec<double,3> vel_c = linalg::mul(RM, v_pf);


    POS = { pos_c.x, pos_c.y, pos_c.z };
    VEL = { vel_c.x, vel_c.y, vel_c.z };

}

//https://github.com/atichat45/orbital-mechanics-toolkit/blob/main/src/orbital_mechanics/core/orbital_elements.py
//https://orbital-mechanics.space/classical-orbital-elements/orbital-elements-and-the-state-vector.html
//Calculate on-rails orbital elements given keplarian elements
void Orbit::physics_to_rails(double epoch) {

//     printf("1POS: (%f,%f,%f)\n", POS.x, POS.y, POS.z);
// printf("1VEL: (%f,%f,%f)\n", VEL.x, VEL.y, VEL.z);
// printf("1ECC: %f\n", eccentricity);


    //Use return codes later
    if (mu == 0 ) { printf("E 134814: MU NOT SET!!!!\n"); return;}
    if (linalg::length(POS) < 1e-10 || linalg::length(VEL) < 1e-10) {
        printf("E 632131: Bad position or velocity!\n"); return;}

    

    //Dont use linalg::normalize() to find length.
    double Omega = -1;
    double POS_mag = linalg::length(POS);
    double VEL_mag = linalg::length(VEL);
    //double v_r = linalg::dot(POS, VEL) / POS_mag; //radial velocity
    //double v_p = linalg::sqrt((VEL_mag*VEL_mag) - (v_r*v_r));   //azimuthal velocity

    
    //Angular momentum (h, h_mag)
    linalg::vec<double, 3> h = linalg::cross(POS,VEL);
    h_mag = linalg::length(h);
   

    //Inclination
    double i = linalg::acos(linalg::clamp(h.z / h_mag, -1.0, 1.0));

    //Right Ascension @ Ascending Node
    //N=^K x h
    
    linalg::vec<double,3> K = {0,0,1};
    //Node line (n, n_mag)
    linalg::vec<double,3> n = linalg::cross(K, h);
    double n_mag = linalg::length(n);

    if (n_mag > 1e-8) {
        //printf("MAG\n");
        //Omega = atan2(n.y, n.x);
        Omega = 0.0f; //equitorial orbit
    } else {
        Omega = linalg::acos(n.x/n_mag);
        if (n.y < 0) {
            Omega = 2 * pi - Omega;
        }
    }

    
    //Eccentricity (e, e_mag)
//    linalg::vec<double,3> e = (linalg::cross(VEL, h) / mu) - (POS / POS_mag);
    linalg::vec<double,3> e = (((VEL_mag*VEL_mag) - mu/POS_mag) * POS
    - linalg::dot(POS,VEL) * VEL) / mu;
    double e_mag = linalg::length(e);
    if (e_mag < 1e-10) {
        printf("E 167411 : Bad Eccentricity!\n");
        return;
    }


    //MIGHT BE AT FAULT
    //Argument of periapsis (Lowercase omega)
    double omega = -1;

    if (n_mag < 1e-10) {
        omega = linalg::atan2(e.y,e.x);
    } else {
        omega = linalg::acos(linalg::dot(n,e) / (n_mag * e_mag));
        if (e.z < 0) {
            omega = 2 * pi - omega;
        }
    }

    //True anomaly
    //double cos_ta = linalg::clamp(linalg::dot(e, POS) / (e_mag * POS_mag), -1.0, 1.0);
    //double sin_ta = (v_r * h_mag) / (mu * e_mag);
    
    //double ta = atan2(sin_ta, cos_ta);
    
    //if (ta < 0.0) ta += 2.0 * pi;
    double ta = 1;
    if (e_mag < 1e-8) {
        if (n_mag < 1e-8) {
            ta = linalg::atan2(POS.y,POS.x);
        } else {
            ta = linalg::acos(linalg::dot(n,POS) / (n_mag / POS_mag));
            if (linalg::dot(n,POS) < 0 || linalg::dot(linalg::cross(n,POS),h) < 0) {
                ta = 2 * pi - ta;
            }
        }
    } else {
        ta = linalg::acos(linalg::dot(e,POS) / (e_mag * POS_mag));
        if (linalg::dot(POS,VEL) < 0) {
            ta = 2 * pi - ta;
        }
    }

    //double specific_energy = 0.5 * VEL_mag * VEL_mag - mu / POS_mag;

    //Semi major axis
    //double a = -mu / (2.0 * specific_energy);
    double a = -1;
    if (linalg::abs(e_mag - 1.0f) < 1e-8) {
        a = 57;
    } else {
        a = (h_mag*h_mag) / (mu * (1 - (e_mag*e_mag)));
    }
    

    double p = (h_mag * h_mag) / mu;
    eccentricity = e_mag;
    semi_major_axis = a;
    semi_minor_axis = (eccentricity < 1.0) ? a * sqrt(1.0 - e_mag*e_mag) : NAN;
    semi_latus_rectum = p;

    this->epoch = epoch;

    period = (eccentricity < 1.0) ? 2.0 * pi * sqrt(a*a*a / mu) : NAN;
    //orbital_speed = sqrt(mu * (2.0 / POS_mag - 1.0 / a));

    inclination = i;
    
    double E = atan2(sqrt(1.0 - e_mag*e_mag) * sin(ta), e_mag + cos(ta));
    mean_anomaly = E - e_mag * sin(E);
    mean_anomaly_at_epoch = mean_anomaly;

    long_ascending_node = Omega;
    argument_of_periapsis = omega;

    //printf("O %f\n",Omega);
    //printf("o %f\n",omega);
    
}


void Orbit::physics_step(float sdl_dt , float phys_warp_rate) {

    //Run VEL
    leap_frog(sdl_dt, phys_warp_rate);
    
    //Zero acc before stepping next
    //T_ACC = linalg::vec<double,3> { 0,0,0 };
}



//  Calculate gravity force
linalg::vec<double,3> Orbit::grav_f() {
    double r2, r3;

    r2 = dot(POS,POS); //distance**
    r3 = r2 * sqrt(r2);
    linalg::vec<double,3> pull_home;
    pull_home = -mu * POS / r3;
    
    return pull_home;
}

//  Step forward
void Orbit::leap_frog(float sdl_dt, float phys_warp_rate) {

    //Accumulation maybe?
    for (int phys_step_count = 0; phys_step_count < phys_warp_rate; phys_step_count++) {
        linalg::vec<double,3> acc_curr;
        float ddt = (float) sdl_dt;

        //Current time sep acceleration
        acc_curr = grav_f(); 
        VEL += (double)0.5 * ddt* acc_curr; //Kick
        POS += ddt*VEL; //Drift
        acc_curr = grav_f(); //New Kick
        //Propagate
        VEL += (double)0.5 * ddt* acc_curr;
    }
    return;
}
