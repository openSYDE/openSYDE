//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for parameterization widget. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAWIDGETNEW_H
#define C_SYVDAITPAWIDGETNEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"

#include "C_SyvComDriverDiag.h"
#include "C_OSCParamSetInterpretedNode.h"
#include "C_PuiSvDbParam.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaItPaWidgetNew;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItPaWidgetNew :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItPaWidgetNew(const stw_types::uint32 & oru32_ViewIndex,
                                 stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget,
                                 QWidget * const opc_Parent = NULL);
   virtual ~C_SyvDaItPaWidgetNew();

   void InitStaticNames(void) const;
   void ConnectionActiveChanged(const bool oq_Active);
   void SetSyvComDriver(stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver);
   void UpdateECUValues(void) const;
   bool HandleManualOperationFinished(const stw_types::sint32 os32_Result, const stw_types::uint8 ou8_NRC);
   void SetDark(const bool oq_Value) const;

   void ButtonAddClicked(void);

   QString GetSelectedItemTypeTemplate(void) const;
   stw_types::uint32 GetSelectedItemCount(void) const;
   std::vector<stw_types::sint32> GetCurrentColumnWidths(void) const;
   std::vector<stw_types::sint32> GetCurrentColumnPositionIndices(void) const;
   void SetColumnWidth(const std::vector<stw_types::sint32> & orc_NewColWidths) const;
   void SetColumnPositionIndices(const std::vector<stw_types::sint32> & orc_NewColPositionIndices) const;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbExpandedTreeIndex> GetAllExpandedTreeItems(void) const;
   void SetAllExpandedTreeItems(
      const std::vector<stw_opensyde_gui_logic::C_PuiSvDbExpandedTreeIndex> & orc_Items) const;

   void SetEditModeActive(const bool oq_Active);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigImport(void);
   void SigNvmReadList(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_Id);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaItPaWidgetNew(const C_SyvDaItPaWidgetNew &);
   C_SyvDaItPaWidgetNew & operator =(const C_SyvDaItPaWidgetNew &);

   void m_ReadElements(void);
   void m_WriteElements(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ChangedIds);
   void m_LoadElements(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ElementIds,
                       const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                       const stw_types::uint32 ou32_ValidLayers) const;
   void m_SaveElements(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds,
                       const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                       const stw_types::uint32 ou32_ValidLayers) const;
   void m_RecordElements(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);

   stw_types::sint32 m_GetDataFromImport(
      const std::vector<stw_opensyde_core::C_OSCParamSetInterpretedNode> & orc_NewData,
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_FoundIds,
      std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_FoundContent);

   void m_UpdateButtons(void);
   void m_UpdateButtonToolTips(void) const;

   void m_HandleTreeInitAction(void);

   void m_HandleTreeReadAction(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);
   void m_HandleTreeWriteAction(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);

   void m_HandleWriteProcessTrigger(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);
   void m_HandleRemoveTrigger(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);

   void m_UpdateReadAllowedFlag(const bool oq_NewValue);
   static QString mh_GetDefaultFileName(const stw_types::uint32 ou32_ViewIndex,
                                        const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                        const stw_types::uint32 ou32_ValidLayers);
   static QString mh_GetFile(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                             const stw_types::uint32 ou32_Value);

   void m_InformUserFloatRangeCheck(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_InvalidValueIds,
      const std::vector<QString> & orc_InvalidValues, const std::vector<QString> & orc_NewValues) const;

   Ui::C_SyvDaItPaWidgetNew * mpc_Ui;
   const stw_types::uint32 mu32_ViewIndex;
   stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const mpc_DataWidget;
   stw_opensyde_gui_logic::C_SyvComDriverDiag * mpc_ComDriver;
   bool mq_IsConnected;

   // Copy for thread operations
   bool mq_ReadActive;
   bool mq_WriteActive;
   bool mq_Editable;
   bool mq_ReadAllowed;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_ListsWithCRCError;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_ListIds;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_ElementsToWriteAfterRead;
   stw_types::uint32 mu32_ListCounter;

   static const QString mhc_FILE_EXTENSION_PARAMSET;
   static const QString mhc_FILE_EXTENSION_PARAMSETIMAGE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
