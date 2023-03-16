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
#include "stwtypes.hpp"

#include "C_OscNodeDataPoolContent.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSvDbDataElementContent :
   public stw::opensyde_core::C_OscNodeDataPoolContent
{
public:
   C_PuiSvDbDataElementContent(void);
   C_PuiSvDbDataElementContent(const C_PuiSvDbDataElementContent & orc_Source);
   C_PuiSvDbDataElementContent(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Source);
   C_PuiSvDbDataElementContent & operator = (const C_PuiSvDbDataElementContent & orc_Source) &; //lint !e1511 //we want
                                                                                                // to
                                                                                                // hide the base func.

   void SetTimeStamp(const uint32_t ou32_Timestamp);
   uint32_t GetTimeStamp(void) const;

private:
   uint32_t mu32_TimeStamp;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
