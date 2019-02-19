//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for showing all possible openSYDE CAN Monitor settings
 (implementation)

   Widget for showing all possible openSYDE CAN Monitor settings


   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.11.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_CamMosWidget.h"
#include "ui_C_CamMosWidget.h"

#include "C_CamOgeWiSectionHeader.h"
#include "C_GtGetText.h"
#include "C_UsHandler.h"
#include "C_CamMosSectionPopup.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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
   connect(this->mpc_Ui->pc_WiLogging, &C_CamMosLoggingWidget::SigAddLogFileAsc,
           this, &C_CamMosWidget::SigAddLogFileAsc);
   connect(this->mpc_Ui->pc_WiLogging, &C_CamMosLoggingWidget::SigAddLogFileBlf,
           this, &C_CamMosWidget::SigAddLogFileBlf);
   connect(this->mpc_Ui->pc_WiLogging, &C_CamMosLoggingWidget::SigRemoveAllLogFiles,
           this, &C_CamMosWidget::SigRemoveAllLogFiles);
   connect(this->mpc_Ui->pc_WiDllConfig, &C_CamMosDllWidget::SigCANDllConfigured,
           this, &C_CamMosWidget::SigCANDllConfigured);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     15.11.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_CamMosWidget::~C_CamMosWidget()
{
   delete this->mpc_Ui;
   delete this->mpc_PopupDatabase;
   delete this->mpc_PopupDllConfig;
   delete this->mpc_PopupFilter;
   delete this->mpc_PopupLogging;
}

//----------------------------------------------------------------------------
/*!
   \brief   Load all user settings

   \created     21.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
    \brief   Save all user settings

   \created     31.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::SaveUserSettings(void) const
{
   if (C_UsHandler::h_GetInstance()->GetSettingsAreExpanded() == false)
   {
      // remember last opened popup
      C_UsHandler::h_GetInstance()->SetPopOpenSection(this->m_GetPopOpenIdentity());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to forward dbc load finished signal.

   \created     19.12.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::OnLoadFinishedDbc(const stw_types::sint32 os32_Result) const
{
   this->mpc_Ui->pc_WiDatabase->OnLoadFinishedDbc(os32_Result);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to forward openSYDE system definition load finished signal.

   \created     19.12.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::OnLoadFinishedOsySysDef(const stw_types::sint32 os32_Result,
                                             const std::vector<stw_opensyde_core::C_OSCSystemBus> & orc_Busses) const
{
   this->mpc_Ui->pc_WiDatabase->OnLoadFinishedOsySysDef(os32_Result, orc_Busses);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to forward openSYDE bus selection result.

   \param[in]     orc_PathSystemDefinition         system definition identifier (i.e. path)
   \param[in]     os32_Result                      result of bus selection

   \created     07.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::OnSigOsySysDefBusResult(const QString & orc_PathSystemDefinition,
                                             const stw_types::sint32 os32_Result) const
{
   this->mpc_Ui->pc_WiDatabase->OnSigOsySysDefBusResult(orc_PathSystemDefinition, os32_Result);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for communication change signal.

   \created     09.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::OnCommunicationStarted(const bool oq_Online) const
{
   this->mpc_Ui->pc_WiDatabase->OnCommunicationStarted(oq_Online);
   this->mpc_Ui->pc_WiDllConfig->OnCommunicationStarted(oq_Online);
   this->mpc_Ui->pc_WiLogging->OnCommunicationStarted(oq_Online);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to forward log file add result.

   \param[in]   os32_Result       result of log file add operation

   \created     17.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::OnSigLogFileAddResult(const stw_types::sint32 os32_Result) const
{
   this->mpc_Ui->pc_WiLogging->OnSigLogFileAddResult(os32_Result);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot to forward saved-as action.

   \created     05.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::OnSigSavedAsNew(void) const
{
   // forward signal to all widgets including paths which may be relative
   this->mpc_Ui->pc_WiDatabase->OnSigSavedAsNew();
   this->mpc_Ui->pc_WiLogging->OnSigSavedAsNew();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward database drop from main window.

   \param[in]     orc_Path        path of database file

   \created     06.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::OnDatabaseDropped(const QString & orc_Path) const
{
   this->mpc_Ui->pc_WiDatabase->AddDroppedDatabase(orc_Path);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clears the current configuration

   Databases and filter configuration is removed

   \created     14.02.2019  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::ClearConfiguration(void) const
{
   this->mpc_Ui->pc_WiDatabase->Clear();
   this->mpc_Ui->pc_WiFilter->Clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Expand settings by toggling header expand collapse button

   long description of function within several lines

   \param[in]     oq_Expand    true: expand settings subsections
                               false: collapse settings subsections

   \created     04.02.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::ExpandSettings(const bool oq_Expand) const
{
   this->mpc_Ui->pc_WiTitle->SetOpen(oq_Expand);
   // this toggles the ">>" button and therefore emits a signal; on this signal we connect and adapt the GUI
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten move event.

   Here: Handle popup move.

   \param[in,out]    opc_KeyEvent   Event identification and information

   \created     25.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::moveEvent(QMoveEvent * const opc_Event)
{
   if (this->mpc_PopupDatabase->isVisible() == true)
   {
      this->mpc_PopupDatabase->DoMove(
         this->mpc_Ui->pc_PbDatabase->mapToGlobal(this->mpc_Ui->pc_PbDatabase->rect().topLeft()));
   }
   else if (this->mpc_PopupDllConfig->isVisible() == true)
   {
      this->mpc_PopupDllConfig->DoMove(
         this->mpc_Ui->pc_PbDllConfig->mapToGlobal(this->mpc_Ui->pc_PbDllConfig->rect().topLeft()));
   }
   else if (this->mpc_PopupFilter->isVisible() == true)
   {
      this->mpc_PopupFilter->DoMove(
         this->mpc_Ui->pc_PbFilter->mapToGlobal(this->mpc_Ui->pc_PbFilter->rect().topLeft()));
   }
   else if (this->mpc_PopupLogging->isVisible() == true)
   {
      this->mpc_PopupLogging->DoMove(
         this->mpc_Ui->pc_PbLogging->mapToGlobal(this->mpc_Ui->pc_PbLogging->rect().topLeft()));
   }
   else
   {
      // do nothing
   }

   C_OgeWiOnlyBackground::moveEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle expand or collapse settings for subsections.

   \param[in]     oq_Expand        true: expand settings subsections
                                   false: collapse settings subsections

   \created     11.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
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
      default:
         break;
      }
   }

   Q_EMIT (this->SigExpandSettings(oq_Expand));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show database popup.

   \created     15.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopupDatabase(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->mpc_PopupDatabase->Show(
         this->mpc_Ui->pc_PbDatabase->mapToGlobal(this->mpc_Ui->pc_PbDatabase->rect().topLeft()));

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

//-----------------------------------------------------------------------------
/*!
   \brief   Show CAN DLL configuration popup.

   \created     15.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopupDllConfig(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->mpc_PopupDllConfig->Show(
         this->mpc_Ui->pc_PbDllConfig->mapToGlobal(this->mpc_Ui->pc_PbDllConfig->rect().topLeft()));

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
//-----------------------------------------------------------------------------
/*!
   \brief   Show filter popup.

   \created     15.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopupFilter(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->mpc_PopupFilter->Show(
         this->mpc_Ui->pc_PbFilter->mapToGlobal(this->mpc_Ui->pc_PbFilter->rect().topLeft()));

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
//-----------------------------------------------------------------------------
/*!
   \brief   Show logging popup.

   \created     15.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_CamMosWidget::m_ShowPopupLogging(const bool oq_Checked)
{
   if (oq_Checked == true)
   {
      this->mpc_PopupLogging->Show(
         this->mpc_Ui->pc_PbLogging->mapToGlobal(this->mpc_Ui->pc_PbLogging->rect().topLeft()));

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

//-----------------------------------------------------------------------------
/*!
   \brief   Check all popups for open one.

   \return
   opened popup enum or none if no popup is open

   \created     31.01.2019  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
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
