//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for data elements (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEDATAELEMENTTREEVIEW_H
#define C_SYVDAPEDATAELEMENTTREEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.h"
#include "C_TblTreDataElementModel.h"
#include "C_TblTreDataElementSortFilter.h"
#include "C_PuiSvDbNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreDataElementView :
   public stw_opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_TblTreDataElementView(QWidget * const opc_Parent = NULL);
   ~C_TblTreDataElementView(void);

   void SetUseInternalExpandedItems(const bool oq_Use);
   void InitSD(const stw_types::uint32 ou32_NodeIndex, const stw_types::sint32 os32_SkipApplicationIndex,
               const std::vector<stw_types::uint32> & orc_UsedDataPoolIndicesIndex);
   void InitSV(const stw_types::uint32 ou32_ViewIndex, const bool oq_ShowOnlyWriteElements,
               const bool oq_ShowArrayElements, const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
               const bool oq_ShowNVMLists,
               const std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);
   void Search(const QString & orc_Text);
   void SetViewIndex(const stw_types::uint32 ou32_ViewIndex);
   void SwitchMode(const stw_opensyde_gui_logic::C_TblTreDataElementModel::E_Mode & ore_Mode,
                   const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                   const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                   const std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);

   std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> GetSelectedDataElements(void) const;
   bool IsEmpty(void) const;
   void SaveExpandedIndices(void);
   void CleanUpLastView(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigAccept(void);
   void SigSelectionChanged(const stw_types::sintn osn_Count);

protected:
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;

private:
   stw_opensyde_gui_logic::C_TblTreDataElementModel mc_Model;
   stw_opensyde_gui_logic::C_TblTreDataElementSortFilter mc_SortModel;
   bool mq_UseInternalExpandedItems;
   stw_types::uint32 mu32_ViewIndex;
   stw_opensyde_gui_logic::C_TblTreDataElementModel::E_Mode me_Mode;
   static QMap<stw_opensyde_gui_logic::C_TblTreDataElementModel::E_Mode,
               std::vector<std::vector<stw_types::uint32> > > mhc_LastKnownExpandedIndices;

   void m_AppendExpandedIndices(std::vector<std::vector<stw_types::uint32> > & orc_FoundItems,
                                const QModelIndex & orc_CurParent, const stw_types::sintn osn_Column);
   void m_RestoreExpandedIndices(void);
   QModelIndex m_ManualMapFromSource(const QModelIndex & orc_Index) const;
   void m_ExpandInitial(void);
   void m_ExpandAllChildren(const QModelIndex & orc_Index, const stw_types::uint32 ou32_LayerCounter,
                            const stw_types::uint32 ou32_MaxLayer);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
