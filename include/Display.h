#pragma once
#include "MeasureSetParameters.h"
namespace DisplayInterface
{

    class Display
    {
        MeasurementParams_t &measuredParams;
        public:
        Display(MeasurementParams_t &measuredParamsTemp)
            : measuredParams(measuredParamsTemp)
        {
        }
        
        void init(void);
        void process(void);
    };

}