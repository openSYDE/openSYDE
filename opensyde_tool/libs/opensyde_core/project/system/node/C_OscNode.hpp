//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store node

   Store node (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODE_HPP
#define C_OSCNODE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscNodeProperties.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_OscNodeApplication.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscHalcConfig.hpp"
#include "C_OscCanMessageUniqueId.hpp"
#include "C_OscCanOpenManagerInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//----------------------------------------------------------------------------------------------------------------------
///container for all elements that describe one node
class C_OscNode
{
public:
   C_OscNode(void);
   virtual ~C_OscNode(void);

   //lint -sem(stw::opensyde_core::C_OscNode::Initialize,initializer)
   void Initialize(void);

   int32_t InsertDataPool(const uint32_t ou32_DataPoolIndex, const C_OscNodeDataPool & orc_DataPool);
   int32_t DeleteDataPool(const uint32_t ou32_DataPoolIndex);
   int32_t MoveDataPool(const uint32_t ou32_Start, const uint32_t ou32_Target);
   int32_t InsertMessage(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                         const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx, const uint32_t ou32_MessageIndex,
                         const C_OscCanMessage & orc_Message,
                         const std::vector<C_OscNodeDataPoolListElement> & orc_SignalData);
   int32_t SetMessage(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                      const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx, const uint32_t ou32_MessageIndex,
                      const C_OscCanMessage & orc_Message, const bool oq_NewMessageIsTx,
                      const std::vector<C_OscNodeDataPoolListElement> & orc_SignalData);
   int32_t DeleteMessage(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                         const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                         const uint32_t ou32_MessageIndex);
   int32_t InsertSignal(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                        const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx, const uint32_t ou32_MessageIndex,
                        const uint32_t ou32_SignalIndex, const C_OscCanSignal & orc_Signal,
                        const C_OscNodeDataPoolListElement & orc_SignalData);
   int32_t SetSignal(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                     const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx, const uint32_t ou32_MessageIndex,
                     const uint32_t ou32_SignalIndex, const C_OscCanSignal & orc_Signal,
                     const C_OscNodeDataPoolListElement & orc_SignalData);
   int32_t SetSignalPosition(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                             const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                             const uint32_t ou32_MessageIndex, const uint32_t ou32_SignalIndex,
                             const C_OscCanSignal & orc_Signal);
   int32_t SetSignalMuxValue(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                             const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx,
                             const uint32_t ou32_MessageIndex, const uint32_t ou32_SignalIndex,
                             const uint16_t ou16_MultiplexValue);
   int32_t DeleteSignal(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_InterfaceIndex,
                        const uint32_t ou32_DatapoolIndex, const bool oq_MessageIsTx, const uint32_t ou32_MessageIndex,
                        const uint32_t ou32_SignalIndex);

   void CalcHash(uint32_t & oru32_HashValue) const;

   std::vector<C_OscNodeApplication> GetHexApplications(void) const;
   int32_t GetDataPoolIndex(const C_OscNodeDataPool::E_Type oe_DataPoolType,
                            const uint32_t ou32_DataPoolTypeIndex) const;
   int32_t GetDataPoolTypeIndex(const uint32_t ou32_DataPoolIndex) const;
   uint32_t GetDataPoolCount(const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DataPoolType) const;

   static bool h_CompareNameGreater(const C_OscNode & orc_Node1, const C_OscNode & orc_Node2);
   uint32_t GetListsSize(void) const;
   uint32_t GetDataPoolsSize(void) const;
   uint32_t GetDataPoolAbsoluteAddress(const uint32_t ou32_DataPoolIndex) const;
   uint32_t GetListAbsoluteAddress(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex) const;
   uint32_t GetElementAbsoluteAddress(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex,
                                      const uint32_t ou32_ElementIndex) const;
   const C_OscNodeDataPool * GetHalDataPoolConst(const bool oq_SafeDatapool) const;
   const C_OscNodeDataPool * GetComDataPoolConst(const C_OscCanProtocol::E_Type oe_ComProtocol,
                                                 const uint32_t ou32_DataPoolIndex) const;
   C_OscNodeDataPool * GetComDataPool(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_DataPoolIndex);
   std::vector<const C_OscNodeDataPool *> GetComDatapoolsConst(const C_OscCanProtocol::E_Type oe_ComProtocol) const;
   std::vector<C_OscNodeDataPool *> GetComDataPools(const C_OscCanProtocol::E_Type oe_ComProtocol);
   const C_OscCanProtocol * GetCanProtocolConst(const C_OscCanProtocol::E_Type oe_ComProtocol,
                                                const uint32_t ou32_DataPoolIndex) const;
   C_OscCanProtocol * GetCanProtocol(const C_OscCanProtocol::E_Type oe_ComProtocol, const uint32_t ou32_DataPoolIndex);
   std::vector<const C_OscCanProtocol *> GetCanProtocolsConst(const C_OscCanProtocol::E_Type oe_ComProtocol) const;
   std::vector<C_OscCanProtocol *> GetCanProtocols(const C_OscCanProtocol::E_Type oe_ComProtocol);
   const C_OscCanProtocol * GetRelatedCanProtocolConst(const uint32_t ou32_DataPoolIndex) const;
   C_OscCanProtocol * GetRelatedCanProtocol(const uint32_t ou32_DataPoolIndex);
   void CheckErrorCanProtocol(const C_OscCanProtocol::E_Type oe_ComProtocol, const bool oq_ComProtocolUsedByInterface,
                              bool & orq_InvalidMaxRxSignalCount, bool & orq_InvalidMaxTxSignalCount,
                              bool & orq_InvalidCoRxPdoCount, bool & orq_InvalidCoTxPdoCount,
                              bool & orq_InvalidMinSignalCount) const;
   void CheckErrorCanProtocol(const uint32_t ou32_InterfaceIndex, const C_OscCanProtocol::E_Type oe_ComProtocol,
                              const bool oq_ComProtocolUsedByInterface, bool & orq_InvalidMaxRxSignalCount,
                              bool & orq_InvalidMaxTxSignalCount, bool & orq_InvalidCoRxPdoCount,
                              bool & orq_InvalidCoTxPdoCount, bool & orq_InvalidMinSignalCount) const;

   void CheckErrorDataPoolNumListsAndElements(const uint32_t ou32_DataPoolIndex, bool & orq_TooFewListsOrElements,
                                              bool & orq_TooManyListsOrElements) const;
   void CheckErrorDataPool(const uint32_t ou32_DataPoolIndex, bool * const opq_NameConflict,
                           bool * const opq_NameInvalid, bool * const opq_IsErrorInListOrMessage,
                           bool * const opq_TooFewListsOrElements, bool * const opq_TooManyListsOrElements,
                           std::vector<uint32_t> * const opc_InvalidListIndices) const;
   void CheckMessageId(const uint32_t ou32_InterfaceIndex, const C_OscCanMessageUniqueId & orc_MessageId,
                       bool & orq_Valid, const C_OscCanProtocol::E_Type * const ope_SkipComProtocol = NULL,
                       const uint32_t * const opu32_SkipInterfaceIndex = NULL,
                       const bool * const opq_SkipMessageIsTxFlag = NULL,
                       const uint32_t * const opu32_SkipMessageIndex = NULL) const;
   void CheckMessageName(const uint32_t ou32_InterfaceIndex, const stw::scl::C_SclString & orc_MessageName,
                         bool & orq_Valid, const C_OscCanProtocol::E_Type * const ope_SkipComProtocol = NULL,
                         const uint32_t * const opu32_SkipInterfaceIndex = NULL,
                         const bool * const opq_SkipMessageIsTxFlag = NULL,
                         const uint32_t * const opu32_SkipMessageIndex = NULL) const;
   int32_t CheckApplicationProcessIdValid(const uint32_t ou32_ApplicationIndex, bool & orq_Valid) const;
   void CheckHalcConfigValid(bool * const opq_ConfigInvalid,
                             std::vector<uint32_t> * const opc_InvalidDomainIndices) const;
   void ReCalcCanProtocolDataPoolIndices(void);
   bool IsAnyUpdateAvailable(void) const;
   bool IsRoutingAvailable(const C_OscSystemBus::E_Type oe_Type) const;
   void RecalculateAddress(void);
   uint32_t CountAllLocalMessages(void) const;

   C_OscNodeDataPoolListElement * GetDataPoolListElement(const uint32_t ou32_DataPoolIndex,
                                                         const uint32_t ou32_ListIndex,
                                                         const uint32_t ou32_ElementIndex);

   const C_OscDeviceDefinition * pc_DeviceDefinition; ///< Pointer to device definition
   uint32_t u32_SubDeviceIndex;
   stw::scl::C_SclString c_DeviceType; ///< Node type: for non multi-cpu devices (C_OscDeviceDefinition::c_DeviceName =
   ///< C_OscNode::c_DeviceType) for multi-cpu devices (C_OscSubDeviceDefinition::c_DeviceName =
   ///< C_OscNode::c_DeviceType).
   ///< Simply put: When the device type of multi-cpu device is needed:
   ///< "C_OscNode::pc_DeviceDefinition->c_DeviceName" will do the trick.
   C_OscNodeProperties c_Properties;           ///< General node properties
   std::vector<C_OscNodeDataPool> c_DataPools; ///< All datapools assigned to this node, expected type order:
   ///< DIAG, NVM then COM
   bool q_DatapoolAutoNvmStartAddress; ///< Flag if the Datapool NvM start will be calculate automatically

   std::vector<C_OscNodeApplication> c_Applications; ///< All data blocks assigned to this node
   std::vector<C_OscCanProtocol> c_ComProtocols;     ///< All node specific information
   ///< for related communication protocol.
   ///< Created if necessary.
   ///< Maximum size equal to number of possible com protocol types.
   C_OscHalcConfig c_HalcConfig;                                 ///< Optional HALC configuration for this node
   std::map<uint8_t, C_OscCanOpenManagerInfo> c_CanOpenManagers; ///< CANopen managers grouped by their
   ///< according CAN interface ID

   //constraints imposed by openSYDE protocol:
   static const uint32_t hu32_MAX_NUMBER_OF_DATA_POOLS_PER_NODE = 32U;
   static const uint32_t hu32_MAX_NUMBER_OF_LISTS_PER_DATA_POOL = 128U;
   static const uint32_t hu32_MAX_NUMBER_OF_ELEMENTS_PER_LIST   = 2048U;

   //constraints imposed by CANopen protocol:
   static const uint32_t hu32_MAX_NUMBER_OF_ACTIVE_PDO_PER_DIRECTION   = 512U;

private:
   void m_GetAllMessages(const uint32_t ou32_InterfaceIndex, std::vector<const C_OscCanMessage *> & orc_Messages,
                         const C_OscCanProtocol::E_Type * const ope_SkipComProtocol = NULL,
                         const uint32_t * const opu32_SkipInterfaceIndex = NULL,
                         const bool * const opq_SkipMessageIsTxFlag = NULL,
                         const uint32_t * const opu32_SkipMessageIndex = NULL) const;
   void m_AppendAllProtocolMessages(const uint32_t ou32_InterfaceIndex, const C_OscCanProtocol::E_Type oe_ComProtocol,
                                    std::vector<const C_OscCanMessage *> & orc_Messages,
                                    const C_OscCanProtocol::E_Type * const ope_SkipComProtocol = NULL,
                                    const uint32_t * const opu32_SkipInterfaceIndex = NULL,
                                    const bool * const opq_SkipMessageIsTxFlag = NULL,
                                    const uint32_t * const opu32_SkipMessageIndex = NULL) const;
   uint32_t m_GetListHash(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex) const;
   uint32_t m_GetContainerHash(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ContainerIndex) const;
   static void mh_CheckErrorCanProtocolDirection(const std::vector<C_OscCanMessage> & orc_Messages,
                                                 const C_OscCanProtocol::E_Type oe_ComProtocol,
                                                 bool & orq_InvalidMaxSignalCount, bool & orq_InvalidCoPdoCount,
                                                 bool & orq_InvalidMinSignalCount);
   bool m_CheckErrorTooFewElements(const uint32_t ou32_DataPoolIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
