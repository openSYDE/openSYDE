//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Encapsulates non-trivial sequences for NVM access. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDATADEALERNVM_HPP
#define C_OSCDATADEALERNVM_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscDataDealer.hpp"
#include "C_OscNodeDataPoolList.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///openSYDE NVM data dealer
class C_OscDataDealerNvm :
   public C_OscDataDealer
{
public:
   C_OscDataDealerNvm(void);
   C_OscDataDealerNvm(C_OscNode * const opc_Node, const uint32_t ou32_NodeIndex,
                      C_OscDiagProtocolBase * const opc_DiagProtocol);
   virtual ~C_OscDataDealerNvm(void);

   uint16_t NvmCalcCrc(const C_OscNodeDataPoolList & orc_List) const;
   virtual int32_t NvmReadList(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
                               uint8_t * const opu8_NrCode);
   int32_t NvmNotifyOfChanges(const uint8_t ou8_DataPoolIndex, const uint8_t ou8_ListIndex,
                              bool & orq_ApplicationAcknowledge, uint8_t * const opu8_NrCode);

protected:
   int32_t m_NvmReadListRaw(const C_OscNodeDataPoolList & orc_List, std::vector<uint8_t> & orc_Values,
                            uint8_t * const opu8_NrCode);
   int32_t m_SaveDumpToList(const std::vector<uint8_t> & orc_Values, C_OscNodeDataPoolList & orc_List) const;
   int32_t m_SaveDumpValuesToListValues(const std::vector<uint8_t> & orc_Values,
                                        C_OscNodeDataPoolList & orc_List) const;
   static int32_t mh_AdaptProtocolReturnValue(const int32_t os32_ProtReturnValue);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
