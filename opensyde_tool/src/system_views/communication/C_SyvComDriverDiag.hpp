//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       GUI communication driver for diagnostics (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVCOMDRIVERDIAG_H
#define C_SYVCOMDRIVERDIAG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include <set>

#include "stwtypes.hpp"

#include "C_OscComDriverProtocol.hpp"
#include "C_SyvComDataDealer.hpp"
#include "C_OscDiagProtocolKfx.hpp"
#include "C_SyvComPollingThreadDiag.hpp"
#include "C_SyvComDriverThread.hpp"
#include "C_Can.hpp"
#include "C_OscIpDispatcherWinSock.hpp"
#include "C_OscCanSignal.hpp"
#include "C_OscSecurityPemDatabase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvComDriverDiag :
   public QObject,
   public stw::opensyde_core::C_OscComDriverProtocol
{
   Q_OBJECT

public:
   C_SyvComDriverDiag(const uint32_t ou32_ViewIndex = 0);
   ~C_SyvComDriverDiag(void) override;

   int32_t InitDiag(void);

   int32_t SetDiagnosticMode(QString & orc_ErrorDetails);
   int32_t SetUpCyclicTransmissions(QString & orc_ErrorDetails,
                                    std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_FailedIdRegisters, std::vector<QString> & orc_FailedIdErrorDetails, std::map<uint32_t, uint32_t> & orc_FailedNodesElementNumber, std::map<uint32_t, uint32_t> & orc_NodesElementNumber);
   int32_t StopCyclicTransmissions(void);
   int32_t StopDiagnosisServer(void);

   int32_t StartCycling(void);
   void StopCycling(void);

   int32_t SendTesterPresentToActiveNodes(void);

   int32_t PollDataPoolRead(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex,
                            const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex);
   int32_t PollDataPoolWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex,
                             const uint16_t ou16_ListIndex, const uint16_t ou16_ElementIndex);
   int32_t PollNvmRead(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                       const uint16_t ou16_ElementIndex);
   int32_t PollNvmWrite(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex, const uint16_t ou16_ListIndex,
                        const uint16_t ou16_ElementIndex);
   int32_t PollNvmReadList(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex,
                           const uint16_t ou16_ListIndex);
   int32_t PollSafeNvmWriteChangedElements(const uint32_t ou32_NodeIndex,
                                           const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds);
   int32_t GetPollSafeNvmWriteChangedElementsOutput(
      std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ChangedElements) const;
   int32_t PollSafeNvmReadValues(const uint32_t ou32_NodeIndex);
   int32_t GetPollNvmSafeReadValuesOutput(const stw::opensyde_core::C_OscNode * (&orpc_ParamNodeValues)) const;
   int32_t PollSafeNvmSafeWriteCrcs(const uint32_t ou32_NodeIndex);
   int32_t PollNvmNotifyOfChanges(const uint32_t ou32_NodeIndex, const uint8_t ou8_DataPoolIndex,
                                  const uint16_t ou16_ListIndex);
   int32_t GetPollNvmNotifyOfChangesOutput(bool & orq_ApplicationAcknowledge) const;
   int32_t GetPollResults(int32_t & ors32_Result) const;
   int32_t GetPollResultNrc(uint8_t & oru8_Nrc) const;

   int32_t NvmSafeClearInternalContent(const uint32_t ou32_NodeIndex) const;
   int32_t PollNvmSafeReadParameterValues(const uint32_t ou32_NodeIndex,
                                          const std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_ListIds);
   int32_t NvmSafeCreateCleanFileWithoutCrc(const uint32_t ou32_NodeIndex, const QString & orc_Path, const stw::opensyde_core::C_OscParamSetInterpretedFileInfoData & orc_FileInfo =
                                               stw::opensyde_core::C_OscParamSetInterpretedFileInfoData())
   const;
   int32_t NvmSafeReadFileWithoutCrc(const uint32_t ou32_NodeIndex, const QString & orc_Path) const;
   int32_t NvmSafeCheckParameterFileContents(const uint32_t ou32_NodeIndex, const QString & orc_Path,
                                             std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> & orc_DataPoolLists);
   int32_t NvmSafeUpdateCrcForFile(const uint32_t ou32_NodeIndex, const QString & orc_Path) const;

   const std::vector<C_SyvComDataDealer *> & GetAllDataDealer(void) const;

   void RegisterWidget(C_PuiSvDbDataElementHandler * const opc_Widget);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigPollingFinished(const int32_t os32_Result, const uint8_t ou8_Nrc);

protected:
   bool m_GetRoutingMode(stw::opensyde_core::C_OscRoutingCalculation::E_Mode & ore_Mode) const override;
   uint8_t m_GetRoutingSessionId(void) const override;
   bool m_IsRoutingSpecificNecessary(const stw::opensyde_core::C_OscNode & orc_Node) const override;
   int32_t m_StartRoutingSpecific(const uint32_t ou32_ActiveNode, const stw::opensyde_core::C_OscNode * const opc_Node,
                                  const stw::opensyde_core::C_OscRoutingRoutePoint & orc_LastNodeOfRouting,
                                  stw::opensyde_core::C_OscProtocolDriverOsy * const opc_ProtocolOsyOfLastNodeOfRouting,
                                  stw::opensyde_core::C_OscCanDispatcherOsyRouter ** const oppc_RoutingDispatcher)
   override;

   void m_StopRoutingSpecific(const uint32_t ou32_ActiveNode) override;

   bool m_CheckInterfaceForFunctions(const stw::opensyde_core::C_OscNodeComInterfaceSettings & orc_ComItfSettings)
   const override;

   void m_HandleCanMessage(const stw::can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx) override;

private:
   class C_SyvComDriverDiagWidgetRegistration
   {
   public:
      C_SyvComDriverDiagWidgetRegistration(void);
      bool operator ==(const C_SyvComDriverDiagWidgetRegistration & orc_Cmp) const;

      // Widget to inform
      C_PuiSvDbDataElementHandler * pc_Handler;
      // Information to identify CAN message
      uint16_t u16_Dlc;
      bool q_IsExtended;
      // Identification of value for widget
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;
      // Signal for showing in widget
      stw::opensyde_core::C_OscCanSignal c_Signal;
      // In case of a multiplexed signal in c_Signal, this is the multiplexer signal which must have the matching value
      // If c_Signal is no multiplexed signal, this is not relevant
      stw::opensyde_core::C_OscCanSignal c_MultiplexerSignal;
      // And the content as information about the type
      stw::opensyde_core::C_OscNodeDataPoolContent c_ElementContent;
   };

   C_SyvComDriverThread * mpc_AsyncThread; ///< Thread for handling all async messages
   C_SyvComPollingThreadDiag mc_PollingThread;

   std::vector<stw::opensyde_core::C_OscDiagProtocolBase *> mc_DiagProtocols; ///< Holds created
                                                                              // instances
   ///< of either openSYDE or
   ///< KEFEX protocols.
   std::vector<C_SyvComDataDealer *> mc_DataDealers; ///< Array of elements with
                                                     // as many entries as we
                                                     // have nodes.

   std::vector<uint32_t> mc_ActiveDiagNodes; ///< Nodes which has diagnostic active. The other active
   // nodes are active for routing but not for diagnostic.
   // It has the indexes of the mc_ActiveNodesIndexes
   std::vector<uint32_t> mc_ActiveCommunicatingNodes; ///< All nodes which are communicating for dashboard or
   // part of at least one route
   std::set<uint32_t> mc_DiagNodesWithElements; ///< Nodes which has used datapool elements
   std::set<uint32_t> mc_DefectNodeIndices;     ///< Nodes which could not be reached on start
   // Read metadata of all active nodes and its Datapools. First layer are the active
   // nodes, second layer are the Datapools
   std::vector<std::list<stw::opensyde_core::C_OscProtocolDriverOsy::C_DataPoolMetaData> > mc_ReadDatapoolMetadata;

   const uint32_t mu32_ViewIndex;
   stw::can::C_Can * mpc_CanDllDispatcher;
   stw::opensyde_core::C_OscIpDispatcherWinSock * mpc_EthernetDispatcher;

   // Mapping from CAN-ID to registered widget and its used CAN message signal
   QMap<stw::opensyde_core::C_OscCanMessageUniqueId, QList<C_SyvComDriverDiagWidgetRegistration> > mc_AllWidgets;

   // Security PEM database
   stw::opensyde_core::C_OscSecurityPemDatabase mc_PemDatabase;

   int32_t m_InitDiagNodes(void);
   void m_InitDiagProtocolKfx(stw::opensyde_core::C_OscDiagProtocolKfx * const opc_DiagProtocolKefex) const;
   int32_t m_InitDiagProtocol(void);
   int32_t m_InitDataDealer(void);
   int32_t m_StartRoutingDiag(QString & orc_ErrorDetails, std::set<uint32_t> & orc_ErrorActiveNodes);
   int32_t m_StartDiagServers(QString & orc_ErrorDetails);
   int32_t m_GetAllDatapoolMetadata(const uint32_t ou32_ActiveDiagNodeIndex, QString & orc_ErrorDetails);
   int32_t m_CheckOsyDatapoolsAndCreateMapping(const uint32_t ou32_ActiveDiagNodeIndex, QString & orc_ErrorDetails);
   int32_t m_GetReadDatapoolMetadata(const uint32_t ou32_ActiveDiagNodeIndex,
                                     const stw::scl::C_SclString & orc_DatapoolName,
                                     uint32_t & oru32_ServerDatapoolIndex,
                                     stw::opensyde_core::C_OscProtocolDriverOsy::C_DataPoolMetaData & orc_Metadata)
   const;
   uint32_t m_GetActiveDiagIndex(const uint32_t ou32_NodeIndex, bool * const opq_Found = NULL) const;

   int32_t m_Cycle(void);
   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   void m_HandlePollingFinished(void);

   void m_GetRoutingErrorDetails(QString & orc_ErrorDetails, std::set<uint32_t> & orc_ErrorActiveNodes,
                                 const uint32_t ou32_ActiveNode, const uint32_t ou32_ErrorActiveNodeIndex) const;

   //Avoid call
   C_SyvComDriverDiag(const C_SyvComDriverDiag &);
   C_SyvComDriverDiag & operator =(const C_SyvComDriverDiag &) &; //lint !e1511 //we want to hide the base func.
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
