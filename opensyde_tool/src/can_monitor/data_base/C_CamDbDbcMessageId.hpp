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
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbDbcMessageId
{
public:
   C_CamDbDbcMessageId(void);

   uint32_t u32_NodeIndex;
   bool q_MessageIsTx;
   uint32_t u32_Hash;
   uint32_t u32_MessageIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
