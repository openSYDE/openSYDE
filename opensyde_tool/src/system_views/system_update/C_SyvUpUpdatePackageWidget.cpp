//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the update package UI

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include <QSizePolicy>

#include "C_SyvUpUpdatePackageWidget.h"
#include "ui_C_SyvUpUpdatePackageWidget.h"

#include "stwerrors.h"

#include "C_OgeWiCustomMessage.h"
#include "C_GtGetText.h"
#include "constants.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageWidget::C_SyvUpUpdatePackageWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpUpdatePackageWidget)
{
   QSizePolicy c_SizePolicy;

   // Init UI
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_PushButtonClearAll->SetIconSize(24);
   this->mpc_Ui->pc_PushButtonImport->SetIconSize(24);
   this->mpc_Ui->pc_PushButtonExport->SetIconSize(24);
   this->mpc_Ui->pc_PushButtonCreatePackage->SetIconSize(24);

   this->mpc_Ui->pc_PushButtonScrollLeft->setIcon(QIcon("://images/IconScrollLeftEnabled.svg"));
   this->mpc_Ui->pc_PushButtonScrollRight->setIcon(QIcon("://images/IconScrollRightEnabled.svg"));

   this->mc_MissingDataBlocks = QStringList("dummy");
   this->mc_MissingParamFiles = QStringList("dummy");
   this->mc_MissingFiles = QStringList("dummy");
   this->mc_FlashwareWarningsApps = QStringList("dummy");
   this->m_SetUpdatePackageStatusNotLocked(QStringList(), QStringList(), QStringList(), QStringList());

   connect(this->mpc_Ui->pc_PushButtonScrollLeft, &stw_opensyde_gui_elements::C_OgePubIconOnly::clicked,
           this, &C_SyvUpUpdatePackageWidget::m_ButtonLeftClicked);
   connect(this->mpc_Ui->pc_PushButtonScrollRight, &stw_opensyde_gui_elements::C_OgePubIconOnly::clicked,
           this, &C_SyvUpUpdatePackageWidget::m_ButtonRightClicked);
   connect(this->mpc_Ui->pc_ListWidget, &C_SyvUpUpdatePackageListWidget::SigListChanged,
           this, &C_SyvUpUpdatePackageWidget::m_UpdateWidget);

   connect(this->mpc_Ui->pc_PushButtonClearAll, &stw_opensyde_gui_elements::C_OgePubIconText::clicked,
           this, &C_SyvUpUpdatePackageWidget::m_ButtonClearAll);
   connect(this->mpc_Ui->pc_PushButtonExport, &stw_opensyde_gui_elements::C_OgePubIconText::clicked,
           this, &C_SyvUpUpdatePackageWidget::m_ButtonExport);
   connect(this->mpc_Ui->pc_PushButtonImport, &stw_opensyde_gui_elements::C_OgePubIconText::clicked,
           this, &C_SyvUpUpdatePackageWidget::m_ButtonImport);
   connect(this->mpc_Ui->pc_PushButtonCreatePackage, &stw_opensyde_gui_elements::C_OgePubIconText::clicked,
           this, &C_SyvUpUpdatePackageWidget::m_ButtonCreatePackage);

   connect(this->mpc_Ui->pc_ListWidget, &C_SyvUpUpdatePackageListWidget::SigStartCheck,
           this, &C_SyvUpUpdatePackageWidget::m_StartCheck);

   // Timer configuration
   //lint -e{1938}  static const is guaranteed preinitialized before main
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SyvUpUpdatePackageWidget::m_FileCheckTimer);

   this->m_StartCheck(true);

   // reserve space even if widgets are hidden
   c_SizePolicy = this->mpc_Ui->pc_PushButtonScrollLeft->sizePolicy();
   c_SizePolicy.setRetainSizeWhenHidden(true);
   this->mpc_Ui->pc_PushButtonScrollLeft->setSizePolicy(c_SizePolicy);
   this->mpc_Ui->pc_PushButtonScrollRight->setSizePolicy(c_SizePolicy);

   c_SizePolicy = this->mpc_Ui->pc_IndexViewWidget->sizePolicy();
   c_SizePolicy.setRetainSizeWhenHidden(true);
   this->mpc_Ui->pc_IndexViewWidget->setSizePolicy(c_SizePolicy);

   c_SizePolicy = this->mpc_Ui->pc_LabelStateIcon->sizePolicy();
   c_SizePolicy.setRetainSizeWhenHidden(true);
   this->mpc_Ui->pc_LabelStateIcon->setSizePolicy(c_SizePolicy);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpUpdatePackageWidget::~C_SyvUpUpdatePackageWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initializes all visible strings on the widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::InitText(void) const
{
   this->mpc_Ui->pc_LabelTitle->setText(
      QString(C_GtGetText::h_GetText("UPDATE PACKAGE (%1)")).arg(QString::number(this->mpc_Ui->pc_ListWidget->count())));

   this->mpc_Ui->pc_PushButtonClearAll->SetToolTipInformation(C_GtGetText::h_GetText("Clear Update Package"),
                                                              C_GtGetText::h_GetText(
                                                                 "Clear content of complete Update Package."));
   this->mpc_Ui->pc_PushButtonImport->SetToolTipInformation(C_GtGetText::h_GetText("Import Update Package"),
                                                            C_GtGetText::h_GetText(
                                                               "Import Update Package configuration from file."));

   this->mpc_Ui->pc_PushButtonExport->SetToolTipInformation(C_GtGetText::h_GetText("Export Update Package to File"),
                                                            C_GtGetText::h_GetText(
                                                               "Export Update Package configuration to file."));

   this->mpc_Ui->pc_PushButtonCreatePackage->SetToolTipInformation(
      C_GtGetText::h_GetText("Create Service Update Package"),
      C_GtGetText::h_GetText("All required files are packed together (SYSTEM DEFINITION, flashware, ... )"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the view index and initializes the nodes in the list

   \param[in]     ou32_ViewIndex         View index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetViewIndex(const uint32 ou32_ViewIndex)
{
   this->mpc_Ui->pc_ListWidget->SetViewIndex(ou32_ViewIndex);

   this->m_UpdateWidget();

   this->InitText();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the connected state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetConnected(void)
{
   this->mc_Timer.stop();

   this->mpc_Ui->pc_ListWidget->SetConnected();

   // Hide icon
   this->mpc_Ui->pc_LabelStateIcon->setVisible(false);

   // Disable all buttons
   this->mpc_Ui->pc_PushButtonClearAll->setEnabled(false);
   this->mpc_Ui->pc_PushButtonExport->setEnabled(false);
   this->mpc_Ui->pc_PushButtonImport->setEnabled(false);
   this->mpc_Ui->pc_PushButtonCreatePackage->setEnabled(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the updated was started
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetUpdateStarted(void) const
{
   this->mpc_Ui->pc_LabelStateIcon->setVisible(false);

   this->mpc_Ui->pc_ListWidget->SetUpdateStarted();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update was started

   \param[in]     ou32_NodeIndex         Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetUpdateApplicationStarted(const uint32 ou32_NodeIndex) const
{
   this->mpc_Ui->pc_ListWidget->SetUpdateApplicationStarted(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update was finished

   \param[in]     ou32_NodeIndex         Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetUpdateApplicationFinished(const uint32 ou32_NodeIndex) const
{
   this->mpc_Ui->pc_ListWidget->SetUpdateApplicationFinished(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the application update hat an error

   \param[in]     ou32_NodeIndex         Index of node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetUpdateApplicationError(const uint32 ou32_NodeIndex) const
{
   this->mpc_Ui->pc_ListWidget->SetUpdateApplicationError(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle trigger for application information discard

   \param[in] ou32_NodeIndex Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::DiscardApplicationStatus(const uint32 ou32_NodeIndex) const
{
   this->mpc_Ui->pc_ListWidget->DiscardApplicationStatus(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget when the entire update was finished
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetUpdateFinished(void) const
{
   this->mpc_Ui->pc_ListWidget->SetUpdateFinished();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapts the widget for the disconnected state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetDisconnected(void)
{
   this->mpc_Ui->pc_ListWidget->SetDisconnected();

   this->m_StartCheck(true);

   // Disable all buttons
   this->mpc_Ui->pc_PushButtonClearAll->setEnabled(true);
   this->mpc_Ui->pc_PushButtonExport->setEnabled(true);
   this->mpc_Ui->pc_PushButtonImport->setEnabled(true);
   this->mpc_Ui->pc_PushButtonCreatePackage->setEnabled(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update device information

   \param[in] orc_NodeIndexes       Node indices
   \param[in] orc_DeviceInformation Device info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::UpdateDeviceInformation(const std::vector<uint32> & orc_NodeIndexes,
                                                         const std::vector<C_SyvUpDeviceInfo> & orc_DeviceInformation)
const
{
   this->mpc_Ui->pc_ListWidget->UpdateDeviceInformation(orc_NodeIndexes, orc_DeviceInformation);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the progress for a specific node

   \param[in]     ou32_NodeIndex         Index of node
   \param[in]     ou8_Progress           Entire progress of node of update process
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::SetNodeProgress(const uint32 ou32_NodeIndex, const uint8 ou8_Progress) const
{
   this->mpc_Ui->pc_ListWidget->SetNodeProgress(ou32_NodeIndex, ou8_Progress);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Creates and returns the update package with all information for the system update

   \param[out]    orc_ApplicationsToWrite Vector with node update configuration
   \param[out]    orc_NodesOrder          Vector with node update order (index is update position, value is node index)
   \param[out]    opc_AllApplications     Optional vector with all node applications

   \return
   C_NO_ERR    Update package with all information created
   C_NOACT     No nodes for updating
   C_CONFIG    Error in system definition
   C_RD_WR     At least one file does not exist
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SyvUpUpdatePackageWidget::GetUpdatePackage(
   std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
   std::vector<uint32> & orc_NodesOrder,
   std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> * const opc_AllApplications) const
{
   return this->mpc_Ui->pc_ListWidget->GetUpdatePackage(orc_ApplicationsToWrite, orc_NodesOrder,
                                                        opc_AllApplications);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_ButtonRightClicked(void) const
{
   this->mpc_Ui->pc_ListWidget->SetActualLine(this->mpc_Ui->pc_ListWidget->GetActualLine() + 1);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_ButtonLeftClicked(void) const
{
   this->mpc_Ui->pc_ListWidget->SetActualLine(this->mpc_Ui->pc_ListWidget->GetActualLine() - 1);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_ButtonClearAll(void) const
{
   C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
   C_OgeWiCustomMessage c_MessageBox(this->parentWidget(), C_OgeWiCustomMessage::E_Type::eQUESTION);
   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Update Package clear"));
   c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                  "Do you really want to clear the configuration of Update Package?"));
   c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Clear"));
   c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
   e_ReturnMessageBox = c_MessageBox.Execute();

   if (e_ReturnMessageBox == C_OgeWiCustomMessage::eYES)
   {
      this->mpc_Ui->pc_ListWidget->RemoveAllFiles();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_ButtonImport(void) const
{
   this->mpc_Ui->pc_ListWidget->ImportConfig();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_ButtonExport(void) const
{
   this->mpc_Ui->pc_ListWidget->ExportConfig();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_ButtonCreatePackage(void) const
{
   this->mpc_Ui->pc_ListWidget->CreateServiceUpdatePackage();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_UpdateWidget(void)
{
   if (this->mpc_Ui->pc_ListWidget->GetCountLines() > 1)
   {
      // not all items are visible at the same time
      // adapt the ui
      // show the arrows only if the item is active
      this->mpc_Ui->pc_PushButtonScrollLeft->setVisible(true);
      this->mpc_Ui->pc_PushButtonScrollRight->setVisible(true);

      if (this->mpc_Ui->pc_ListWidget->GetActualLine() < 1)
      {
         this->mpc_Ui->pc_PushButtonScrollLeft->setVisible(false);
         this->mpc_Ui->pc_PushButtonScrollRight->setEnabled(true);
      }
      else if (this->mpc_Ui->pc_ListWidget->GetActualLine() >= (this->mpc_Ui->pc_ListWidget->GetCountLines() - 1))
      {
         // position is at the end of the list
         this->mpc_Ui->pc_PushButtonScrollLeft->setEnabled(true);
         this->mpc_Ui->pc_PushButtonScrollRight->setVisible(false);
      }
      else
      {
         this->mpc_Ui->pc_PushButtonScrollLeft->setEnabled(true);
         this->mpc_Ui->pc_PushButtonScrollRight->setEnabled(true);
      }

      // update the index view
      this->mpc_Ui->pc_IndexViewWidget->setVisible(true);
      this->mpc_Ui->pc_IndexViewWidget->SetColor(mc_STYLE_GUIDE_COLOR_4, mc_STYLE_GUIDE_COLOR_10);

      this->mpc_Ui->pc_IndexViewWidget->SetCountIndex(this->mpc_Ui->pc_ListWidget->GetCountLines());
      this->mpc_Ui->pc_IndexViewWidget->SetCurrentIndex(this->mpc_Ui->pc_ListWidget->GetActualLine());
   }
   else
   {
      // all items are visible at the same time
      this->mpc_Ui->pc_PushButtonScrollLeft->setVisible(false);
      this->mpc_Ui->pc_PushButtonScrollRight->setVisible(false);
      this->mpc_Ui->pc_IndexViewWidget->setVisible(false);
   }

   this->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking all files for existence
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_FileCheckTimer(void)
{
   uint32 u32_CountFiles = 0U;

   QStringList c_FlashwareWarningsApps;
   QStringList c_MissingDataBlocks;
   QStringList c_MissingParamFiles;
   QStringList c_MissingFiles;
   sint32 s32_Return =
      this->mpc_Ui->pc_ListWidget->CheckAllPaths(u32_CountFiles, &c_FlashwareWarningsApps, &c_MissingDataBlocks,
                                                 &c_MissingParamFiles, &c_MissingFiles);

   if (s32_Return == C_NO_ERR)
   {
      if ((this->mpc_Ui->pc_ListWidget->count() == 0) || (u32_CountFiles == 0))
      {
         // No nodes for updating
         s32_Return = C_NOACT;

         this->mpc_Ui->pc_PushButtonImport->setEnabled(false);
         this->mpc_Ui->pc_PushButtonExport->setEnabled(false);
         this->mpc_Ui->pc_PushButtonClearAll->setEnabled(false);

         this->mpc_Ui->pc_LabelStateIcon->setVisible(false);
      }
      else
      {
         this->mpc_Ui->pc_PushButtonImport->setEnabled(true);
         this->mpc_Ui->pc_PushButtonExport->setEnabled(true);
         this->mpc_Ui->pc_PushButtonClearAll->setEnabled(true);

         this->mpc_Ui->pc_LabelStateIcon->setVisible(true);
         this->m_SetUpdatePackageStatusNotLocked(QStringList(), QStringList(), QStringList(), c_FlashwareWarningsApps);
      }
   }
   else
   {
      this->mpc_Ui->pc_PushButtonImport->setEnabled(true);
      this->mpc_Ui->pc_PushButtonExport->setEnabled(true);
      this->mpc_Ui->pc_PushButtonClearAll->setEnabled(true);

      this->mpc_Ui->pc_LabelStateIcon->setVisible(true);
      this->m_SetUpdatePackageStatusNotLocked(c_MissingDataBlocks, c_MissingParamFiles, c_MissingFiles,
                                              c_FlashwareWarningsApps);
   }

   this->mpc_Ui->pc_PushButtonCreatePackage->setEnabled(s32_Return == C_NO_ERR);

   Q_EMIT (this->SigUpdatePackageState(s32_Return));

   this->mc_Timer.setInterval(mhu32_FILE_CHECK_INTERVAL);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_StartCheck(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->mc_Timer.start(25);
   }
   else
   {
      this->mc_Timer.stop();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set update package status to: warning or OK

   \param[in]     orc_MissingDataBlocks      List of missing data block files (including applications)
   \param[in]     orc_MissingParamFiles      List of missing parameter set image files (including applications)
   \param[in]     orc_MissingFiles           List of missing files (including applications)
   \param[in]     orc_FlashwareWarningsApps  List of apps with flashware warnings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpUpdatePackageWidget::m_SetUpdatePackageStatusNotLocked(const QStringList & orc_MissingDataBlocks,
                                                                   const QStringList & orc_MissingParamFiles,
                                                                   const QStringList & orc_MissingFiles,
                                                                   const QStringList & orc_FlashwareWarningsApps)
{
   // only update if something changed
   if ((this->mc_MissingDataBlocks != orc_MissingDataBlocks) || (this->mc_MissingParamFiles != orc_MissingParamFiles) ||
       (this->mc_MissingFiles != orc_MissingFiles) || (this->mc_FlashwareWarningsApps != orc_FlashwareWarningsApps))
   {
      QString c_LabelText = "";
      QString c_TooltipHeading;
      QString c_TooltipContent = "";

      this->mc_MissingDataBlocks = orc_MissingDataBlocks;
      this->mc_MissingParamFiles = orc_MissingParamFiles;
      this->mc_MissingFiles = orc_MissingFiles;
      this->mc_FlashwareWarningsApps = orc_FlashwareWarningsApps;

      // missing apps
      if ((orc_MissingDataBlocks.length() > 0) || (orc_MissingParamFiles.length() > 0) ||
          (orc_MissingFiles.length() > 0))
      {
         const sint32 s32_TotalMissingFiles =
            static_cast<sint32>(orc_MissingDataBlocks.length() + orc_MissingParamFiles.length() +
                                orc_MissingFiles.length());
         c_TooltipHeading = C_GtGetText::h_GetText("Update Package Invalid");
         if (orc_MissingDataBlocks.length() > 0)
         {
            c_TooltipContent += C_GtGetText::h_GetText("Missing Data Block files:\n");
            c_TooltipContent += orc_MissingDataBlocks.join("\n");
         }
         if (orc_MissingParamFiles.length() > 0)
         {
            //Handle previous content spacing
            if (c_TooltipContent.isEmpty() == false)
            {
               c_TooltipContent += "\n\n";
            }
            c_TooltipContent += C_GtGetText::h_GetText("Missing Parameter set image files:\n");
            c_TooltipContent += orc_MissingParamFiles.join("\n");
         }
         if (orc_MissingFiles.length() > 0)
         {
            //Handle previous content spacing
            if (c_TooltipContent.isEmpty() == false)
            {
               c_TooltipContent += "\n\n";
            }
            c_TooltipContent += C_GtGetText::h_GetText("Missing files:\n");
            c_TooltipContent += orc_MissingFiles.join("\n");
         }
         if (s32_TotalMissingFiles == 1)
         {
            c_LabelText = C_GtGetText::h_GetText("One file is missing!");
         }
         else
         {
            c_LabelText = QString::number(s32_TotalMissingFiles) + C_GtGetText::h_GetText(
               " files are missing!");
         }
         this->mpc_Ui->pc_LabelStateIcon->SetSvg("://images/system_views/IconWarning.svg");
      }
      else
      {
         c_TooltipHeading = C_GtGetText::h_GetText("Update Package Complete");
         c_TooltipContent = C_GtGetText::h_GetText("Update Package is complete and valid.");
         this->mpc_Ui->pc_LabelStateIcon->SetSvg("://images/system_views/IconOk.svg");
      }

      // flashware warnings
      if (orc_FlashwareWarningsApps.size() > 0)
      {
         c_LabelText += C_GtGetText::h_GetText(" Flashware warnings occured!");
         c_TooltipContent += C_GtGetText::h_GetText("\n\nFlashware warnings:\n");
         c_TooltipContent += orc_FlashwareWarningsApps.join("\n");
      }

      this->mpc_Ui->pc_LabelState->setText(c_LabelText);
      this->mpc_Ui->pc_LabelState->SetToolTipInformation(c_TooltipHeading, c_TooltipContent);
      this->mpc_Ui->pc_LabelStateIcon->SetToolTipInformation(c_TooltipHeading, c_TooltipContent);
   }
}
