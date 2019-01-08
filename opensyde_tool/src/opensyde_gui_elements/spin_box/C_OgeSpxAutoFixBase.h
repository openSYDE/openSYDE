//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Spin box with auto fix to nearest raw value base functionaliy (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGESPXAUTOFIXBASE_H
#define C_OGESPXAUTOFIXBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QVariant>
#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeSpxAutoFixBase
{
public:
   C_OgeSpxAutoFixBase(void);
   virtual ~C_OgeSpxAutoFixBase(void);

   void Init(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
             const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max, const stw_types::float64 of64_Factor = 1.0,
             const stw_types::float64 of64_Offset = 0.0, const stw_types::uint32 ou32_Index = 0);

protected:
   stw_opensyde_core::C_OSCNodeDataPoolContent mc_UnscaledMin;
   stw_opensyde_core::C_OSCNodeDataPoolContent mc_UnscaledMax;
   stw_types::uint64 mu64_NumberOfStepsAvailable;
   stw_types::float64 mf64_Factor;
   stw_types::float64 mf64_Offset;
   stw_types::uint32 mu32_Index;

   virtual void m_Init(void) = 0;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
