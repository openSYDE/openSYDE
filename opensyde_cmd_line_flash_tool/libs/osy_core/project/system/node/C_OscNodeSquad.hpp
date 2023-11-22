//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       The squad of nodes. In case of multiple CPUs it has all indexes of the sub nodes and has its base name

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODESQUAD_HPP
#define C_OSCNODESQUAD_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>

#include "stwtypes.hpp"
#include "C_SclString.hpp"

#include "C_OscNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeSquad
{
public:
   C_OscNodeSquad(void);
   virtual ~C_OscNodeSquad(void);

   static stw::scl::C_SclString h_CombineNames(const stw::scl::C_SclString & orc_MainDeviceName,
                                               const stw::scl::C_SclString & orc_SubDeviceName);
   int32_t SetBaseName(std::vector<C_OscNode> & orc_Nodes, const stw::scl::C_SclString & orc_NodeBaseName);
   static bool h_CheckIsMultiDevice(const uint32_t ou32_NodeIndex,
                                    const std::vector<stw::opensyde_core::C_OscNodeSquad> & orc_AvailableGroups,
                                    uint32_t * const opu32_GroupIndex = NULL);

   static const stw::scl::C_SclString hc_SEPARATOR; ///< Default separator for GUI and device type checks
   stw::scl::C_SclString c_BaseName;                ///< Base name for all sub nodes
   std::vector<uint32_t> c_SubNodeIndexes;          ///< Indexes of all containing sub nodes in the squad
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
