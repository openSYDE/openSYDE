//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for system definition user

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QApplication>
#include <QFileInfo>
#include "C_PopErrorHandling.h"
#include "C_PuiProject.h"
#include "C_SdHandlerWidget.h"
#include "ui_C_SdHandlerWidget.h"
#include "C_OgeWiCustomMessage.h"
#include "C_OSCNode.h"

#include "constants.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_HeHandler.h"
#include "C_UsHandler.h"
#include "C_ImpUtil.h"
#include "C_Uti.h"
#include "C_PuiSdHandler.h"
#include "C_CieUtil.h"
#include "C_OSCCanProtocol.h"
#include "C_CieDataPoolListAdapter.h"
#include "C_SdBueBusEditWidget.h"
#include "ui_C_SdBueBusEditWidget.h"
#include "stwerrors.h"
#include "C_RtfExportWidget.h"
#include "C_PopUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const uint32 C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_APPLY = 0U;
const uint32 C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_GENERATE_CODE = 1U;
const uint32 C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_EXPORT = 2U;
const uint32 C_SdHandlerWidget::mhu32_USER_INPUT_FUNC_RTF_EXPORT = 3U;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_parent Optional pointer to parent

   \created     11.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdHandlerWidget::C_SdHandlerWidget(QWidget * const opc_Parent) :
   C_NagUseCaseWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdHandlerWidget),
   mpc_Topology(NULL),
   mpc_ActNodeEdit(NULL),
   mpc_ActBusEdit(NULL),
   msn_NodeEditTabIndex(0),
   msn_BusEditTabIndex(0),
   ms32_SubMode(-1),
   mu32_Index(0U),
   mq_DataChanged(false),
   mc_TOOLTIP_GENERAT_CODE_HEADING(C_GtGetText::h_GetText("Generate Code")),
   mc_TOOLTIP_GENERAT_CODE_CONTENT_SYSDEF(C_GtGetText::h_GetText(
                                             "Generate Code for all nodes with programming support based on defined SYSTEM DEFINITION."
                                             "\n - openSYDE server initialization wrapper"
                                             "\n   Create a .c and .h file providing initialization structures for the OSS DPD and DPH init functions."
                                             "\n - COMM stack definition"
                                             "\n   Create a .c and .h file providing entire communication stack configuration."
                                             "\n - Datapools"
                                             "\n   Generate code for Datapool settings of an openSYDE node.")),
   mc_TOOLTIP_GENERAT_CODE_CONTENT_NODE(C_GtGetText::h_GetText(
                                           "Generate Code for current node based on defined SYSTEM DEFINITION."
                                           "\n - openSYDE server initialization wrapper"
                                           "\n   Create a .c and .h file providing initialization structures for the OSS DPD and DPH init functions."
                                           "\n - COMM stack definition"
                                           "\n   Create a .c and .h file providing entire communication stack configuration."
                                           "\n - Datapools"
                                           "\n   Generate code for Datapool settings of current openSYDE node."))
{
   sintn sn_Index;

   mpc_Ui->setupUi(this);

   // create the topology
   this->mpc_Topology = new C_SdTopologyWidget(this);
   this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_Topology);
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigChanged, this, &C_SdHandlerWidget::m_DataChanged);
   // forwarding of this signal
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigChangeMode, this, &C_SdHandlerWidget::SigChangeMode);
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigNodeChanged, this, &C_SdHandlerWidget::m_NodeChanged);
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigBusChanged, this, &C_SdHandlerWidget::m_BusChanged);
   connect(this->mpc_Topology, &C_SdTopologyWidget::SigErrorChange, this,
           &C_SdHandlerWidget::m_ErrorChange);

   sn_Index = this->mpc_Ui->pc_VerticalLayout->indexOf(this->mpc_Topology);
   this->mpc_Ui->pc_VerticalLayout->setStretch(sn_Index, 1);

   C_UsHandler::h_GetInstance()->GetProjLastSysDefTabIndex(this->msn_NodeEditTabIndex, this->msn_BusEditTabIndex);

   // configure toolbar functions
   // the order of adding the function is very important
   C_NagToolBarButtonProperties c_ButtonProperties;

   // Function index 0: mhu32_USER_INPUT_FUNC_APPLY
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Save Project");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Save Project");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Save project data to file.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 1: mhu32_USER_INPUT_FUNC_GENERATE_CODE
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Generate Code");
   c_ButtonProperties.c_ToolTipHeading = mc_TOOLTIP_GENERAT_CODE_HEADING;
   c_ButtonProperties.c_ToolTipContent = mc_TOOLTIP_GENERAT_CODE_CONTENT_SYSDEF;
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 2: mhu32_USER_INPUT_FUNC_EXPORT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("DBC File Export");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("DBC File Export");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText(
      "Export messages and signals of current bus in standard DBC file format.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);

   // Function index 3: mhu32_USER_INPUT_FUNC_RTF_EXPORT
   c_ButtonProperties.c_ButtonText = C_GtGetText::h_GetText("Report");
   c_ButtonProperties.c_ToolTipHeading = C_GtGetText::h_GetText("Report");
   c_ButtonProperties.c_ToolTipContent = C_GtGetText::h_GetText("Export the SYSTEM DEFINITION as RTF document file.");
   this->mc_VecUserInputFuncNames.append(c_ButtonProperties);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     11.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdHandlerWidget::~C_SdHandlerWidget()
{
   C_UsHandler::h_GetInstance()->SetProjLastSysDefTabIndex(this->msn_NodeEditTabIndex, this->msn_BusEditTabIndex);

   delete mpc_Ui;
   //lint -e{1579,1740}  no memory leak because of the parent all elements and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to set the parent of the widget

   \created     13.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::SetParentHook(QWidget * const opc_Parent)
{
   if (this->mpc_Topology != NULL)
   {
      this->mpc_Topology->SetParentHook(opc_Parent);
   }

   C_NagUseCaseWidget::SetParentHook(opc_Parent);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function for toolbar

   Saves the project.

   \param[in]  ou32_FuncNumber   Number of function

   \created     12.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::UserInputFunc(const uint32 ou32_FuncNumber)
{
   switch (ou32_FuncNumber)
   {
   case mhu32_USER_INPUT_FUNC_APPLY:
      this->Save();
      break;
   case mhu32_USER_INPUT_FUNC_GENERATE_CODE:
      this->GenerateCode();
      break;
   case mhu32_USER_INPUT_FUNC_EXPORT:
      this->Export();
      break;
   case mhu32_USER_INPUT_FUNC_RTF_EXPORT:
      if (this->mpc_Topology != NULL)
      {
         this->mpc_Topology->SaveToData();
      }
      this->RtfExport();
      break;
   default:
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to save data

   Saves the system definition

   \created     08.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::Save(void)
{
   QApplication::setOverrideCursor(Qt::WaitCursor);
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
   //Check if valid save
   if (C_PuiProject::h_GetInstance()->IsEmptyProject() == true)
   {
      // open project save
      this->SaveAs();
   }
   else
   {
      //Save to file
      C_PopErrorHandling::mh_ProjectSaveErr(C_PuiProject::h_GetInstance()->Save(), this);

      Q_EMIT this->SigDataChanged(false, true, ms32_MODE_SYSDEF);

      this->mq_DataChanged = false;
   }
   QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to open a concrete datapool, datapool list or dataelement

   \param[in] os32_Index            First index
   \param[in] os32_SubIndex         Second index
   \param[in] os32_SubSubIndex      Third index
   \param[in] os32_SubSubSubIndex   Fourth index
   \param[in] os32_Flag             Optional flag for further information

   \created     16.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::OpenDetail(const sint32 os32_Index, const sint32 os32_SubIndex, const sint32 os32_SubSubIndex,
                                   const sint32 os32_SubSubSubIndex, const sint32 os32_Flag)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Function to prepare closing the widget

   Updating of the system definition without saving it into the files

   \return
   true     Preparation successful. Can be closed.
   false    Preparation not finished. Can not be closed.

   \created     22.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdHandlerWidget::PrepareToClose(void)
{
   // save all changes of the active edit widgets to the core
   if (this->mpc_ActNodeEdit != NULL)
   {
      // save the tab index
      this->msn_NodeEditTabIndex = this->mpc_ActNodeEdit->GetTabIndex();
      this->mpc_ActNodeEdit->Save();
   }
   if (this->mpc_ActBusEdit != NULL)
   {
      // save the tab index
      this->msn_BusEditTabIndex = this->mpc_ActBusEdit->GetTabIndex();
      this->mpc_ActBusEdit->Save();
   }
   if (this->mpc_Topology != NULL)
   {
      this->mpc_Topology->PrepareToClose();
   }

   return true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the actual mode of the system definition handler

   Do not call this function internally. Use the signal SigChangeMode to
   inform the entire application about the change.

   \param[in]     os32_SubMode     Actual sub mode
   \param[in]     ou32_Index       Index for node or bus
   \param[in]     ou32_Flag        Flag for special functionality

   \created     28.02.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::SetSubMode(const sint32 os32_SubMode, const uint32 ou32_Index, const uint32 ou32_Flag)
{
   const QIcon c_IconSave(":images/IconSave.svg");
   const QIcon c_IconCompile("://images/system_definition/IconGenerateCode.svg");
   const QIcon c_IconExport("://images/IconExportDbc.svg");
   const QIcon c_IconRtfExport("://images/IconExportRtf.svg");

   // set the icons. this can not be done in constructor
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_APPLY, c_IconSave));
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE, c_IconCompile));
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_EXPORT, c_IconExport));
   Q_EMIT (this->SigSetIconForUserInputFunc(mhu32_USER_INPUT_FUNC_RTF_EXPORT, c_IconRtfExport));

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
         this->msn_NodeEditTabIndex = this->mpc_ActNodeEdit->GetTabIndex();

         // remove widget
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigChanged, this,
                    &C_SdHandlerWidget::m_DataChanged);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigErrorChange, this,
                    &C_SdHandlerWidget::m_ErrorChange);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBus, this,
                    &C_SdHandlerWidget::m_SwitchToBus);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigNameChanged, this,
                    &C_SdHandlerWidget::SigNameChanged);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSave, this,
                    &C_SdHandlerWidget::Save);
         disconnect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSaveAs, this,
                    &C_SdHandlerWidget::SaveAs);
         delete this->mpc_ActNodeEdit;
         this->mpc_ActNodeEdit = NULL;
      }
      if (this->mpc_ActBusEdit != NULL)
      {
         const stw_opensyde_core::C_OSCSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOSCBus(this->mu32_Index);

         this->mpc_Ui->pc_VerticalLayout->removeWidget(this->mpc_ActBusEdit);

         // save changes
         if (this->mpc_ActBusEdit->WasChanged() == true)
         {
            this->mpc_ActBusEdit->Save();
         }
         // save the tab index
         this->msn_BusEditTabIndex = this->mpc_ActBusEdit->GetTabIndex();

         // show button for DBC file export, Ethernet is not supported yet!
         if (pc_Bus->e_Type == C_OSCSystemBus::E_Type::eCAN)
         {
            Q_EMIT this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_EXPORT, true);
         }

         // remove widget
         disconnect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigChanged, this,
                    &C_SdHandlerWidget::m_DataChanged);
         disconnect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigErrorChange, this,
                    &C_SdHandlerWidget::m_ErrorChange);
         disconnect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigNameChanged, this,
                    &C_SdHandlerWidget::SigNameChanged);

         delete this->mpc_ActBusEdit;
         this->mpc_ActBusEdit = NULL;
      }

      //Clean up buttons
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE, false));
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_EXPORT, false));
      Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_RTF_EXPORT, false));

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
                                                     this->mc_TOOLTIP_GENERAT_CODE_HEADING,
                                                     this->mc_TOOLTIP_GENERAT_CODE_CONTENT_SYSDEF));
      }
      else if (os32_SubMode == ms32_SUBMODE_SYSDEF_NODEEDIT)
      {
         // show the node edit widget
         if (this->mpc_Topology != NULL)
         {
            this->mpc_Topology->HideAll(true);
         }

         this->mpc_ActNodeEdit = new C_SdNdeNodeEditWidget(ou32_Index, this->msn_NodeEditTabIndex, this);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigChanged, this, &C_SdHandlerWidget::m_DataChanged);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigErrorChange, this,
                 &C_SdHandlerWidget::m_ErrorChange);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSwitchToBus, this,
                 &C_SdHandlerWidget::m_SwitchToBus);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigNameChanged, this,
                 &C_SdHandlerWidget::SigNameChanged);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSave, this,
                 &C_SdHandlerWidget::Save);
         connect(this->mpc_ActNodeEdit, &C_SdNdeNodeEditWidget::SigSaveAs, this,
                 &C_SdHandlerWidget::SaveAs);

         //Buttons
         Q_EMIT (this->SigShowUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE, true));
         Q_EMIT (this->SigSetToolTipForUserInputFunc(mhu32_USER_INPUT_FUNC_GENERATE_CODE,
                                                     this->mc_TOOLTIP_GENERAT_CODE_HEADING,
                                                     this->mc_TOOLTIP_GENERAT_CODE_CONTENT_NODE));

         if (this->mpc_ActNodeEdit != NULL)
         {
            this->mpc_Ui->pc_VerticalLayout->addWidget(this->mpc_ActNodeEdit);
            this->mpc_ActNodeEdit->show();
         }
      }
      else if (os32_SubMode == ms32_SUBMODE_SYSDEF_BUSEDIT)
      {
         const stw_opensyde_core::C_OSCSystemBus * const pc_Bus =
            C_PuiSdHandler::h_GetInstance()->GetOSCBus(ou32_Index);

         // show the bus edit widget
         if (this->mpc_Topology != NULL)
         {
            this->mpc_Topology->HideAll(true);
         }

         this->mpc_ActBusEdit = new C_SdBueBusEditWidget(ou32_Index, this->msn_BusEditTabIndex, this);
         connect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigChanged, this, &C_SdHandlerWidget::m_DataChanged);
         connect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigErrorChange, this, &C_SdHandlerWidget::m_ErrorChange);
         connect(this->mpc_ActBusEdit, &C_SdBueBusEditWidget::SigNameChanged, this, &C_SdHandlerWidget::SigNameChanged);

         // show button for DBC file export, Ethernet is not supported yet!
         if (pc_Bus->e_Type == C_OSCSystemBus::E_Type::eCAN)
         {
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

      this->ms32_SubMode = os32_SubMode;
      this->mu32_Index = ou32_Index;
   }

   this->m_SetFlag(ou32_Flag);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle for global key press event

   \param[in,out] opc_Event Event identification and information

   \return
   True  Handled
   False Not handled

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_SdHandlerWidget::GlobalUserKeyPress(QKeyEvent * const opc_Event)
{
   if ((opc_Event->key() == static_cast<sintn>(Qt::Key_S)) &&
       (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      this->Save();
   }
   else if ((opc_Event->key() == static_cast<sintn>(Qt::Key_F)) &&
            (opc_Event->modifiers().testFlag(Qt::ControlModifier) == true))
   {
      Q_EMIT this->SigSearch();
   }
   else if (opc_Event->key() == static_cast<sintn>(Qt::Key_F12))
   {
      // open project save
      this->SaveAs();
   }
   else
   {
      //No specific handling
   }

   return false;
}

//-----------------------------------------------------------------------------
void C_SdHandlerWidget::m_DataChanged(void)
{
   this->mq_DataChanged = true;

   Q_EMIT this->SigDataChanged(true, false, ms32_MODE_SYSDEF, this->ms32_SubMode, this->mu32_Index);
}

//-----------------------------------------------------------------------------
void C_SdHandlerWidget::m_NodeChanged(const uint32 ou32_Index)
{
   this->mq_DataChanged = true;

   Q_EMIT this->SigDataChanged(true, false, ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_NODEEDIT, ou32_Index);
}

//-----------------------------------------------------------------------------
void C_SdHandlerWidget::m_BusChanged(const uint32 ou32_Index)
{
   this->mq_DataChanged = true;

   Q_EMIT this->SigDataChanged(true, false, ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, ou32_Index);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Switch to bus edit widget

   \param[in] ou32_Index   Bus index
   \param[in] orc_BusName  Bus name

   \created     24.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::m_SwitchToBus(const uint32 ou32_Index, const QString & orc_BusName)
{
   Q_EMIT this->SigChangeMode(ms32_MODE_SYSDEF, ms32_SUBMODE_SYSDEF_BUSEDIT, ou32_Index, orc_BusName, "",
                              mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR);
}

//-----------------------------------------------------------------------------
void C_SdHandlerWidget::m_SetFlag(const uint32 ou32_Flag) const
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

//-----------------------------------------------------------------------------
/*!
   \brief   Handle error change

   \created     31.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::m_ErrorChange(void)
{
   Q_EMIT this->SigErrorChanged(this->mu32_Index);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle generate code action

   \created     03.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::GenerateCode(void) const
{
   if (C_ImpUtil::h_CheckProjForCodeGeneration(this->parentWidget()) == true)
   {
      if (this->ms32_SubMode == ms32_SUBMODE_SYSDEF_TOPOLOGY)
      {
         C_ImpUtil::h_ExportCodeAll(this->parentWidget());
      }
      else if (this->ms32_SubMode == ms32_SUBMODE_SYSDEF_NODEEDIT)
      {
         std::vector<uint32> c_Indices;
         const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_Index);
         tgl_assert(pc_Node != NULL);
         c_Indices.push_back(this->mu32_Index);
         C_ImpUtil::h_ExportCodeNodes(c_Indices, this->parentWidget());
      }
      else
      {
         //Unexpected
         tgl_assert(false);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call for (DBC file) export functionality.

   \created     16.05.2018  STW/D.Pohl
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::Export(void)
{
   uint32 u32_NumOfInputNodes = 0;
   uint32 u32_NumOfInputMessages = 0;

   std::set<uint32> c_CanMessageIds; // to count CAN messages
   uint32 u32_NumOfInputSignals = 0;

   tgl_assert(this->mpc_ActBusEdit != NULL);
   if (this->mpc_ActBusEdit != NULL)
   {
      // Is it Possible to Use a dbc for Ethernet Configuration?
      // answer: yes (see https://kb.vector.com/entry/798/)
      // but currently only CAN networks are supported!
      const stw_opensyde_core::C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
         this->mu32_Index);

      // check if we have problems for export
      bool q_BusConflict = C_PuiSdHandler::h_GetInstance()->CheckBusConflict(this->mu32_Index);
      if (q_BusConflict == true)
      {
         // display error
         stw_scl::C_SCLString c_Message = "Bus \"" + pc_Bus->c_Name + "\" has invalid content. "
                                          "DBC file export cannot be performed.";
         C_OgeWiCustomMessage c_ExportWarnings(this, C_OgeWiCustomMessage::E_Type::eERROR);
         c_ExportWarnings.SetHeading(C_GtGetText::h_GetText("DBC file export"));
         c_ExportWarnings.SetDescription(C_GtGetText::h_GetText(c_Message.c_str()));
         c_ExportWarnings.Execute();
      }
      else
      {
         // network OK, ready to export
         if (pc_Bus->e_Type == C_OSCSystemBus::E_Type::eCAN) // defensive style: normally else case can't occur because
                                                             // then the export button is not displayed.
         {
            std::vector<uint32> c_NodeIndexes;
            std::vector<uint32> c_InterfaceIndexes;
            C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(this->mu32_Index,
                                                                                               c_NodeIndexes,
                                                                                               c_InterfaceIndexes);
            // for each node there must be an interface index therefore same size is guaranteed
            tgl_assert(c_NodeIndexes.size() == c_InterfaceIndexes.size());

            // get selected CAN protocol
            const Ui::C_SdBueBusEditWidget * const pc_SdBueBusEdit = this->mpc_ActBusEdit->GetUiWidget();
            const C_OSCCanProtocol::E_Type e_ComType = pc_SdBueBusEdit->pc_WidgetComIfDescr->GetActProtocol();

            C_CieConverter::C_CIECommDefinition c_CommDef; // data structure for DBC file export
            c_CommDef.c_Bus.c_Name = pc_Bus->c_Name;
            c_CommDef.c_Bus.c_Comment = pc_Bus->c_Comment;

            u32_NumOfInputNodes = c_NodeIndexes.size();
            for (uint32 u32_Pos = 0; u32_Pos < u32_NumOfInputNodes; u32_Pos++)
            {
               uint32 u32_NodeIndex = c_NodeIndexes[u32_Pos];
               uint32 u32_InterfaceIndex = c_InterfaceIndexes[u32_Pos];
               const C_OSCCanMessageContainer * const pc_CanMessageContainer = C_PuiSdHandler::h_GetInstance()->
                                                                               GetCanProtocolMessageContainer(
                  u32_NodeIndex,
                  e_ComType,
                  u32_InterfaceIndex);

               // only export active devices
               if ((pc_CanMessageContainer != NULL) && (pc_CanMessageContainer->q_IsComProtocolUsedByInterface == true))
               {
                  sint32 s32_Error = stw_errors::C_NO_ERR;
                  stw_scl::C_SCLStringList c_Warnings;
                  const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(u32_NodeIndex);
                  C_CieConverter::C_CIENode c_CurrentCIENode;
                  c_CurrentCIENode.c_Properties.c_Name = pc_Node->c_Properties.c_Name;
                  c_CurrentCIENode.c_Properties.c_Comment = pc_Node->c_Properties.c_Comment;

                  // get TX can messages
                  const std::vector<C_OSCCanMessage> c_TxMsgs = pc_CanMessageContainer->GetMessagesConst(true);
                  std::vector<C_OSCCanMessage>::const_iterator c_TxIter;
                  for (c_TxIter = c_TxMsgs.begin(); c_TxIter != c_TxMsgs.end(); ++c_TxIter)
                  {
                     C_CieConverter::C_CIENodeMessage c_CurrentMessage;
                     s32_Error |= C_CieDataPoolListAdapter::h_ConvertToDBCImportMessage(this->mu32_Index, e_ComType,
                                                                                        *c_TxIter, c_CurrentMessage,
                                                                                        c_Warnings);
                     c_CurrentCIENode.c_TxMessages.push_back(c_CurrentMessage);
                     // for counting messages and signals
                     std::set<uint32>::const_iterator c_Iter = c_CanMessageIds.find(c_TxIter->u32_CanId);
                     if (c_Iter == c_CanMessageIds.end())
                     {
                        c_CanMessageIds.insert(c_TxIter->u32_CanId);
                        // count signals
                        u32_NumOfInputSignals += c_TxIter->c_Signals.size();
                     }
                  }

                  // get RX can messages
                  const std::vector<C_OSCCanMessage> & c_RxMsgs = pc_CanMessageContainer->GetMessagesConst(false);
                  std::vector<C_OSCCanMessage>::const_iterator c_RxIter;
                  for (c_RxIter = c_RxMsgs.begin(); c_RxIter != c_RxMsgs.end(); ++c_RxIter)
                  {
                     C_CieConverter::C_CIENodeMessage c_CurrentMessage;
                     s32_Error |= C_CieDataPoolListAdapter::h_ConvertToDBCImportMessage(this->mu32_Index, e_ComType,
                                                                                        *c_RxIter, c_CurrentMessage,
                                                                                        c_Warnings);
                     c_CurrentCIENode.c_RxMessages.push_back(c_CurrentMessage);
                     // for counting messages and signals
                     std::set<uint32>::const_iterator c_Iter = c_CanMessageIds.find(c_RxIter->u32_CanId);
                     if (c_Iter == c_CanMessageIds.end())
                     {
                        c_CanMessageIds.insert(c_RxIter->u32_CanId);
                        // count signals
                        u32_NumOfInputSignals += c_RxIter->c_Signals.size();
                     }
                  }
                  // save current node
                  c_CommDef.c_Nodes.push_back(c_CurrentCIENode);

                  u32_NumOfInputMessages = c_CanMessageIds.size();

                  if (s32_Error != stw_errors::C_NO_ERR)
                  {
                     // display warnings
                     C_OgeWiCustomMessage c_ExportWarnings(this, C_OgeWiCustomMessage::E_Type::eWARNING);
                     c_ExportWarnings.SetHeading(C_GtGetText::h_GetText("DBC file export"));
                     c_ExportWarnings.SetDescription(C_GtGetText::h_GetText(
                                                        "Warnings occurred during DBC file export."));
                     c_ExportWarnings.SetDetails(c_Warnings.GetText().c_str());
                     c_ExportWarnings.Execute();
                  }
               }
            }

            // export to file
            C_CieUtil::h_ExportFile(c_CommDef, this, u32_NumOfInputNodes, u32_NumOfInputMessages,
                                    u32_NumOfInputSignals);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call for (RTF file) documentation functionality.

   \created     22.06.2018  STW/D.Pohl
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::RtfExport(void)
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
         c_Message.Execute();
      }
      else
      {
         // export dialog
         QPointer<C_OgePopUpDialog> const c_PopUpDialog = new C_OgePopUpDialog(this, this);
         C_RtfExportWidget * const pc_DialogExportReport = new C_RtfExportWidget(*c_PopUpDialog);

         // resize
         const QSize c_SizeImportReport(890, 410);

         c_PopUpDialog->SetSize(c_SizeImportReport);

         stw_scl::C_SCLString c_RtfPath = stw_scl::C_SCLString(
            C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownRtfPath().toStdString().c_str());
         stw_scl::C_SCLString c_CompanyName = stw_scl::C_SCLString(
            C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownRtfCompanyName().toStdString().c_str());
         stw_scl::C_SCLString c_CompanyLogoPath = stw_scl::C_SCLString(
            C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownRtfCompanyLogoPath().toStdString().c_str());

         if (c_RtfPath == "")
         {
            // get default file name with project path (if using the RTF dialog the first time)

            QFileInfo c_FileTmp; // get project name by project file path
            c_FileTmp.setFile(C_PuiProject::h_GetInstance()->GetPath());
            QString c_DefaultFilename = c_FileTmp.baseName().toStdString().c_str();
            c_DefaultFilename += ".rtf";

            QString c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath(); // get project path

            c_RtfPath = stw_scl::C_SCLString(c_Folder.toStdString().c_str()) +
                        stw_scl::C_SCLString(c_DefaultFilename.toStdString().c_str());
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
         if (c_PopUpDialog->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            // save inputs as user settings
            pc_DialogExportReport->GetRtfPath(c_RtfPath);
            C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownRtfPath(QString(c_RtfPath.c_str()));
            pc_DialogExportReport->GetCompanyName(c_CompanyName);
            C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownRtfCompanyName(QString(c_CompanyName.c_str()));
            pc_DialogExportReport->GetCompanyLogoPath(c_CompanyLogoPath);
            C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownRtfCompanyLogoPath(QString(
                                                                                          c_CompanyLogoPath.c_str()));
            C_UsHandler::h_GetInstance()->Save();

            // export to RTF file
            stw_scl::C_SCLStringList c_Warnings;
            stw_scl::C_SCLString c_Error;
            sint32 s32_Return = pc_DialogExportReport->ExportToRtf(c_RtfPath, c_CompanyName, c_CompanyLogoPath,
                                                                   this->mpc_Topology,
                                                                   c_Warnings, c_Error);
            if (s32_Return == stw_errors::C_NO_ERR)
            {
               QString c_Details = QString("%1<a href=\"file:%2\"><span style=\"color: %3;\">%4</span></a>.").
                                   arg(C_GtGetText::h_GetText("File saved at ")).
                                   arg(QString(c_RtfPath.c_str())).
                                   arg(mc_STYLESHEET_GUIDE_COLOR_LINK).
                                   arg(QString(c_RtfPath.c_str()));
               C_OgeWiCustomMessage c_MessageResult(this);
               c_MessageResult.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
               c_MessageResult.SetDescription(C_GtGetText::h_GetText("RTF document successfully created."));
               c_MessageResult.SetDetails(c_Details);
               c_MessageResult.Execute();
            }
            else if (s32_Return == stw_errors::C_WARN)
            {
               stw_scl::C_SCLString c_Details = "Warnings: \r\n" + c_Warnings.GetText();

               C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eWARNING);
               c_MessageResult.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
               c_MessageResult.SetDescription(C_GtGetText::h_GetText("Warnings occurred on RTF File Export."));
               c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_Details.c_str()));
               c_MessageResult.Execute();
            }
            else
            {
               C_OgeWiCustomMessage c_MessageResult(this, C_OgeWiCustomMessage::E_Type::eERROR);
               c_MessageResult.SetHeading(C_GtGetText::h_GetText("RTF File Export"));
               c_MessageResult.SetDescription(C_GtGetText::h_GetText("RTF file export error occurred."));
               c_MessageResult.SetDetails(C_GtGetText::h_GetText(c_Error.c_str()));
               c_MessageResult.Execute();
            }
         }

         if (c_PopUpDialog != NULL)
         {
            c_PopUpDialog->HideOverlay();
         }
         //lint -e{429}  no memory leak because of the parent of pc_DialogExportReport and the Qt memory management
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call specific help pages

   \created     30.10.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_SdHandlerWidget::CallHelp(void)
{
   if (this->mpc_ActNodeEdit != NULL)
   {
      //TabIndex == Datapools
      if (this->mpc_ActNodeEdit->GetTabIndex() == 1)
      {
         stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
            "stw_opensyde_gui::C_SdNdeDbViewWidget");
      }
      else if (this->mpc_ActNodeEdit->GetTabIndex() == 2) //TabIndex == COMM
      {
         stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
            "stw_opensyde_gui::C_SdBueComIfDescriptionWidget");
      }
      else //properties
      {
         stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
            "stw_opensyde_gui::C_SdNdeNodeEditWidget");
      }
   }
   else if (this->mpc_ActBusEdit != NULL)
   {
      //TabIndex == COMM
      if (this->mpc_ActBusEdit->GetTabIndex() == 1)
      {
         stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
            "stw_opensyde_gui::C_SdBueComIfDescriptionWidget");
      }
      else //properties
      {
         stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
            "stw_opensyde_gui::C_SdBueBusEditWidget");
      }
   }
   else
   {
      //ms32_SUBMODE_SYSDEF_TOPOLOGY and others call stw_opensyde_gui::C_SdHandlerWidget
      stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(
         "stw_opensyde_gui::C_SdHandlerWidget");
   }
}
