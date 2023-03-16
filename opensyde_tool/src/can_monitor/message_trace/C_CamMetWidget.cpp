//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing entire CAN trace and its buttons (implementation)

   Widget for showing entire CAN trace and its buttons

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamMetWidget.hpp"
#include "ui_C_CamMetWidget.h"

#include "stwerrors.hpp"
#include "C_Uti.hpp"
#include "C_CamOgeWiSectionHeader.hpp"
#include "C_GtGetText.hpp"
#include "C_OscSystemBus.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
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
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetWidget::C_CamMetWidget(QWidget * const opc_Parent) :
   C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMetWidget),
   mq_OsySysDef(false),
   ms32_CanBitrate(0)
{
   this->mpc_Ui->setupUi(this);

   this->SetBackgroundColor(11);

   this->mpc_Ui->pc_WiTitle->SetTitle(C_GtGetText::h_GetText("Trace"));
   this->mpc_Ui->pc_WiTitle->SetIconType(C_CamOgeWiSectionHeader::E_ButtonType::eNOBUTTON);

   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigStartLogging,
           this, &C_CamMetWidget::m_StartLogging);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigPauseLogging,
           this, &C_CamMetWidget::SigPauseLogging);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigStopLogging,
           this, &C_CamMetWidget::SigStopLogging);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigContinueLogging,
           this, &C_CamMetWidget::SigContinueLogging);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigClearData, this, &C_CamMetWidget::ClearData);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayAsHex, this->mpc_Ui->pc_TraceView,
           &C_CamMetTreeView::SetDisplayAsHex);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayTimestampRelative,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetDisplayTimestampRelative);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayTimestampTimeOfDay,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetDisplayTimestampAbsoluteTimeOfDay);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigTraceBufferSize,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetTraceBufferSize);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigChangeProtocol,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetProtocol);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayTree,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetDisplayTree);
   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigDisplayUniqueMessages,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SetDisplayUniqueMessages);

   connect(this->mpc_Ui->pc_ControlWidget, &C_CamMetControlBarWidget::SigSearchTrace,
           this->mpc_Ui->pc_TraceView, &C_CamMetTreeView::SearchTrace);

   // File loading
   connect(&this->mc_DatabaseTimer, &QTimer::timeout, this, &C_CamMetWidget::m_DatabaseTimer);
   this->mc_DatabaseTimer.setInterval(100);

   // Status bar update
   connect(&this->mc_StatusBarTimer, &QTimer::timeout, this, &C_CamMetWidget::m_StatusBarTimer);
   this->mc_StatusBarTimer.start(500);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMetWidget::~C_CamMetWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::LoadUserSettings(void)
{
   this->mpc_Ui->pc_ControlWidget->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save all user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::SaveUserSettings(void) const
{
   this->mpc_Ui->pc_ControlWidget->SaveUserSettings();
   this->mpc_Ui->pc_TraceView->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Stopping logging in case of an error
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::StopLogging(void)
{
   this->mpc_Ui->pc_ControlWidget->StopLogging();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Triggers focus on SearchBar
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::SearchBarFocus() const
{
   this->mpc_Ui->pc_ControlWidget->SearchBarFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Triggers searching the next match on trace
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::SearchNext(void) const
{
   this->mpc_Ui->pc_ControlWidget->SearchNext();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Triggers searching the previous match on trace
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::SearchPrev(void) const
{
   this->mpc_Ui->pc_ControlWidget->SearchPrev();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clears the previous communication
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::ClearData(void)
{
   this->mpc_Ui->pc_TraceView->ActionClearData();
   this->mpc_Ui->pc_StatusWidget->SetBusLoad(0U, this->ms32_CanBitrate);
   this->mpc_Ui->pc_StatusWidget->SetFilteredMessages(0U);

   // transmit information is resetted by data reset (C_OscComDriverBase::StopLogging)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds new filter configurations

   \param[in]  orc_FilterItems   Filter configurations to add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::AddFilterItems(const QList<C_CamProFilterItemData> & orc_FilterItems)
{
   QList<C_CamProFilterItemData>::const_iterator c_ItItem;

   for (c_ItItem = orc_FilterItems.begin(); c_ItItem != orc_FilterItems.end(); ++c_ItItem)
   {
      this->mpc_Ui->pc_TraceView->AddFilter(*c_ItItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes specific filter configurations

   \param[in]  orc_FilterItems   All filter configurations for removing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::RemoveFilterItems(const QList<C_CamProFilterItemData> & orc_FilterItems)
{
   QList<C_CamProFilterItemData>::const_iterator c_ItItem;

   for (c_ItItem = orc_FilterItems.begin(); c_ItItem != orc_FilterItems.end(); ++c_ItItem)
   {
      this->mpc_Ui->pc_TraceView->RemoveFilter(*c_ItItem);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes the entire filter configuration
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::RemoveAllFilters(void)
{
   this->mpc_Ui->pc_TraceView->RemoveAllFilter();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the status bar filter label

   \param[in]  ou32_ActiveFilters   Count of active filters
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::SetActiveFilters(const uint32_t ou32_ActiveFilters) const
{
   this->mpc_Ui->pc_StatusWidget->SetActiveFilters(ou32_ActiveFilters);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds a new openSYDE system definition

   \param[in]  orc_PathSystemDefinition   Path of system definition file (Must be .syde_sysdef)
   \param[in]  orc_OrgPath                Org path
   \param[in]  os32_BusIndex              Bus index of CAN bus of system definition for monitoring
                                          Set to -1 if no known yet
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::AddDatabaseOsySysDef(const QString & orc_PathSystemDefinition, const QString & orc_OrgPath,
                                          const int32_t os32_BusIndex)
{
   int32_t s32_Return;

   Q_UNUSED(orc_OrgPath)

   if (os32_BusIndex >= 0)
   {
      s32_Return = this->mpc_Ui->pc_TraceView->StartAddOsySysDef(orc_PathSystemDefinition.toStdString().c_str(),
                                                                 static_cast<uint32_t>(os32_BusIndex));
   }
   else
   {
      s32_Return = this->mpc_Ui->pc_TraceView->StartAddOsySysDef(orc_PathSystemDefinition.toStdString().c_str());
   }

   if (s32_Return == C_NO_ERR)
   {
      this->mq_OsySysDef = true;
      this->mc_DatabaseTimer.start(100);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds a new DBC file

   \param[in]  orc_PathDbc    Path of DBC file (Must be .dbc)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::AddDatabaseDbc(const QString & orc_PathDbc)
{
   const int32_t s32_Return = this->mpc_Ui->pc_TraceView->StartAddDbcFile(orc_PathDbc.toStdString().c_str());

   if (s32_Return == C_NO_ERR)
   {
      this->mq_OsySysDef = false;
      this->mc_DatabaseTimer.start(100);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Removes a database for interpretation

   \param[in]  orc_PathDatabase  Path with file name of database
   \param[in]  orc_OrgPath       Org path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::RemoveDatabase(const QString & orc_PathDatabase, const QString & orc_OrgPath)
{
   Q_UNUSED(orc_OrgPath)
   this->mpc_Ui->pc_TraceView->RemoveDatabase(orc_PathDatabase.toStdString().c_str());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Activates a database for interpretation

   \param[in]  orc_PathDatabase  Path with file name of database
   \param[in]  orc_OrgPath       Org path
   \param[in]  oq_Active         Flag if database shall be active or not for interpretation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::ActivateDatabase(const QString & orc_PathDatabase, const QString & orc_OrgPath,
                                      const bool oq_Active)
{
   Q_UNUSED(orc_OrgPath)
   this->mpc_Ui->pc_TraceView->ActivateDatabase(orc_PathDatabase.toStdString().c_str(), oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Updates the set bus index of the specific system definition

   This function is thread safe.

   The result of the function will be sent by signal SigDatabaseSetOsySysDefBusResult
   C_NO_ERR    Bus index for this system definition adapted
   C_NOACT     No system definition found with this path

   \param[in]  orc_PathSystemDefinition   Path of system definition file (Must be .syde_sysdef)
   \param[in]  orc_OrgPath                Org path
   \param[in]  ou32_BusIndex              Bus index of CAN bus of system definition for monitoring
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::SetDatabaseOsySysDefBus(const QString & orc_PathSystemDefinition, const QString & orc_OrgPath,
                                             const uint32_t ou32_BusIndex)
{
   int32_t s32_Return;

   Q_UNUSED(orc_OrgPath)

   s32_Return = this->mpc_Ui->pc_TraceView->SetOsySysDefBus(orc_PathSystemDefinition.toStdString().c_str(),
                                                            ou32_BusIndex);
   Q_EMIT (this->SigDatabaseSetOsySysDefBusResult(orc_PathSystemDefinition, s32_Return));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new ASC file logging.

   \param[in]  orc_FilePath   file path to log file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::AddLogFileAsc(const QString & orc_FilePath)
{
   const int32_t s32_Result =
      this->mpc_Ui->pc_TraceView->AddLogFileAsc(orc_FilePath.toStdString().c_str(),
                                                this->mpc_Ui->pc_TraceView->GetDisplayAsHex(),
                                                this->mpc_Ui->pc_TraceView->GetDisplayTimestampRelative());

   Q_EMIT (this->SigLogFileAddResult(s32_Result));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add new BLF file logging.

   \param[in]  orc_FilePath   file path to log file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::AddLogFileBlf(const QString & orc_FilePath)
{
   const int32_t s32_Result = this->mpc_Ui->pc_TraceView->AddLogFileBlf(orc_FilePath.toStdString().c_str());

   Q_EMIT (this->SigLogFileAddResult(s32_Result));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Remove all configured log files.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::RemoveAllLogFiles(void) const
{
   this->mpc_Ui->pc_TraceView->RemoveAllLogFiles();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the used message monitor

   \return
   Pointer to message logger
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui_logic::C_SyvComMessageMonitor * C_CamMetWidget::GetMessageMonitor(void) const
{
   return this->mpc_Ui->pc_TraceView;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Stops the logging
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::m_StartLogging(void)
{
   this->mpc_Ui->pc_TraceView->ActionClearData();

   Q_EMIT (this->SigStartLogging());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Timer function for updating status bar
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::m_StatusBarTimer(void)
{
   this->mpc_Ui->pc_StatusWidget->SetBusLoad(this->mpc_Ui->pc_TraceView->GetBusLoad(), this->ms32_CanBitrate);
   this->mpc_Ui->pc_StatusWidget->SetTransmittedMessages(this->mpc_Ui->pc_TraceView->GetTxCount());
   this->mpc_Ui->pc_StatusWidget->SetTxErrors(this->mpc_Ui->pc_TraceView->GetTxErrors());
   this->mpc_Ui->pc_StatusWidget->SetFilteredMessages(this->mpc_Ui->pc_TraceView->GetFilteredMessages());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks the progress of the current loading database file

   Starts and checks the thread for loading openSYDE system definition and DBC files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::m_DatabaseTimer(void)
{
   int32_t s32_ThreadResult;
   int32_t s32_Return;

   s32_Return = this->mpc_Ui->pc_TraceView->GetResults(s32_ThreadResult);

   if (s32_Return == C_NO_ERR)
   {
      // Nothing to do anymore
      this->mc_DatabaseTimer.stop();

      if (this->mq_OsySysDef == false)
      {
         // DBC file
         Q_EMIT (this->SigDatabaseLoadResultDbc(s32_ThreadResult));
      }
      else
      {
         std::vector<stw::opensyde_core::C_OscSystemBus> c_Busses;
         this->mpc_Ui->pc_TraceView->GetResultBusses(c_Busses);

         // openSYDE system definition
         Q_EMIT (this->SigDatabaseLoadResultOsySysDef(s32_ThreadResult, c_Busses));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set bitrate

   \param[in]  os32_Value  New bitrate
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMetWidget::SetCanBitrate(const int32_t os32_Value)
{
   ms32_CanBitrate = os32_Value;
}
