//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for read data element configuration information (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUISVREADDATACONFIGURATION_H
#define C_PUISVREADDATACONFIGURATION_H

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

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

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;
   stw_types::sint32 InitDefaultThreshold(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                          const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max);

   E_TransmissionMode e_TransmissionMode;
   stw_types::uint8 u8_RailIndex; ///< 0: fast, 1: medium, 2: slow, else: unexpected
   stw_opensyde_core::C_OSCNodeDataPoolContent c_ChangeThreshold;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
