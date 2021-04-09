//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for toolbox of system definition

   Offers devices in different lists (C_SdTopologyListWidget) as icons.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"
#include <QLabel>
#include <QFileDialog>
#include <QMimeData>
#include <QBoxLayout>
#include <QDebug>

#include "TGLFile.h"
#include "constants.h"
#include "CSCLIniFile.h"
#include "C_SdTopologyToolbox.h"
#include "C_GtGetText.h"
#include "ui_C_SdTopologyToolbox.h"
#include "C_SebToolboxUtil.h"
#include "C_OSCSystemDefinition.h"
#include "C_UsHandler.h"
#include "C_OgeWiCustomMessage.h"
#include "C_PuiProject.h"
#include "C_PuiSdHandler.h"
#include "C_OgePubIconOnly.h"
#include "C_ImpUtil.h"
#include "C_Uti.h"
#include "C_OSCLoggingHandler.h"

#include "stwtypes.h"
#include "stwerrors.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_tgl;
using namespace stw_scl;
using namespace stw_errors;

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
C_SdTopologyToolbox::C_SdTopologyToolbox(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdTopologyToolbox),
   mpc_List(NULL),
   mpc_Spacer(new QSpacerItem(20, 10, QSizePolicy::Expanding)),
   mpc_Label(new C_OgeLabTopologyToolboxUserNodes(this))
{
   this->mpc_Ui->setupUi(this);
   this->setAcceptDrops(true);
   this->mpc_Ui->pc_ScrollAreaSearch->setVisible(false);
   this->mpc_Ui->pc_BlankWidgetSearch->setVisible(false);
   this->mpc_Ui->pc_BlankWidgetSearch->SetBackgroundColor(12);
   this->mpc_Ui->pc_BlankTopWidgetSearch->setVisible(false);
   this->mpc_Ui->pc_BlankTopWidgetSearch->SetBackgroundColor(12);
   this->mpc_Ui->pc_LabelSearchNotFound->setVisible(false);
   this->mpc_Ui->pc_LabelSearchNotFound->setText(C_GtGetText::h_GetText("No results found"));
   this->mpc_Ui->pc_LabelSearchNotFound->setStyleSheet("background-color: rgb(250, 250, 250);");

   this->mpc_Ui->pc_ScrollAreaNodesWidget->SetBackgroundColor(-1);
   this->mpc_Ui->pc_ScrollAreaBusesWidget->SetBackgroundColor(-1);
   this->mpc_Ui->pc_ScrollAreaDrawingWidget->SetBackgroundColor(-1);
   this->mpc_Ui->pc_ScrollAreaSearchWidget->SetBackgroundColor(-1);
   this->mpc_Ui->pc_ScrollAreaSearch->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   this->InitStaticNames();
   this->m_FillToolboxStatic();
   this->m_FillToolboxDynamic();

   // set the 'nodes' tab initially
   this->mpc_Ui->pc_TabWidget->setCurrentIndex(0);

   // The search list shall use the whole free area of the widget
   this->mpc_Ui->pc_ListWidgetSearch->SetMaximumHeightAdaption(false);

   this->mpc_Ui->pc_ListWidgetDrawing->setDropIndicatorShown(false);
   this->mpc_Ui->pc_ListWidgetSearch->setDropIndicatorShown(false);
   //this->mpc_Ui->pc_ListWidgetSearch->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   // Add label
   this->mpc_Label->setText("No user nodes declared.\nAdd any via the \"+\" button.");
   this->mpc_Label->setAlignment(Qt::AlignHCenter); // AlignHCenter
   this->mpc_Ui->pc_VerticalLayout1->setDirection(QBoxLayout::TopToBottom);
   this->mpc_Ui->pc_VerticalLayout1->addWidget(this->mpc_Label);

   // Add spacer after label
   this->mpc_Ui->pc_VerticalLayout1->addSpacerItem(this->mpc_Spacer);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent all elements and the Qt memory management

C_SdTopologyToolbox::~C_SdTopologyToolbox()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for a search entry change

   If the search text is empty, the search was finished.
   If the search text is not empty, the search result has to be updated.

   \param[in] orc_Text   Updated search text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::SearchChanged(const QString & orc_Text)
{
   this->mpc_Ui->pc_ListWidgetSearch->clear();
   QList<QListWidgetItem *> c_ListItems;
   QList<QListWidgetItem *>::const_iterator c_ItItem;
   bool q_NoItemsFound = true;

   if (orc_Text == "")
   {
      // no active search
      this->mpc_Ui->pc_BlankWidgetSearch->setVisible(false);
      this->mpc_Ui->pc_BlankTopWidgetSearch->setVisible(false);
      this->mpc_Ui->pc_LabelSearchNotFound->setVisible(false);
      this->mpc_Ui->pc_ScrollAreaSearch->setVisible(false);
      this->mpc_Ui->pc_TabWidget->setVisible(true);
      this->mpc_Ui->pc_ListWidgetSearch->SetMinimumSize(true);
   }
   else
   {
      this->mpc_Ui->pc_TabWidget->setVisible(false);
      this->mpc_Ui->pc_BlankWidgetSearch->setVisible(true);
      this->mpc_Ui->pc_BlankTopWidgetSearch->setVisible(true);
      this->mpc_Ui->pc_ScrollAreaSearch->setVisible(true);

      if (this->mpc_Ui->pc_ListWidgetSearch->GetMinimumSize() == true)
      {
         this->mpc_Ui->pc_ListWidgetSearch->SetMinimumSize(false);
         this->mpc_Ui->pc_ListWidgetSearch->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      }
   }

   for (sintn sn_ItListWidget = 0; sn_ItListWidget < mc_ListWidgets.size(); ++sn_ItListWidget)
   {
      c_ListItems = mc_ListWidgets[sn_ItListWidget]->findItems(orc_Text, Qt::MatchContains);
      for (c_ItItem = c_ListItems.begin(); c_ItItem != c_ListItems.end(); ++c_ItItem)
      {
         this->mpc_Ui->pc_ListWidgetSearch->addItem((*c_ItItem)->clone());
         q_NoItemsFound = false;
      }
   }

   c_ListItems = this->mpc_Ui->pc_ListWidgetDrawing->findItems(orc_Text, Qt::MatchContains);
   for (c_ItItem = c_ListItems.begin(); c_ItItem != c_ListItems.end(); ++c_ItItem)
   {
      this->mpc_Ui->pc_ListWidgetSearch->addItem((*c_ItItem)->clone());
      q_NoItemsFound = false;
   }

   this->mpc_Ui->pc_LabelSearchNotFound->setVisible(q_NoItemsFound);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(0, C_GtGetText::h_GetText("Nodes"));
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(1, C_GtGetText::h_GetText("Buses"));
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(2, C_GtGetText::h_GetText("Meta"));
   this->mpc_Ui->pc_LabelFreeElements->setText(C_GtGetText::h_GetText("Meta Information Elements"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten enter event slot

   To get the correct cursor back from the resizing implementation of
   C_OgeWiHover.

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::enterEvent(QEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   this->setCursor(Qt::ArrowCursor);
   this->parentWidget()->setMouseTracking(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten leave event slot

   To get the correct cursor back from the resizing implementation of
   C_OgeWiHover.

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::leaveEvent(QEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   this->parentWidget()->setMouseTracking(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag enter event slot

   Here: Accept external files

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   if (opc_Event->mimeData()->hasUrls() == true)
   {
      for (sintn sn_Counter = 0; sn_Counter < opc_Event->mimeData()->urls().size(); sn_Counter++)
      {
         const QFileInfo c_File(opc_Event->mimeData()->urls().at(sn_Counter).toString());

         // Accepting openSYDE device definition files as image
         if (c_File.completeSuffix().compare("syde_devdef") == 0)
         {
            opc_Event->accept();
         }
         if (c_File.completeSuffix().compare("ini") == 0)
         {
            opc_Event->accept();
         }
      }
   }

   QWidget::dragEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag move event slot

   Here: Accept external files

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   QWidget::dragMoveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event

   \param[in,out] opc_Event   Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::dropEvent(QDropEvent * const opc_Event)
{
   QStringList c_UserDeviceDefPaths;
   QStringList c_Errors;

   for (sintn sn_Counter = 0; sn_Counter < opc_Event->mimeData()->urls().size(); sn_Counter++)
   {
      QString c_Path = opc_Event->mimeData()->urls().at(sn_Counter).path();
      this->m_LoadUserDeviceDefinitionPaths(c_Path.remove(0, 1),
                                            c_UserDeviceDefPaths, c_Errors);
   }

   c_UserDeviceDefPaths = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_UserDeviceDefPaths, C_Uti::h_GetExePath());
   this->m_AddUserNodesToIni(c_UserDeviceDefPaths, c_Errors);

   QWidget::dropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fill toolbox with dynamic content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_FillToolboxDynamic(void)
{
   std::vector<C_OgePubIconOnly *> c_Icons;
   std::vector<C_OSCDeviceDefinition> c_Devices;
   std::vector<C_OSCDeviceGroup> c_DeviceGroups = C_OSCSystemDefinition::hc_Devices.GetDeviceGroups();
   for (uint32 u32_ItDeviceGroup = 0U; u32_ItDeviceGroup < c_DeviceGroups.size(); ++u32_ItDeviceGroup)
   {
      c_Devices = c_DeviceGroups[u32_ItDeviceGroup].GetDevices();

      if (c_DeviceGroups[u32_ItDeviceGroup].GetGroupName() != "User Nodes")
      {
         this->mc_Icon.addPixmap(static_cast<QPixmap>("://images/system_definition/PreviewNode.svg"), QIcon::Normal);
         this->mc_Icon.addPixmap(static_cast<QPixmap>("://images/system_definition/PreviewNode.svg"), QIcon::Selected);

         this->mpc_List = C_SebToolboxUtil::h_AddNewList(
            c_DeviceGroups[u32_ItDeviceGroup].GetGroupName().c_str(),
            this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets, this);

         if (this->mpc_List != NULL)
         {
            for (uint32 u32_ItDevice = 0U; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
            {
               this->m_FillToolboxWithDynamicNodes(c_Devices[u32_ItDevice]);
            }
         }
      }
      if (u32_ItDeviceGroup == static_cast<uint32>(c_DeviceGroups.size() - 1))
      {
         // Add final spacer
         C_SebToolboxUtil::h_AddFinalSpacer(this->mpc_Ui->pc_VerticalLayout1, this->mpc_List);

         // Add user nodes heading
         c_Icons = C_SebToolboxUtil::h_AddNewUserHeading("User Nodes",
                                                         this->mpc_Ui->pc_VerticalLayout1,
                                                         this);
         C_OgePubIconOnly * const pc_IconButton = c_Icons[0];
         pc_IconButton->SetToolTipInformation(C_GtGetText::h_GetText("Add User Nodes"),
                                              C_GtGetText::h_GetText(
                                                 "Add user nodes to toolbox from file device desription file (.syde_defdev)"
                                                 " or user nodes ini file (.ini)."));
         connect(pc_IconButton, &C_OgePubIconOnly::clicked, this, &C_SdTopologyToolbox::m_FileBrowseDialog);

         C_OgePubIconOnly * const pc_ClearAllUserNodesButton = c_Icons[1];
         pc_ClearAllUserNodesButton->SetToolTipInformation(C_GtGetText::h_GetText("Clear User Nodes"),
                                                           C_GtGetText::h_GetText(
                                                              "Delete all user nodes from toolbox."));

         connect(pc_ClearAllUserNodesButton, &C_OgePubIconOnly::clicked, this,
                 &C_SdTopologyToolbox::m_IconClearAllClicked);

         this->mc_Icon.addPixmap(static_cast<QPixmap>("://images/Icon_toolbox_user_node.svg"), QIcon::Normal);
         this->mc_Icon.addPixmap(static_cast<QPixmap>("://images/Icon_toolbox_user_node.svg"), QIcon::Selected);

         this->mpc_List = C_SebToolboxUtil::h_AddNewList("", this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                                         this);

         // True: Fill the toolbox with user devices
         // False: Set label no user devices existing
         if (c_DeviceGroups[u32_ItDeviceGroup].GetGroupName() == "User Nodes")
         {
            for (uint32 u32_ItDevice = 0U; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
            {
               this->m_FillToolboxWithDynamicNodes(c_Devices[u32_ItDevice]);
            }
            if (c_Devices.size() != 0)
            {
               this->mpc_Label->setVisible(false);
               this->mpc_Ui->pc_VerticalLayout1->removeItem(this->mpc_Spacer);
            }
         }
         else
         {
            this->mpc_Label->setVisible(true);
         }
      }
      // Add final spacer
      C_SebToolboxUtil::h_AddFinalSpacer(this->mpc_Ui->pc_VerticalLayout1, this->mpc_List);
   }

   connect(this->mpc_List, &C_SdTopologyListWidget::SigDelete, this, &C_SdTopologyToolbox::m_DeleteUserNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fill toolbox with static content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_FillToolboxStatic(void)
{
   QListWidget * pc_List;

   // fill draw lists with items
   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "Bus Types"), this->mpc_Ui->pc_VerticalLayout1_3, this->mc_ListWidgets,
                                            this);

   C_SebToolboxUtil::h_AddElementToList(pc_List, C_GtGetText::h_GetText(
                                           "CAN Bus"), "://images/system_definition/IconBus.svg", "",
                                        C_GtGetText::h_GetText("CAN Bus"));
   C_SebToolboxUtil::h_AddElementToList(pc_List, C_GtGetText::h_GetText(
                                           "Ethernet Bus"), "://images/system_definition/IconBus.svg", "",
                                        C_GtGetText::h_GetText("Ethernet Bus"));
   C_SebToolboxUtil::h_AddFinalSpacer(this->mpc_Ui->pc_VerticalLayout1_3, pc_List);

   C_SebToolboxUtil::h_InitFreeElements(this->mpc_Ui->pc_ListWidgetDrawing);

   // configure Headings
   this->mpc_Ui->pc_LabelFreeElements->setMaximumHeight(C_SebToolboxUtil::hsn_LabelSize);
   this->mpc_Ui->pc_LabelFreeElements->setMinimumHeight(C_SebToolboxUtil::hsn_LabelSize);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add a device to toolbox
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_FillToolboxWithDynamicNodes(const C_OSCDeviceDefinition & orc_Device)
{
   const QString c_DeviceName = orc_Device.c_DeviceName.c_str();
   // Tooltip
   const QString c_DeviceDescription = static_cast<QString>(orc_Device.c_DeviceDescription.c_str());

   if (this->mpc_List != NULL)
   {
      QListWidgetItem * pc_Item;
      this->mpc_List->addItem(orc_Device.GetDisplayName().c_str());
      pc_Item = this->mpc_List->item(this->mpc_List->count() - 1);
      pc_Item->setData(msn_USER_ROLE_ADDITIONAL_INFORMATION, c_DeviceName);
      // Toolbox icon
      pc_Item->setIcon(this->mc_Icon);
      // Tooltip
      pc_Item->setData(msn_USER_ROLE_TOOL_TIP_HEADING, orc_Device.GetDisplayName().c_str());
      pc_Item->setData(msn_USER_ROLE_TOOL_TIP_CONTENT, c_DeviceDescription);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of clear-all button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_IconClearAllClicked()
{
   // ask user to confirm
   C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eQUESTION);

   c_MessageBox.SetHeading(C_GtGetText::h_GetText("Delete User Nodes"));
   c_MessageBox.SetDescription(C_GtGetText::h_GetText(
                                  "Do you really want to delete all user nodes?"));
   c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Delete"));
   c_MessageBox.SetNOButtonText(C_GtGetText::h_GetText("Keep"));
   c_MessageBox.SetCustomMinHeight(180, 180);

   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
   {
      this->m_ClearAllUserNodes();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handles all GUI Feedback regarding errors on adding user nodes to toolbox

   \param[in]     orc_Errors    list of error messages
   \param[in]     orc_PathGood  paths to .syde_devdef-files added successfully
   \param[in]     orc_PathFail  paths to .syde_devdef-file failed to add
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_ErrorHandlingUserFeedback(const QStringList & orc_Errors, const QStringList & orc_PathGood,
                                                      const QStringList & orc_PathFail)
{
   C_OgeWiCustomMessage c_Message(this);
   QString c_Description;
   QString c_Details = "";
   const QString c_Success = QString::number(orc_PathGood.size()) + C_GtGetText::h_GetText(
      " node(s) successfully added to toolbox.<br/>");
   const QString c_Fail = QString::number(orc_PathFail.size()) + C_GtGetText::h_GetText(
      " node(s) could not be added to toolbox.<br/>");
   const QString c_LogLink = C_GtGetText::h_GetText("For details see ") +
                             C_Uti::h_GetLink(C_GtGetText::h_GetText("log file."), mc_STYLE_GUIDE_COLOR_LINK,
                                              C_OSCLoggingHandler::h_GetCompleteLogFileLocation().c_str());

   // no errors
   if ((orc_Errors.size() == 0) && (orc_PathFail.size() == 0))
   {
      c_Description = C_GtGetText::h_GetText("Node(s) successfully added to toolbox.");
      c_Message.SetType(C_OgeWiCustomMessage::eINFORMATION);
      c_Details += C_GtGetText::h_GetText("Summary: <br/>");
      c_Details += c_Success;
   }
   // errors occured, but nodes could be added to .ini and toolbox
   else if ((orc_Errors.size() > 0) && (orc_PathGood.size() > 0))
   {
      c_Message.SetType(C_OgeWiCustomMessage::eWARNING);
      c_Description = C_GtGetText::h_GetText(
         "Node(s) added to toolbox with errors. See details for further information.");

      c_Details += C_GtGetText::h_GetText("Summary: <br/>");
      c_Details += c_Success;
      c_Details += c_Fail + "<br/>";

      c_Details += C_GtGetText::h_GetText("Errors: <br/>");
      for (sintn sn_Error = 0; sn_Error < orc_Errors.size(); ++sn_Error)
      {
         c_Details += QString::number(sn_Error + 1) + ". " + orc_Errors[sn_Error] + "<br/>";
      }
   }
   // errors occured, no nodes were added
   else
   {
      c_Message.SetType(C_OgeWiCustomMessage::eERROR);
      c_Description = C_GtGetText::h_GetText("Errors occured! See details for further information.");

      c_Details += C_GtGetText::h_GetText("Summary: <br/>");
      c_Details += c_Fail + "<br/>";

      c_Details += C_GtGetText::h_GetText("Errors: <br/>");
      for (sintn sn_Error = 0; sn_Error < orc_Errors.size(); ++sn_Error)
      {
         c_Details += QString::number(sn_Error + 1) + ". " + orc_Errors[sn_Error] + "<br/>";
      }
   }
   c_Details += "<br/>" + c_LogLink;

   c_Message.SetHeading(C_GtGetText::h_GetText("Add User Node"));
   c_Message.SetDescription(c_Description);
   c_Message.SetDetails(c_Details);
   c_Message.SetCustomMinHeight(200, 400);
   c_Message.Execute();
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Opens on click on "+" Icon. Calls function to handle user device management.

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_FileBrowseDialog(void)
{
   QString c_Folder = C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownDeviceDefPath();

   if (c_Folder.compare("") == 0)
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   {
      QFileDialog c_Dialog(this, C_GtGetText::h_GetText("Select Device Definition or Ini File"),
                           c_Folder, static_cast<QString>(C_GtGetText::h_GetText(
                                                             "Device Definition")) + " (*.syde_devdef *.ini)");

      c_Dialog.setDefaultSuffix("*.syde_devdef");
      c_Dialog.setFileMode(QFileDialog::ExistingFiles);

      if (c_Dialog.exec() == static_cast<sintn>(QDialog::Accepted))
      {
         QStringList c_Errors;
         QStringList c_UserDeviceDefPaths;

         c_Folder = c_Dialog.directory().absolutePath();
         C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownDeviceDefPath(c_Folder);

         for (sintn sn_Counter = 0; sn_Counter < c_Dialog.selectedFiles().size(); ++sn_Counter)
         {
            this->m_LoadUserDeviceDefinitionPaths(c_Dialog.selectedFiles().at(
                                                     sn_Counter), c_UserDeviceDefPaths, c_Errors);
         }

         c_UserDeviceDefPaths =
            C_ImpUtil::h_AskUserToSaveRelativePath(this, c_UserDeviceDefPaths, C_Uti::h_GetExePath());
         this->m_AddUserNodesToIni(c_UserDeviceDefPaths, c_Errors);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  collects paths from all user-node files. They may come from a syde_devdef-file directly or from a .ini file

   \param[in]       orc_Path                path to file, chosen by user
   \param[in,out]   orc_UserDeviceDefPaths  paths to .syde_devdef files
   \param[in,out]   orc_Errors              error messages for GUI Feedback

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_LoadUserDeviceDefinitionPaths(const QString & orc_Path,
                                                          QStringList & orc_UserDeviceDefPaths,
                                                          QStringList & orc_Errors) const
{
   sint32 s32_Result;
   const QFileInfo c_FileInfo(orc_Path);
   C_OSCDeviceManager c_UserDeviceManager;

   // user selected an ini file?
   if (c_FileInfo.completeSuffix().toLower().contains("ini") == true)
   {
      // load the file
      s32_Result = c_UserDeviceManager.LoadFromFile(orc_Path.toStdString().c_str(), false);

      // handle errors from c_UserDeviceManager.LoadFromFile
      if (s32_Result == C_NO_ERR)
      {
         const C_SCLString c_CheckString = "User Nodes";
         // get DeviceGroups from device manager
         std::vector<C_OSCDeviceGroup> c_DeviceGroups = c_UserDeviceManager.GetDeviceGroups();
         std::vector<C_OSCDeviceDefinition> c_UserDevices;
         bool q_IsValidUserIni = false;

         for (uint32 u32_ItGroup = 0U; u32_ItGroup < c_DeviceGroups.size(); ++u32_ItGroup)
         {
            if (c_DeviceGroups[u32_ItGroup].GetGroupName() == c_CheckString)
            {
               c_UserDevices = c_DeviceGroups[u32_ItGroup].GetDevices();
               for (uint32 u32_ItDev = 0U; u32_ItDev < c_UserDevices.size(); ++u32_ItDev)
               {
                  const QString c_Path = static_cast<QString>(c_UserDevices[u32_ItDev].c_FilePath.c_str());
                  orc_UserDeviceDefPaths.append(c_Path);
               }
               q_IsValidUserIni = true;
               // leave the loop as soon as the GroupName "User Nodes" was found
               break;
            }
            else
            {
               q_IsValidUserIni = false;
            }
         }
         if (q_IsValidUserIni == false)
         {
            orc_Errors.append(static_cast<QString>(C_GtGetText::h_GetText("Could not load file: ")) + orc_Path);
            osc_write_log_error("Loading .ini-file",
                                "invalid .ini-file \"" + static_cast<C_SCLString>(
                                   orc_Path.toStdString().c_str()) + "\". File contains no user nodes.");
         }
      }
      // something went wrong in LoadFromFile: C_RD_WR
      else
      {
         // logging comes from either C_OSCDeviceManager::LoadFromFile or  C_OSCDeviceGroup::LoadGroup
         orc_Errors.append(static_cast<QString>(C_GtGetText::h_GetText("Could not load file: ")) + orc_Path);
      }
   }
   else if (c_FileInfo.completeSuffix().toLower().contains("syde_devdef") == true)
   {
      orc_UserDeviceDefPaths.append(orc_Path);
   }
   else
   {
      orc_UserDeviceDefPaths.append(orc_Path);
      orc_Errors.append(
         static_cast<QString>(C_GtGetText::h_GetText("File type '%1' not allowed.")).arg(c_FileInfo.completeSuffix()));
      osc_write_log_error("Loading file",
                          "Wrong file suffix \"" + static_cast<stw_scl::C_SCLString>(orc_Path.toStdString().c_str()) +
                          "\".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds user nodes from the given .syde_devdef-files to the user_devices.ini

   \param[in]       orc_UserDeviceDefPaths  paths to .syde_devdef files
   \param[in,out]   orc_Errors              error messages for GUI Feedback
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_AddUserNodesToIni(const QStringList & orc_UserDeviceDefPaths, QStringList & orc_Errors)
{
   sint32 s32_Result;
   QStringList c_PathGood;
   QStringList c_PathFail;

   if (orc_UserDeviceDefPaths.size() > 0)
   {
      for (sintn sn_ItPath = 0; sn_ItPath < orc_UserDeviceDefPaths.size(); ++sn_ItPath)
      {
         s32_Result = C_OSCSystemDefinition::hc_Devices.AddDevice(
            orc_UserDeviceDefPaths[sn_ItPath].toStdString().c_str(),
            "User Nodes",
            "../devices/user_devices.ini");

         // handling errors from AddDevice
         switch (s32_Result)
         {
         case C_NO_ERR:
            c_PathGood.append(orc_UserDeviceDefPaths[sn_ItPath]);
            break;
         case C_RD_WR:
            // could not load information -> C_OSCDeviceGroup::LoadGroup already logs this error
            orc_Errors.append(static_cast<QString>(C_GtGetText::h_GetText("Could not add device: ")) +
                              orc_UserDeviceDefPaths[sn_ItPath]);
            c_PathFail.append(orc_UserDeviceDefPaths[sn_ItPath]);
            break;
         case C_CONFIG:
            osc_write_log_error("Add device",
                                "device already exists \"" + static_cast<stw_scl::C_SCLString>(
                                   orc_UserDeviceDefPaths[sn_ItPath].toStdString().c_str()) + "\".");
            orc_Errors.append(static_cast<QString>(C_GtGetText::h_GetText("Could not add device: ")) +
                              orc_UserDeviceDefPaths[sn_ItPath]);
            c_PathFail.append(orc_UserDeviceDefPaths[sn_ItPath]);
            break;
         case C_NOACT:
            // invalid xml-file -> C_OSCDeviceDefinitionFiler::h_Load already logs this error.
            orc_Errors.append(static_cast<QString>(C_GtGetText::h_GetText("Could not add device: ")) +
                              orc_UserDeviceDefPaths[sn_ItPath]);
            c_PathFail.append(orc_UserDeviceDefPaths[sn_ItPath]);
            break;
         default:
            osc_write_log_error("Add device",
                                "unknown error \"" + static_cast<stw_scl::C_SCLString>(
                                   orc_UserDeviceDefPaths[sn_ItPath].toStdString().c_str()) + "\".");
            orc_Errors.append(static_cast<QString>(C_GtGetText::h_GetText("Could not add device: ")) +
                              orc_UserDeviceDefPaths[sn_ItPath] +
                              static_cast<QString>(C_GtGetText::h_GetText("\n (Unknown error)")));
            c_PathFail.append(orc_UserDeviceDefPaths[sn_ItPath]);
            break;
         }
      }
      this->m_AddUserNodesToToolbox();
      // TODO Check if call always is necessary??
      this->m_ErrorHandlingUserFeedback(orc_Errors, c_PathGood, c_PathFail);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds the user node to the toolbox

   Adds a new user node from user_devices.ini to the list widget in toolbox and calls function to actually
   make the new node visible.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_AddUserNodesToToolbox(void)
{
   // get names of all nodes, which already are in toolbox
   if (this->mpc_List != NULL)
   {
      std::vector<C_OSCDeviceDefinition> c_UserDevices;
      const std::vector<C_OSCDeviceGroup> c_DeviceGroups = C_OSCSystemDefinition::hc_Devices.GetDeviceGroups();

      QStringList c_ToolboxItems;
      const C_SCLString c_CheckString = "User Nodes";

      for (sintn sn_Counter = 0; sn_Counter < this->mpc_List->count(); ++sn_Counter)
      {
         c_ToolboxItems.append(this->mpc_List->item(sn_Counter)->text());
      }

      // get all user nodes
      for (uint32 u32_ItGroup = 0U; u32_ItGroup < c_DeviceGroups.size(); ++u32_ItGroup)
      {
         if (c_DeviceGroups[u32_ItGroup].GetGroupName() == c_CheckString)
         {
            c_UserDevices = c_DeviceGroups[u32_ItGroup].GetDevices();
         }
      }

      // set label invisible and remove spacer
      if (this->mpc_Label != NULL)
      {
         this->mpc_Label->setVisible(false);
         this->mpc_Ui->pc_VerticalLayout1->removeItem(this->mpc_Spacer);
      }

      for (uint32 u32_ItDev = 0U; u32_ItDev < c_UserDevices.size(); ++u32_ItDev)
      {
         // only add the nodes, which are not already in toolbox
         if (c_ToolboxItems.contains(static_cast<QString>(c_UserDevices[u32_ItDev].c_DeviceName.c_str())) == false)
         {
            this->m_FillToolboxWithDynamicNodes(c_UserDevices[u32_ItDev]);
         }
      }

      this->mpc_List->UpdateSize();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete user node

   \return
   C_NO_ERR    User node deleted
   C_WARN      Wrong suffix of device definition file
   C_BUSY      Device always exits
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdTopologyToolbox::m_DeleteUserNode(const QPoint & orc_Pos)
{
   sint32 s32_Return = C_NO_ERR;
   sint32 s32_Result = C_NO_ERR;

   if (this->mpc_List != NULL)
   {
      QListWidgetItem * const pc_Item = this->mpc_List->itemAt(orc_Pos);

      std::vector<C_OSCDeviceDefinition> c_Devices;
      std::vector<C_OSCDeviceDefinition>::iterator c_ItDevice;
      std::vector<C_OSCDeviceDefinition>::iterator c_ItEraseDevice;
      std::vector<C_OSCDeviceGroup> c_DeviceGroups = C_OSCSystemDefinition::hc_Devices.GetDeviceGroups();
      c_Devices = c_DeviceGroups[c_DeviceGroups.size() - 1].GetDevices();

      // Search for the device which should be deleted
      for (c_ItDevice = c_Devices.begin(); c_ItDevice != c_Devices.end(); ++c_ItDevice)
      {
         if ((*c_ItDevice).GetDisplayName().c_str() == pc_Item->text().toStdString())
         {
            c_ItEraseDevice = c_ItDevice;
         }
      }

      // get all nodes, which are currently used in Network Topology
      for (uint32 u32_ItNode = 0U; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItNode)
      {
         C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(u32_ItNode);

         if (pc_Node != NULL)
         {
            // check if the Node to be deleted is used in Network Topology. If so, no delete!
            if (pc_Node->c_DeviceType == (*c_ItEraseDevice).GetDisplayName().c_str())
            {
               C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
               c_Message.SetHeading(C_GtGetText::h_GetText("Delete User Nodes"));
               c_Message.SetDescription(C_GtGetText::h_GetText("Selected user node is still used in Network Topology."));
               c_Message.SetCustomMinHeight(180, 180);
               c_Message.Execute();
               s32_Result = C_BUSY;
               break;
            }
         }
      }

      if (s32_Result == C_BUSY)
      {
         s32_Return = C_BUSY;
      }
      else
      {
         // Delete the found device from the device definition vector
         c_Devices.erase(c_ItEraseDevice);

         // Delete device from device group "User Nodes" in user_devices.ini-File
         s32_Return = C_OSCSystemDefinition::hc_Devices.ChangeDevices(c_Devices,
                                                                      c_DeviceGroups[c_DeviceGroups.size() - 1].GetGroupName(),
                                                                      "../devices/user_devices.ini");

         // Remove the deleted device from the toolbox
         delete pc_Item;
         if (s32_Return == C_WARN)
         {
            if (this->mpc_Label != NULL)
            {
               this->mpc_Label->setVisible(true);
               // Add spacer after label
               this->mpc_Ui->pc_VerticalLayout1->addSpacerItem(this->mpc_Spacer);
            }
         }
         this->mpc_List->UpdateSize();
      }
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Delete all user nodes (from Toolbox and user_devices.ini)

   \return
   C_NO_ERR   all user nodes deleted
   C_WARN     Wrong suffix of device definition file
   C_DEFAULT  Nodes can not be deleted, because they are still used in Network Topology
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdTopologyToolbox::m_ClearAllUserNodes()
{
   sint32 s32_Return = C_NO_ERR;
   sint32 s32_Result = C_NO_ERR;

   if (this->mpc_List != NULL)
   {
      std::vector<C_OSCDeviceDefinition> c_Devices;
      std::vector<C_OSCDeviceGroup> c_DeviceGroups = C_OSCSystemDefinition::hc_Devices.GetDeviceGroups();
      c_Devices = c_DeviceGroups[c_DeviceGroups.size() - 1].GetDevices();

      // get all nodes, which are currently used in Network Topology
      for (uint32 u32_ItNode = 0U; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItNode)
      {
         C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(u32_ItNode);

         if (pc_Node != NULL)
         {
            for (uint32 u32_ItDevice = 0U; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
            {
               // check if any of the User Nodes is currently used in Network Topology. If so the Nodes can't be
               // deleted.
               if (pc_Node->c_DeviceType == c_Devices[u32_ItDevice].c_DeviceName)
               {
                  C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
                  c_Message.SetHeading(C_GtGetText::h_GetText("Delete User Nodes"));
                  c_Message.SetDescription(C_GtGetText::h_GetText(
                                              "At least one user node is used in Network Topology."));
                  c_Message.SetCustomMinHeight(180, 180);
                  c_Message.Execute();
                  s32_Result = C_DEFAULT;
                  break;
               }
            }
         }
      }
      if (s32_Result == C_DEFAULT)
      {
         s32_Return = C_DEFAULT;
      }
      else
      {
         //delete all devices from device definition vector
         c_Devices.clear();
         //delete devices from device group "User Nodes" in user_devices.ini
         s32_Return = C_OSCSystemDefinition::hc_Devices.ChangeDevices(c_Devices,
                                                                      c_DeviceGroups[c_DeviceGroups.size() - 1].GetGroupName(),
                                                                      "../devices/user_devices.ini");

         //remove all deleted devices from toolbox
         this->mpc_List->clear();

         if (s32_Return == C_WARN)
         {
            if (this->mpc_Label != NULL)
            {
               this->mpc_Label->setVisible(true);
               //Add spacer after label
               this->mpc_Ui->pc_VerticalLayout1->addSpacerItem(this->mpc_Spacer);
            }
         }
         this->mpc_List->UpdateSize();
      }
   }
   return s32_Return;
}
