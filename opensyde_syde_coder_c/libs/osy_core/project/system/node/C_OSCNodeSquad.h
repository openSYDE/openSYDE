//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       The squad of nodes. In case of multiple CPUs it has all indexes of the sub nodes and has its base name

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODESQUAD_H
#define C_OSCNODESQUAD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.h"
#include "CSCLString.h"

#include "C_OSCNode.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeSquad
{
public:
   C_OSCNodeSquad(void);
   virtual ~C_OSCNodeSquad(void);

   static stw_scl::C_SCLString h_CombineNames(const stw_scl::C_SCLString & orc_MainDeviceName,
                                              const stw_scl::C_SCLString & orc_SubDeviceName);
   stw_types::sint32 SetBaseName(std::vector<C_OSCNode> & orc_Nodes, const stw_scl::C_SCLString & orc_NodeBaseName);
   static bool h_CheckIsMultiDevice(const stw_types::uint32 ou32_NodeIndex,
                                    const std::vector<stw_opensyde_core::C_OSCNodeSquad> & orc_AvailableGroups,
                                    stw_types::uint32 * const opu32_GroupIndex = NULL);

   static const stw_scl::C_SCLString hc_SEPARATOR;  ///< Default separator for GUI and device type checks
   stw_scl::C_SCLString c_BaseName;                 ///< Base name for all sub nodes
   std::vector<stw_types::uint32> c_SubNodeIndexes; ///< Indexes of all containing sub nodes in the squad
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
