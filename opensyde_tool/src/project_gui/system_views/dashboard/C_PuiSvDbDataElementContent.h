//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Holds datapool element content and its timestamp (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBDATAELEMENTCONTENT_H
#define C_PUISVDBDATAELEMENTCONTENT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

#include "C_OSCNodeDataPoolContent.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElementContent :
   public stw_opensyde_core::C_OSCNodeDataPoolContent
{
public:
   C_PuiSvDbDataElementContent(void);
   C_PuiSvDbDataElementContent(const C_PuiSvDbDataElementContent & orc_Source);
   C_PuiSvDbDataElementContent(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Source);
   C_PuiSvDbDataElementContent & operator = (const C_PuiSvDbDataElementContent & orc_Source);

   void SetTimeStamp(const stw_types::uint32 ou32_Timestamp);
   stw_types::uint32 GetTimeStamp(void) const;

private:
   stw_types::uint32 mu32_TimeStamp;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
