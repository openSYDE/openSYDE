//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class, containing all raw entries for one node in a parameter set file (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETRAWNODE_HPP
#define C_OSCPARAMSETRAWNODE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_OscParamSetRawEntry.hpp"
#include "C_OscParamSetDataPoolInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscParamSetRawNode
{
public:
   C_OscParamSetRawNode(void);

   stw::scl::C_SclString c_Name;                       ///< Node name
   std::vector<C_OscParamSetRawEntry> c_Entries;       ///< All entries to write to this node
   std::vector<C_OscParamSetDataPoolInfo> c_DataPools; ///< Information about used data pools
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
