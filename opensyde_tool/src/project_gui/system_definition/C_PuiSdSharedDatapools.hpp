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

#include "stwtypes.hpp"
#include "C_OscNodeDataPoolId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PuiSdSharedDatapools
{
public:
   C_PuiSdSharedDatapools(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   void Clear(void);

   // Handling of changed Datapool shared state
   void AddSharedDatapool(const stw::opensyde_core::C_OscNodeDataPoolId & orc_NewDatapool,
                          const stw::opensyde_core::C_OscNodeDataPoolId & orc_ShareDatapool);
   int32_t AddSharedDatapool(const stw::opensyde_core::C_OscNodeDataPoolId & orc_NewDatapool,
                             const uint32_t ou32_SharedDatapoolGroup);
   int32_t RemoveSharedDatapool(const stw::opensyde_core::C_OscNodeDataPoolId & orc_Datapool);

   // Getter functions
   bool IsSharedDatapool(const uint32_t ou32_SharedNodeIndex, const uint32_t ou32_SharedDatapoolIndex,
                         uint32_t * const opu32_SharedDatapoolGroup) const;
   bool IsSharedDatapool(const stw::opensyde_core::C_OscNodeDataPoolId & orc_Datapool,
                         uint32_t * const opu32_SharedDatapoolGroup) const;
   int32_t GetSharedDatapoolGroup(const uint32_t ou32_SharedDatapoolGroup,
                                  std::vector<stw::opensyde_core::C_OscNodeDataPoolId> & orc_SharedDatapoolGroup)
   const;
   bool IsSharedAndGetDatapoolGroup(const uint32_t ou32_SharedNodeIndex, const uint32_t ou32_SharedDatapoolIndex,
                                    std::vector<stw::opensyde_core::C_OscNodeDataPoolId> & orc_SharedDatapoolGroup)
   const;
   bool IsSharedAndGetDatapoolGroup(const stw::opensyde_core::C_OscNodeDataPoolId & orc_Datapool,
                                    std::vector<stw::opensyde_core::C_OscNodeDataPoolId> & orc_SharedDatapoolGroup)
   const;

   // Helper functions to adapt configuration on change of Datapool configuration in system definition
   void OnNodeRemoved(const uint32_t ou32_NodeIndex);
   void OnDatapoolInserted(const stw::opensyde_core::C_OscNodeDataPoolId & orc_Datapool);
   void OnDatapoolRemoved(const stw::opensyde_core::C_OscNodeDataPoolId & orc_Datapool);
   void OnDatapoolMoved(const stw::opensyde_core::C_OscNodeDataPoolId & orc_SourceDatapool,
                        const stw::opensyde_core::C_OscNodeDataPoolId & orc_TargetDatapool);

   // Each vector in the upper vector equals a connected group of shared datapools
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolId> > c_SharedDatapools;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
