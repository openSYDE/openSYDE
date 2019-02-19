//-----------------------------------------------------------------------------
/*!
   \file
   \brief       GUI communication driver for diagnostics (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.11.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVCOMDRIVERDIAG_H
#define C_SYVCOMDRIVERDIAG_H

/* -- Includes ------------------------------------------------------------- */
#include <QObject>
#include <set>

#include "stwtypes.h"

#include "C_OSCComDriverProtocol.h"
#include "C_SyvComDataDealer.h"
#include "C_OSCDiagProtocolKfx.h"
#include "C_SyvComPollingThreadDiag.h"
#include "C_SyvComDriverThread.h"
#include "CCAN.h"
#include "C_OSCIpDispatcherWinSock.h"
#include "C_OSCCanSignal.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvComDriverDiag :
   public QObject,
   public stw_opensyde_core::C_OSCComDriverProtocol
{
   Q_OBJECT

public:
   C_SyvComDriverDiag(const stw_types::uint32 ou32_ViewIndex);
   ~C_SyvComDriverDiag(void);

   stw_types::sint32 InitDiag(void);

   stw_types::sint32 SetDiagnosticMode(QString & orc_ErrorDetails);
   stw_types::sint32 SetUpCyclicTransmissions(QString & orc_ErrorDetails,
                                              std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_FailedIdRegisters, std::vector<QString> & orc_FailedIdErrorDetails, std::map<stw_types::uint32, stw_types::uint32> & orc_FailedNodesElementNumber, std::map<stw_types::uint32, stw_types::uint32> & orc_NodesElementNumber);
   stw_types::sint32 StopCyclicTransmissions(void);
   stw_types::sint32 StopDiagnosisServer(void);

   stw_types::sint32 StartCycling(void);
   void StopCycling(void);

   stw_types::sint32 SendTesterPresentWithoutDefectNodes(void);

   stw_types::sint32 PollDataPoolRead(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                                      const stw_types::uint16 ou16_ListIndex,
                                      const stw_types::uint16 ou16_ElementIndex);
   stw_types::sint32 PollDataPoolWrite(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                                       const stw_types::uint16 ou16_ListIndex,
                                       const stw_types::uint16 ou16_ElementIndex);
   stw_types::sint32 PollNvmRead(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                                 const stw_types::uint16 ou16_ListIndex, const stw_types::uint16 ou16_ElementIndex);
   stw_types::sint32 PollNvmWrite(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                                  const stw_types::uint16 ou16_ListIndex, const stw_types::uint16 ou16_ElementIndex);
   stw_types::sint32 PollNvmReadList(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_DataPoolIndex,
                                     const stw_types::uint16 ou16_ListIndex);
   stw_types::sint32 PollSafeNvmWriteChangedElements(const stw_types::uint32 ou32_NodeIndex,
                                                     const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds);
   stw_types::sint32 GetPollSafeNvmWriteChangedElementsOutput(
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ChangedElements) const;
   stw_types::sint32 PollSafeNvmReadValues(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 GetPollNvmSafeReadValuesOutput(const stw_opensyde_core::C_OSCNode * (&orpc_ParamNodeValues)) const;
   stw_types::sint32 PollSafeNvmSafeWriteCrcs(const stw_types::uint32 ou32_NodeIndex);
   stw_types::sint32 PollNvmNotifyOfChanges(const stw_types::uint32 ou32_NodeIndex,
                                            const stw_types::uint8 ou8_DataPoolIndex,
                                            const stw_types::uint16 ou16_ListIndex);
   stw_types::sint32 GetPollNvmNotifyOfChangesOutput(bool & orq_ApplicationAcknowledge) const;
   stw_types::sint32 GetPollResults(stw_types::sint32 & ors32_Result) const;
   stw_types::sint32 GetPollResultNRC(stw_types::uint8 & oru8_NRC) const;

   void NvmSafeClearInternalContent(void) const;
   stw_types::sint32 PollNvmSafeReadParameterValues(const stw_types::uint32 ou32_NodeIndex,
                                                    const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds);
   stw_types::sint32 NvmSafeCreateCleanFileWithoutCRC(const QString & orc_Path) const;
   stw_types::sint32 NvmSafeReadFileWithoutCRC(const QString & orc_Path) const;
   stw_types::sint32 NvmSafeCheckParameterFileContents(const stw_types::uint32 ou32_NodeIndex, const QString & orc_Path,
                                                       std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_DataPoolLists);
   stw_types::sint32 NvmSafeUpdateCRCForFile(const QString & orc_Path) const;

   const std::vector<C_SyvComDataDealer *> & GetAllDataDealer(void) const;

   void RegisterWidget(C_PuiSvDbDataElementHandler * const opc_Widget);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigPollingFinished(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC);

protected:
   virtual bool m_GetRoutingMode(stw_opensyde_core::C_OSCRoutingCalculation::E_Mode & ore_Mode) const override;
   virtual stw_types::uint8 m_GetRoutingSessionId(void) const override;
   virtual bool m_IsRoutingSpecificNecessary(const stw_opensyde_core::C_OSCNode & orc_Node) const;
   virtual stw_types::sint32 m_StartRoutingSpecific(const stw_types::uint32 ou32_ActiveNode,
                                                    const stw_opensyde_core::C_OSCNode * const opc_Node,
                                                    const stw_opensyde_core::C_OSCRoutingRoutePoint & orc_LastNodeOfRouting, stw_opensyde_core::C_OSCProtocolDriverOsy * const opc_ProtocolOsyOfLastNodeOfRouting, stw_opensyde_core::C_OSCCanDispatcherOsyRouter ** const oppc_RoutingDispatcher)
   override;

   virtual void m_StopRoutingSpecific(const stw_types::uint32 ou32_ActiveNode) override;

   virtual bool m_CheckInterfaceForFunctions(
      const stw_opensyde_core::C_OSCNodeComInterfaceSettings & orc_ComItfSettings)
   const override;

   virtual void m_HandleCanMessage(const stw_can::T_STWCAN_Msg_RX & orc_Msg, const bool oq_IsTx) override;

private:
   class C_SyvComDriverDiagWidgetRegistration
   {
   public:
      C_SyvComDriverDiagWidgetRegistration(void);
      bool operator ==(const C_SyvComDriverDiagWidgetRegistration & orc_Cmp) const;

      // Widget to inform
      C_PuiSvDbDataElementHandler * pc_Handler;
      // Information to identify CAN message
      stw_types::uint16 u16_Dlc;
      bool q_IsExtended;
      // Identification of value for widget
      C_PuiSvDbNodeDataPoolListElementId c_ElementId;
      // Signal for showing in widget
      stw_opensyde_core::C_OSCCanSignal c_Signal;
      // And the content as information about the type
      stw_opensyde_core::C_OSCNodeDataPoolContent c_ElementContent;
   };

   C_SyvComDriverThread * mpc_AsyncThread; ///< Thread for handling all async messages
   C_SyvComPollingThreadDiag mc_PollingThread;

   std::vector<stw_opensyde_core::C_OSCDiagProtocolBase *> mc_DiagProtocols; ///< Holds created
                                                                             // instances
   ///< of either openSYDE or
   ///< KEFEX protocols.
   std::vector<C_SyvComDataDealer *> mc_DataDealers; ///< Array of elements with
                                                     // as many entries as we
                                                     // have nodes.

   std::set<stw_types::uint32> mc_DiagNodes;         ///< Nodes which has used datapool elements
   std::set<stw_types::uint32> mc_DefectNodeIndices; ///< Nodes which could not be reached on start

   const stw_types::uint32 mu32_ViewIndex;
   stw_can::C_CAN * mpc_CanDllDispatcher;
   stw_opensyde_core::C_OSCIpDispatcherWinSock * mpc_EthernetDispatcher;

   // Mapping from CAN-ID to registered widget and its used CAN message signal
   QMap<stw_types::uint32, QList<C_SyvComDriverDiagWidgetRegistration> > mc_AllWidgets;

   stw_types::sint32 m_InitDiagNodes(void);
   void m_InitDiagProtocolKfx(stw_opensyde_core::C_OSCDiagProtocolKfx * const opc_DiagProtocolKefex) const;
   stw_types::sint32 m_InitDiagProtocol(void);
   stw_types::sint32 m_InitDataDealer(void);
   stw_types::sint32 m_StartRoutingDiag(QString & orc_ErrorDetails, std::set<stw_types::uint32> & orc_ErrorActiveNodes);
   stw_types::sint32 m_StartDiagServers(QString & orc_ErrorDetails);

   stw_types::sint32 m_Cycle(void);
   static void mh_ThreadFunc(void * const opv_Instance);
   void m_ThreadFunc(void);

   void m_HandlePollingFinished(void);

   static QString mh_GetNodeNameForActiveNodeIndex(const stw_types::uint32 ou32_ViewIndex,
                                                   const stw_types::uint32 ou32_ActiveNodeIndex);

   //Avoid call
   C_SyvComDriverDiag(const C_SyvComDriverDiag &);
   C_SyvComDriverDiag & operator =(const C_SyvComDriverDiag &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
