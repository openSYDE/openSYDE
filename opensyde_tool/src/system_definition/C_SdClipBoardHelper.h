//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCLIPBOARDHELPER_H
#define C_SDCLIPBOARDHELPER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <QString>
#include <QModelIndexList>
#include "stwtypes.h"
#include "C_OSCNodeDataPool.h"
#include "C_PuiSdNodeDataPool.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCCanMessage.h"
#include "C_PuiSdNodeCanSignal.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_SdTopologyDataSnapshot.h"
#include "C_UtiClipBoardHelper.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreDataPool(const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCContent,
                               const stw_opensyde_gui_logic::C_PuiSdNodeDataPool & orc_UIContent);
   static stw_types::sint32 h_LoadToDataPool(stw_opensyde_core::C_OSCNodeDataPool & orc_OSCContent,
                                             stw_opensyde_gui_logic::C_PuiSdNodeDataPool & orc_UIContent);
   static void h_StoreDataPoolLists(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolList> & orc_OSCContent,
                                    const std::vector<C_PuiSdNodeDataPoolList> & orc_UIContent,
                                    const stw_opensyde_core::C_OSCNodeDataPool::E_Type & ore_Type);
   static stw_types::sint32 h_LoadToDataPoolLists(
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolList> & orc_OSCContent,
      std::vector<C_PuiSdNodeDataPoolList> & orc_UIContent, stw_opensyde_core::C_OSCNodeDataPool::E_Type & ore_Type);
   static void h_StoreDataPoolListElementsToClipBoard(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCContent,
      const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent);
   static stw_types::sint32 h_LoadToDataPoolListElementsFromClipBoard(
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCContent,
      std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent);
   static void h_StoreDataPoolListElementsToString(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCContent,
      const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent, QString & orc_Output);
   static stw_types::sint32 h_LoadToDataPoolListElementsFromString(
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCContent,
      std::vector<C_PuiSdNodeDataPoolListElement> & orc_UIContent, const QString & orc_Input);
   static void h_StoreIndicesToString(const std::vector<stw_types::uint32> & orc_Indices, QString & orc_Output);
   static stw_types::sint32 h_LoadIndicesFromString(std::vector<stw_types::uint32> & orc_Indices,
                                                    const QString & orc_Input);
   static void h_StoreDataPoolListDataSetsToClipBoard(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> & orc_OSCNames,
      const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues);
   static stw_types::sint32 h_LoadToDataPoolListDataSetsFromClipBoard(
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> & orc_OSCNames,
      std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues);
   static void h_StoreDataPoolListDataSetsToString(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> & orc_OSCNames,
      const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues,
      QString & orc_Output);
   static stw_types::sint32 h_LoadToDataPoolListDataSetsFromString(
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolDataSet> & orc_OSCNames,
      std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> > & orc_OSCDataSetValues,
      const QString & orc_Input);
   static void h_StoreMessages(const std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_Messages, const std::
                               vector<std::vector<stw_opensyde_core::
                                                  C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons,
                               const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons,
                               const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals,
                               const std::vector<std::vector<QString> > & orc_OwnerNodeName,
                               const std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeInterfaceIndex,
                               const std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeDatapoolIndex,
                               const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag);
   static stw_types::sint32 h_LoadMessages(std::vector<stw_opensyde_core::C_OSCCanMessage> & orc_Messages, std::
                                           vector<std::vector<stw_opensyde_core::
                                                              C_OSCNodeDataPoolListElement> > & orc_OSCSignalCommons, std::
                                           vector<std::vector<
                                                     C_PuiSdNodeDataPoolListElement> > & orc_UISignalCommons,
                                           std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UISignals,
                                           std::vector<std::vector<QString> > & orc_OwnerNodeName,
                                           std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeInterfaceIndex,
                                           std::vector<std::vector<stw_types::uint32> > & orc_OwnerNodeDatapoolIndex,
                                           std::vector<std::vector<bool> > & orc_OwnerIsTxFlag);
   static void h_StoreSignalsToClipboard(const std::vector<stw_opensyde_core::C_OSCCanSignal> & orc_Signals,
                                         const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals);
   static stw_types::sint32 h_LoadSignalsFromClipboard(std::vector<stw_opensyde_core::C_OSCCanSignal> & orc_Signals,
                                                       std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, std::vector<C_PuiSdNodeCanSignal> & orc_UISignals);
   static void h_StoreMessageIndexToString(
      const std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageIds,
      QString & orc_Output);
   static stw_types::sint32 h_LoadMessageIndexFromString(const QString & orc_Input,
                                                         std::vector<stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & orc_MessageIds);
   static void h_StoreDataSnapShotToClipboard(const C_SdTopologyDataSnapshot & orc_Data);
   static stw_types::sint32 h_LoadDataSnapShotFromClipboard(C_SdTopologyDataSnapshot & orc_Data);

private:
   C_SdClipBoardHelper(void);

   static void mh_StoreSignalsToString(const std::vector<stw_opensyde_core::C_OSCCanSignal> & orc_Signals,
                                       const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UISignals, QString & orc_Output);
   static stw_types::sint32 mh_LoadSignalsFromString(const QString & orc_Input,
                                                     std::vector<stw_opensyde_core::C_OSCCanSignal> & orc_Signals,
                                                     std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElement> & orc_OSCSignalCommons, std::vector<C_PuiSdNodeDataPoolListElement> & orc_UISignalCommons, std::vector<C_PuiSdNodeCanSignal> & orc_UISignals);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
