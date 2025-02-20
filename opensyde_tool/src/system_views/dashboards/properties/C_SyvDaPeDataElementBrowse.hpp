//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard data element selection (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAPEDATAELEMENTBROWSE_HPP
#define C_SYVDAPEDATAELEMENTBROWSE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"
#include "C_TblTreDataElementModel.hpp"
#include "C_PuiSvDbNodeDataPoolListElementId.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaPeDataElementBrowse;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaPeDataElementBrowse :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaPeDataElementBrowse(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const uint32_t ou32_ViewIndex, const bool oq_MultiSelect,
                                       const bool oq_ShowOnlyWriteElements, const bool oq_ShowArrayElements,
                                       const bool oq_ShowArrayIndexElements, const bool oq_Show64BitValues,
                                       const bool oq_ShowNvmLists,
                                       const std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> * const opc_AlreasyUsedElements =
                                          NULL, const bool oq_UseInSysViews = true,
                                       const uint32_t ou32_SdDataLoggerUseCaseNodeIndex = 0UL,
                                       const uint32_t ou32_SdDataLoggerUseCaseDataLoggerIndex = 0UL);
   ~C_SyvDaPeDataElementBrowse(void) override;

   void InitStaticNames(void) const;
   void SaveUserSettings(void) const;
   std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> GetSelectedDataElements(void) const;
   void PrepareCleanUp(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SyvDaPeDataElementBrowse(const C_SyvDaPeDataElementBrowse &);
   C_SyvDaPeDataElementBrowse & operator =(const C_SyvDaPeDataElementBrowse &) &;

   Ui::C_SyvDaPeDataElementBrowse * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   static const int32_t mhs32_INDEX_DATAPOOL_ELEMENT;
   static const int32_t mhs32_INDEX_BUS_SIGNAL;
   static int32_t mhs32_LastSelectedComboBoxIndex;
   bool mq_ShowOnlyWriteElements;
   bool mq_ShowArrayElements;
   bool mq_ShowArrayIndexElements;
   bool mq_Show64BitValues;
   const std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> * mpc_AlreasyUsedElements;

   void m_OkClicked(void);
   void m_HandleHiding(void) const;
   void m_CancelClicked(void);
   void m_OnSearch(const QString & orc_Text) const;
   void m_SwitchType(const int32_t os32_Index) const;
   void m_UpdateSelection(const int32_t os32_SelectionCount) const;
   void m_SetupContextMenu(const bool & orq_MultiSelect);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
