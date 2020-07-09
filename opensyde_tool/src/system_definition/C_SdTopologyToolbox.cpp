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
   mpc_Item(NULL),
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
C_SdTopologyToolbox::~C_SdTopologyToolbox()
{
   //lint -e{1540}  no memory leak because of the parent all elements and the Qt memory management
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
   QStringList c_PathList;

   for (sintn sn_Counter = 0; sn_Counter < opc_Event->mimeData()->urls().size(); sn_Counter++)
   {
      QString c_Path = opc_Event->mimeData()->urls().at(sn_Counter).path();
      QFileInfo c_FileInfo(c_Path);
      if (c_FileInfo.completeSuffix().toLower() == "syde_devdef")
      {
         c_Path = c_Path.remove(0, 1);
         c_PathList.append(c_Path);
      }
      else
      {
         this->m_AddUserNodeFromIni(c_Path.remove(0, 1));
      }
   }

   c_PathList = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_PathList, C_Uti::h_GetExePath());
   for (sintn sn_ItPath = 0; sn_ItPath < c_PathList.size(); ++sn_ItPath)
   {
      this->m_AddUserNode(c_PathList[sn_ItPath]);
   }

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

      if (c_DeviceGroups[u32_ItDeviceGroup].GetGroupName().c_str() != stw_scl::C_SCLString("User Nodes"))
      {
         this->mc_Icon.addPixmap(QPixmap("://images/system_definition/PreviewNode.svg"), QIcon::Normal);
         this->mc_Icon.addPixmap(QPixmap("://images/system_definition/PreviewNode.svg"), QIcon::Selected);

         this->mpc_List = C_SebToolboxUtil::h_AddNewList(
            c_DeviceGroups[u32_ItDeviceGroup].GetGroupName().c_str(),
            this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets, this);

         if (this->mpc_List != NULL)
         {
            for (uint32 u32_ItDevice = 0; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
            {
               this->m_FillToolboxWithDynamicNodes(c_Devices[u32_ItDevice]);
            }
         }
      }
      if (u32_ItDeviceGroup == (c_DeviceGroups.size() - 1UL))
      {
         // Add final spacer
         C_SebToolboxUtil::h_AddFinalSpacer(this->mpc_Ui->pc_VerticalLayout1, this->mpc_List);

         // Add user nodes heading
         c_Icons = C_SebToolboxUtil::h_AddNewUserHeading("User Nodes",
                                                         this->mpc_Ui->pc_VerticalLayout1,
                                                         this);
         C_OgePubIconOnly * pc_IconButton = c_Icons[0];
         pc_IconButton->SetToolTipInformation(C_GtGetText::h_GetText("Add User Nodes"),
                                              C_GtGetText::h_GetText(
                                                 "Add user nodes to toolbox from file device desription file (.syde_defdev)"
                                                 " or user nodes ini file (.ini)."));
         connect(pc_IconButton, &C_OgePubIconOnly::clicked, this, &C_SdTopologyToolbox::m_IconAddNodeClicked);

         C_OgePubIconOnly * pc_ClearAllUserNodesButton = c_Icons[1];
         pc_ClearAllUserNodesButton->SetToolTipInformation(C_GtGetText::h_GetText("Clear User Nodes"),
                                                           C_GtGetText::h_GetText(
                                                              "Delete all user nodes from toolbox."));

         connect(pc_ClearAllUserNodesButton, &C_OgePubIconOnly::clicked, this,
                 &C_SdTopologyToolbox::m_IconClearAllClicked);

         this->mc_Icon.addPixmap(QPixmap("://images/Icon_toolbox_user_node.svg"), QIcon::Normal);
         this->mc_Icon.addPixmap(QPixmap("://images/Icon_toolbox_user_node.svg"), QIcon::Selected);

         this->mpc_List = C_SebToolboxUtil::h_AddNewList("", this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                                         this);

         // True: Fill the toolbox with user devices
         // False: Set label no user devices existing
         if (c_DeviceGroups[u32_ItDeviceGroup].GetGroupName().c_str() == stw_scl::C_SCLString("User Nodes"))
         {
            for (uint32 u32_ItDevice = 0; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
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
void C_SdTopologyToolbox::m_FillToolboxWithDynamicNodes(const C_OSCDeviceDefinition & rc_Device)
{
   const QString c_DeviceName = rc_Device.c_DeviceName.c_str();
   // Tooltip
   const QString c_DeviceDescription = QString(rc_Device.c_DeviceDescription.c_str());

   if (this->mpc_List != NULL)
   {
      this->mpc_List->addItem(rc_Device.GetDisplayName().c_str());
      this->mpc_Item = this->mpc_List->item(this->mpc_List->count() - 1);
      this->mpc_Item->setData(msn_USER_ROLE_ADDITIONAL_INFORMATION, c_DeviceName);
      // Toolbox icon
      this->mpc_Item->setIcon(this->mc_Icon);
      // Tooltip
      this->mpc_Item->setData(msn_USER_ROLE_TOOL_TIP_HEADING, rc_Device.GetDisplayName().c_str());
      this->mpc_Item->setData(msn_USER_ROLE_TOOL_TIP_CONTENT, c_DeviceDescription);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of plus button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_IconAddNodeClicked(void)
{
   this->m_LoadDeviceDefinitionFile();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add a new user device to toolbox
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_AddUserNodeToList(const sint32 os32_Return)
{
   if (os32_Return == C_NO_ERR)
   {
      std::vector<C_OSCDeviceDefinition> c_Devices;
      std::vector<C_OSCDeviceGroup> c_DeviceGroups = C_OSCSystemDefinition::hc_Devices.GetDeviceGroups();
      c_Devices = c_DeviceGroups[c_DeviceGroups.size() - 1].GetDevices();

      // Set label invisible and remove spacer
      if (this->mpc_Label != NULL)
      {
         this->mpc_Label->setVisible(false);
         this->mpc_Ui->pc_VerticalLayout1->removeItem(this->mpc_Spacer);
      }
      this->m_FillToolboxWithDynamicNodes(c_Devices[c_Devices.size() - 1]);
      this->mpc_List->UpdateSize();

      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eINFORMATION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add User Node"));
      c_Message.SetDescription(C_GtGetText::h_GetText("User node successfully added to toolbox."));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }
   else if (os32_Return == C_CONFIG)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add user node"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Node of this type already exists in the toolbox."));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }
   else if (os32_Return == C_NOACT)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add user node"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Wrong file type."));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }
   else
   {
      // nothing to do
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
/*! \brief   Load device definition file for user nodes

   Supported are .syde_devdef and .ini (only with "User Nodes" TAG) files.

   \return
   C_NO_ERR    User node added
   C_WARN      Wrong suffix of device definition file
   C_CONFIG    Device always exits
   C_NOACT     No action
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdTopologyToolbox::m_LoadDeviceDefinitionFile(void)
{
   sint32 s32_Return = C_NOACT;
   QStringList c_Paths;

   // Open file selector with .syde_devdef filter
   QString c_Folder = C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownImportPath();

   // Replace default path if necessary
   if (c_Folder.compare("") == 0)
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   // Load user settings value and folder for file dialog
   QFileDialog c_Dialog(this, C_GtGetText::h_GetText("Select Device Definition File"),
                        c_Folder, QString(C_GtGetText::h_GetText("Device Definition")) + " (*.syde_devdef *.ini)");
   c_Dialog.setDefaultSuffix("*.syde_devdef");
   c_Dialog.setFileMode(QFileDialog::ExistingFiles);

   if (c_Dialog.exec() == static_cast<sintn>(QDialog::Accepted))
   {
      // if an .ini file was selected, we need to call another function
      for (sintn sn_Counter = 0; sn_Counter < c_Dialog.selectedFiles().size(); sn_Counter++)
      {
         if (c_Dialog.selectedFiles().at(sn_Counter).contains(".ini") == true)
         {
            this->m_AddUserNodeFromIni(c_Dialog.selectedFiles().at(sn_Counter));
         }
         else
         {
            c_Paths.append(c_Dialog.selectedFiles().at(sn_Counter));
            //s32_Return = this->m_AddUserNode(c_Path);
         }
      }
      c_Paths = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_Paths, C_Uti::h_GetExePath());
      for (sintn sn_Counter = 0; sn_Counter < c_Paths.size(); ++sn_Counter)
      {
         this->m_AddUserNode(c_Paths[sn_Counter]);
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add user node to .ini file

   \return
   C_NO_ERR    User node added
   C_WARN      Wrong suffix of device definition file
   C_CONFIG    Device always exits
   C_NOACT     No action
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdTopologyToolbox::m_AddUserNode(const QString & orc_DeviceDefinitionPath)
{
   sint32 s32_Return = C_NOACT;

   // Check for user abort (empty string)
   if (orc_DeviceDefinitionPath != "")
   {
      const QFileInfo c_FileInfo(orc_DeviceDefinitionPath);
      // Store new user settings value
      C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownImportPath(c_FileInfo.absoluteDir().absolutePath());
      // Only use lower case suffix
      if (c_FileInfo.completeSuffix().toLower().contains("syde_devdef") == false)
      {
         // Strange
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Add user node"));
         c_Message.SetDescription(C_GtGetText::h_GetText("File type not allowed."));
         c_Message.SetCustomMinHeight(180, 180);
         c_Message.Execute();
         s32_Return = C_WARN;
      }
      else
      {
         s32_Return = C_NO_ERR;
      }

      if (s32_Return == C_NO_ERR)
      {
         // Add device to device group "User Nodes" in user_devices.ini-File
         s32_Return = C_OSCSystemDefinition::hc_Devices.AddDevice(stw_scl::C_SCLString(orc_DeviceDefinitionPath
                                                                                       .toStdString().c_str()),
                                                                  stw_scl::C_SCLString("User Nodes"),
                                                                  stw_scl::C_SCLString("../devices/user_devices.ini"));
      }
   }
   this->m_AddUserNodeToList(s32_Return);

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds UserNodes to Toolbox on a drag and drop of the user_devices.ini

   \param[in,out]   QString   Path to user_devices.ini based on mouse drag

   \return
   C_NO_ERR
   C_RD_WR     wrong file suffix, or wrong .ini file.
   C_CONFIG    could not add user node
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdTopologyToolbox::m_AddUserNodeFromIni(const QString & orc_UserDevicesIniPath)
{
   sint32 s32_Return;
   sint16 s16_DevCount = 0;

   C_OSCDeviceManager c_UserDeviceManager;

   std::vector<C_OSCDeviceGroup> c_DeviceGroups;
   std::vector<C_OSCDeviceDefinition> c_UserDevices;

   // load ini file
   s32_Return = c_UserDeviceManager.LoadFromFile(orc_UserDevicesIniPath.toStdString().c_str(), false);

   if (s32_Return == C_NO_ERR)
   {
      C_SCLString c_CheckString = "User Nodes";
      // get DeviceGroups from .ini
      c_DeviceGroups = c_UserDeviceManager.GetDeviceGroups();

      for (uint32 u32_ItGroup = 0; u32_ItGroup < c_DeviceGroups.size(); ++u32_ItGroup)
      {
         // is this the user_devices.ini?
         if (c_DeviceGroups[u32_ItGroup].GetGroupName() == c_CheckString)
         {
            // get paths to devdef-files
            c_UserDevices = c_DeviceGroups[u32_ItGroup].GetDevices();
            s32_Return = C_NO_ERR;
            break;
         }
         else
         {
            s32_Return = C_RD_WR;
         }
      }
   }

   // add devices to Toolbox
   if (s32_Return == C_NO_ERR)
   {
      if (this->mpc_Label != NULL)
      {
         this->mpc_Label->setVisible(false);
         this->mpc_Ui->pc_VerticalLayout1->removeItem(this->mpc_Spacer);
      }

      for (uint32 u32_ItDef = 0; u32_ItDef < c_UserDevices.size(); ++u32_ItDef)
      {
         // add device in user_devices.ini
         s32_Return = C_OSCSystemDefinition::hc_Devices.AddDevice(c_UserDevices[u32_ItDef].c_FilePath,
                                                                  stw_scl::C_SCLString("User Nodes"),
                                                                  stw_scl::C_SCLString("../devices/user_devices.ini"));

         if (s32_Return == C_NO_ERR)
         {
            this->m_FillToolboxWithDynamicNodes(c_UserDevices[u32_ItDef]);
            this->mpc_List->UpdateSize();
            s16_DevCount++;
         }
         else
         {
            break;
         }
      }
   }

   if (s32_Return == C_NO_ERR)
   {
      C_SCLString c_Success = "";
      // sucess message for adding nodes
      c_Success = C_SCLString::IntToStr(s16_DevCount) + C_GtGetText::h_GetText(" user node(s) successfully added.");

      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eINFORMATION);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add User Node"));
      c_Message.SetDescription(C_GtGetText::h_GetText(c_Success.c_str()));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }

   // something went wrong while adding the nodes
   else if (s32_Return == C_CONFIG)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add user node"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Could not add user node.\n"
                                                      "Node of this type already exists in toolbox."));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }

   // something went wrong on loading the .ini file or it is another .ini as expected
   else if (s32_Return == C_RD_WR)
   {
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("Add User Node"));
      c_Message.SetDescription(C_GtGetText::h_GetText("No user nodes found."));
      c_Message.SetCustomMinHeight(180, 180);
      c_Message.Execute();
   }

   else
   {
      // nothing to do here
   }

   return s32_Return;
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
      QListWidgetItem * pc_Item = this->mpc_List->itemAt(orc_Pos);

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
      for (uint32 u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItNode)
      {
         C_OSCNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(u32_ItNode);

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
                                                                      stw_scl::C_SCLString(
                                                                         "../devices/user_devices.ini"));

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
      for (uint32 u32_ItNode = 0; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOSCNodesSize(); ++u32_ItNode)
      {
         C_OSCNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(u32_ItNode);

         if (pc_Node != NULL)
         {
            for (uint32 u32_ItDevice = 0; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
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
                                                                      stw_scl::C_SCLString(
                                                                         "../devices/user_devices.ini"));

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
