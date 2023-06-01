//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all possible openSYDE CAN Monitor settings
 (implementation)

   Widget for showing all possible openSYDE CAN Monitor settings

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamMosWidget.hpp"
#include "ui_C_CamMosWidget.h"

#include "C_CamOgeWiSectionHeader.hpp"
#include "C_GtGetText.hpp"
#include "C_UsHandler.hpp"
#include "C_CamMosSectionPopup.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

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
C_CamMosWidget::C_CamMosWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMosWidget),
   mpc_PopupDatabase(new C_CamMosSectionPopup()),
   mpc_PopupDllConfig(new C_CamMosSectionPopup()),
   mpc_PopupFilter(new C_CamMosSectionPopup()),
   mpc_PopupLogging(new C_CamMosSectionPopup())
{
   this->mpc_Ui->setupUi(this);

   // initialize GUI elements
   this->SetBackgroundColor(5);
   this->mpc_Ui->pc_WiTitle->SetTitle(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_WiTitle->SetIconType(C_CamOgeWiSectionHeader::E_ButtonType::eLEFTRIGHT);

   this->mpc_Ui->pc_PbDatabase->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_PbDllConfig->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_PbFilter->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_PbLogging->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_PbDatabase->setIcon(QIcon("://images/IconDatabase.svg"));
   this->mpc_Ui->pc_PbDllConfig->setIcon(QIcon("://images/IconConfig.svg"));
   this->mpc_Ui->pc_PbFilter->setIcon(QIcon("://images/IconFilter.svg"));
   this->mpc_Ui->pc_PbLogging->setIcon(QIcon("://images/IconLogging.svg"));
   this->mpc_Ui->pc_WiCollapsed->SetBackgroundColor(1);
   this->mpc_Ui->pc_WiExpanded->SetBackgroundColor(1);

   // set parents here
   // opc_Parent is a layout and would try to integrate the popup, so we better use its parent.
   this->mpc_PopupDatabase->setParent(opc_Parent->parentWidget());
   this->mpc_PopupDllConfig->setParent(opc_Parent->parentWidget());
   this->mpc_PopupFilter->setParent(opc_Parent->parentWidget());
   this->mpc_PopupLogging->setParent(opc_Parent->parentWidget());

   // hide popups on start
   this->mpc_PopupDatabase->setVisible(false);
   this->mpc_PopupDllConfig->setVisible(false);
   this->mpc_PopupFilter->setVisible(false);
   this->mpc_PopupLogging->setVisible(false);

   // button checked = popup open
   this->mpc_Ui->pc_PbDatabase->setCheckable(true);
   this->mpc_Ui->pc_PbDllConfig->setCheckable(true);
   this->mpc_Ui->pc_PbFilter->setCheckable(true);
   this->mpc_Ui->pc_PbLogging->setCheckable(true);

   // connect buttons to section popups
   connect(this->mpc_Ui->pc_PbDatabase, &C_CamOgePubSettingsAdd::toggled, this, &C_CamMosWidget::m_ShowPopupDatabase);
   connect(this->mpc_Ui->pc_PbDllConfig, &C_CamOgePubSettingsAdd::toggled, this, &C_CamMosWidget::m_ShowPopupDllConfig);
   connect(this->mpc_Ui->pc_PbFilter, &C_CamOgePubSettingsAdd::toggled, this, &C_CamMosWidget::m_ShowPopupFilter);
   connect(this->mpc_Ui->pc_PbLogging, &C_CamOgePubSettingsAdd::toggled, this, &C_CamMosWidget::m_ShowPopupLogging);

   // connect hide signal of widgets
   connect(this->mpc_Ui->pc_WiDatabase, &C_CamMosDatabaseWidget::SigHide, this, &C_CamMosWidget::m_HidePopupDatabase);
   connect(this->mpc_Ui->pc_WiDllConfig, &C_CamMosDllWidget::SigHide, this, &C_CamMosWidget::m_HidePopupDllConfig);
   connect(this->mpc_Ui->pc_WiFilter, &C_CamMosFilterWidget::SigHide, this, &C_CamMosWidget::m_HidePopupFilter);
   connect(this->mpc_Ui->pc_WiLogging, &C_CamMosLoggingWidget::SigHide, this, &C_CamMosWidget::m_HidePopupLogging);

   // expand collapse section
   connect(this->mpc_Ui->pc_WiTitle, &C_CamOgeWiSectionHeader::SigExpandSection,
           this, &C_CamMosWidget::m_OnExpandSettings);

   // forward information about new settings
   connect(this->mpc_Ui->pc_WiFilter, &C_CamMosFilterWidget::SigFilterNumberChanged,
           this, &C_CamMosWidget::SigFilterNumberChanged);
   connect(this->mpc_Ui->pc_WiFilter, &C_CamMosFilterWidget::SigAddFilterItems,
           this, &C_CamMosWidget::SigAddFilterItems);
   connect(this->mpc_Ui->pc_WiFilter, &C_CamMosFilterWidget::SigRemoveFilterItems,
           this, &C_CamMosWidget::SigRemoveFilterItems);
   connect(this->mpc_Ui->pc_WiFilter, &C_CamMosFilterWidget::SigRemoveAllFilters,
           this, &C_CamMosWidget::SigRemoveAllFilters);
   connect(this->mpc_Ui->pc_WiDatabase, &C_CamMosDatabaseWidget::SigAddDatabaseDbc,
           this, &C_CamMosWidget::SigAddDatabaseDbc);
   connect(this->mpc_Ui->pc_WiDatabase, &C_CamMosDatabaseWidget::SigAddDatabaseOsySysDef,
           this, &C_CamMosWidget::SigAddDatabaseOsySysDef);
   connect(this->mpc_Ui->pc_WiDatabase, &C_CamMosDatabaseWidget::SigRemoveDatabase,
           this, &C_CamMosWidget::SigRemoveDatabase);
   connect(this->mpc_Ui->pc_WiDatabase, &C_CamMosDatabaseWidget::SigActivateDatabase,
           this, &C_CamMosWidget::SigActivateDatabase);
   connect(this->mpc_Ui->pc_WiDatabase, &C_CamMosDatabaseWidget::SigSetDatabaseOsySysDefBus,
           this, &C_CamMosWidget::SigSetDatabaseOsySysDefBus);
   connect(this->mpc_Ui->pc_WiDatabase, &C_CamMosDatabaseWidget::SigNotifyMissingDataBase,
           this, &C_CamMosWidget::SigNotifyMissingDataBase);
   connect(this->mpc_Ui->pc_WiLogging, &C_CamMosLoggingWidget::SigAddLogFileAsc,
           this, &C_CamMosWidget::SigAddLogFileAsc);
   connect(this->mpc_Ui->pc_WiLogging, &C_CamMosLoggingWidget::SigAddLogFileBlf,
           this, &C_CamMosWidget::SigAddLogFileBlf);
   connect(this->mpc_Ui->pc_WiLogging, &C_CamMosLoggingWidget::SigRemoveAllLogFiles,
           this, &C_CamMosWidget::SigRemoveAllLogFiles);
   connect(this->mpc_Ui->pc_WiDllConfig, &C_CamMosDllWidget::SigCanDllConfigured,
           this, &C_CamMosWidget::SigCanDllConfigured);
   connect(this, &C_CamMosWidget::SigEmitAddFilterToChildWidget, this->mpc_Ui->pc_WiFilter,
           &C_CamMosFilterWidget::SetAddFilter);
   connect(this->mpc_Ui->pc_WiFilter, &C_CamMosFilterWidget::SigSendCanFilterMsgDroppedToParentWidget, this,
           &C_CamMosWidget::CanFilterMsgDropped);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosWidget::~C_CamMosWidget()
{
   delete this->mpc_Ui;
   delete this->mpc_PopupDatabase;
   delete this->mpc_PopupDllConfig;
   delete this->mpc_PopupFilter;
   delete this->mpc_PopupLogging;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::LoadUserSettings(void)
{
   this->mpc_Ui->pc_WiDatabase->LoadUserSettings();
   this->mpc_Ui->pc_WiDllConfig->LoadUserSettings();
   this->mpc_Ui->pc_WiFilter->LoadUserSettings();
   this->mpc_Ui->pc_WiLogging->LoadUserSettings();

   // after single widgets
   this->ExpandSettings(C_UsHandler::h_GetInstance()->GetSettingsAreExpanded());
   // adapt GUI by hand (signal gets not received)
   this->m_OnExpandSettings(C_UsHandler::h_GetInstance()->GetSettingsAreExpanded());
}

//----------------------------------------------------------------------------------------------------------------------
/*!
    \brief   Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::SaveUserSettings(void) const
{
   if (C_UsHandler::h_GetInstance()->GetSettingsAreExpanded() == false)
   {
      // remember last opened popup
      C_UsHandler::h_GetInstance()->SetPopOpenSection(this->m_GetPopOpenIdentity());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot to forward dbc load finished signal.

   \param[in]  os32_Result    Result of DBC file load
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::OnLoadFinishedDbc(const int32_t os32_Result) const
{
   this->mpc_Ui->pc_WiDatabase->OnLoadFinishedDbc(os32_Result);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot to forward openSYDE system definition load finished signal.

   \param[in]  os32_Result    Result of openSYDE system definition file load
   \param[in]  orc_Busses     Buses found in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::OnLoadFinishedOsySysDef(const int32_t os32_Result,
                                             const std::vector<stw::opensyde_core::C_OscSystemBus> & orc_Busses) const
{
   this->mpc_Ui->pc_WiDatabase->OnLoadFinishedOsySysDef(os32_Result, orc_Busses);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot to forward openSYDE bus selection result.

   \param[in]     orc_PathSystemDefinition         system definition identifier (i.e. path)
   \param[in]     os32_Result                      result of bus selection
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::OnSigOsySysDefBusResult(const QString & orc_PathSystemDefinition, const int32_t os32_Result) const
{
   this->mpc_Ui->pc_WiDatabase->OnSigOsySysDefBusResult(orc_PathSystemDefinition, os32_Result);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for communication change signal.

   \param[in]  oq_Online    Online/offline flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::OnCommunicationStarted(const bool oq_Online) const
{
   this->mpc_Ui->pc_WiDatabase->OnCommunicationStarted(oq_Online);
   this->mpc_Ui->pc_WiDllConfig->OnCommunicationStarted(oq_Online);
   this->mpc_Ui->pc_WiLogging->OnCommunicationStarted(oq_Online);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot to forward log file add result.

   \param[in]   os32_Result       result of log file add operation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::OnSigLogFileAddResult(const int32_t os32_Result) const
{
   this->mpc_Ui->pc_WiLogging->OnSigLogFileAddResult(os32_Result);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot to forward saved-as action.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::OnSigSavedAsNew(void) const
{
   // forward signal to all widgets including paths which may be relative
   this->mpc_Ui->pc_WiDatabase->OnSigSavedAsNew();
   this->mpc_Ui->pc_WiLogging->OnSigSavedAsNew();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward database drop from main window.

   \param[in]     orc_Path        path of database file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::OnDatabaseDropped(const QString & orc_Path) const
{
   this->mpc_Ui->pc_WiDatabase->AddDroppedDatabase(orc_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clears the current configuration

   Databases and filter configuration is removed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::ClearConfiguration(void) const
{
   this->mpc_Ui->pc_WiDatabase->Clear();
   this->mpc_Ui->pc_WiFilter->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Expand settings by toggling header expand collapse button

   long description of function within several lines

   \param[in]     oq_Expand    true: expand settings subsections
                               false: collapse settings subsections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::ExpandSettings(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiTitle->SetOpen(oq_Expand);
   // this toggles the ">>" button and therefore emits a signal; on this signal we connect and adapt the GUI
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function call of connect mechanism from parent widget constructor. Used to receive and pass the
 *  QList of selected messages Can Id's  and IsCanMsgExtended data to child widget

   \param[in]       oc_CanMsgId     List of selected message CanId's
   \param[in]       oc_CanMsgXtd     List of selected CanMessage has extended format

   \return
   Emit a signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::AddFilterData(const QList<int32_t> oc_CanMsgId, const QList<uint8_t> oc_CanMsgXtd)
{
   Q_EMIT C_CamMosWidget::SigEmitAddFilterToChildWidget(oc_CanMsgId, oc_CanMsgXtd);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function call of connect mechanism from child widget constructor. Used to pass CanMsgDropped Information to parent widget

   \return
   Emit a signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::CanFilterMsgDropped()
{
   Q_EMIT C_CamMosWidget::SigSendCanMsgDroppedToParentWidget();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten move event.

   Here: Handle popup move.

   \param[in,out]    opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::moveEvent(QMoveEvent * const opc_Event)
{
   QPoint c_Point;

   if (this->mpc_PopupDatabase->isVisible() == true)
   {
      c_Point = this->m_GetPopupMovePoint(this->mpc_Ui->pc_PbDatabase);
      this->mpc_PopupDatabase->DoMove(c_Point);
   }
   else if (this->mpc_PopupDllConfig->isVisible() == true)
   {
      c_Point = this->m_GetPopupMovePoint(this->mpc_Ui->pc_PbDllConfig);
      this->mpc_PopupDllConfig->DoMove(c_Point);
   }
   else if (this->mpc_PopupFilter->isVisible() == true)
   {
      c_Point = this->m_GetPopupMovePoint(this->mpc_Ui->pc_PbFilter);
      this->mpc_PopupFilter->DoMove(c_Point);
   }
   else if (this->mpc_PopupLogging->isVisible() == true)
   {
      c_Point = this->m_GetPopupMovePoint(this->mpc_Ui->pc_PbLogging);
      this->mpc_PopupLogging->DoMove(c_Point);
   }
   else
   {
      // do nothing
   }

   C_OgeWiOnlyBackground::moveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle expand or collapse settings for subsections.

   \param[in]     oq_Expand        true: expand settings subsections
                                   false: collapse settings subsections
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_OnExpandSettings(const bool oq_Expand)
{
   // always show whole widget in popup state
   this->mpc_Ui->pc_WiDatabase->PrepareForExpanded(oq_Expand);
   this->mpc_Ui->pc_WiDllConfig->PrepareForExpanded(oq_Expand);
   this->mpc_Ui->pc_WiFilter->PrepareForExpanded(oq_Expand);
   this->mpc_Ui->pc_WiLogging->PrepareForExpanded(oq_Expand);

   if (oq_Expand == true)
   {
      // remember last opened popup
      C_UsHandler::h_GetInstance()->SetPopOpenSection(this->m_GetPopOpenIdentity());

      // hide buttons
      this->mpc_Ui->pc_WiCollapsed->setVisible(false);
      this->mpc_Ui->pc_WiExpanded->setVisible(true);

      // move widgets from popups back (order matters!)
      this->mpc_Ui->pc_WiExpanded->layout()->addWidget(this->mpc_Ui->pc_WiDatabase);
      this->mpc_Ui->pc_WiExpanded->layout()->addWidget(this->mpc_Ui->pc_WiFilter);
      this->mpc_Ui->pc_WiExpanded->layout()->addWidget(this->mpc_Ui->pc_WiLogging);
      this->mpc_Ui->pc_WiExpanded->layout()->addWidget(this->mpc_Ui->pc_WiDllConfig);

      // hide all popups
      this->mpc_Ui->pc_PbDatabase->setChecked(false);
      this->mpc_Ui->pc_PbDllConfig->setChecked(false);
      this->mpc_Ui->pc_PbFilter->setChecked(false);
      this->mpc_Ui->pc_PbLogging->setChecked(false);
   }
   else
   {
      // show buttons
      this->mpc_Ui->pc_WiExpanded->setVisible(false);
      this->mpc_Ui->pc_WiCollapsed->setVisible(true);

      // move widgets to popups
      this->mpc_PopupDatabase->SetWidget(this->mpc_Ui->pc_WiDatabase);
      this->mpc_PopupDllConfig->SetWidget(this->mpc_Ui->pc_WiDllConfig);
      this->mpc_PopupFilter->SetWidget(this->mpc_Ui->pc_WiFilter);
      this->mpc_PopupLogging->SetWidget(this->mpc_Ui->pc_WiLogging);

      // open last open popup
      switch (C_UsHandler::h_GetInstance()->GetPopOpenSection())
      {
      case C_UsHandler::eDATABASE:
         this->mpc_Ui->pc_PbDatabase->setChecked(true);
         break;
      case C_UsHandler::eDLLCONFIG:
         this->mpc_Ui->pc_PbDllConfig->setChecked(true);
         break;
      case C_UsHandler::eFILTER:
         this->mpc_Ui->pc_PbFilter->setChecked(true);
         break;
      case C_UsHandler::eLOGGING:
         this->mpc_Ui->pc_PbLogging->setChecked(true);
         break;
      case C_UsHandler::eNONE:
      default:
         break;
      }
   }

   Q_EMIT (this->SigExpandSettings(oq_Expand));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide database popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_HidePopupDatabase(void) const
{
   this->mpc_Ui->pc_PbDatabase->setChecked(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide CAN DLL configuration popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_HidePopupDllConfig(void) const
{
   this->mpc_Ui->pc_PbDllConfig->setChecked(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide filter popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_HidePopupFilter(void) const
{
   this->mpc_Ui->pc_PbFilter->setChecked(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide logging popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_HidePopupLogging(void) const
{
   this->mpc_Ui->pc_PbLogging->setChecked(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show database popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopupDatabase(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->m_ShowPopup(this->mpc_PopupDatabase, this->mpc_Ui->pc_PbDatabase);

      // hide all other popups
      this->mpc_Ui->pc_PbDllConfig->setChecked(false);
      this->mpc_Ui->pc_PbFilter->setChecked(false);
      this->mpc_Ui->pc_PbLogging->setChecked(false);
   }
   else
   {
      this->mpc_PopupDatabase->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show CAN DLL configuration popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopupDllConfig(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->m_ShowPopup(this->mpc_PopupDllConfig, this->mpc_Ui->pc_PbDllConfig);

      // hide all other popups
      this->mpc_Ui->pc_PbDatabase->setChecked(false);
      this->mpc_Ui->pc_PbFilter->setChecked(false);
      this->mpc_Ui->pc_PbLogging->setChecked(false);
   }
   else
   {
      this->mpc_PopupDllConfig->setVisible(false);
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show filter popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopupFilter(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->m_ShowPopup(this->mpc_PopupFilter, this->mpc_Ui->pc_PbFilter);

      // hide all other popups
      this->mpc_Ui->pc_PbDatabase->setChecked(false);
      this->mpc_Ui->pc_PbDllConfig->setChecked(false);
      this->mpc_Ui->pc_PbLogging->setChecked(false);
   }
   else
   {
      this->mpc_PopupFilter->setVisible(false);
   }
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show logging popup.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopupLogging(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->m_ShowPopup(this->mpc_PopupLogging, this->mpc_Ui->pc_PbLogging);

      // hide all other popups
      this->mpc_Ui->pc_PbDatabase->setChecked(false);
      this->mpc_Ui->pc_PbDllConfig->setChecked(false);
      this->mpc_Ui->pc_PbFilter->setChecked(false);
   }
   else
   {
      this->mpc_PopupLogging->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show an popup

   \param[in]       opc_Popup    Specific popup widget
   \param[in]       opc_Button   Specific button as start point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopup(C_CamMosSectionPopup * const opc_Popup,
                                 const C_CamOgePubSettingsAdd * const opc_Button) const
{
   const QPoint c_Point = this->m_GetPopupMovePoint(opc_Button);

   opc_Popup->Show(c_Point);
   //lint -e{429}  no memory leak because opc_Popup points to instances which are freed in destructor
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the point for the DoMove function of C_CamMosSectionPopup

   \param[in]       opc_Button   Specific button as start point

   \return
   Point for DoMove function on parent of splitter widget
*/
//----------------------------------------------------------------------------------------------------------------------
QPoint C_CamMosWidget::m_GetPopupMovePoint(const C_CamOgePubSettingsAdd * const opc_Button) const
{
   // Position of button in C_CamMosWidget widget
   const QPoint c_Settings = opc_Button->mapToParent(opc_Button->rect().topLeft());
   // Position on splitter level
   const QPoint c_Splitter = this->mapToParent(c_Settings);
   // Position on parent of splitter
   const QPoint c_TopWidget = this->parentWidget()->mapToParent(c_Splitter);

   return c_TopWidget;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check all popups for open one.

   \return
   opened popup enum or none if no popup is open
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsHandler::E_SettingsSubSection C_CamMosWidget::m_GetPopOpenIdentity(void) const
{
   C_UsHandler::E_SettingsSubSection e_Return;
   if (this->mpc_PopupDatabase->isVisible() == true)
   {
      e_Return = C_UsHandler::eDATABASE;
   }
   else if (this->mpc_PopupDllConfig->isVisible() == true)
   {
      e_Return = C_UsHandler::eDLLCONFIG;
   }
   else if (this->mpc_PopupFilter->isVisible() == true)
   {
      e_Return = C_UsHandler::eFILTER;
   }
   else if (this->mpc_PopupLogging->isVisible() == true)
   {
      e_Return = C_UsHandler::eLOGGING;
   }
   else
   {
      e_Return = C_UsHandler::eNONE;
   }

   return e_Return;
}
