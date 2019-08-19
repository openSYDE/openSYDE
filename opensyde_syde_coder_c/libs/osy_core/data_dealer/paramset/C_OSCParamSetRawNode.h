//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing all raw entries for one node in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETRAWNODE_H
#define C_OSCPARAMSETRAWNODE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCParamSetRawEntry.h"
#include "C_OSCParamSetDataPoolInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCParamSetRawNode
{
public:
   C_OSCParamSetRawNode(void);

   stw_scl::C_SCLString c_Name;                        ///< Node name
   std::vector<C_OSCParamSetRawEntry> c_Entries;       ///< All entries to write to this node
   std::vector<C_OSCParamSetDataPoolInfo> c_DataPools; ///< Information about used data pools
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
