//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System definition shared Datapool information (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISDSHAREDDATAPOOLS_H
#define C_PUISDSHAREDDATAPOOLS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include <list>

#include "stwtypes.h"
#include "C_OSCNodeDataPoolId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdSharedDatapools
{
public:
   C_PuiSdSharedDatapools(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   void Clear(void);

   // Handling of changed Datapool shared state
   void AddSharedDatapool(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_NewDatapool,
                          const stw_opensyde_core::C_OSCNodeDataPoolId & orc_ShareDatapool);
   stw_types::sint32 AddSharedDatapool(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_NewDatapool,
                                       const stw_types::uint32 ou32_SharedDatapoolGroup);
   stw_types::sint32 RemoveSharedDatapool(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_Datapool);

   // Getter functions
   bool IsSharedDatapool(const stw_types::uint32 ou32_SharedNodeIndex, const stw_types::uint32 ou32_SharedDatapoolIndex,
                         stw_types::uint32 * const opu32_SharedDatapoolGroup) const;
   bool IsSharedDatapool(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_Datapool,
                         stw_types::uint32 * const opu32_SharedDatapoolGroup) const;
   stw_types::sint32 GetSharedDatapoolGroup(const stw_types::uint32 ou32_SharedDatapoolGroup,
                                            std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> & orc_SharedDatapoolGroup)
   const;
   bool IsSharedAndGetDatapoolGroup(const stw_types::uint32 ou32_SharedNodeIndex,
                                    const stw_types::uint32 ou32_SharedDatapoolIndex,
                                    std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> & orc_SharedDatapoolGroup) const;
   bool IsSharedAndGetDatapoolGroup(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_Datapool,
                                    std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> & orc_SharedDatapoolGroup) const;

   // Helper functions to adapt configuration on change of Datapool configuration in system definition
   void OnNodeRemoved(const stw_types::uint32 ou32_NodeIndex);
   void OnDatapoolInserted(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_Datapool);
   void OnDatapoolRemoved(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_Datapool);
   void OnDatapoolMoved(const stw_opensyde_core::C_OSCNodeDataPoolId & orc_SourceDatapool,
                        const stw_opensyde_core::C_OSCNodeDataPoolId & orc_TargetDatapool);

   // Each vector in the upper vector equals a connected group of shared datapools
   std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolId> > c_SharedDatapools;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
