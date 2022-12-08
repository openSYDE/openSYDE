//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for read data element configuration information (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVREADDATACONFIGURATION_H
#define C_PUISVREADDATACONFIGURATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvReadDataConfiguration
{
public:
   enum E_TransmissionMode
   {
      eTM_CYCLIC,
      eTM_ON_CHANGE,
      eTM_ON_TRIGGER
   };

   C_PuiSvReadDataConfiguration(void);
   virtual ~C_PuiSvReadDataConfiguration(void);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;
   int32_t InitDefaultThreshold(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max);

   E_TransmissionMode e_TransmissionMode;
   uint8_t u8_RailIndex; ///< 0: fast, 1: medium, 2: slow, else: unexpected
   stw::opensyde_core::C_OscNodeDataPoolContent c_ChangeThreshold;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
