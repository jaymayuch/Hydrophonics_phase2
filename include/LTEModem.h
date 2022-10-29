#pragma once
#include "MeasureSetParameters.h"
namespace ModemInterface
{
    class Modem
    {
    public:
        MeasurementParams_t &measuredParams;
        setParameters_t               &setParams;       
    public:
        Modem(MeasurementParams_t &measuredParamsTemp,setParameters_t &setParamsTemp)
            : measuredParams(measuredParamsTemp),setParams(setParamsTemp)
        {
        }
        void init(void);
        void sendIOTServer(void);
        void sendSMSAlert(char *);

    };
}