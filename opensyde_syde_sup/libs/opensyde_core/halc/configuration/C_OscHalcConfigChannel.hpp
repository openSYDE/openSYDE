//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Group domain channel config data for HALC configuration
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCCONFIGCHANNEL_HPP
#define C_OSCHALCCONFIGCHANNEL_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscHalcDefBase.hpp"
#include "C_OscHalcConfigParameterStruct.hpp"
#include "C_OscSystemNameMaxCharLimitChangeReportItem.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcConfigChannel
{
public:
   C_OscHalcConfigChannel(void);
   virtual ~C_OscHalcConfigChannel();

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   void CheckConfigValid(bool * const opq_NameInvalid) const;
   void HandleFileLoadPostProcessing(const C_OscHalcDefBase::E_SafetyMode oe_SafetyMode);
   void HandleNameMaxCharLimit(const uint32_t ou32_NameMaxCharLimit, const stw::scl::C_SclString & orc_Type,
                               std::list<C_OscSystemNameMaxCharLimitChangeReportItem> * const opc_ChangedItems);

   stw::scl::C_SclString c_Name;                             ///< User configured name for this channel
   stw::scl::C_SclString c_Comment;                          ///< User configured description for this channel
   bool q_SafetyRelevant;                                    ///< Flag if channel is safety relevant
   uint32_t u32_UseCaseIndex;                                ///< Current selected use-case index
   std::vector<C_OscHalcConfigParameterStruct> c_Parameters; ///< Parameter configuration (synced with C_OscHalcDefBase)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
