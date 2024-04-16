#include "pid/pid.hpp"
#include <iostream>
#include <algorithm>
#include <complex>

PID::PID(double Kp, double Ki, double Kd, double Umax, double Umin, double Kr,
         double Km, double Kiae)
    : Kp(Kp)
    , Ki(Ki)
    , Kd(Kd)
    , Umax(Umax)
    , Umin(Umin)
    , Kiae(Kiae)
    , Kr(Kr)
    , Km(Km)
{
    pOut = 0.0;
    iOut = 0.0;
    dOut = 0.0;
    dErr = 0.0;
    preSatOut = 0.0;
    last_iOut = 0.0;
    last_dOut = 0.0;
    last_dErr = 0.0;
    sat_record = 0.0;
    last_Iae = 0.0;
    Err = 0.0;
    Out = 0.0;
    x[0] = 0.0;
    x[1] = 0.0;
    x[2] = 0.0;
    w[0] = Kp;
    w[1] = Kp * Ki;
    w[2] = Kp * Kd;
    u_ = 0.0;
    last_u_ = 0;
    pre_u_ = 0;
}

double PID::Update(double Ref, double Fbk)
{
    pOut = Kr * Ref - Fbk;
    Err = Ref - Fbk;
    Iae = last_Iae + Kiae * abs(Err);
    w1 = abs(sat_record) < 1e-6 ? 1 : 0;
    iOut = last_iOut + w1 * Ki * Err;
    dErr = Km * Ref - Fbk;
    dOut = Kd * (c2 * last_dOut + c1 * dErr - c1 * last_dErr);

    preSatOut = Kp * (pOut + iOut + dOut);
    if (preSatOut > Umax)
        Out = Umax;
    else if (preSatOut < Umin)
        Out = Umin;
    else
        Out = preSatOut;

    // updatePIDparam();

    sat_record = Out - preSatOut;
    last_iOut = iOut;
    last_dOut = dOut;
    last_dErr = dErr;
    last_Err = Err;
    last_Iae = Iae;

    return Out;
}

void PID::setFilter(double a, double T)
{
    c1 = a;
    c2 = 1. - c1 * T;
}