//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Datapool ID (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLID_HPP
#define C_OSCNODEDATAPOOLID_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeDataPoolId
{
public:
   C_OscNodeDataPoolId(void);
   virtual ~C_OscNodeDataPoolId(void);
   C_OscNodeDataPoolId(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex);

   virtual bool operator < (const C_OscNodeDataPoolId & orc_Cmp) const;
   virtual bool operator == (const C_OscNodeDataPoolId & orc_Cmp) const;
   virtual bool operator != (const C_OscNodeDataPoolId & orc_Cmp) const;

   uint32_t u32_NodeIndex;
   uint32_t u32_DataPoolIndex;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
