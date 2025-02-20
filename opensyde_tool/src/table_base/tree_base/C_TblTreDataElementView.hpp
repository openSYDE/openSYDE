//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view for data elements (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEDATAELEMENTTREEVIEW_HPP
#define C_SYVDAPEDATAELEMENTTREEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_TblTreDataElementModel.hpp"
#include "C_TblTreDataElementSortFilter.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblTreDataElementView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_TblTreDataElementView(QWidget * const opc_Parent = NULL);
   ~C_TblTreDataElementView(void) override;

   void SetUseInternalExpandedItems(const bool oq_Use);
   void InitSd(const uint32_t ou32_NodeIndex, const int32_t os32_SkipApplicationIndex,
               const std::vector<uint32_t> & orc_UsedDataPoolIndicesIndex);
   void InitSv(const uint32_t ou32_ViewIndex, const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
               const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues, const bool oq_ShowNvmLists,
               const std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements, const bool oq_UseInSysViews = true, const uint32_t ou32_SdDataLoggerUseCaseNodeIndex = 0UL, const uint32_t ou32_SdDataLoggerUseCaseDataLoggerIndex = 0UL);
   void Search(const QString & orc_Text);
   void SetViewIndex(const uint32_t ou32_ViewIndex);
   void SwitchMode(const stw::opensyde_gui_logic::C_TblTreDataElementModel::E_Mode & ore_Mode,
                   const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                   const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                   const std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements);

   std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> GetSelectedDataElements(void) const;
   bool IsEmpty(void) const;
   void SaveExpandedIndices(void);
   void CleanUpLastView(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigAccept(void);
   void SigSelectionChanged(const int32_t os32_Count);

protected:
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;

private:
   stw::opensyde_gui_logic::C_TblTreDataElementModel mc_Model;
   stw::opensyde_gui_logic::C_TblTreDataElementSortFilter mc_SortModel;
   bool mq_UseInternalExpandedItems;
   uint32_t mu32_ViewIndex;
   stw::opensyde_gui_logic::C_TblTreDataElementModel::E_Mode me_Mode;
   static QMap<stw::opensyde_gui_logic::C_TblTreDataElementModel::E_Mode,
               std::vector<std::vector<uint32_t> > > mhc_LastKnownExpandedIndices;

   void m_AppendExpandedIndices(std::vector<std::vector<uint32_t> > & orc_FoundItems, const QModelIndex & orc_CurParent,
                                const int32_t os32_Column);
   void m_RestoreExpandedIndices(void);
   QModelIndex m_ManualMapFromSource(const QModelIndex & orc_Index) const;
   void m_ExpandInitial(void);
   void m_ExpandAllChildren(const QModelIndex & orc_Index, const uint32_t ou32_LayerCounter,
                            const uint32_t ou32_MaxLayer);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
