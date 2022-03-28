//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter tree view (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPATREEVIEW_H
#define C_SYVDAITPATREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PuiSvDbParam.h"
#include "C_SyvDaItPaTreeModel.h"
#include "C_SyvDaItPaTreeDelegate.h"
#include "C_OgeTreeViewToolTipBase.h"
#include "C_PuiSvDbDataElementHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaTreeView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_SyvDaItPaTreeView(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItPaTreeView(void);

   bool IsEmpty(void) const;
   void ClearECUValues(void);
   void ReloadSetValues(void);
   void ApplySelectedEcuValues(void);
   bool CheckRange(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds,
                   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> & orc_ListIds2) const;
   bool CheckAllListsRead(void) const;
   void PrepareChangedValues(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds) const;
   void RemoveValuesChangedFlag(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds)
   const;
   void SetDark(const bool oq_Value);
   void SetEditMode(const bool oq_EditMode);
   void SetConnected(const bool oq_Connected);
   void SetLoadSaveActive(const bool oq_Active);
   void SetActionActive(const bool oq_Active);
   QString GetSelectedItemTypeTemplate(void) const;
   stw_types::uint32 GetSelectedItemCount(void) const;
   void Init(stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget);
   void UpdateECUValues(void);
   std::vector<stw_types::sint32> GetCurrentColumnWidths(void) const;
   std::vector<stw_types::sint32> GetCurrentColumnPositionIndices(void) const;
   void DeleteSpecified(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);
   void SetColumnPositionIndices(const std::vector<stw_types::sint32> & orc_NewColPositionIndices);
   void SetColumnWidth(const std::vector<stw_types::sint32> & orc_NewColWidths);
   bool CheckListsRead(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds) const;
   std::vector<stw_opensyde_gui_logic::C_PuiSvDbExpandedTreeIndex> GetAllExpandedTreeItems(void) const;
   void SetAllExpandedTreeItems(const std::vector<stw_opensyde_gui_logic::C_PuiSvDbExpandedTreeIndex> & orc_Items);
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> GetAllListIds(void) const;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> GetChangedListElementIds(void) const;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> GetInvalidListIds(void) const;
   void SetCRCStatus(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_ListId, const bool oq_Status);
   void GetListSetValues(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_ListId,
                         std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_ListValues) const;
   void HideToolTip(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigActionRead(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);
   void SigActionWrite(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);
   void SigActionLoad(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ElementIds,
                      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                      const stw_types::uint32 ou32_ValidLayers);
   void SigActionSave(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds,
                      const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                      const stw_types::uint32 ou32_ValidLayers);
   void SigActionRecord(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);
   void SigActionRemove(const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListIds);
   void SigInformUserFloatRangeCheck(
      const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_InvalidValueIds,
      const std::vector<QString> & orc_InvalidValues, const std::vector<QString> & orc_NewValues);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   stw_opensyde_gui_logic::C_SyvDaItPaTreeDelegate mc_Delegate;
   stw_opensyde_gui_logic::C_SyvDaItPaTreeModel mc_Model;
   stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * mpc_DataWidget;
   bool mq_IgnoreChanges;

   void m_HandleChange(void);
   bool m_ColumnsSortedAsExpected(const std::vector<stw_types::sint32> & orc_NewColPositionIndices) const;
   void m_HandleLinkClicked(const QModelIndex & orc_Index);
   void m_HandleActionRead(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                           const stw_types::uint32 ou32_ValidLayers);
   void m_HandleActionWrite(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                            const stw_types::uint32 ou32_ValidLayers);
   void m_HandleActionApply(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                            const stw_types::uint32 ou32_ValidLayers);
   void m_HandleActionLoad(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                           const stw_types::uint32 ou32_ValidLayers);
   void m_HandleActionSave(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                           const stw_types::uint32 ou32_ValidLayers);
   void m_HandleActionRecord(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                             const stw_types::uint32 ou32_ValidLayers);
   void m_HandleActionRemove(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                             const stw_types::uint32 ou32_ValidLayers);
   void m_ScrollBarRangeChangedVer(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;
   void m_ScrollBarRangeChangedHor(const stw_types::sintn osn_Min, const stw_types::sintn osn_Max) const;

   //Avoid call
   C_SyvDaItPaTreeView(const C_SyvDaItPaTreeView &);
   C_SyvDaItPaTreeView & operator =(const C_SyvDaItPaTreeView &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
