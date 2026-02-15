#include "orbit.h"


//Newton-Rhapson equation solver
double Orbit::solveEccentricAnomaly(double M, double ecc, double maxError = 1E-07)
{
    double adjust = 1.0;
    double guess = M + ecc * linalg::sin(M) + 0.5 * ecc * ecc * linalg::sin(2.0 * M);
    while (linalg::abs(adjust) > maxError)
    {
        double ans = guess - ecc * linalg::sin(guess);
        adjust = (M - ans) / (1.0 - ecc * linalg::cos(guess));
        guess += adjust;
    }
    return guess;
}

double Orbit::solveEccentricAnomalyHyp(double M, double ecc, double maxError = 1E-07)
{
		double adjust = 1.0;
		double guess = linalg::log(2.0 * M / ecc + 1.8);
		while (linalg::abs(adjust) > maxError)
		{
			adjust = (eccentricity * linalg::sinh(guess) - guess - M) / (eccentricity * linalg::cosh(guess) - 1.0);
			guess -= adjust;
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
        double n = sqrt(mu / (semi_major_axis*semi_major_axis*semi_major_axis));
        mean_anomaly = mean_anomaly_at_epoch + n * ObT;
        mean_anomaly = fmod(mean_anomaly, 2.0*pi);

        //Calc EA
        if (eccentricity < 1E-05) { //Near-circular orbits
            eccentric_anomaly = mean_anomaly;
            true_anomaly = mean_anomaly;
        } else {    //Noncircular orbits
            if (eccentricity < 0.9) {
                eccentric_anomaly = solveEccentricAnomaly(mean_anomaly,eccentricity);
            } else {
                printf("ECC TOO HIGHH!!! SOLVE EXTREME CASE WITH OTHER FUNC\n");
            }
                //Calc TA
                true_anomaly = linalg::atan2(
                    sqrt(1 - eccentricity*eccentricity) * sin(eccentric_anomaly),
                    cos(eccentric_anomaly) - eccentricity
                );
        }

        radius = semi_major_axis * (1.0 - eccentricity * eccentricity)
        / (1.0 + eccentricity * linalg::cos(true_anomaly));

    } else if (eccentricity > 1.0) { //SOLVE HYPERBOLIC
        if (eccentricity == 1.0)    //Turn parabolic orbit edge case into hyperbolic
            eccentricity += 1E-10;
        mean_anomaly = 2.0 * pi * linalg::abs(ObT) / period;
        eccentric_anomaly = solveEccentricAnomalyHyp(mean_anomaly,eccentricity);
        true_anomaly = linalg::atan2(linalg::sqrt(eccentricity * eccentricity - 1.0) * linalg::sinh(eccentric_anomaly), eccentricity - linalg::cosh(eccentric_anomaly));
        if (ObT < 2.0) {
            true_anomaly = pi * 2 - true_anomaly;
        }
        radius = (0.0 - semi_major_axis) * (eccentricity * eccentricity - 1.0) / (1.0 + eccentricity * linalg::cos(true_anomaly));
    }

    if (radius < 0) {
        printf("E 128385 : Something went terribly wrong %f\n",eccentricity);
    }

    double h = sqrt(mu * semi_major_axis * (1 - eccentricity*eccentricity));
    double v_r = mu / h * eccentricity * sin(true_anomaly);
    double v_theta = mu / h * (1 + eccentricity * cos(true_anomaly));

    linalg::vec<double,3> r_pf;
    linalg::vec<double,3> v_pf;

    //Now convert to perifocal plane
    //Describes a 2D plane along the plane of the orbit
    //POS
    r_pf.x = radius * cos(true_anomaly);
    r_pf.y = radius * sin(true_anomaly);
    r_pf.z = 0.0;

    v_pf.x = v_r * cos(true_anomaly) - v_theta * sin(true_anomaly);
    v_pf.y = v_r * sin(true_anomaly) + v_theta * cos(true_anomaly);
    v_pf.z = 0.0;


    //Rotation matrix
    linalg::mat<double,3,3> RM;

    //https://en.wikipedia.org/wiki/Perifocal_coordinate_system
    //Dont forget to make it Y up! if you end up converting to perifocal for some reason!
    //Precompute terms
    double cos_O = cos(long_ascending_node);
    double sin_O = sin(long_ascending_node);
    double cos_i = cos(inclination);
    double sin_i = sin(inclination);
    double cos_w = cos(argument_of_periapsis);
    double sin_w = sin(argument_of_periapsis);

    // RM[0][0] = -sin_O*cos_i*cos_w + sin_O*sin_w;
    // RM[0][1] = sin_i*sin_w;
    // RM[0][2] = cos_O*cos_w + sin_O*cos_i*sin_w;

    // RM[1][0] = -cos_O*cos_i*cos_w - sin_O*sin_w;
    // RM[1][1] = sin_i*cos_w;
    // RM[1][2] = sin_O*cos_w - cos_O*cos_i*sin_w;

    // RM[2][0] = sin_i*sin_w;
    // RM[2][1] = cos_i;
    // RM[2][2] = sin_i*cos_w;


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
    POS = linalg::mul(RM,r_pf);
    VEL = linalg::mul(RM,v_pf);
}

//https://orbital-mechanics.space/classical-orbital-elements/orbital-elements-and-the-state-vector.html
//Calculate on-rails orbital elements given keplarian elements
void Orbit::physics_to_rails(double epoch) {
    //Y is up Z forward!
    linalg::vec<double,3> K = {0,1,0};

    if (mu == 0 ) { printf("E 134814: MU NOT SET!!!!\n");}
    //Dont use linalg::normalize() to find length.
    double Omega = -1;
    double POS_mag = linalg::length(POS);
    double VEL_mag = linalg::length(VEL);
    double v_r = linalg::dot(POS, VEL) / POS_mag; //radial velocity
    //double v_p = linalg::sqrt((VEL_mag*VEL_mag) - (v_r*v_r));   //azimuthal velocity

    //Angular momentum (h, h_mag)
    linalg::vec<double, 3> h = linalg::cross(POS,VEL);
    double h_mag = linalg::length(h);
    

    //Inclination
    double i = linalg::acos(linalg::clamp(linalg::dot(h, K) / h_mag, -1.0, 1.0));

    //Right Ascension @ Ascending Node
    //N=^K x h
    

    //Node line (n, n_mag)
    linalg::vec<double,3> n = linalg::cross(K, h);
    double n_mag = linalg::length(n);

    //LAN (Omega)
    if (n_mag < 1E-7) {
        Omega = 0.0;
    } else {
        Omega = atan2(n.z, n.x);
        if (Omega < 0) Omega += 2*pi;
    }

    //Eccentricity (e, e_mag)
    linalg::vec<double,3> e = (linalg::cross(VEL, h) / mu) - (POS / POS_mag);
    double e_mag = linalg::length(e);


    //Argument of periapsis (Lowercase omega)
    double omega = linalg::atan2(linalg::dot(linalg::cross(n, e), h) / h_mag,linalg::dot(n, e));
    if (omega < 0) omega += 2.0*pi;
    //True anomaly
    double ta = linalg::atan2(v_r * h_mag / mu,linalg::dot(e, POS) / e_mag);
    if (ta < 0) ta += 2.0*pi;


    double specific_energy = (VEL_mag*VEL_mag)/2.0 - (mu / POS_mag);
    double a = -mu / (2.0 * specific_energy);
    double p = (h_mag * h_mag) / mu;
    //STO ECC
    eccentricity = e_mag;

    //STO SmA
    if (eccentricity < 1.0) { //CAnnot be hyperbonlic
        semi_minor_axis = a * linalg::sqrt(1 - eccentricity*eccentricity);
    } else {
        semi_minor_axis = NAN;
    }
    //STO SMA
    semi_major_axis = a;

    //STO SLR
    semi_latus_rectum = p;

    //STO Epoch
    this->epoch = epoch;

    //STO Period
    if (eccentricity < 1.0) {
        period = 2*pi * linalg::sqrt((a*a*a)/mu);
    } else {
        period = NAN;
    }

    //STO Orbital speed
    orbital_speed = linalg::sqrt(mu * (2.0/POS_mag - 1.0/a)); //Derived from orbital elems

    //STO INC
    inclination = i;

    //STO MA
    ///double E = 2.0 * linalg::atan( linalg::tan(ta/2.0) *
    //linalg::sqrt((1.0 - eccentricity)/(1.0 + eccentricity)));
    double E = linalg::atan2(
        linalg::sqrt(1.0 - eccentricity*eccentricity) * linalg::sin(ta),
        eccentricity + linalg::cos(ta)
    );
    

    mean_anomaly = E - eccentricity * linalg::sin(E);

    //STO MA-E
    mean_anomaly_at_epoch = mean_anomaly;

    //STO LAN
    long_ascending_node = Omega;

    //STO A_P
    argument_of_periapsis = omega;


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
