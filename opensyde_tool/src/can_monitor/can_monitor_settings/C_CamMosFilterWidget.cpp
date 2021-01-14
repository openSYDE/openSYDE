//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling CAN Message filter configuration (implementation)

   Widget for handling CAN Message filter configuration

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QListWidgetItem>

#include "C_CamMosFilterWidget.h"
#include "ui_C_CamMosFilterWidget.h"

#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "C_OgePopUpDialog.h"
#include "C_CamMosFilterPopup.h"
#include "C_CamProHandler.h"
#include "C_Uti.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_UsHandler.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosFilterWidget::C_CamMosFilterWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMosFilterWidget)
{
   this->mpc_Ui->setupUi(this);

   // initialize GUI elements
   this->m_InitUi();

   // load configuration
   connect(C_CamProHandler::h_GetInstance(), &C_CamProHandler::SigNewConfiguration,
           this, &C_CamMosFilterWidget::m_LoadConfig);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosFilterWidget::~C_CamMosFilterWidget()
{
   // clean up item widgets
   this->Clear();

   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all user settings.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::LoadUserSettings(void) const
{
   this->mpc_Ui->pc_WiHeader->SetExpanded(C_UsHandler::h_GetInstance()->GetWiFilterExpanded());
   this->m_OnExpand(C_UsHandler::h_GetInstance()->GetWiFilterExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Prepare widget for menu view in collapsed state resp. expanded view.

   \param[in]     oq_Expand       true: prepare for expanded, false: prepare for collapsed menu
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::PrepareForExpanded(const bool oq_Expand) const
{
   if (oq_Expand == false)
   {
      this->mpc_Ui->pc_WiContent->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_WiContent->setVisible(C_UsHandler::h_GetInstance()->GetWiFilterExpanded());
   }

   // hide arrow button
   this->mpc_Ui->pc_WiHeader->ShowExpandButton(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::Clear(void)
{
   // remove all filters
   Q_EMIT (this->SigRemoveAllFilters());
   Q_EMIT (this->SigFilterNumberChanged(0));

   for (uint32 u32_ItEntry = 0; u32_ItEntry < this->mc_Entries.size(); ++u32_ItEntry)
   {
      // delete widget from GUI
      this->m_RemoveFilterWidget(this->mc_Entries[u32_ItEntry]);
   }

   // clear list of filter widgets
   mc_Entries.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize GUI elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_InitUi(void)
{
   // initialize colors
   this->SetBackgroundColor(5);
   this->mpc_Ui->pc_ScrollAreaContents->SetBackgroundColor(5);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameBottom, "HasColor8Background", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_FrameTop, "HasColor8Background", true);

   // initialize title widget
   this->mpc_Ui->pc_WiHeader->SetTitle(C_GtGetText::h_GetText("Receive Filter"));
   this->mpc_Ui->pc_WiHeader->SetIcon("://images/IconFilter.svg");
   this->mpc_Ui->pc_WiHeader->SetToggle(true);
   this->mpc_Ui->pc_WiHeader->SetToggleToolTip(C_GtGetText::h_GetText("Receive Filter"),
                                               C_GtGetText::h_GetText("Disable or enable all filters."));

   // initialize add button
   QIcon c_Icon;
   c_Icon.addFile("://images/IconAdd.svg", QSize(), QIcon::Normal);
   c_Icon.addFile("://images/IconAddDisabled.svg", QSize(), QIcon::Disabled);
   this->mpc_Ui->pc_BtnAdd->setIconSize(QSize(11, 11));
   this->mpc_Ui->pc_BtnAdd->setIcon(c_Icon);
   this->mpc_Ui->pc_BtnAdd->setText(C_GtGetText::h_GetText("Add Filter"));
   this->mpc_Ui->pc_BtnAdd->SetToolTipInformation(C_GtGetText::h_GetText("Add New Filter"),
                                                  C_GtGetText::h_GetText(
                                                     "Add new receive filter and configure filter items."));

   // initialize label
   this->mpc_Ui->pc_LabNoFilter->SetForegroundColor(0);
   this->mpc_Ui->pc_LabNoFilter->SetFontPixel(12);
   this->mpc_Ui->pc_LabNoFilter->setEnabled(false);

   this->mpc_Ui->pc_LabNoFilter->setText(C_GtGetText::h_GetText(
                                            "No receive filters are declared.\nadd any via \"Add Filter\"."));

   // connects
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigExpandSection,
           this, &C_CamMosFilterWidget::m_OnExpand);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigHide,
           this, &C_CamMosFilterWidget::SigHide);
   connect(this->mpc_Ui->pc_WiHeader, &C_CamOgeWiSettingSubSection::SigToggled,
           this, &C_CamMosFilterWidget::m_EnableFilters);
   connect(this->mpc_Ui->pc_BtnAdd, &C_CamOgePubSettingsAdd::clicked, this, &C_CamMosFilterWidget::m_OnAddClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load configuration (i.e. filter widgets)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_LoadConfig(void)
{
   // show no filter label as default
   this->mpc_Ui->pc_GroupBoxNoElements->setVisible(true);

   // initialize filter widgets
   std::vector<C_CamProFilterData> c_Filters = C_CamProHandler::h_GetInstance()->GetFilters();

   for (uint32 u32_ItFilter = 0UL; u32_ItFilter < c_Filters.size(); ++u32_ItFilter)
   {
      this->m_AddFilterWidget(c_Filters[u32_ItFilter]);
   }

   // initialize enabled state (setChecked also emits toggled signal, so m_EnableFilters gets called automatically)
   this->mpc_Ui->pc_WiHeader->SetToggleState(C_CamProHandler::h_GetInstance()->GetFilterWidgetEnabled());

   // update title
   this->m_UpdateTitleFilterCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove filter widget (GUI only)

   Counterpart of m_AddFilterWidget.
   Does no external data handling.

   \param[in]     opc_ItemWidget      Filter widget to remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_RemoveFilterWidget(C_CamMosFilterItemWidget * const opc_ItemWidget) const
{
   if (opc_ItemWidget != NULL)
   {
      // disconnect everything
      opc_ItemWidget->disconnect();

      // remove widget from layout
      this->mpc_Ui->pc_LayoutContents->removeWidget(opc_ItemWidget);

      // delete widget
      delete (opc_ItemWidget);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for toggle button.

   \param[in]     orq_Enabled    true: enable, false: disable

   Switch between disable and enable filters.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_EnableFilters(const bool & orq_Enabled)
{
   // update data handling
   C_CamProHandler::h_GetInstance()->SetFilterWidgetEnabled(orq_Enabled);

   // inform main window about changes of active filters
   if (orq_Enabled == true)
   {
      QList<C_CamProFilterItemData> c_ActiveFilterItems;
      const std::vector<C_CamProFilterData> c_Filters = C_CamProHandler::h_GetInstance()->GetFilters();

      // check all filter (packages)
      for (uint32 u32_PosFilter = 0; u32_PosFilter < c_Filters.size(); u32_PosFilter++)
      {
         if (c_Filters[u32_PosFilter].q_Enabled == true)
         {
            // check all filter items
            QList<C_CamProFilterItemData> c_Temp = c_Filters[u32_PosFilter].c_FilterItems;
            this->m_GetActiveFilterItems(c_Temp);
            c_ActiveFilterItems.append(c_Temp);
         }
      }

      // add checked filters
      Q_EMIT (this->SigAddFilterItems(c_ActiveFilterItems));

      // number of checked filters changed
      Q_EMIT (this->SigFilterNumberChanged(this->m_CountActiveFilterPackages()));
   }
   else
   {
      // remove all filters
      Q_EMIT (this->SigRemoveAllFilters());

      // disabled means no active filter at all
      Q_EMIT (this->SigFilterNumberChanged(0));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for add button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_OnAddClicked()
{
   // Create unique name
   std::map<stw_scl::C_SCLString, bool> c_Names;
   std::vector<C_CamProFilterData> c_Filters = C_CamProHandler::h_GetInstance()->GetFilters();
   const QString c_ProposedName = "NewFilter";
   C_CamProFilterData c_FilterData;

   for (uint32 u32_ItFilter = 0UL; u32_ItFilter < c_Filters.size(); ++u32_ItFilter)
   {
      c_Names[c_Filters[u32_ItFilter].c_Name.toStdString().c_str()] = true;
   }
   c_FilterData.c_Name = C_Uti::h_GetUniqueNameQ(c_Names, c_ProposedName);

   // Pop up dialog (use scroll area widget as parent to make sure scroll bars are styled correctly)
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this->mpc_Ui->pc_ScrollAreaContents,
                                                                 this->mpc_Ui->pc_ScrollAreaContents);
   C_CamMosFilterPopup * const pc_Dialog = new C_CamMosFilterPopup(c_FilterData, *c_New);

   Q_UNUSED(pc_Dialog)

   //Resize
   c_New->SetSize(QSize(700, 820));

   // Update settings on accept
   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      QList<C_CamProFilterItemData> c_TempItems;
      c_FilterData = pc_Dialog->GetFilterData();
      // c_FilterData.q_Enabled is not set from dialog but we want the default anyway

      // add in data handling
      C_CamProHandler::h_GetInstance()->AddFilter(c_FilterData);

      // add new widget
      this->m_AddFilterWidget(c_FilterData);

      // if receive filters are disabled ask user for enabling
      if (C_CamProHandler::h_GetInstance()->GetFilterWidgetEnabled() == false)
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
         c_Message.SetHeading(C_GtGetText::h_GetText("Receive Filter Disabled"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Filters are not applied as long as receive filter setting is "
                                                         "disabled. Do you want to enable it now?"));
         c_Message.SetOKButtonText("Enable");
         c_Message.SetNOButtonText("Keep Disabled");
         if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
         {
            C_CamProHandler::h_GetInstance()->SetFilterWidgetEnabled(true);
            this->mpc_Ui->pc_WiHeader->SetToggleState(true);
            // this also emits toggled signal, so filter items get enabled automatically
         }
      }
      else
      {
         // enable new filter items if filter widget is already enabled
         c_TempItems = c_FilterData.c_FilterItems;
         this->m_GetActiveFilterItems(c_TempItems);

         Q_EMIT (this->SigAddFilterItems(c_TempItems));

         // inform about new number of filters
         Q_EMIT (this->SigFilterNumberChanged(this->m_CountActiveFilterPackages()));
      }
   }

   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add new filter widget (GUI only).

   Counterpart of m_RemoveFilterWidget.
   Does no external data handling.

   \param[in]     orc_FilterData      filter data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_AddFilterWidget(const C_CamProFilterData & orc_FilterData)
{
   C_CamMosFilterItemWidget * const pc_NewItem = new C_CamMosFilterItemWidget(orc_FilterData, this);

   // remember item
   this->mc_Entries.push_back(pc_NewItem);

   // insert new item before spacer -> "-1"
   this->mpc_Ui->pc_LayoutContents->insertWidget(this->mpc_Ui->pc_LayoutContents->count() - 1, pc_NewItem);

   // hide "no filter" label
   this->mpc_Ui->pc_GroupBoxNoElements->setVisible(false);

   // update count in title
   this->m_UpdateTitleFilterCount();

   // connect
   connect(pc_NewItem, &C_CamMosFilterItemWidget::SigRemoveFilter, this, &C_CamMosFilterWidget::m_RemoveFilter);
   connect(pc_NewItem, &C_CamMosFilterItemWidget::SigEnableFilter, this, &C_CamMosFilterWidget::m_ActivateFilter);
   connect(pc_NewItem, &C_CamMosFilterItemWidget::SigUpdateFilter, this,
           &C_CamMosFilterWidget::m_UpdateFilterConfiguration);
} //lint !e429  no memory leak because of the parent of pc_NewItem and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remove existing filter widget.

   \param[in]     opc_Item       Filter widget to remove
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_RemoveFilter(C_CamMosFilterItemWidget * const opc_ItemWidget)
{
   // ask user
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eQUESTION);

   c_Message.SetHeading(C_GtGetText::h_GetText("Filter Delete"));
   c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to delete this filter?"));
   c_Message.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
   c_Message.SetNOButtonText(C_GtGetText::h_GetText("Keep"));

   if (c_Message.Execute() == C_OgeWiCustomMessage::eOK)
   {
      if (opc_ItemWidget != NULL)
      {
         sint32 s32_IndexToRemove = 0;

         // remove widget from list
         for (std::vector<C_CamMosFilterItemWidget *>::iterator c_It = mc_Entries.begin(); c_It != mc_Entries.end();
              ++c_It)
         {
            if (*c_It == opc_ItemWidget)
            {
               this->mc_Entries.erase(c_It);
               break;
            }
            s32_IndexToRemove++;
         }

         // delete widget from GUI
         this->m_RemoveFilterWidget(opc_ItemWidget);

         // update data handling
         tgl_assert(C_CamProHandler::h_GetInstance()->DeleteFilter(s32_IndexToRemove) == stw_errors::C_NO_ERR);

         // remove filter items if filter widget is enabled
         if (C_CamProHandler::h_GetInstance()->GetFilterWidgetEnabled() == true)
         {
            // remove (disable) filter items that were active till now
            QList<C_CamProFilterItemData> c_FilterItems =
               C_CamProHandler::h_GetInstance()->GetFilters()[s32_IndexToRemove].c_FilterItems;
            this->m_GetActiveFilterItems(c_FilterItems);
            Q_EMIT (this->SigRemoveFilterItems(c_FilterItems));

            // inform about new number of filters (after update data handling!)
            Q_EMIT (this->SigFilterNumberChanged(this->m_CountActiveFilterPackages()));
         }
      }

      // number of filter widgets should now equal number of filters in data handling
      tgl_assert(C_CamProHandler::h_GetInstance()->GetFilters().size() == mc_Entries.size());

      // eventually show "no filter" label
      if (this->mc_Entries.size() == 0)
      {
         this->mpc_Ui->pc_GroupBoxNoElements->setVisible(true);
      }

      // update count in title
      this->m_UpdateTitleFilterCount();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activate or deactivate given filter.

   \param[in]     opc_ItemWidget    Filter widget to select
   \param[in]     orq_Enable        True: activate filter, false: deactivate filter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_ActivateFilter(const C_CamMosFilterItemWidget * const opc_ItemWidget,
                                            const bool & orq_Enable)
{
   const sint32 s32_IndexToActivate = this->m_GetIndexFromWidget(opc_ItemWidget);

   // index is -1 if item widget is NULL which can not happen
   tgl_assert(s32_IndexToActivate >= 0);
   if (s32_IndexToActivate >= 0)
   {
      // update data handling
      C_CamProHandler::h_GetInstance()->SetFilterEnabled(s32_IndexToActivate, orq_Enable);

      // only use filter if filter widget is enabled
      if (C_CamProHandler::h_GetInstance()->GetFilterWidgetEnabled() == true)
      {
         // search for filter items with flag "enabled"
         QList<C_CamProFilterItemData> c_FilterItems =
            C_CamProHandler::h_GetInstance()->GetFilters()[s32_IndexToActivate].c_FilterItems;
         this->m_GetActiveFilterItems(c_FilterItems);

         // activate or deactivate filter
         if (orq_Enable == true)
         {
            Q_EMIT (this->SigAddFilterItems(c_FilterItems));
         }
         else
         {
            Q_EMIT (this->SigRemoveFilterItems(c_FilterItems));
         }

         // inform about new number of filters
         Q_EMIT (this->SigFilterNumberChanged(this->m_CountActiveFilterPackages()));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update filter configuration of specified filter.

   Slot for signal from item widget on editing finished.

   \param[in]     opc_ItemWidget      widget of filter item
   \param[in]     orc_Filter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_UpdateFilterConfiguration(const C_CamMosFilterItemWidget * const opc_ItemWidget,
                                                       const C_CamProFilterData & orc_FilterOld,
                                                       const C_CamProFilterData & orc_FilterNew)
{
   QList<C_CamProFilterItemData> c_FilterItems;
   const sint32 s32_Index = this->m_GetIndexFromWidget(opc_ItemWidget);

   // index is -1 if item widget is NULL which can not happen
   tgl_assert(s32_Index >= 0);
   if (s32_Index >= 0)
   {
      C_CamProHandler::h_GetInstance()->SetFilter(s32_Index, orc_FilterNew);

      // disable or enable filter items if filter widget is enabled
      if ((C_CamProHandler::h_GetInstance()->GetFilterWidgetEnabled() == true))
      {
         // remove old active filter items if filter was enabled
         if (orc_FilterOld.q_Enabled == true)
         {
            c_FilterItems = orc_FilterOld.c_FilterItems;
            this->m_GetActiveFilterItems(c_FilterItems);
            Q_EMIT (this->SigRemoveFilterItems(c_FilterItems));
         }

         // add new active filter items if filter is enabled
         if (orc_FilterNew.q_Enabled == true)
         {
            c_FilterItems = orc_FilterNew.c_FilterItems;
            this->m_GetActiveFilterItems(c_FilterItems);
            Q_EMIT (this->SigAddFilterItems(c_FilterItems));
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Count active filters

   \return
   number of active filters
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMosFilterWidget::m_CountActiveFilterPackages() const
{
   sint32 s32_Return = 0;
   const std::vector<C_CamProFilterData> c_Filters = C_CamProHandler::h_GetInstance()->GetFilters();

   for (uint32 u32_Pos = 0; u32_Pos < c_Filters.size(); u32_Pos++)
   {
      if (c_Filters[u32_Pos].q_Enabled == true)
      {
         s32_Return++;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data handling index of filter from filter widget pointer.

   \param[in]  opc_ItemWidget    pointer to filter widget

   \return
   index of filter in data handling
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMosFilterWidget::m_GetIndexFromWidget(const C_CamMosFilterItemWidget * const opc_ItemWidget)
{
   sint32 s32_Return = 0;

   if (opc_ItemWidget != NULL)
   {
      // get index from opc_Item
      for (std::vector<C_CamMosFilterItemWidget *>::iterator c_It = mc_Entries.begin(); c_It != mc_Entries.end();
           ++c_It)
      {
         if (*c_It == opc_ItemWidget)
         {
            break;
         }
         s32_Return++;
      }
   }
   else
   {
      s32_Return = -1;
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all active filter items in given list of filter items.

   \param[in,out]     orc_ActiveFilterItems     active filter items

   \return
   list of active filter items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_GetActiveFilterItems(QList<C_CamProFilterItemData> & orc_ActiveFilterItems) const
{
   QList<C_CamProFilterItemData> c_TempItems;
   for (sint32 s32_PosItem = 0; s32_PosItem < orc_ActiveFilterItems.size(); s32_PosItem++)
   {
      if (orc_ActiveFilterItems[s32_PosItem].q_Enabled == true)
      {
         c_TempItems.append(orc_ActiveFilterItems[s32_PosItem]);
      }
   }

   orc_ActiveFilterItems = c_TempItems;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of expand signal.

   \param[in] oq_Expand    true: expand; false: collapse
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_OnExpand(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiContent->setVisible(oq_Expand);
   C_UsHandler::h_GetInstance()->SetWiFilterExpanded(oq_Expand);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update title: show filter number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterWidget::m_UpdateTitleFilterCount(void) const
{
   this->mpc_Ui->pc_WiHeader->SetTitle(
      static_cast<QString>(C_GtGetText::h_GetText("Receive Filter (%1)")).arg(this->mc_Entries.size()));
}
