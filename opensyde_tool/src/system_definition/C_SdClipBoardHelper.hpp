//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDCLIPBOARDHELPER_HPP
#define C_SDCLIPBOARDHELPER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <vector>
#include <QString>
#include <QModelIndexList>
#include "stwtypes.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_PuiSdNodeDataPool.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_OscCanMessage.hpp"
#include "C_PuiSdNodeCanSignal.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_SdTopologyDataSnapshot.hpp"
#include "C_OscHalcConfigStandalone.hpp"
#include "C_UtiClipBoardHelper.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreDataPool(const stw::opensyde_core::C_OscNodeDataPool & orc_OscContent,
                               const stw::opensyde_gui_logic::C_PuiSdNodeDataPool & orc_UiContent);
   static int32_t h_LoadToDataPool(stw::opensyde_core::C_OscNodeDataPool & orc_OscContent,
                                   stw::opensyde_gui_logic::C_PuiSdNodeDataPool & orc_UiContent);
   static void h_StoreDataPoolLists(const std::vector<stw::opensyde_core::C_OscNodeDataPoolList> & orc_OscContent,
                                    const std::vector<C_PuiSdNodeDataPoolList> & orc_UiContent,
                                    const stw::opensyde_core::C_OscNodeDataPool::E_Type & ore_Type);
   static int32_t h_LoadToDataPoolLists(std::vector<stw::opensyde_core::C_OscNodeDataPoolList> & orc_OscContent,
                                        std::vector<C_PuiSdNodeDataPoolList> & orc_UiContent,
                                        stw::opensyde_core::C_OscNodeDataPool::E_Type & ore_Type);
   static void h_StoreDataPoolListElementsToClipBoard(
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscContent,
      const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent,
      const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DatapoolType);
   static int32_t h_LoadToDataPoolListElementsFromClipBoard(
      std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscContent,
      std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent);
   static void h_StoreDataPoolListElementsToString(
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscContent,
      const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent,
      const stw::opensyde_core::C_OscNodeDataPool::E_Type oe_DatapoolType, QString & orc_Output);
   static int32_t h_LoadToDataPoolListElementsFromString(
      std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscContent,
      std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiContent, const QString & orc_Input);
   static void h_StoreIndicesToString(const std::vector<uint32_t> & orc_Indices, QString & orc_Output);
   static int32_t h_LoadIndicesFromString(std::vector<uint32_t> & orc_Indices, const QString & orc_Input);
   static void h_StoreDataPoolListDataSetsToClipBoard(
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
      const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > & orc_OscDataSetValues);
   static int32_t h_LoadToDataPoolListDataSetsFromClipBoard(
      std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
      std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > & orc_OscDataSetValues);
   static void h_StoreDataPoolListDataSetsToString(
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
      const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > & orc_OscDataSetValues,
      QString & orc_Output);
   static int32_t h_LoadToDataPoolListDataSetsFromString(
      std::vector<stw::opensyde_core::C_OscNodeDataPoolDataSet> & orc_OscNames,
      std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> > & orc_OscDataSetValues,
      const QString & orc_Input);
   static void h_StoreMessages(const std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages, const std::
                               vector<std::vector<stw::opensyde_core::
                                                  C_OscNodeDataPoolListElement> > & orc_OscSignalCommons,
                               const std::vector<std::vector<C_PuiSdNodeDataPoolListElement> > & orc_UiSignalCommons,
                               const std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UiSignals,
                               const std::vector<std::vector<QString> > & orc_OwnerNodeName,
                               const std::vector<std::vector<uint32_t> > & orc_OwnerNodeInterfaceIndex,
                               const std::vector<std::vector<uint32_t> > & orc_OwnerNodeDatapoolIndex,
                               const std::vector<std::vector<bool> > & orc_OwnerIsTxFlag,
                               const opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType);
   static int32_t h_LoadMessages(std::vector<stw::opensyde_core::C_OscCanMessage> & orc_Messages, std::
                                 vector<std::vector<stw::opensyde_core::
                                                    C_OscNodeDataPoolListElement> > & orc_OscSignalCommons, std::
                                 vector<std::vector<
                                           C_PuiSdNodeDataPoolListElement> > & orc_UiSignalCommons,
                                 std::vector<std::vector<C_PuiSdNodeCanSignal> > & orc_UiSignals,
                                 std::vector<std::vector<QString> > & orc_OwnerNodeName,
                                 std::vector<std::vector<uint32_t> > & orc_OwnerNodeInterfaceIndex,
                                 std::vector<std::vector<uint32_t> > & orc_OwnerNodeDatapoolIndex,
                                 std::vector<std::vector<bool> > & orc_OwnerIsTxFlag);
   static void h_StoreSignalsToClipboard(const std::vector<stw::opensyde_core::C_OscCanSignal> & orc_Signals,
                                         const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals, const opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType);
   static int32_t h_LoadSignalsFromClipboard(std::vector<stw::opensyde_core::C_OscCanSignal> & orc_Signals,
                                             std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommons, std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons, std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals);
   static void h_StoreMessageIndexToString(
      const std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageIds,
      QString & orc_Output);
   static int32_t h_LoadMessageIndexFromString(const QString & orc_Input,
                                               std::vector<stw::opensyde_core::C_OscCanMessageIdentificationIndices> & orc_MessageIds);
   static void h_StoreDataSnapShotToClipboard(const C_SdTopologyDataSnapshot & orc_Data);
   static int32_t h_LoadDataSnapShotFromClipboard(C_SdTopologyDataSnapshot & orc_Data);

   static void h_StoreHalcItemConfigToClipboard(const stw::opensyde_core::C_OscHalcConfigStandalone & orc_Data);
   static int32_t h_LoadHalcItemConfigFromClipboard(stw::opensyde_core::C_OscHalcConfigStandalone & orc_Data);

private:
   C_SdClipBoardHelper(void);

   static void mh_StoreSignalsToString(const std::vector<stw::opensyde_core::C_OscCanSignal> & orc_Signals,
                                       const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommons, const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons, const std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals, const opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType, QString & orc_Output);
   static int32_t mh_LoadSignalsFromString(const QString & orc_Input,
                                           std::vector<stw::opensyde_core::C_OscCanSignal> & orc_Signals,
                                           std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalCommons, std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalCommons, std::vector<C_PuiSdNodeCanSignal> & orc_UiSignals);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
