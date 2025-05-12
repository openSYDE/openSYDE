//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition user

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>
#include <QFileInfo>
#include "C_PopErrorHandling.hpp"
#include "C_PuiProject.hpp"
#include "C_SdHandlerWidget.hpp"
#include "ui_C_SdHandlerWidget.h"
#include "C_OgeWiCustomMessage.hpp"
#include "C_OscNode.hpp"

#include "constants.hpp"
#include "TglUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_HeHandler.hpp"
#include "C_UsHandler.hpp"
#include "C_ImpUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_CieUtil.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_CieDataPoolListAdapter.hpp"
#include "C_SdBueBusEditWidget.hpp"
#include "stwerrors.hpp"
#include "C_RtfExportWidget.hpp"
#include "C_PopUtil.hpp"
#include "C_PuiUtil.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_SdCodeGenerationDialog.hpp"
#include "C_SdBueImportCommMessagesWidget.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PopSaveAsDialogWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_APPLY = 0U;
const uint32_t C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_GENERATE_CODE = 1U;
const uint32_t C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_IMPORT = 2U;
const uint32_t C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_EXPORT = 3U;
const uint32_t C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_RTF_EXPORT = 4U;
const uint32_t C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_TSP_IMPORT = 5U;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdHandlerWidget::C_SdHandlerWidget(QWidget * const opc_Parent) :
   C_NagUseCaseWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdHandlerWidget),
   mpc_Topology(NULL),
   mpc_ActNodeEdit(NULL),
   mpc_ActBusEdit(NULL),
   ms32_NodeEditTabIndex(0),
   ms32_BusEditTabIndex(0),
   ms32_SubMode(-1),
   mu32_Index(0U),
   mq_DataChanged(false),
   mc_TooltipGenerateCodeHeading(C_GtGetText::h_GetText("Generate Files")),
   mc_TooltipGenerateCodeContentSysdef(C_GtGetText::h_GetText(
                                          "Generate files for all nodes with file generation support based on defined SYSTEM DEFINITION."
                                          "\n - openSYDE server initialization wrapper"
                                          "\n   Create source code providing initialization structures for the OSS DPD and DPH init functions."
                                          "\n - COMM stack definition"
                                          "\n   Create source code providing entire communication stack configuration."
                                          "\n - Datapools"
                                          "\n   Create source code for Datapool settings of an openSYDE node."
                                          "\n - NVM-based Hardware Configuration"
                                          "\n   Generate parameter set image file(s) for hardware configuration of an openSYDE node.")),
   mc_TooltipGenerateCodeContentNode(C_GtGetText::h_GetText(
                                        "Generate files for current node based on defined SYSTEM DEFINITION."
                                        "\n - openSYDE server initialization wrapper"
                                        "\n   Create source code providing initialization structures for the OSS DPD and DPH init functions."
                                        "\n - COMM stack definition"
                                        "\n   Create source code providing entire communication stack configuration."
                                        "\n - Datapools"
                                        "\n   Create source code for Datapool settings of current openSYDE node."
                                        "\n - NVM-based Hardware Configuration"
                                        "\n   Generate parameter set image file(s) for hardware configuration of current openSYDE node."))
{
   int32_t s32_Index;

   mpc_Ui->setupUi(this);

   // create the topology
   this->mpc_Topology = new C_SdTopologyWidget(this);
   this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_Topology);
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigChanged, this, &C_SdHandlerWidget::m_DataChanged);
   // forwarding of this signal
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigChangeMode, this, &C_SdHandlerWidget::SigChangeMode);
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigOpenTsp, this, &C_SdHandlerWidget::m_TspImportForNewNode);

   connect(this->mpc_Topology, &C_SdTopologyWidget::SigNodeChanged, this, &C_SdHandlerWidget::m_NodeChanged);
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigBusChanged, this, &C_SdHandlerWidget::m_BusChanged);
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigErrorChange, this,
           &C_SdHandlerWidget::m_ErrorChange);

   s32_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_Topology);
   this->mpc_Ui->pc_VerticalLayout->setStretch(s32_Index, 1);

   this->ms32_NodeEditTabIndex = C_UsHandler::h_GetInstance()->GetProjLastSysDefNodeTabIndex();
   this->ms32_BusEditTabIndex = C_UsHandler::h_GetInstance()->GetProjLastSysDefBusTabIndex();

   // configure toolbar functions
   // the order of adding the function is very important
   C_NagToolBarButtonProperties c_ButtonProperties;

   // Function index 0: mhu32_USER_INPUT_FUNC_APPLY
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Save Project");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Save Project");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Save project data to file.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 1: mhu32_USER_INPUT_FUNC_GENERATE_CODE
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Generate Files");
   c_ButtonProperties.c_ToolTipHeading = mc_TooltipGenerateCodeHeading;
   c_ButtonProperties.c_ToolTipContent = mc_TooltipGenerateCodeContentSysdef;
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 2: mhu32_USER_INPUT_FUNC_IMPORT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Import Messages");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Import Messages");
   c_ButtonProperties.c_ToolTipContent =
      C_GtGetText::h_GetText("Import messages and signals from DBC, EDS or DCF file.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 3: mhu32_USER_INPUT_FUNC_EXPORT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("DBC File Export");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("DBC File Export");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText(
      "Export messages and signals of current bus in standard DBC file format.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 4: mhu32_USER_INPUT_FUNC_RTF_EXPORT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Report");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Report");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Export the SYSTEM DEFINITION as RTF document file.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 5: mhu32_USER_INPUT_FUNC_TSP_IMPORT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Import TSP");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Import TSP");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText(
      "Import node configuration from openSYDE Target Support Package (*.syde_tsp)");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent all elements and the Qt memory management
C_SdHandlerWidget::~C_SdHandlerWidget()
{
   C_UsHandler::h_GetInstance()->SetProjLastSysDefNodeTabIndex(this->ms32_NodeEditTabIndex);
   C_UsHandler::h_GetInstance()->SetProjLastSysDefBusTabIndex(this->ms32_BusEditTabIndex);

   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to set the parent of the widget

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::SetParentHook(QWidget * const opc_Parent)
{
   if (this->mpc_Topology != NULL)
   {
      this->mpc_Topology->SetParentHook(opc_Parent);
   }

   C_NagUseCaseWidget::SetParentHook(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for toolbar

   Saves the project.

   \param[in]  ou32_FuncNumber   Number of function
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::UserInputFunc(const uint32_t ou32_FuncNumber)
{
   switch (ou32_FuncNumber)
   {
   case mhu32_USER_INPUT_FUNC_APPLY:
      this->Save();
      break;
   case mhu32_USER_INPUT_FUNC_GENERATE_CODE:
      //Trigger CRC update
      if (this->mpc_Topology != NULL)
      {
         this->mpc_Topology->SaveToData();
      }
      this->m_GenerateCode();
      break;
   case mhu32_USER_INPUT_FUNC_IMPORT:
      this->m_TriggerImport();
      break;
   case mhu32_USER_INPUT_FUNC_EXPORT:
      this->m_Export();
      break;
   case mhu32_USER_INPUT_FUNC_RTF_EXPORT:
      //Trigger CRC update
      if (this->mpc_Topology != NULL)
      {
         this->mpc_Topology->SaveToData();
      }
      this->m_RtfExport();
      break;
   case mhu32_USER_INPUT_FUNC_TSP_IMPORT:
      this->m_TspImport(false);
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to save data

   Saves the system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::Save(void)
{
   const uint16_t u16_Timer = osc_write_log_performance_start();

   QApplication::setOverrideCursor(Qt::WaitCursor);

   // generate HALC Datapools
   this->m_GenerateHalcDatapools();

   // save all changes of the active edit widgets to the core
   if (this->mpc_ActNodeEdit != NULL)
   {
      this->mpc_ActNodeEdit->Save();
   }
   if (this->mpc_ActBusEdit != NULL)
   {
      this->mpc_ActBusEdit->Save();
   }
   if (this->mpc_Topology != NULL)
   {
      this->mpc_Topology->SaveToData();
   }

   // check names
   if (C_PopUtil::h_CheckCriticalNamingConflict(this, true) == false)
   {
      //Check if valid save
      if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
      {
         // open project save
         this->SaveAs();
      }
      else
      {
         //Save to file
         C_PopErrorHandling::h_ProjectSaveErr(C_PuiProject::h_GetInstance()->Save(), this);

         Q_EMIT this->SigDataChanged(false, true, ms32_MODE_SYSDEF);

         this->mq_DataChanged = false;
      }
   }
   QApplication::restoreOverrideCursor();
   osc_write_log_performance_stop(u16_Timer, "System Definition Save");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to open a concrete datapool, datapool list or dataelement

   \param[in]  os32_Index           First index
   \param[in]  os32_SubIndex        Second index
   \param[in]  os32_SubSubIndex     Third index
   \param[in]  os32_SubSubSubIndex  Fourth index
   \param[in]  os32_Flag            Optional flag for further information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::OpenDetail(const int32_t os32_Index, const int32_t os32_SubIndex,
                                   const int32_t os32_SubSubIndex, const int32_t os32_SubSubSubIndex,
                                   const int32_t os32_Flag)
{
   if ((this->ms32_SubMode == ms32_SUBMODE_SYSDEF_NODEEDIT) &&
       (this->mpc_ActNodeEdit != NULL))
   {
      this->mpc_ActNodeEdit->OpenDetail(os32_Index, os32_SubIndex, os32_SubSubIndex, os32_Flag);
   }
   else if ((this->ms32_SubMode == ms32_SUBMODE_SYSDEF_BUSEDIT) &&
            (this->mpc_ActBusEdit != NULL))
   {
      this->mpc_ActBusEdit->OpenDetail(os32_Index, os32_SubIndex, os32_SubSubIndex, os32_SubSubSubIndex, os32_Flag);
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to prepare closing the widget

   Updating of the system definition without saving it into the files

   \return
   true     Preparation successful. Can be closed.
   false    Preparation not finished. Can not be closed.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdHandlerWidget::PrepareToClose(void)
{
   // generate HALC Datapools
   this->m_GenerateHalcDatapools();

   // save all changes of the active edit widgets to the core
   if (this->mpc_ActNodeEdit != NULL)
   {
      // save the tab index
      this->ms32_NodeEditTabIndex = this->mpc_ActNodeEdit->GetTabIndex();
      this->mpc_ActNodeEdit->Save();
   }
   if (this->mpc_ActBusEdit != NULL)
   {
      // save the tab index
      this->ms32_BusEditTabIndex = this->mpc_ActBusEdit->GetTabIndex();
      this->mpc_ActBusEdit->Save();
   }
   if (this->mpc_Topology != NULL)
   {
      this->mpc_Topology->PrepareToClose();
   }

   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the actual mode of the system definition handler

   Do not call this function internally. Use the signal SigChangeMode to
   inform the entire application about the change.

   \param[in]  os32_SubMode   Actual sub mode
   \param[in]  ou32_Index     Index for node or bus
   \param[in]  ou32_Flag      Flag for special functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::SetSubMode(const int32_t os32_SubMode, const uint32_t ou32_Index, const uint32_t ou32_Flag)
{
   const QIcon c_IconSave(":images/IconSave.svg");
   const QIcon c_IconCompile("://images/system_definition/IconGenerateCode.svg");
   const QIcon c_IconExport("://images/IconExportDbc.svg");
   const QIcon c_IconImport("://images/IconImportFile.svg");
   const QIcon c_IconRtfExport("://images/IconExportRtf.svg");
   const QIcon c_IconTspImport("://images/IconImportFile");

   // set the icons. this can not be done in constructor
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, c_IconSave));
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE, c_IconCompile));
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_IMPORT, c_IconImport));
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_EXPORT, c_IconExport));
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_RTF_EXPORT, c_IconRtfExport));
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_TSP_IMPORT, c_IconTspImport));

   // if the submode is topology it can be the initial call or a kind of refresh because
   // of the toolbar, so do it always
   if ((os32_SubMode == ms32_SUBMODE_SYSDEF_TOPOLOGY) ||
       ((os32_SubMode != this->ms32_SubMode) ||
        (ou32_Index != this->mu32_Index)))
   {
      // delete or remove from the layout the other widgets if necessary
      if (this->mpc_ActNodeEdit != NULL)
      {
         this->mpc_Ui->pc_VerticalLayout->removeWidget(this->mpc_ActNodeEdit);

         // save changes
         if (this->mpc_ActNodeEdit->WasChanged() == true)
         {
            this->mpc_ActNodeEdit->Save();
         }
         // save the tab index
         this->ms32_NodeEditTabIndex = this->mpc_ActNodeEdit->GetTabIndex();

         // remove widget
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigChanged, this,
                    &C_SdHandlerWidget::m_DataChanged);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigErrorChange, this,
                    &C_SdHandlerWidget::m_ErrorChange);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBus, this,
                    &C_SdHandlerWidget::m_SwitchToBus);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBusProtocol, this,
                    &C_SdHandlerWidget::m_SwitchToBusProtocol);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBusProtocolMessage, this,
                    &C_SdHandlerWidget::m_SwitchToBusProtocolMessage);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToDeviceNodeInCoManager, this,
                    &C_SdHandlerWidget::m_SwitchToDeviceNodeInCoManager);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigNameChanged, this,
                    &C_SdHandlerWidget::SigNameChanged);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSave, this,
                    &C_SdHandlerWidget::Save);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSaveAs, this,
                    &C_SdHandlerWidget::SaveAs);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBusProperties, this,
                    &C_SdHandlerWidget::m_SwitchToBusProperties);
         delete this->mpc_ActNodeEdit;
         this->mpc_ActNodeEdit = NULL;
      }
      if (this->mpc_ActBusEdit != NULL)
      {
         const stw::opensyde_core::C_OscSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOscBus(this->mu32_Index);

         this->mpc_Ui->pc_VerticalLayout->removeWidget(this->mpc_ActBusEdit);

         // save changes
         if (this->mpc_ActBusEdit->WasChanged() == true)
         {
            this->mpc_ActBusEdit->Save();
         }
         // save the tab index
         this->ms32_BusEditTabIndex = this->mpc_ActBusEdit->GetTabIndex();

         // show button for DBC file export and messages import
         if (pc_Bus->e_Type == C_OscSystemBus::E_Type::eCAN) // Ethernet is not supported yet
         {
            Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_IMPORT, true));
            Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_EXPORT, true));
         }

         // remove widget
         disconnect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigChanged, this,
                    &C_SdHandlerWidget::m_DataChanged);
         disconnect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigErrorChange, this,
                    &C_SdHandlerWidget::m_ErrorChange);
         disconnect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigNameChanged, this,
                    &C_SdHandlerWidget::SigNameChanged);
         disconnect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigSwitchToCoManager, this,
                    &C_SdHandlerWidget::m_SwitchToCoManager);

         delete this->mpc_ActBusEdit;
         this->mpc_ActBusEdit = NULL;
      }

      //Clean up buttons
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE, false));
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_IMPORT, false));
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_EXPORT, false));
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_RTF_EXPORT, false));
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_TSP_IMPORT, false));

      //Set indices as soon as possible (after clean up) to have additional checks directly compare with the current
      // indices to avoid calling the system definition change more than once on rapid use-case change calls
      // from this point onward the indices may only be used for the new use-case
      this->ms32_SubMode = os32_SubMode;
      this->mu32_Index = ou32_Index;

      if (os32_SubMode == ms32_SUBMODE_SYSDEF_TOPOLOGY)
      {
         if (this->mpc_Topology != NULL)
         {
            this->mpc_Topology->HideAll(false);
         }
         //Buttons
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE, true));
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_RTF_EXPORT, true));

         Q_EMIT (this->SigSetToolTipForUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE,
                                                     this->mc_TooltipGenerateCodeHeading,
                                                     this->mc_TooltipGenerateCodeContentSysdef));
      }
      else if (os32_SubMode == ms32_SUBMODE_SYSDEF_NODEEDIT)
      {
         // show the node edit widget
         if (this->mpc_Topology != NULL)
         {
            this->mpc_Topology->HideAll(true);
         }

         this->mpc_ActNodeEdit = new C_SdNdeNodeEditWidget(ou32_Index, this->ms32_NodeEditTabIndex, this);

         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigChanged, this, &C_SdHandlerWidget::m_DataChanged);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigErrorChange, this,
                 &C_SdHandlerWidget::m_ErrorChange);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBus, this,
                 &C_SdHandlerWidget::m_SwitchToBus);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBusProtocol, this,
                 &C_SdHandlerWidget::m_SwitchToBusProtocol);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBusProtocolMessage, this,
                 &C_SdHandlerWidget::m_SwitchToBusProtocolMessage);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToDeviceNodeInCoManager, this,
                 &C_SdHandlerWidget::m_SwitchToDeviceNodeInCoManager);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigNameChanged, this,
                 &C_SdHandlerWidget::SigNameChanged);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSave, this,
                 &C_SdHandlerWidget::Save);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSaveAs, this,
                 &C_SdHandlerWidget::SaveAs);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBusProperties, this,
                 &C_SdHandlerWidget::m_SwitchToBusProperties);
         //Buttons
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE, true));
         Q_EMIT (this->SigSetToolTipForUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE,
                                                     this->mc_TooltipGenerateCodeHeading,
                                                     this->mc_TooltipGenerateCodeContentNode));
         // show tsp import button
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_TSP_IMPORT, true));

         if (this->mpc_ActNodeEdit != NULL)
         {
            this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_ActNodeEdit);
            this->mpc_ActNodeEdit->show();
         }
      }
      else if (os32_SubMode == ms32_SUBMODE_SYSDEF_BUSEDIT)
      {
         const stw::opensyde_core::C_OscSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOscBus(ou32_Index);

         // show the bus edit widget
         if (this->mpc_Topology != NULL)
         {
            this->mpc_Topology->HideAll(true);
         }

         this->mpc_ActBusEdit = new C_SdBueBusEditWidget(ou32_Index, this->ms32_BusEditTabIndex, this);

         connect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigChanged, this, &C_SdHandlerWidget::m_DataChanged);
         connect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigErrorChange, this, &C_SdHandlerWidget::m_ErrorChange);
         connect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigNameChanged, this, &C_SdHandlerWidget::SigNameChanged);
         connect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigSwitchToCoManager, this,
                 &C_SdHandlerWidget::m_SwitchToCoManager);

         //Buttons
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE, true));
         Q_EMIT (this->SigSetToolTipForUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE,
                                                     this->mc_TooltipGenerateCodeHeading,
                                                     this->mc_TooltipGenerateCodeContentNode));

         // show button for DBC file export and messages import
         if (pc_Bus->e_Type == C_OscSystemBus::E_Type::eCAN) // Ethernet is not supported yet
         {
            Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_IMPORT, true));
            Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_EXPORT, true));
         }

         if (this->mpc_ActBusEdit != NULL)
         {
            this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_ActBusEdit);
            this->mpc_ActBusEdit->show();
         }
      }
      else
      {
         // not good at all...
      }
   }
   this->m_SetFlag(ou32_Flag);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle for global key press event

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Handled
   False Not handled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdHandlerWidget::GlobalUserKeyPress(QKeyEvent * const opc_Event)
{
   if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_S)) &&
       (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      this->Save();
   }
   else if ((opc_Event->key() == static_cast<int32_t>(Qt::Key_F)) &&
            (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      Q_EMIT this->SigSearch();
   }
   else if (opc_Event->key() == static_cast<int32_t>(Qt::Key_F12))
   {
      // open project save
      this->SaveAs();
   }
   else if (opc_Event->key() == static_cast<int32_t>(Qt::Key_F8))
   {
      // open color picker
      this->OpenColorPicker();
   }
   else
   {
      //No specific handling
   }

   return false;
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_DataChanged(void)
{
   this->mq_DataChanged = true;

   Q_EMIT this->SigDataChanged(true, false, ms32_MODE_SYSDEF, this->ms32_SubMode, this->mu32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_NodeChanged(const uint32_t ou32_Index)
{
   this->mq_DataChanged = true;

   Q_EMIT this->SigDataChanged(true, false, ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_NODEEDIT, ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_BusChanged(const uint32_t ou32_Index)
{
   this->mq_DataChanged = true;

   Q_EMIT this->SigDataChanged(true, false, ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, ou32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Switch to bus edit widget

   \param[in]  ou32_Index     Bus index
   \param[in]  orc_BusName    Bus name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_SwitchToBus(const uint32_t ou32_Index, const QString & orc_BusName)
{
   Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, ou32_Index, orc_BusName, "",
                              mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Switch to bus edit widget with a specific protocol type

   \param[in]  ou32_Index         Bus index
   \param[in]  orc_BusName        Bus name
   \param[in]  oe_ProtocolType    Protocol type to switch to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_SwitchToBusProtocol(const uint32_t ou32_Index, const QString & orc_BusName,
                                              const C_OscCanProtocol::E_Type oe_ProtocolType)
{
   Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, ou32_Index, orc_BusName, "",
                              mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR_PROTOCOL + static_cast<uint32_t>(oe_ProtocolType));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Switch to bus edit widget with a specific message

   \param[in]  ou32_Index         Bus index
   \param[in]  orc_BusName        Bus name
   \param[in]  orc_MessageId      Message id to jump to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_SwitchToBusProtocolMessage(const uint32_t ou32_Index, const QString & orc_BusName,
                                                     const C_OscCanMessageIdentificationIndices & orc_MessageId)
{
   Q_EMIT (this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, ou32_Index, orc_BusName, "",
                               mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR));

   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(orc_MessageId.u32_NodeIndex);
   if ((pc_Node != NULL) &&
       (orc_MessageId.u32_DatapoolIndex < pc_Node->c_DataPools.size()))
   {
      uint32_t u32_ListIndex;
      if (C_OscCanProtocol::h_GetComListIndex(pc_Node->c_DataPools[orc_MessageId.u32_DatapoolIndex],
                                              orc_MessageId.u32_InterfaceIndex, orc_MessageId.q_MessageIsTx,
                                              u32_ListIndex) == C_NO_ERR)
      {
         Q_EMIT (this->OpenDetail(orc_MessageId.u32_NodeIndex, orc_MessageId.u32_DatapoolIndex,
                                  u32_ListIndex, orc_MessageId.u32_MessageIndex, 2));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Switch to bus edit widget, properties tab

   \param[in]  ou32_Index     Bus index
   \param[in]  orc_BusName    Bus name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_SwitchToBusProperties(const uint32_t ou32_Index, const QString & orc_BusName)
{
   Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, ou32_Index, orc_BusName, "",
                              mu32_FLAG_OPEN_PROPERTIES);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Switch to node edit widget in CANopen Manager tab with navigation to the device in the tree

   \param[in]  ou32_ManagerNodeIndex     Manager Node index
   \param[in]  orc_ManagerNodeName       Manager Node name
   \param[in]  ou8_InterfaceNumber       Manager Node interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_SwitchToCoManager(const uint32_t ou32_ManagerNodeIndex, const QString & orc_ManagerNodeName,
                                            const uint8_t ou8_InterfaceNumber)
{
   Q_EMIT (this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_NODEEDIT, ou32_ManagerNodeIndex,
                               orc_ManagerNodeName, "",
                               mu32_FLAG_OPEN_SYSDEF_CANOPENMANAGER + static_cast<uint32_t>(ou8_InterfaceNumber)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Switch to node edit widget in CANopen Manager tab with navigation to the device in the tree

   \param[in]  ou32_ManagerNodeIndex     Manager Node index
   \param[in]  orc_ManagerNodeName       Manager Node name
   \param[in]  ou32_DeviceNodeIndex      Device Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_SwitchToDeviceNodeInCoManager(const uint32_t ou32_ManagerNodeIndex,
                                                        const QString & orc_ManagerNodeName,
                                                        const uint32_t ou32_DeviceNodeIndex)
{
   Q_EMIT (this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_NODEEDIT, ou32_ManagerNodeIndex,
                               orc_ManagerNodeName, "",
                               mu32_FLAG_OPEN_SYSDEF_DEVICENODE_IN_CANOPENMANAGER + ou32_DeviceNodeIndex));
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_SetFlag(const uint32_t ou32_Flag) const
{
   if (ou32_Flag != mu32_FLAG_DEFAULT)
   {
      if (this->mpc_ActNodeEdit != NULL)
      {
         this->mpc_ActNodeEdit->SetFlag(ou32_Flag);
      }
      else if (this->mpc_ActBusEdit != NULL)
      {
         this->mpc_ActBusEdit->SetFlag(ou32_Flag);
      }
      else
      {
         // nothing to do
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle error change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_ErrorChange(void)
{
   Q_EMIT this->SigErrorChanged(this->mu32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle generate file action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_GenerateCode(void) const
{
   // Make sure HALC magician did its work
   this->m_GenerateHalcDatapools();

   //Check for changes
   if (C_ImpUtil::h_CheckProjForCodeGeneration(this->parentWidget()) == true)
   {
      const QPointer<C_OgePopUpDialog> c_PopUpDialog = new C_OgePopUpDialog(this->parentWidget(), this->parentWidget());
      C_SdCodeGenerationDialog * const pc_CodeGenerationDialog = new C_SdCodeGenerationDialog(*c_PopUpDialog);
      // Resize
      c_PopUpDialog->SetSize(QSize(1000, 700));
      if (this->ms32_SubMode == ms32_SUBMODE_SYSDEF_TOPOLOGY)
      {
         std::vector<uint32_t> c_Indices;
         c_Indices.reserve(C_PuiSdHandler::h_GetInstance()->GetOscNodesSize());
         for (uint32_t u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize(); ++u32_ItNode)
         {
            c_Indices.push_back(u32_ItNode);
         }
         pc_CodeGenerationDialog->PrepareDialog(c_Indices);
      }
      else if (this->ms32_SubMode == ms32_SUBMODE_SYSDEF_NODEEDIT)
      {
         std::vector<uint32_t> c_Indices;
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_Index);
         tgl_assert(pc_Node != NULL);
         c_Indices.push_back(this->mu32_Index);
         pc_CodeGenerationDialog->PrepareDialog(c_Indices);
      }
      else if (this->ms32_SubMode == ms32_SUBMODE_SYSDEF_BUSEDIT)
      {
         std::vector<uint32_t> c_Indices;
         std::vector<uint32_t> c_NodeIndexes;
         std::vector<uint32_t> c_InterfaceIndexes;
         C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_Index,
                                                                                            c_NodeIndexes,
                                                                                            c_InterfaceIndexes);
         for (uint32_t u32_Counter = 0; u32_Counter < c_NodeIndexes.size(); u32_Counter++)
         {
            const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(c_NodeIndexes
                                                                                               .at(u32_Counter));
            tgl_assert(pc_Node != NULL);
            c_Indices.push_back(c_NodeIndexes.at(u32_Counter));
         }
         pc_CodeGenerationDialog->PrepareDialog(c_Indices);
      }
      else
      {
         //Unexpected
         tgl_assert(false);
      }

      if (c_PopUpDialog->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         std::vector<uint32_t> c_NodeIndices;
         std::vector< std::vector<uint32_t> > c_AppIndicesPerNode;
         pc_CodeGenerationDialog->GetCheckedItems(c_NodeIndices, c_AppIndicesPerNode);
         C_ImpUtil::h_ExportCode(c_NodeIndices, c_AppIndicesPerNode, this->parentWidget());
      }
      if (c_PopUpDialog != NULL)
      {
         c_PopUpDialog->HideOverlay();
         c_PopUpDialog->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_CodeGenerationDialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call for (DBC file) export functionality.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_Export(void)
{
   std::set<uint32_t> c_CanMessageIds; // to count CAN messages
   std::set<C_OscCanMessageUniqueId> c_CanMessageIdsWithExtended;

   tgl_assert(this->mpc_ActBusEdit != NULL);
   if (this->mpc_ActBusEdit != NULL)
   {
      // Is it Possible to Use a dbc for Ethernet Configuration?
      // answer: yes (see https://kb.vector.com/entry/798/)
      // but currently only CAN networks are supported!
      const stw::opensyde_core::C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
         this->mu32_Index);

      // check if we have problems for export
      const bool q_BusConflict = C_PuiSdHandler::h_GetInstance()->CheckBusConflict(this->mu32_Index);
      if (q_BusConflict == true)
      {
         // display error
         const stw::scl::C_SclString c_Message = "Bus \"" + pc_Bus->c_Name + "\" has invalid content. "
                                                 "DBC file export cannot be performed.";
         C_OgeWiCustomMessage c_ExportWarnings(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_ExportWarnings.SetHeading(C_GtGetText::h_GetText("DBC file export"));
         c_ExportWarnings.SetDescription(C_GtGetText::h_GetText(c_Message.c_str()));
         c_ExportWarnings.SetCustomMinHeight(180, 180);
         c_ExportWarnings.Execute();
      }
      else
      {
         // network OK, ready to export
         if (pc_Bus->e_Type == C_OscSystemBus::E_Type::eCAN) // defensive style: normally else case can't occur because
                                                             // then the export button is not displayed.
         {
            uint32_t u32_NumOfInputSignals = 0;
            std::vector<uint32_t> c_NodeIndexes;
            std::vector<uint32_t> c_InterfaceIndexes;
            std::vector<uint32_t> c_DatapoolIndexes;
            std::set<uint32_t> c_UniqueNodeIndexes;
            uint32_t u32_NumOfInputMessages = 0;

            C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeAndComDpIndexesOfBus(
               this->mu32_Index,
               c_NodeIndexes,
               c_InterfaceIndexes,
               c_DatapoolIndexes);

            // for each node there must be an interface index and a Datapool index therefore same size is guaranteed
            tgl_assert(c_NodeIndexes.size() == c_InterfaceIndexes.size());
            tgl_assert(c_NodeIndexes.size() == c_DatapoolIndexes.size());

            C_CieConverter::C_CieCommDefinition c_CommDef; // data structure for DBC file export
            c_CommDef.c_Bus.c_Name = pc_Bus->c_Name;
            c_CommDef.c_Bus.c_Comment = pc_Bus->c_Comment;

            for (uint32_t u32_Pos = 0; u32_Pos < c_NodeIndexes.size(); u32_Pos++)
            {
               const uint32_t u32_NodeIndex = c_NodeIndexes[u32_Pos];
               const uint32_t u32_InterfaceIndex = c_InterfaceIndexes[u32_Pos];
               const uint32_t u32_DatapoolIndex = c_DatapoolIndexes[u32_Pos];
               C_OscCanProtocol::E_Type e_ComType;

               tgl_assert(C_PuiSdHandler::h_GetInstance()->GetCanProtocolType(u32_NodeIndex,
                                                                              u32_DatapoolIndex,
                                                                              e_ComType) == C_NO_ERR);
               const C_OscCanMessageContainer * const pc_CanMessageContainer =
                  C_PuiSdHandler::h_GetInstance()->GetCanProtocolMessageContainer(
                     u32_NodeIndex,
                     e_ComType,
                     u32_InterfaceIndex,
                     u32_DatapoolIndex);

               // only export active devices
               if ((pc_CanMessageContainer != NULL) && (pc_CanMessageContainer->q_IsComProtocolUsedByInterface == true))
               {
                  int32_t s32_Error = C_NO_ERR;
                  stw::scl::C_SclStringList c_Warnings;
                  const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_NodeIndex);
                  C_CieConverter::C_CieNode c_CurrentCieNode;

                  // The number of nodes is necessary.
                  c_UniqueNodeIndexes.insert(u32_NodeIndex);

                  c_CurrentCieNode.c_Properties.c_Name = pc_Node->c_Properties.c_Name;
                  c_CurrentCieNode.c_Properties.c_Comment = pc_Node->c_Properties.c_Comment;

                  // get Tx can messages
                  const std::vector<C_OscCanMessage> c_TxMsgs = pc_CanMessageContainer->GetMessagesConst(true);
                  std::vector<C_OscCanMessage>::const_iterator c_TxIter;
                  for (c_TxIter = c_TxMsgs.begin(); c_TxIter != c_TxMsgs.end(); ++c_TxIter)
                  {
                     if ((e_ComType == C_OscCanProtocol::eCAN_OPEN) && (!c_TxIter->q_CanOpenManagerMessageActive))
                     {
                        //Skip inactive message
                     }
                     else
                     {
                        C_CieConverter::C_CieNodeMessage c_CurrentMessage;
                        s32_Error += C_CieDataPoolListAdapter::h_ConvertToDbcImportMessage(this->mu32_Index, e_ComType,
                                                                                           *c_TxIter, c_CurrentMessage,
                                                                                           c_Warnings);
                        c_CurrentCieNode.c_TxMessages.push_back(c_CurrentMessage);
                        // for counting messages and signals
                        const std::set<uint32_t>::const_iterator c_Iter = c_CanMessageIds.find(c_TxIter->u32_CanId);
                        if (c_Iter == c_CanMessageIds.end())
                        {
                           c_CanMessageIdsWithExtended.emplace(C_OscCanMessageUniqueId(c_TxIter->u32_CanId,
                                                                                       c_TxIter->q_IsExtended));
                           c_CanMessageIds.insert(c_TxIter->u32_CanId);
                           // count signals
                           u32_NumOfInputSignals += c_TxIter->c_Signals.size();
                        }
                        else
                        {
                           const std::set<C_OscCanMessageUniqueId>::const_iterator c_IterWithExtended =
                              c_CanMessageIdsWithExtended.find(C_OscCanMessageUniqueId(c_TxIter->u32_CanId,
                                                                                       c_TxIter
                                                                                       ->q_IsExtended));
                           if (c_IterWithExtended == c_CanMessageIdsWithExtended.end())
                           {
                              const stw::scl::C_SclString c_Message = "Can't export message \"" +
                                                                      c_TxIter->c_Name + "\" in bus \"" +
                                                                      stw::scl::C_SclString::IntToStr(this->mu32_Index)
                                                                      +
                                                                      "\" because message ID is not unique.";
                              c_Warnings.Append(c_Message);
                              osc_write_log_warning("DBC Export", c_Message);
                              s32_Error += C_WARN;
                           }
                        }
                     }
                  }

                  // get Rx can messages
                  const std::vector<C_OscCanMessage> & rc_RxMsgs = pc_CanMessageContainer->GetMessagesConst(false);
                  std::vector<C_OscCanMessage>::const_iterator c_RxIter;
                  for (c_RxIter = rc_RxMsgs.begin(); c_RxIter != rc_RxMsgs.end(); ++c_RxIter)
                  {
                     if ((e_ComType == C_OscCanProtocol::eCAN_OPEN) && (!c_RxIter->q_CanOpenManagerMessageActive))
                     {
                        //Skip inactive message
                     }
                     else
                     {
                        C_CieConverter::C_CieNodeMessage c_CurrentMessage;
                        s32_Error += C_CieDataPoolListAdapter::h_ConvertToDbcImportMessage(this->mu32_Index, e_ComType,
                                                                                           *c_RxIter, c_CurrentMessage,
                                                                                           c_Warnings);
                        c_CurrentCieNode.c_RxMessages.push_back(c_CurrentMessage);
                        // for counting messages and signals
                        const std::set<uint32_t>::const_iterator c_Iter = c_CanMessageIds.find(c_RxIter->u32_CanId);
                        if (c_Iter == c_CanMessageIds.end())
                        {
                           c_CanMessageIdsWithExtended.emplace(C_OscCanMessageUniqueId(c_RxIter->u32_CanId,
                                                                                       c_RxIter->q_IsExtended));
                           c_CanMessageIds.insert(c_RxIter->u32_CanId);
                           // count signals
                           u32_NumOfInputSignals += c_RxIter->c_Signals.size();
                        }
                        else
                        {
                           const std::set<C_OscCanMessageUniqueId>::const_iterator c_IterWithExtended =
                              c_CanMessageIdsWithExtended.find(C_OscCanMessageUniqueId(c_RxIter->u32_CanId,
                                                                                       c_RxIter
                                                                                       ->q_IsExtended));
                           if (c_IterWithExtended == c_CanMessageIdsWithExtended.end())
                           {
                              const stw::scl::C_SclString c_Message = "Can't export message \"" +
                                                                      c_RxIter->c_Name + "\" in bus \"" +
                                                                      stw::scl::C_SclString::IntToStr(this->mu32_Index)
                                                                      +
                                                                      "\" because message ID is not unique. Message is ignored.";
                              c_Warnings.Append(c_Message);
                              osc_write_log_warning("DBC Export", c_Message);
                              s32_Error += C_WARN;
                           }
                        }
                     }
                  }
                  // save current node
                  c_CommDef.c_Nodes.push_back(c_CurrentCieNode);

                  u32_NumOfInputMessages = c_CanMessageIds.size();

                  if (s32_Error != C_NO_ERR)
                  {
                     // display warnings
                     C_OgeWiCustomMessage c_ExportWarnings(this, C_OgeWiCustomMessage::E_Type::eWARNING);
                     c_ExportWarnings.SetHeading(C_GtGetText::h_GetText("DBC file export"));
                     c_ExportWarnings.SetDescription(C_GtGetText::h_GetText(
                                                        "Warnings occurred during DBC file export."));
                     c_ExportWarnings.SetDetails(c_Warnings.GetText().c_str());
                     c_ExportWarnings.SetCustomMinHeight(180, 300);
                     c_ExportWarnings.Execute();
                  }
               }
            }

            // export to file
            C_CieUtil::h_ExportFile(c_CommDef, this, c_UniqueNodeIndexes.size(), u32_NumOfInputMessages,
                                    u32_NumOfInputSignals);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create import popup

   \return
   QPointer<C_OgePopUpDialog>
*/
//----------------------------------------------------------------------------------------------------------------------
const QPointer<C_OgePopUpDialog> stw::opensyde_gui::C_SdHandlerWidget::m_CreateImportPopupDialog(void)
{
   //Size of the popup
   const uint16_t u16_WIDTH = 650;
   const uint16_t u16_HEIGHT = 350;

   const QPointer<C_OgePopUpDialog> c_PopUpDialog = new C_OgePopUpDialog(this, this);

   c_PopUpDialog->SetSize(QSize(u16_WIDTH, u16_HEIGHT));

   return c_PopUpDialog;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Call for DBC/EDS/DCF import functionality.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_TriggerImport(void)
{
   if (m_CheckImportPossible())
   {
      //Node
      const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(mu32_Index);

      tgl_assert((pc_Bus != NULL) && (mpc_ActBusEdit != NULL));
      if ((pc_Bus != NULL) && (mpc_ActBusEdit != NULL))
      {
         const stw::scl::C_SclString c_BusName = pc_Bus->c_Name;

         const QPointer<C_OgePopUpDialog> c_PopUpDialog = m_CreateImportPopupDialog();
         C_SdBueImportCommMessagesWidget * const pc_ImportCommMessagesWidget = new C_SdBueImportCommMessagesWidget(
            *c_PopUpDialog,
            c_BusName);

         C_OscCanProtocol::E_Type e_CurrentProtocolView;

         //Open tab (before interacting with it!)
         mpc_ActBusEdit->ChangeToCommMessagesTab();

         e_CurrentProtocolView = mpc_ActBusEdit->GetActProtocol();

         pc_ImportCommMessagesWidget->SetDefaultCommunicationProtocol(e_CurrentProtocolView);

         if (c_PopUpDialog->exec() == static_cast<int32_t>(QDialog::Accepted))
         {
            const C_OscCanProtocol::E_Type e_SelectedCommProtocol =
               pc_ImportCommMessagesWidget->GetSelectedCommunicationProtocol();
            m_ContinueImporting(e_SelectedCommProtocol);
         }

         if (c_PopUpDialog != NULL)
         {
            c_PopUpDialog->HideOverlay();
            c_PopUpDialog->deleteLater();
         }

         delete (pc_ImportCommMessagesWidget);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Continue Importing

   Change Protocol tab view and open file explorer

   \param[in]       ore_Protocol     Communication protocol defined in the combo box
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_ContinueImporting(const C_OscCanProtocol::E_Type & ore_Protocol)
{
   m_SwitchProtocolTab(ore_Protocol);
   m_Import();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call for (RTF file) documentation functionality.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_RtfExport(void)
{
   // check for unsaved data and start the export dialog later (only saved items are exported to RTF file)
   if (C_PopUtil::h_AskUserToContinue(this, false) == true)
   {
      // if the project is empty do not continue
      if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
         c_Message.SetDescription(C_GtGetText::h_GetText("Cannot export RTF file without a valid project path. "
                                                         "Save project to continue."));
         c_Message.SetDetails(C_GtGetText::h_GetText(
                                 "For RTF file export the SYSTEM DEFINITION must be saved to disk."));
         c_Message.SetCustomMinHeight(230, 250);
         c_Message.Execute();
      }
      else
      {
         // export dialog
         const QPointer<C_OgePopUpDialog> c_PopUpDialog = new C_OgePopUpDialog(this, this);
         C_RtfExportWidget * const pc_DialogExportReport = new C_RtfExportWidget(*c_PopUpDialog);

         // resize
         const QSize c_SIZE_IMPORT_REPORT(890, 434);

         c_PopUpDialog->SetSize(c_SIZE_IMPORT_REPORT);

         stw::scl::C_SclString c_RtfPath = static_cast<stw::scl::C_SclString>(
            C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownRtfPath().toStdString().c_str());
         stw::scl::C_SclString c_CompanyName = static_cast<stw::scl::C_SclString>(
            C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownRtfCompanyName().toStdString().c_str());
         stw::scl::C_SclString c_CompanyLogoPath = static_cast<stw::scl::C_SclString>(
            C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownRtfCompanyLogoPath().toStdString().c_str());

         if (c_RtfPath == "")
         {
            // get default file name with project path (if using the RTF dialog the first time)
            QString c_DefaultFilename = C_PuiProject::h_GetInstance()->GetName();
            c_DefaultFilename += ".rtf";
            c_RtfPath = C_PuiUtil::h_GetAbsolutePathFromProject(c_DefaultFilename).toStdString().c_str();
         }

         if (c_RtfPath != "")
         {
            pc_DialogExportReport->SetRtfPath(c_RtfPath);
         }
         if (c_CompanyName != "")
         {
            pc_DialogExportReport->SetCompanyName(c_CompanyName);
         }
         if (c_CompanyLogoPath != "")
         {
            pc_DialogExportReport->SetCompanyLogoPath(c_CompanyLogoPath);
         }

         // display message report
         if (c_PopUpDialog->exec() == static_cast<int32_t>(QDialog::Accepted))
         {
            // save inputs as user settings
            pc_DialogExportReport->GetRtfPath(c_RtfPath);
            C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownRtfPath(static_cast<QString>(c_RtfPath.c_str()));
            pc_DialogExportReport->GetCompanyName(c_CompanyName);
            C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownRtfCompanyName(static_cast<QString>(c_CompanyName.
                                                                                                        c_str()));
            pc_DialogExportReport->GetCompanyLogoPath(c_CompanyLogoPath);
            C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownRtfCompanyLogoPath(static_cast<QString>(
                                                                                          c_CompanyLogoPath.c_str()));
            C_UsHandler::h_GetInstance()->Save();

            // export to RTF file
            stw::scl::C_SclStringList c_Warnings;
            stw::scl::C_SclString c_Error;
            const int32_t s32_Return = pc_DialogExportReport->ExportToRtf(c_RtfPath, c_CompanyName, c_CompanyLogoPath,
                                                                          this->mpc_Topology, c_Warnings, c_Error);
            if (s32_Return == C_NO_ERR)
            {
               const QString c_Details =
                  static_cast<QString>("%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                  arg(C_GtGetText::h_GetText("File saved at ")).
                  arg(static_cast<QString>(c_RtfPath.c_str())).
                  arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                  arg(static_cast<QString>(c_RtfPath.c_str()));
               C_OgeWiCustomMessage c_MessageResult(this);
               c_MessageResult.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
               c_MessageResult.SetDescription(C_GtGetText::h_GetText("RTF document successfully created."));
               c_MessageResult.SetDetails(c_Details);
               c_MessageResult.SetCustomMinHeight(180, 250);
               c_MessageResult.Execute();
            }
            else if (s32_Return == C_WARN)
            {
               const stw::scl::C_SclString c_Details = "Warnings: \r\n" + c_Warnings.GetText();
               C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eWARNING);
               c_MessageResult.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
               c_MessageResult.SetDescription(C_GtGetText::h_GetText("Warnings occurred on RTF File Export."));
               c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_Details.c_str()));
               c_MessageResult.SetCustomMinHeight(180, 250);
               c_MessageResult.Execute();
            }
            else
            {
               C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_MessageResult.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
               c_MessageResult.SetDescription(C_GtGetText::h_GetText("RTF file export error occurred."));
               c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_Error.c_str()));
               c_MessageResult.SetCustomMinHeight(180, 250);
               c_MessageResult.Execute();
            }
         }

         if (c_PopUpDialog != NULL)
         {
            c_PopUpDialog->HideOverlay();
            c_PopUpDialog->deleteLater();
         }
      } //lint !e429  //no memory leak because of the parent of pc_DialogExportReport and the Qt memory management
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Show warning unstored Project popup message

   \return
   C_OgeWiCustomMessage *
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiCustomMessage * C_SdHandlerWidget::m_ShowWarningUnstoredProjectPopupMessage()
{
   C_OgeWiCustomMessage * const pc_MessageBox = new C_OgeWiCustomMessage(this);

   pc_MessageBox->SetType(C_OgeWiCustomMessage::eWARNING);

   pc_MessageBox->SetHeading(C_GtGetText::h_GetText("Import TSP"));
   pc_MessageBox->SetDescription(C_GtGetText::h_GetText(
                                    "This project is not saved yet. Adding Data Blocks might cause "
                                    "problems with file or directory paths."));
   pc_MessageBox->SetDetails(C_GtGetText::h_GetText(
                                "Paths that are handled as relative to *.syde file can not be resolved correctly!"));
   pc_MessageBox->SetOkButtonText(C_GtGetText::h_GetText("Continue"));
   pc_MessageBox->SetCustomMinHeight(230, 270);
   pc_MessageBox->SetCancelButtonText(C_GtGetText::h_GetText("Cancel"));

   return pc_MessageBox;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Switch communication protocol tab

   \param[in]       oe_Protocol     Currently selected protocol
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_SwitchProtocolTab(const C_OscCanProtocol::E_Type & ore_Protocol) const
{
   // only available on bus edit
   tgl_assert(this->mpc_ActBusEdit != NULL);

   if (this->mpc_ActBusEdit != NULL)
   {
      mpc_ActBusEdit->ChangeProtocolTab(ore_Protocol);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open file explorer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_Import()
{
   // only available on bus edit
   tgl_assert(this->mpc_ActBusEdit != NULL);

   if (this->mpc_ActBusEdit != NULL)
   {
      this->mpc_ActBusEdit->ImportMessages();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check import possible

   \return
   Flags

   \retval   True    Import possible
   \retval   False   Import impossible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdHandlerWidget::m_CheckImportPossible()
{
   bool q_Retval;

   std::vector<uint32_t> c_NodeIndexes;
   std::vector<uint32_t> c_InterfaceIndexes;

   C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().GetNodeIndexesOfBus(
      mu32_Index, c_NodeIndexes, c_InterfaceIndexes);

   if (c_NodeIndexes.size() == 0)
   {
      C_OgeWiCustomMessage pc_MessageBox(this);
      q_Retval = false;

      pc_MessageBox.SetType(C_OgeWiCustomMessage::eERROR);

      pc_MessageBox.SetHeading(C_GtGetText::h_GetText("Import Messages"));
      pc_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                      "Cannot import messages without any connected nodes."));
      pc_MessageBox.SetCustomMinHeight(230, 270);
      pc_MessageBox.Execute();
   }
   else
   {
      q_Retval = true;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create warning message box if unsaved project is used and calls TSP import function of node edit page
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_TspImport(const bool oq_IsNodeNew)
{
   if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Save Project"));
      c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                     "This project is not saved yet. TSP import might cause "
                                     "problems with file or directory paths."));
      c_MessageBox.SetDetails(C_GtGetText::h_GetText(
                                 "Paths that are handled as relative to *.syde file can not be resolved correctly!"));
      c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("Save and Continue"));
      c_MessageBox.SetNoButtonText(C_GtGetText::h_GetText("Cancel"));
      c_MessageBox.SetCustomMinHeight(200, 280);

      const C_OgeWiCustomMessage::E_Outputs e_Output = c_MessageBox.Execute();
      const QPointer<stw::opensyde_gui_elements::C_OgePopUpDialog> c_New =
         new stw::opensyde_gui_elements::C_OgePopUpDialog(this, this);
      const C_PopSaveAsDialogWidget * const pc_Dialog = new C_PopSaveAsDialogWidget(*c_New);

      const QSize c_SIZE(906, 415);
      c_New->SetSize(c_SIZE);
      switch (e_Output)
      {
      case C_OgeWiCustomMessage::eYES:
         // open save as dialog
         if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
         {
            if (mpc_ActNodeEdit != NULL)
            {
               mpc_ActNodeEdit->AddFromTsp(oq_IsNodeNew);
            }
            if (c_New != NULL)
            {
               pc_Dialog->SaveUserSettings();
               c_New->HideOverlay();
            }
         }
         break;
      case C_OgeWiCustomMessage::eNO:
         c_New->HideOverlay();
         break;
      case C_OgeWiCustomMessage::eCANCEL:
         break;
      default:
         break;
      }
   } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   else
   {
      if (mpc_ActNodeEdit != NULL)
      {
         mpc_ActNodeEdit->AddFromTsp(oq_IsNodeNew);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set Information for new node to avoid warning message box
 *  calls TSP import function of node edit page
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_TspImportForNewNode()
{
   this->m_TspImport(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Generate HALC datapools with HALC magician
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::m_GenerateHalcDatapools(void) const
{
   if (this->mpc_ActNodeEdit != NULL)
   {
      const int32_t s32_Result = C_PuiSdHandler::h_GetInstance()->HalcGenerateDatapools(this->mu32_Index);

      tgl_assert((s32_Result == C_NO_ERR) || (s32_Result == C_NOACT));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call specific help pages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdHandlerWidget::CallHelp(void)
{
   if (this->mpc_ActNodeEdit != NULL)
   {
      const int32_t s32_TabIndex = this->mpc_ActNodeEdit->GetTabIndex();
      if (s32_TabIndex == C_SdNdeNodeEditWidget::hs32_TAB_INDEX_DATA_POOL)
      {
         //TabIndex == Datapools
         stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
            "stw::opensyde_gui::C_SdNdeDbViewWidget");
      }
      else if (s32_TabIndex == C_SdNdeNodeEditWidget::hs32_TAB_INDEX_COMM)
      {
         //TabIndex == COMM
         stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
            "stw::opensyde_gui::C_SdBueComIfDescriptionWidget");
      }
      else if (s32_TabIndex == C_SdNdeNodeEditWidget::hs32_TAB_INDEX_HALC)
      {
         // TabIndex == HALC
         stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
            "stw::opensyde_gui::C_SdNdeHalcChannelWidget");
      }
      else
      {
         // Properties
         stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
            "stw::opensyde_gui::C_SdNdeNodeEditWidget");
      }
   }
   else if (this->mpc_ActBusEdit != NULL)
   {
      //TabIndex == COMM
      if (this->mpc_ActBusEdit->GetTabIndex() == 1)
      {
         stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
            "stw::opensyde_gui::C_SdBueComIfDescriptionWidget");
      }
      else //properties
      {
         stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
            "stw::opensyde_gui::C_SdBueBusEditWidget");
      }
   }
   else
   {
      //ms32_SUBMODE_SYSDEF_TOPOLOGY and others call stw::opensyde_gui::C_SdHandlerWidget
      stw::opensyde_gui_logic::C_HeHandler::h_GetInstance().CallSpecificHelpPage(
         "stw::opensyde_gui::C_SdHandlerWidget");
   }
}
