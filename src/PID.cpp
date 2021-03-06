#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
    
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;

    i_error = 0.0;
    has_a_data_point = false;

}

void PID::UpdateError(double cte) {
    if (! has_a_data_point){
        d_error = 0;
        has_a_data_point = true;
    } else {
        d_error = cte - p_error;
    }
    
    p_error = cte;
    i_error += cte;
}

double PID::TotalError() {
    return i_error;
}

