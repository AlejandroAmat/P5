#include <iostream>
#include <math.h>
#include "instrument_fm.h"
#include "keyvalue.h"
#include "stdio.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

InstrumentFM::InstrumentFM(const std::string &param)
    : adsr(SamplingRate, param)
{
    bActive = false;
    x.resize(BSIZE);

    KeyValue kv(param);
    if (!kv.to_float("I", I))
    {
        I = 10;
    }
    if (!kv.to_float("N1", N1))
    {
        N1 = 3;
    }
    if (!kv.to_float("N2", N2))
    {
        N2 = 2;
    }

    I = 1. - pow(2, -I / 12.);
}

void InstrumentFM::command(long cmd, long note, long vel)
{

    if (cmd == 9)
    {
        bActive = true;
        adsr.start();

        N = N1 / N2;
        f0 = 440 * (pow(2, ((note - 69.) / 12.)));
        nota = f0 / SamplingRate;
        fm = N * nota;

        A = vel / 127.0;
    }
    else if (cmd == 8)
    {
        adsr.stop();
    }
    else if (cmd == 0)
    {
        adsr.end();
    }
}

const vector<float> &InstrumentFM::synthesize()
{

    if (!bActive)
        return x;

    if (!adsr.active())
    {
        bActive = false;
        x.assign(x.size(), 0);
        return x;
    }

    FILE *fpfm;
    fpfm = fopen("xvec_fm.log","a");
    for (unsigned int i = 0; i < x.size(); ++i)
    {

        x[i] = A * sin(a + I * sin(b));
        fprintf(fpfm,"%f\n",x[i]);

        a = a + 2 * M_PI * nota;
        b = b + 2 * M_PI * fm;

        while (a >= 2 * M_PI)
        {
            a -= 2 * M_PI;
        }
        while (b >= 2 * M_PI)
        {
            b -= 2 * M_PI;
        }
    }

    adsr(x); // apply envelope to x and update internal status of ADSR
    fclose(fpfm);
    return x;
}