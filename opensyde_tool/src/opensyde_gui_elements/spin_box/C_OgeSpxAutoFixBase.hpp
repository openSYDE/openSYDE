//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box with auto fix to nearest raw value base functionaliy (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGESPXAUTOFIXBASE_H
#define C_OGESPXAUTOFIXBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QVariant>
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeSpxAutoFixBase
{
public:
   C_OgeSpxAutoFixBase(void);
   virtual ~C_OgeSpxAutoFixBase(void);

   void Init(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
             const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max, const float64_t of64_Factor = 1.0,
             const float64_t of64_Offset = 0.0, const uint32_t ou32_Index = 0);

protected:
   stw::opensyde_core::C_OscNodeDataPoolContent mc_UnscaledMin;
   stw::opensyde_core::C_OscNodeDataPoolContent mc_UnscaledMax;
   uint64_t mu64_NumberOfStepsAvailable;
   float64_t mf64_Factor;
   float64_t mf64_Offset;
   uint32_t mu32_Index;

   virtual void m_Init(void) = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
