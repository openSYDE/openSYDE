//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for showing an application in the application list (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.01.2016 STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFileDialog>

#include "C_Uti.h"
#include "C_PuiSdUtil.h"
#include "stwerrors.h"
#include "CSCLString.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDbWidget.h"
#include "C_OgePopUpDialog.h"
#include "ui_C_SdNdeDbWidget.h"
#include "C_PuiProject.h"
#include "constants.h"
#include "C_SdUtil.h"
#include "C_OgeWiUtil.h"
#include "C_OSCLoggingHandler.h"
#include "C_SdNdeDbProperties.h"
#include "C_OgeWiCustomMessage.h"
#include "C_ImpUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_scl;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     ou32_NodeIndex        Node index
   \param[in]     ou32_ApplicationIndex Application index
   \param[in,out] opc_Parent            Optional pointer to parent

   \created     30.01.2016 STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdNdeDbWidget::C_SdNdeDbWidget(const uint32 ou32_NodeIndex, const uint32 ou32_ApplicationIndex,
                                 QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbWidget),
   mu32_ApplicationIndex(ou32_ApplicationIndex),
   mu32_NodeIndex(ou32_NodeIndex),
   mpc_DataPoolLabel(NULL),
   mq_MessageBoxVisible(false)
{
   const QIcon c_IconError = QIcon("://images/Error_iconV2.svg");

   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "C_SdNdeDbWidget", true);

   //Remove debug string
   this->mpc_Ui->pc_PushButtonEdit->setText("");
   this->mpc_Ui->pc_PushButtonDelete->setText("");
   this->mpc_Ui->pc_PushButtonGenerateCode->setText("");
   this->mpc_Ui->pc_PushButtonOpenIde->setText("");
   this->mpc_Ui->pc_LabelErrorIcon->setText("");

   //Error icon
   this->mpc_Ui->pc_LabelErrorIcon->setPixmap(c_IconError.pixmap(mc_ICON_SIZE_24));

   //Edit Icon (SVG)
   this->mpc_Ui->pc_PushButtonEdit->setIconSize(mc_ICON_SIZE_24);
   this->mpc_Ui->pc_PushButtonEdit->SetCustomIcons("://images/IconEditActive.svg", "://images/IconEditHovered.svg",
                                                   "://images/IconEditClicked.svg",
                                                   "://images/IconEditDisabledBright.svg");

   //Delete Icon (SVG)
   this->mpc_Ui->pc_PushButtonDelete->setIconSize(mc_ICON_SIZE_24);
   this->mpc_Ui->pc_PushButtonDelete->SetCustomIcons("://images/system_views/IconTabClose.svg",
                                                     "://images/system_views/IconTabCloseHover.svg",
                                                     "://images/IconCloseClicked.svg",
                                                     "://images/IconCloseDisabled.svg");

   this->mpc_Ui->pc_PushButtonGenerateCode->setIconSize(mc_ICON_SIZE_24);
   this->mpc_Ui->pc_PushButtonGenerateCode->SetCustomIcons("://images/system_definition/IconGenerateCode.svg",
                                                           "://images/system_definition/IconGenerateCode.svg",
                                                           "://images/system_definition/IconGenerateCode.svg",
                                                           "://images/system_definition/IconGenerateCode.svg");

   this->mpc_Ui->pc_PushButtonOpenIde->setIconSize(mc_ICON_SIZE_24);
   this->mpc_Ui->pc_PushButtonOpenIde->SetCustomIcons("://images/system_definition/IconOpenExternalTool.svg",
                                                      "://images/system_definition/IconOpenExternalTool.svg",
                                                      "://images/system_definition/IconOpenExternalTool.svg",
                                                      "://images/system_definition/IconOpenExternalTool.svg");

   //Also does data connect!
   m_LoadData();

   //Visual connections
   connect(this->mpc_Ui->pc_PushButtonEdit, &QPushButton::clicked, this, &C_SdNdeDbWidget::m_OnEdit);
   connect(this->mpc_Ui->pc_PushButtonDelete, &QPushButton::clicked, this, &C_SdNdeDbWidget::m_OnDelete);
   connect(this->mpc_Ui->pc_PushButtonGenerateCode, &QPushButton::clicked, this,
           &C_SdNdeDbWidget::m_OnCodeExportClicked);
   connect(this->mpc_Ui->pc_PushButtonOpenIde, &QPushButton::clicked, this, &C_SdNdeDbWidget::m_OnOpenIdeClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     30.01.2016 STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdNdeDbWidget::~C_SdNdeDbWidget(void)
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_DataPoolLabel and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_LabelType->setText(C_GtGetText::h_GetText("Type"));
   this->mpc_Ui->pc_LabelDataPoolEmpty->setText(C_GtGetText::h_GetText("none"));
   this->mpc_Ui->pc_LabelDataPoolDescription->setText(C_GtGetText::h_GetText("Owned Datapools"));

   //Update application index
   this->mpc_Ui->pc_LabelNamePrefix->setText(QString(C_GtGetText::h_GetText("#%1 -")).arg(this->mu32_ApplicationIndex +
                                                                                          1));

   //Tool tips
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText("Name"),
                                                     C_GtGetText::h_GetText(
                                                        "Symbolic Data Block name. Unique within node\n"
                                                        "\nFollowing C naming conventions are required:"
                                                        "\n - must not be empty"
                                                        "\n - only alphanumeric characters + \"_\""
                                                        "\n - should not be longer than 31 characters"));
   this->mpc_Ui->pc_LabelType->SetToolTipInformation(C_GtGetText::h_GetText("Type"),
                                                     C_GtGetText::h_GetText("Type of Data Block."));
   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this Data Block."));
   this->mpc_Ui->pc_LabelDataPoolDescription->SetToolTipInformation(C_GtGetText::h_GetText("Owned Datapools"),
                                                                    C_GtGetText::h_GetText(
                                                                       "List of all Datapools which are mapped to this programmable application."
                                                                       "\n(Relevant for code generation)"));

   this->mpc_Ui->pc_PushButtonGenerateCode->SetToolTipInformation(C_GtGetText::h_GetText("Generate Code"),
                                                                  C_GtGetText::h_GetText(
                                                                     "Generate Code for this programming application."
                                                                     "\n - openSYDE server initialization wrapper"
                                                                     "\n   Create a .c and .h file providing initialization structures for the OSS DPD and DPH init functions."
                                                                     "\n - COMM stack definition"
                                                                     "\n   Create a .c and .h file providing entire communication stack configuration."
                                                                     "\n - Datapools"
                                                                     "\n   Generate code for Datapool settings of an openSYDE node."));

   this->mpc_Ui->pc_PushButtonOpenIde->SetToolTipInformation(C_GtGetText::h_GetText("Open IDE"),
                                                             C_GtGetText::h_GetText(
                                                                "Execute IDE Call specified in project properties."));

   this->mpc_Ui->pc_PushButtonEdit->SetToolTipInformation(C_GtGetText::h_GetText("Edit"),
                                                          C_GtGetText::h_GetText("Edit Data Block properties."));

   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete"),
                                                            C_GtGetText::h_GetText("Delete Data Block."));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update data pool section

   \created     03.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::UpdateDataPools(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   uint8 u8_OwnedDpCounter = 0;

   if (pc_Node != NULL)
   {
      //Clear existing
      if (this->mpc_DataPoolLabel != NULL)
      {
         this->mpc_DataPoolLabel->hide();
         this->mpc_DataPoolLabel = NULL;
      }

      //Add new
      if (pc_Node->c_DataPools.size() > 0UL)
      {
         QString c_Text;
         //Add link for each data pool
         for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
         {
            const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
            if (rc_DataPool.s32_RelatedDataBlockIndex >= 0)
            {
               if (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) == this->mu32_ApplicationIndex)
               {
                  //increase counter
                  ++u8_OwnedDpCounter;

                  if (c_Text.isEmpty() == false)
                  {
                     c_Text += "; <br/>";
                  }
                  c_Text += QString(C_Uti::h_GetLink(QString("%1_(%2)").arg(rc_DataPool.c_Name.c_str()).arg(
                                                        C_PuiSdUtil::h_ConvertDataPoolTypeToString(rc_DataPool.
                                                                                                   e_Type)),
                                                     mc_STYLE_GUIDE_COLOR_6, QString::number(u32_ItDataPool)));
               }
            }
         }
         if (c_Text.isEmpty() == false)
         {
            C_OgeLabGroupItem * const pc_NewLabel = new C_OgeLabGroupItem(this);
            //Configure label
            pc_NewLabel->setMaximumWidth(275);
            pc_NewLabel->setWordWrap(false);
            pc_NewLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            pc_NewLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
            pc_NewLabel->setFocusPolicy(Qt::NoFocus);
            connect(pc_NewLabel, &QLabel::linkActivated, this, &C_SdNdeDbWidget::m_OnDataPoolLinkClicked);
            //Set text (last!)
            pc_NewLabel->setText(c_Text);
            //Add to UI
            this->mpc_Ui->verticalLayout_5->insertWidget(0, pc_NewLabel, 1);
            //Store internally
            this->mpc_DataPoolLabel = pc_NewLabel;
         }
      }

      //update owned dp count
      this->mpc_Ui->pc_LabelDataPoolDescription->setText(QString(C_GtGetText::h_GetText("Owned Datapools (%1)")).arg(
                                                            u8_OwnedDpCounter));
   }
   //Conditional visibility for empty data pool label
   this->mpc_Ui->pc_LabelDataPoolEmpty->setVisible(this->mpc_DataPoolLabel == NULL);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the application index

   \param[in]     ou32_ApplicationIndex          New index of the application

   \created     21.12.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::UpdateApplicationIndex(const uint32 ou32_ApplicationIndex)
{
   this->mu32_ApplicationIndex = ou32_ApplicationIndex;

   this->InitStaticNames();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get node index

   \return
   Current node index

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_SdNdeDbWidget::GetNodeIndex(void) const
{
   return this->mu32_NodeIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get application index

   \return
   Current application index

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_SdNdeDbWidget::GetApplicationIndex(void) const
{
   return this->mu32_ApplicationIndex;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check process ID value

   \param[in] osn_Value Current value

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::CheckProcessIdError(void) const
{
   QString c_Info;
   bool q_Valid = false;
   const std::vector<uint32> c_UsedIds = C_SdUtil::h_GetUsedProcessIdsForApplicationUniqueAndSortedAscending(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      tgl_assert(pc_Node->CheckApplicationProcessIdValid(this->mu32_ApplicationIndex, q_Valid) == C_NO_ERR);
      c_Info =
         C_SdUtil::h_InitUsedIdsString(c_UsedIds, pc_Node->c_Properties.c_Name.c_str(), C_GtGetText::h_GetText("node"));
   }
   this->mpc_Ui->pc_LabelErrorIcon->setVisible(!q_Valid);
   if (q_Valid == false)
   {
      const QString c_Heading = C_GtGetText::h_GetText("Data block: Process ID invalid");
      this->mpc_Ui->pc_LabelErrorIcon->SetToolTipInformation(c_Heading, c_Info, C_NagToolTip::eERROR);
   }
   else
   {
      this->mpc_Ui->pc_LabelErrorIcon->SetToolTipInformation("", "", C_NagToolTip::eERROR);
   }
   //Deactivate error tool tip on interaction item
   //this->mpc_Ui->pc_SpinBoxProcessId->SetToolTipAdditionalInfo(c_Info, !q_Valid);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load new data into UI

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_LoadData(void)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   const C_OSCNodeApplication * const pc_Application = C_PuiSdHandler::h_GetInstance()->GetApplication(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);

   if ((pc_Application != NULL) && (pc_Node != NULL))
   {
      const C_OSCDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
      if (pc_DevDef != NULL)
      {
         //Update name label
         this->mpc_Ui->pc_LabelName->setText(pc_Application->c_Name.c_str());
         if (pc_Application->c_Comment == "")
         {
            this->mpc_Ui->pc_LabelCommentValue->setText(C_GtGetText::h_GetText("<No comment>"));
         }
         else
         {
            this->mpc_Ui->pc_LabelCommentValue->setText(pc_Application->c_Comment.c_str());
         }

         UpdateDataPools();

         switch (pc_Application->e_Type)
         {
         case C_OSCNodeApplication::E_Type::eFILE_CONTAINER:
            this->mpc_Ui->pc_LabelTypeValue->setText(C_GtGetText::h_GetText("File Container"));
            break;
         case C_OSCNodeApplication::E_Type::ePROGRAMMABLE_APPLICATION:
            this->mpc_Ui->pc_LabelTypeValue->setText(C_GtGetText::h_GetText("Programmable Application"));
            break;
         case C_OSCNodeApplication::E_Type::eBINARY:
            this->mpc_Ui->pc_LabelTypeValue->setText(C_GtGetText::h_GetText("Binary"));
            break;
         }
      }

      //This section has to be after the application type combo box is set
      // (also does not change data so there is no conflict with the warning above)
      m_HandleType();

      //Initial error check
      CheckProcessIdError();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   handle show or hide of data pool section

   \created     03.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_HandleType(void) const
{
   C_OSCNodeApplication::E_Type e_Type = m_GetType();
   const bool q_Visible = (e_Type == C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION);

   //Data pool
   this->mpc_Ui->pc_GroupBoxDataPoolDescription->setVisible(q_Visible);
   this->mpc_Ui->pc_ScrollAreaDataPool->setVisible(q_Visible);

   //Project
   this->mpc_Ui->pc_PushButtonGenerateCode->setVisible(q_Visible);
   this->mpc_Ui->pc_PushButtonOpenIde->setVisible(e_Type != C_OSCNodeApplication::eFILE_CONTAINER);
}

//-----------------------------------------------------------------------------
/*!
   \brief   On edit action

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnEdit(void)
{
   //Previous implementation
   //this->mpc_Ui->pc_LabelName->setVisible(false);
   //this->mpc_Ui->pc_LineEditName->setVisible(true);
   //this->mpc_Ui->pc_LineEditName->selectAll();
   //this->mpc_Ui->pc_LineEditName->setFocus();

   m_ShowProperties();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show properties

   \created     04.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_ShowProperties(void)
{
   const C_OSCNodeApplication * const pc_Application = C_PuiSdHandler::h_GetInstance()->GetApplication(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);

   if (pc_Application != NULL)
   {
      QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
      C_SdNdeDbProperties * const pc_Dialog = new C_SdNdeDbProperties(this->mu32_NodeIndex,
                                                                      this->mu32_ApplicationIndex, *c_New);

      //Resize (200 -> use minimum possible height automatically)
      if (pc_Application->e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
      {
         c_New->SetSize(C_SdNdeDbProperties::h_GetBinaryWindowSize());
      }
      else
      {
         c_New->SetSize(C_SdNdeDbProperties::h_GetDefaultWindowSize());
      }

      if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
      {
         C_OSCNodeApplication c_Copy = *pc_Application;
         pc_Dialog->ApplyNewData(c_Copy);
         tgl_assert(C_PuiSdHandler::h_GetInstance()->SetApplication(this->mu32_NodeIndex, this->mu32_ApplicationIndex,
                                                                    c_Copy) == C_NO_ERR);
         pc_Dialog->HandleDataPools(this->mu32_ApplicationIndex);
         //Reload all
         this->m_LoadData();
      }

      if (c_New != NULL)
      {
         c_New->HideOverlay();
      }
      //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   On delete action

   \created     15.12.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnDelete(void)
{
   bool q_DeletePoosible = false;

   if (m_GetType() != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
   {
      q_DeletePoosible = true;
   }
   else
   {
      //More complex handling required for programmable applications
      //Check if the action does have critical impact
      if (m_CountAllAssociatedDataPools() > 0)
      {
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
         //Detected change from programmable application to other type so ask user to confirm
         c_Message.SetHeading(C_GtGetText::h_GetText("Data Block delete"));
         c_Message.SetDescription(C_GtGetText::h_GetText(
                                     "Do you really want to delete this Data Block?\n"
                                     "All owned Datapools will become unassigned."));
         c_Message.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
         c_Message.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
         if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
         {
            //Delete each associated data pool
            m_UnassignAllAssociatedDataPools();
            //Trigger application delete
            Q_EMIT this->SigDelete(this->mu32_NodeIndex, this->mu32_ApplicationIndex);
         }
         else
         {
            //No action requested
         }
      }
      else
      {
         q_DeletePoosible = true;
      }
   }
   if (q_DeletePoosible == true)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::E_Type::eQUESTION);
      //Detected change from programmable application to other type so ask user to confirm
      c_Message.SetHeading(C_GtGetText::h_GetText("Data Block delete "));
      c_Message.SetDescription(C_GtGetText::h_GetText("Do you really want to delete this Data Block?"));
      c_Message.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
      c_Message.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
      if (c_Message.Execute() == C_OgeWiCustomMessage::eYES)
      {
         //Trigger application delete
         Q_EMIT this->SigDelete(this->mu32_NodeIndex, this->mu32_ApplicationIndex);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle data pool link clicked

   \created     08.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnDataPoolLinkClicked(const QString & orc_Link)
{
   bool q_Ok;
   const uint32 u32_Index = orc_Link.toUInt(&q_Ok);

   if (q_Ok == true)
   {
      Q_EMIT this->SigOpenDataPool(u32_Index);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Unassign all data pools associated to this application

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_UnassignAllAssociatedDataPools(void) const
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) == this->mu32_ApplicationIndex))
         {
            tgl_assert(C_PuiSdHandler::h_GetInstance()->UnassignDataPoolApplication(this->mu32_NodeIndex,
                                                                                    u32_ItDataPool) == C_NO_ERR);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current application type

   \return
   Current application type

   \created     03.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCNodeApplication::E_Type C_SdNdeDbWidget::m_GetType(void) const
{
   C_OSCNodeApplication::E_Type e_Retval = C_OSCNodeApplication::eBINARY;
   const C_OSCNodeApplication * const pc_Application = C_PuiSdHandler::h_GetInstance()->GetApplication(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);

   if (pc_Application != NULL)
   {
      e_Retval = pc_Application->e_Type;
   }

   return e_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Count all data pools which are associated with this application

   \return
   Number of data pools which are associated with this application (0 in error case)

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_SdNdeDbWidget::m_CountAllAssociatedDataPools(void) const
{
   uint32 u32_Retval = 0;
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      for (uint32 u32_ItDataPool = 0; u32_ItDataPool < pc_Node->c_DataPools.size(); ++u32_ItDataPool)
      {
         const C_OSCNodeDataPool & rc_DataPool = pc_Node->c_DataPools[u32_ItDataPool];
         if ((rc_DataPool.s32_RelatedDataBlockIndex >= 0) &&
             (static_cast<uint32>(rc_DataPool.s32_RelatedDataBlockIndex) == this->mu32_ApplicationIndex))
         {
            ++u32_Retval;
         }
      }
   }
   return u32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   trigger code export

   \created     09.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnCodeExportClicked(void)
{
   std::vector<stw_types::uint32> c_Node;
   std::vector<stw_types::uint32> c_Application;
   std::vector<std::vector<stw_types::uint32> > c_Applications;

   if (C_ImpUtil::h_CheckProjForCodeGeneration(this) == true)
   {
      c_Node.push_back(this->mu32_NodeIndex);               // c_Node = [mu32_NodeIndex] (1-length-vector)
      c_Application.push_back(this->mu32_ApplicationIndex); // c_Application = [mu32_ApplicationIndex] (1-length-vector)
      c_Applications.push_back(c_Application);              // c_Applications = [[mu32_ApplicationIndex]] (vector of
                                                            // lenght 1, where the only entry is another vector
                                                            // of lenght 1)
      C_ImpUtil::h_ExportCode(c_Node, c_Applications, this);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger open IDE

   \created     09.10.2018  STW/G.Landsgesell
*/
//-----------------------------------------------------------------------------
void C_SdNdeDbWidget::m_OnOpenIdeClicked(void)
{
   const C_OSCNodeApplication * const pc_Application = C_PuiSdHandler::h_GetInstance()->GetApplication(
      this->mu32_NodeIndex, this->mu32_ApplicationIndex);
   const QString c_IDECall = QString(pc_Application->c_IDECall.c_str());

   if (c_IDECall == "")
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR,
                                        C_GtGetText::h_GetText("No IDE provided. Please edit Data Block Properties and "
                                                               "insert an IDE Call."));
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Open IDE"));
      c_MessageBox.Execute();
   }
   else if (C_ImpUtil::h_OpenIDE(c_IDECall) != C_NO_ERR)
   {
      C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR,
                                        C_GtGetText::h_GetText("Could not start IDE most likely due to insufficient "
                                                               "permissions or the executable is missing."));
      c_MessageBox.SetHeading(C_GtGetText::h_GetText("Open IDE"));
      c_MessageBox.SetDetails(QString(C_GtGetText::h_GetText(
                                         "The following call returned an error: \n%1")).arg(c_IDECall));
      c_MessageBox.Execute();
   }
   else
   {
      // Nothing to do
   }
}
