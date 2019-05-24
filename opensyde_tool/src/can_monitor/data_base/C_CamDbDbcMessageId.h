//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Required indices for DBC message identification (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBDBCMESSAGEID_H
#define C_CAMDBDBCMESSAGEID_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbDbcMessageId
{
public:
   C_CamDbDbcMessageId(void);

   stw_types::uint32 u32_NodeIndex;
   bool q_MessageIsTx;
   stw_types::uint32 u32_MessageIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
