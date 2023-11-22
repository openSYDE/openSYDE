//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for parameterization widget. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAWIDGETNEW_HPP
#define C_SYVDAITPAWIDGETNEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"

#include "C_SyvComDriverDiag.hpp"
#include "C_OscParamSetInterpretedNode.hpp"
#include "C_PuiSvDbParam.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaItPaWidgetNew;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItPaWidgetNew :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItPaWidgetNew(const uint32_t & oru32_ViewIndex,
                                 stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget,
                                 QWidget * const opc_Parent = NULL);
   ~C_SyvDaItPaWidgetNew() override;

   void InitStaticNames(void) const;
   void ConnectionActiveChanged(const bool oq_Active);
   void SetSyvComDriver(stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver);
   void UpdateEcuValues(void) const;
   bool HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc);
   void SetDark(const bool oq_Value) const;

   void ButtonAddClicked(void);

   QString GetSelectedItemTypeTemplate(void) const;
   uint32_t GetSelectedItemCount(void) const;
   std::vector<int32_t> GetCurrentColumnWidths(void) const;
   std::vector<int32_t> GetCurrentColumnPositionIndices(void) const;
   void SetColumnWidth(const std::vector<int32_t> & orc_NewColWidths) const;
   void SetColumnPositionIndices(const std::vector<int32_t> & orc_NewColPositionIndices) const;
   std::vector<stw::opensyde_gui_logic::C_PuiSvDbExpandedTreeIndex> GetAllExpandedTreeItems(void) const;
   void SetAllExpandedTreeItems(const std::vector<stw::opensyde_gui_logic::C_PuiSvDbExpandedTreeIndex> & orc_Items)
   const;

   void SetEditMode(const bool oq_EditMode, const bool oq_EditContentMode);
   void HideToolTip(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigImport(void);
   void SigNvmReadList(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_Id);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   //Avoid call
   C_SyvDaItPaWidgetNew(const C_SyvDaItPaWidgetNew &);
   C_SyvDaItPaWidgetNew & operator =(const C_SyvDaItPaWidgetNew &) &;

   void m_ReadElements(void);
   void m_WriteElements(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ChangedIds);
   void m_LoadElements(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ElementIds,
                       const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                       const uint32_t ou32_ValidLayers) const;
   void m_SaveElements(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds,
                       const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                       const uint32_t ou32_ValidLayers) const;
   void m_RecordElements(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds);

   int32_t m_GetDataFromImport(const std::vector<stw::opensyde_core::C_OscParamSetInterpretedNode> & orc_NewData,
                               std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_FoundIds,
                               std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> & orc_FoundContent);

   void m_UpdateButtons(void);
   void m_UpdateButtonToolTips(void) const;

   void m_HandleTreeInitAction(void);

   void m_HandleTreeReadAction(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds);
   void m_HandleTreeWriteAction(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds);

   void m_HandleWriteProcessTrigger(
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds);
   void m_HandleRemoveTrigger(const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListIds);

   void m_UpdateReadAllowedFlag(const bool oq_NewValue);
   static QString mh_GetDefaultFileName(const uint32_t ou32_ViewIndex,
                                        const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                        const uint32_t ou32_ValidLayers);
   static QString mh_GetFile(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                             const uint32_t ou32_Value);

   void m_InformUserFloatRangeCheck(
      const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_InvalidValueIds,
      const std::vector<QString> & orc_InvalidValues, const std::vector<QString> & orc_NewValues) const;

   Ui::C_SyvDaItPaWidgetNew * mpc_Ui;
   const uint32_t mu32_ViewIndex;
   stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const mpc_DataWidget;
   stw::opensyde_gui_logic::C_SyvComDriverDiag * mpc_ComDriver;
   bool mq_IsConnected;

   // Copy for thread operations
   bool mq_ReadActive;
   bool mq_WriteActive;
   bool mq_Editable;
   bool mq_ReadAllowed;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_ListsWithCrcError;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_ListIds;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_ElementsToWriteAfterRead;
   uint32_t mu32_ListCounter;

   static const QString mhc_FILE_EXTENSION_PARAMSET;
   static const QString mhc_FILE_EXTENSION_PARAMSETIMAGE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
