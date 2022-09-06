//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store node

   Store node (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEH
#define C_OSCNODEH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCNodeProperties.h"
#include "C_OSCNodeDataPool.h"
#include "C_OSCNodeApplication.h"
#include "C_OSCCanProtocol.h"
#include "C_OSCHalcConfig.h"
#include "C_OSCCanMessageUniqueId.h"
#include "C_OSCCanOpenManagerInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------
///container for all elements that describe one node
class C_OSCNode
{
public:
   C_OSCNode(void);
   virtual ~C_OSCNode(void);

   //lint -sem(stw_opensyde_core::C_OSCNode::Initialize,initializer)
   void Initialize(void);

   stw_types::sint32 InsertDataPool(const stw_types::uint32 ou32_DataPoolIndex, const C_OSCNodeDataPool & orc_DataPool);
   stw_types::sint32 DeleteDataPool(const stw_types::uint32 ou32_DataPoolIndex);
   stw_types::sint32 MoveDataPool(const stw_types::uint32 ou32_Start, const stw_types::uint32 ou32_Target);
   stw_types::sint32 InsertMessage(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                   const stw_types::uint32 ou32_InterfaceIndex,
                                   const stw_types::uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                   const stw_types::uint32 ou32_MessageIndex, const C_OSCCanMessage & orc_Message,
                                   const std::vector<C_OSCNodeDataPoolListElement> & orc_SignalData);
   stw_types::sint32 SetMessage(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                const stw_types::uint32 ou32_InterfaceIndex, const stw_types::uint32 ou32_DatapoolIndex,
                                const bool oq_MessageIsTx, const stw_types::uint32 ou32_MessageIndex,
                                const C_OSCCanMessage & orc_Message, const bool oq_NewMessageIsTx,
                                const std::vector<C_OSCNodeDataPoolListElement> & orc_SignalData);
   stw_types::sint32 DeleteMessage(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                   const stw_types::uint32 ou32_InterfaceIndex,
                                   const stw_types::uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                   const stw_types::uint32 ou32_MessageIndex);
   stw_types::sint32 InsertSignal(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                  const stw_types::uint32 ou32_InterfaceIndex,
                                  const stw_types::uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                  const stw_types::uint32 ou32_MessageIndex, const stw_types::uint32 ou32_SignalIndex,
                                  const C_OSCCanSignal & orc_Signal,
                                  const C_OSCNodeDataPoolListElement & orc_SignalData);
   stw_types::sint32 SetSignal(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                               const stw_types::uint32 ou32_InterfaceIndex, const stw_types::uint32 ou32_DatapoolIndex,
                               const bool oq_MessageIsTx, const stw_types::uint32 ou32_MessageIndex,
                               const stw_types::uint32 ou32_SignalIndex, const C_OSCCanSignal & orc_Signal,
                               const C_OSCNodeDataPoolListElement & orc_SignalData);
   stw_types::sint32 SetSignalPosition(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                       const stw_types::uint32 ou32_InterfaceIndex,
                                       const stw_types::uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                       const stw_types::uint32 ou32_MessageIndex,
                                       const stw_types::uint32 ou32_SignalIndex, const C_OSCCanSignal & orc_Signal);
   stw_types::sint32 SetSignalMUXValue(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                       const stw_types::uint32 ou32_InterfaceIndex,
                                       const stw_types::uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                       const stw_types::uint32 ou32_MessageIndex,
                                       const stw_types::uint32 ou32_SignalIndex,
                                       const stw_types::uint16 ou16_MultiplexValue);
   stw_types::sint32 DeleteSignal(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                  const stw_types::uint32 ou32_InterfaceIndex,
                                  const stw_types::uint32 ou32_DatapoolIndex, const bool oq_MessageIsTx,
                                  const stw_types::uint32 ou32_MessageIndex, const stw_types::uint32 ou32_SignalIndex);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_types::sint32 GetDataPoolIndex(const C_OSCNodeDataPool::E_Type oe_DataPoolType,
                                      const stw_types::uint32 ou32_DataPoolTypeIndex) const;
   stw_types::sint32 GetDataPoolTypeIndex(const stw_types::uint32 ou32_DataPoolIndex) const;
   stw_types::uint32 GetDataPoolCount(const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DataPoolType) const;

   static bool h_CompareNameGreater(const C_OSCNode & orc_Node1, const C_OSCNode & orc_Node2);
   stw_types::uint32 GetListsSize(void) const;
   stw_types::uint32 GetDataPoolsSize(void) const;
   stw_types::uint32 GetDataPoolAbsoluteAddress(const stw_types::uint32 ou32_DataPoolIndex) const;
   stw_types::uint32 GetListAbsoluteAddress(const stw_types::uint32 ou32_DataPoolIndex,
                                            const stw_types::uint32 ou32_ListIndex) const;
   stw_types::uint32 GetElementAbsoluteAddress(const stw_types::uint32 ou32_DataPoolIndex,
                                               const stw_types::uint32 ou32_ListIndex,
                                               const stw_types::uint32 ou32_ElementIndex) const;
   const C_OSCNodeDataPool * GetHalDataPoolConst(const bool oq_SafeDatapool) const;
   const C_OSCNodeDataPool * GetComDataPoolConst(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                                 const stw_types::uint32 ou32_DataPoolIndex) const;
   C_OSCNodeDataPool * GetComDataPool(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                      const stw_types::uint32 ou32_DataPoolIndex);
   std::vector<const C_OSCNodeDataPool *> GetComDatapoolsConst(const C_OSCCanProtocol::E_Type oe_ComProtocol) const;
   std::vector<C_OSCNodeDataPool *> GetComDataPools(const C_OSCCanProtocol::E_Type oe_ComProtocol);
   const C_OSCCanProtocol * GetCANProtocolConst(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                                const stw_types::uint32 ou32_DataPoolIndex) const;
   C_OSCCanProtocol * GetCANProtocol(const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                     const stw_types::uint32 ou32_DataPoolIndex);
   std::vector<const C_OSCCanProtocol *> GetCANProtocolsConst(const C_OSCCanProtocol::E_Type oe_ComProtocol) const;
   std::vector<C_OSCCanProtocol *> GetCANProtocols(const C_OSCCanProtocol::E_Type oe_ComProtocol);
   const C_OSCCanProtocol * GetRelatedCANProtocolConst(const stw_types::uint32 ou32_DataPoolIndex) const;
   C_OSCCanProtocol * GetRelatedCANProtocol(const stw_types::uint32 ou32_DataPoolIndex);
   void CheckErrorCANProtocol(const C_OSCCanProtocol::E_Type oe_ComProtocol, const bool oq_ComProtocolUsedByInterface,
                              bool & orq_InvalidRxSignalCount, bool & orq_InvalidTxSignalCount,
                              bool & orq_InvalidCoRPDOCount, bool & orq_InvalidCoTPDOCount) const;
   void CheckErrorCANProtocol(const stw_types::uint32 ou32_InterfaceIndex,
                              const C_OSCCanProtocol::E_Type oe_ComProtocol, const bool oq_ComProtocolUsedByInterface,
                              bool & orq_InvalidRxSignalCount, bool & orq_InvalidTxSignalCount,
                              bool & orq_InvalidCoRPDOCount, bool & orq_InvalidCoTPDOCount) const;

   void CheckErrorDataPoolNumListsAndElements(const stw_types::uint32 ou32_DataPoolIndex,
                                              bool & orq_InvalidNumberOfListsOrElements) const;
   void CheckErrorDataPool(const stw_types::uint32 ou32_DataPoolIndex, bool * const opq_NameConflict,
                           bool * const opq_NameInvalid, bool * const opq_IsErrorInListOrMessage,
                           bool * const opq_InvalidNumberOfListsOrElements,
                           std::vector<stw_types::uint32> * const opc_InvalidListIndices) const;
   void CheckMessageId(const stw_types::uint32 ou32_InterfaceIndex, const C_OSCCanMessageUniqueId & orc_MessageId,
                       bool & orq_Valid, const C_OSCCanProtocol::E_Type * const ope_SkipComProtocol = NULL,
                       const stw_types::uint32 * const opu32_SkipInterfaceIndex = NULL,
                       const bool * const opq_SkipMessageIsTxFlag = NULL,
                       const stw_types::uint32 * const opu32_SkipMessageIndex = NULL) const;
   void CheckMessageName(const stw_types::uint32 ou32_InterfaceIndex, const stw_scl::C_SCLString & orc_MessageName,
                         bool & orq_Valid, const C_OSCCanProtocol::E_Type * const ope_SkipComProtocol = NULL,
                         const stw_types::uint32 * const opu32_SkipInterfaceIndex = NULL,
                         const bool * const opq_SkipMessageIsTxFlag = NULL,
                         const stw_types::uint32 * const opu32_SkipMessageIndex = NULL) const;
   stw_types::sint32 CheckApplicationProcessIdValid(const stw_types::uint32 ou32_ApplicationIndex,
                                                    bool & orq_Valid) const;
   void CheckHalcConfigValid(bool * const opq_ConfigInvalid,
                             std::vector<stw_types::uint32> * const opc_InvalidDomainIndices) const;
   void ReCalcCanProtocolDataPoolIndices(void);
   bool IsAnyUpdateAvailable(void) const;
   bool IsRoutingAvailable(const C_OSCSystemBus::E_Type oe_Type) const;
   void RecalculateAddress(void);

   C_OSCNodeDataPoolListElement * GetDataPoolListElement(const stw_types::uint32 ou32_DataPoolIndex,
                                                         const stw_types::uint32 ou32_ListIndex,
                                                         const stw_types::uint32 ou32_ElementIndex);

   const C_OSCDeviceDefinition * pc_DeviceDefinition; ///< Pointer to device definition
   stw_types::uint32 u32_SubDeviceIndex;
   stw_scl::C_SCLString c_DeviceType; ///< Node type: for non multi-cpu devices (C_OSCDeviceDefinition::c_DeviceName =
   ///< C_OSCNode::c_DeviceType) for multi-cpu devices (C_OSCSubDeviceDefinition::c_DeviceName =
   ///< C_OSCNode::c_DeviceType).
   ///< Simply put: When the device type of multi-cpu device is needed:
   ///< "C_OSCNode::pc_DeviceDefinition->c_DeviceName" will do the trick.
   C_OSCNodeProperties c_Properties;           ///< General node properties
   std::vector<C_OSCNodeDataPool> c_DataPools; ///< All datapools assigned to this node, expected type order:
   ///< DIAG, NVM then COM
   bool q_DatapoolAutoNvMStartAddress; ///< Flag if the Datapool NvM start will be calculate automatically

   std::vector<C_OSCNodeApplication> c_Applications; ///< All data blocks assigned to this node
   std::vector<C_OSCCanProtocol> c_ComProtocols;     ///< All node specific information
   ///< for related communication protocol.
   ///< Created if necessary.
   ///< Maximum size equal to number of possible com protocol types.
   C_OSCHalcConfig c_HALCConfig;                                          ///< Optional HALC configuration for this node
   std::map<stw_types::uint8, C_OSCCanOpenManagerInfo> c_CanOpenManagers; ///< CANopen managers grouped by their
   ///< according CAN interface ID

   //constraints imposed by openSYDE protocol:
   static const stw_types::uint32 hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE = 32U;
   static const stw_types::uint32 hu32_MAX_NUMBER_OF_LISTS_PER_DATA_POOL = 128U;
   static const stw_types::uint32 hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST   = 2048U;

   //constraints imposed by CANopen protocol:
   static const stw_types::uint32 hu32_MAX_NUMBER_OF_ACTIVE_PDO_PER_DIRECTION   = 512U;

private:
   void m_GetAllMessages(const stw_types::uint32 ou32_InterfaceIndex,
                         std::vector<const C_OSCCanMessage *> & orc_Messages,
                         const C_OSCCanProtocol::E_Type * const ope_SkipComProtocol = NULL,
                         const stw_types::uint32 * const opu32_SkipInterfaceIndex = NULL,
                         const bool * const opq_SkipMessageIsTxFlag = NULL,
                         const stw_types::uint32 * const opu32_SkipMessageIndex = NULL) const;
   void m_AppendAllProtocolMessages(const stw_types::uint32 ou32_InterfaceIndex,
                                    const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                    std::vector<const C_OSCCanMessage *> & orc_Messages,
                                    const C_OSCCanProtocol::E_Type * const ope_SkipComProtocol = NULL,
                                    const stw_types::uint32 * const opu32_SkipInterfaceIndex = NULL,
                                    const bool * const opq_SkipMessageIsTxFlag = NULL,
                                    const stw_types::uint32 * const opu32_SkipMessageIndex = NULL) const;
   stw_types::uint32 m_GetListHash(const stw_types::uint32 ou32_DataPoolIndex,
                                   const stw_types::uint32 ou32_ListIndex) const;
   stw_types::uint32 m_GetContainerHash(const stw_types::uint32 ou32_DataPoolIndex,
                                        const stw_types::uint32 ou32_ContainerIndex) const;
   static void mh_CheckErrorCANProtocolDirection(const std::vector<C_OSCCanMessage> & orc_Messages,
                                                 const C_OSCCanProtocol::E_Type oe_ComProtocol,
                                                 bool & orq_InvalidSignalCount, bool & orq_InvalidCoPDOCount);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}

#endif
