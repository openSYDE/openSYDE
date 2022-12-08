//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBNODEDATAELEMENTCONFIG_H
#define C_PUISVDBNODEDATAELEMENTCONFIG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbDataElementScaling.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_PuiSvDbDataElementDisplayFormatter.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbNodeDataElementConfig
{
public:
   C_PuiSvDbNodeDataElementConfig(void);
   virtual ~C_PuiSvDbNodeDataElementConfig(void);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   C_PuiSvDbNodeDataPoolListElementId c_ElementId;
   C_PuiSvDbDataElementScaling c_ElementScaling;
   C_PuiSvDbDataElementDisplayFormatter c_DisplayFormatter;
   QString c_DisplayName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
