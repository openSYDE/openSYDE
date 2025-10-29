//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for toolbox of system definition

   Offers devices in different lists (C_SdTopologyListWidget) as icons.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include <QLabel>
#include <QFileDialog>
#include <QMimeData>
#include <QBoxLayout>
#include <QDebug>

#include "TglFile.hpp"
#include "constants.hpp"
#include "C_SclIniFile.hpp"
#include "C_SdTopologyToolbox.hpp"
#include "C_GtGetText.hpp"
#include "ui_C_SdTopologyToolbox.h"
#include "C_SebToolboxUtil.hpp"
#include "C_OscSystemDefinition.hpp"
#include "C_UsHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_PuiProject.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgePubIconOnly.hpp"
#include "C_ImpUtil.hpp"
#include "C_Uti.hpp"
#include "C_OscLoggingHandler.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;
using namespace stw::tgl;
using namespace stw::scl;
using namespace stw::errors;

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
   QList<QListWidgetItem *>::iterator c_ItItem;
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

   for (int32_t s32_ItListWidget = 0; s32_ItListWidget < mc_ListWidgets.size(); ++s32_ItListWidget)
   {
      c_ListItems = mc_ListWidgets[s32_ItListWidget]->findItems(orc_Text, Qt::MatchContains);
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
void C_SdTopologyToolbox::enterEvent(QEnterEvent * const opc_Event)
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
      for (int32_t s32_Counter = 0; s32_Counter < opc_Event->mimeData()->urls().size(); s32_Counter++)
      {
         const QFileInfo c_File(opc_Event->mimeData()->urls().at(s32_Counter).toString());

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
   int32_t s32_DeviceCount = 0;
   int32_t s32_AddDeviceCount;

   for (int32_t s32_Counter = 0; s32_Counter < opc_Event->mimeData()->urls().size(); s32_Counter++)
   {
      QString c_Path = opc_Event->mimeData()->urls().at(s32_Counter).path();
      this->m_LoadUserDeviceDefinitionPaths(c_Path.remove(0, 1),
                                            c_UserDeviceDefPaths, c_Errors, s32_DeviceCount);
   }

   c_UserDeviceDefPaths = C_ImpUtil::h_AskUserToSaveRelativePath(this, c_UserDeviceDefPaths,
                                                                 C_Uti::h_GetExePath());
   s32_AddDeviceCount = this->m_AddUserNodesToIni(c_UserDeviceDefPaths, c_Errors);
   this->m_ErrorHandlingUserFeedback(c_Errors, s32_AddDeviceCount, s32_DeviceCount);

   QWidget::dropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Fill toolbox with dynamic content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_FillToolboxDynamic(void)
{
   std::vector<C_OgePubIconOnly *> c_Icons;
   std::vector<C_OscDeviceGroup> c_DeviceGroups = C_OscSystemDefinition::hc_Devices.GetDeviceGroups();
   for (uint32_t u32_ItDeviceGroup = 0U; u32_ItDeviceGroup < c_DeviceGroups.size(); ++u32_ItDeviceGroup)
   {
      const std::vector<C_OscDeviceDefinition> c_Devices = c_DeviceGroups[u32_ItDeviceGroup].GetDevices();

      if (c_DeviceGroups[u32_ItDeviceGroup].GetGroupName() != "User Nodes")
      {
         this->mpc_List = C_SebToolboxUtil::h_AddNewList(
            c_DeviceGroups[u32_ItDeviceGroup].GetGroupName().c_str(),
            this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets, this);

         if (this->mpc_List != NULL)
         {
            for (uint32_t u32_ItDevice = 0U; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
            {
               if (c_Devices[u32_ItDevice].c_ManufacturerDisplayValue != "Sensor-Technik Wiedemann GmbH")
               {
                  this->mc_Icon.addPixmap(
                     static_cast<QPixmap>(c_Devices[u32_ItDevice].c_ToolboxIcon.c_str()), QIcon::Normal);
                  this->mc_Icon.addPixmap(
                     static_cast<QPixmap>(c_Devices[u32_ItDevice].c_ToolboxIcon.c_str()), QIcon::Selected);
               }
               else
               {
                  this->mc_Icon.addPixmap(static_cast<QPixmap>("://images/system_definition/PreviewNode.svg"),
                                          QIcon::Normal);
                  this->mc_Icon.addPixmap(static_cast<QPixmap>("://images/system_definition/PreviewNode.svg"),
                                          QIcon::Selected);
               }
               this->m_FillToolboxWithDynamicNodes(c_Devices[u32_ItDevice]);
            }
         }
      }
      if (u32_ItDeviceGroup == static_cast<uint32_t>(c_DeviceGroups.size() - 1))
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
            for (uint32_t u32_ItDevice = 0U; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
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
   this->mpc_Ui->pc_LabelFreeElements->setMaximumHeight(C_SebToolboxUtil::hs32_LABEL_SIZE);
   this->mpc_Ui->pc_LabelFreeElements->setMinimumHeight(C_SebToolboxUtil::hs32_LABEL_SIZE);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add a device to toolbox
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_FillToolboxWithDynamicNodes(const C_OscDeviceDefinition & orc_Device)
{
   const QString c_DeviceName = orc_Device.c_DeviceName.c_str();
   // Tooltip
   const QString c_DeviceDescription = static_cast<QString>(orc_Device.c_DeviceDescription.c_str());

   if (this->mpc_List != NULL)
   {
      QListWidgetItem * pc_Item;
      this->mpc_List->addItem(orc_Device.GetDisplayName().c_str());
      pc_Item = this->mpc_List->item(this->mpc_List->count() - 1);
      pc_Item->setData(ms32_USER_ROLE_ADDITIONAL_INFORMATION, c_DeviceName);
      // Toolbox icon
      pc_Item->setIcon(this->mc_Icon);
      // Tooltip
      pc_Item->setData(ms32_USER_ROLE_TOOL_TIP_HEADING, orc_Device.GetDisplayName().c_str());
      pc_Item->setData(ms32_USER_ROLE_TOOL_TIP_CONTENT, c_DeviceDescription);
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
   c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("Delete"));
   c_MessageBox.SetNoButtonText(C_GtGetText::h_GetText("Keep"));
   c_MessageBox.SetCustomMinHeight(180, 180);

   if (c_MessageBox.Execute() == C_OgeWiCustomMessage::eYES)
   {
      this->m_ClearAllUserNodes();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handles all GUI Feedback regarding errors on adding user nodes to toolbox

   \param[in]     orc_Errors               List of error messages
   \param[in]     ors32_AddDeviceCount     Number of devices that could be added successfully
   \param[in]     ors32_DeviceCount        Number of devices that were originally to be added
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_ErrorHandlingUserFeedback(const QStringList & orc_Errors,
                                                      const int32_t & ors32_AddDeviceCount,
                                                      const int32_t & ors32_DeviceCount)
{
   C_OgeWiCustomMessage c_Message(this);
   QString c_Description;
   QString c_Details;
   const QString c_LogLink = C_GtGetText::h_GetText("For details and possible errors see ") +
                             C_Uti::h_GetLink(C_GtGetText::h_GetText("log file."), mc_STYLE_GUIDE_COLOR_LINK,
                                              C_OscLoggingHandler::h_GetCompleteLogFileLocation().c_str());

   // nodes could be added (possibly with errors) -> info box with count of added nodes
   if (ors32_AddDeviceCount > 0)
   {
      c_Description = QString::number(ors32_AddDeviceCount) + C_GtGetText::h_GetText(" of ") + QString::number(
         ors32_DeviceCount) + C_GtGetText::h_GetText(" node(s) successfully added to toolbox.<br/>");
      c_Message.SetType(C_OgeWiCustomMessage::eINFORMATION);
      c_Message.SetCustomMinHeight(180, 210);
   }
   else
   {
      // something went wrong. Wrong ini, broken xml, node already exists
      c_Description = C_GtGetText::h_GetText("Errors occured! See details for further information.");
      for (int32_t s32_Error = 0; s32_Error < orc_Errors.size(); ++s32_Error)
      {
         c_Details += "* " + orc_Errors[s32_Error] + "<br/>";
         c_Message.SetType(C_OgeWiCustomMessage::eERROR);
         c_Message.SetCustomMinHeight(180, 410);
      }
   }

   c_Details += c_LogLink;

   c_Message.SetHeading(C_GtGetText::h_GetText("Add User Node"));
   c_Message.SetDescription(c_Description);
   c_Message.SetDetails(c_Details);
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

      if (c_Dialog.exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         int32_t s32_DeviceCount = 0;
         int32_t s32_AddDeviceCount;
         QStringList c_Errors;
         QStringList c_UserDeviceDefPaths;

         c_Folder = c_Dialog.directory().absolutePath();
         C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownDeviceDefPath(c_Folder);

         for (int32_t s32_Counter = 0; s32_Counter < c_Dialog.selectedFiles().size(); ++s32_Counter)
         {
            this->m_LoadUserDeviceDefinitionPaths(c_Dialog.selectedFiles().at(
                                                     s32_Counter), c_UserDeviceDefPaths, c_Errors, s32_DeviceCount);
         }

         c_UserDeviceDefPaths =
            C_ImpUtil::h_AskUserToSaveRelativePath(this, c_UserDeviceDefPaths, C_Uti::h_GetExePath());
         s32_AddDeviceCount = this->m_AddUserNodesToIni(c_UserDeviceDefPaths, c_Errors);
         this->m_ErrorHandlingUserFeedback(c_Errors, s32_AddDeviceCount, s32_DeviceCount);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  collects paths from all user-node files. They may come from a syde_devdef-file directly or from a .ini file

   \param[in]       orc_Path                path to file, chosen by user
   \param[in,out]   orc_UserDeviceDefPaths  paths to .syde_devdef files
   \param[in,out]   orc_Errors              error messages for GUI Feedback
   \param[in,out]   ors32_DeviceCount       keeps track of how many devices are supposed to be added

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdTopologyToolbox::m_LoadUserDeviceDefinitionPaths(const QString & orc_Path,
                                                          QStringList & orc_UserDeviceDefPaths,
                                                          QStringList & orc_Errors, int32_t & ors32_DeviceCount) const
{
   const QFileInfo c_FileInfo(orc_Path);
   C_OscDeviceManager c_UserDeviceManager;
   int32_t * const ps32_PtrDeviceCount = &ors32_DeviceCount;

   // user selected an ini file?
   if (c_FileInfo.completeSuffix().toLower().contains("ini") == true)
   {
      // load the file
      const C_SclString c_CheckString = "User Nodes";
      std::vector<C_OscDeviceGroup> c_DeviceGroups;
      std::vector<C_OscDeviceDefinition> c_UserDevices;
      bool q_IsValidUserIni = false;

      c_UserDeviceManager.LoadFromFile(orc_Path.toStdString().c_str(), false, ps32_PtrDeviceCount);
      c_DeviceGroups = c_UserDeviceManager.GetDeviceGroups();

      // get DeviceGroups from device manager
      for (uint32_t u32_ItGroup = 0U; u32_ItGroup < c_DeviceGroups.size(); ++u32_ItGroup)
      {
         if (c_DeviceGroups[u32_ItGroup].GetGroupName() == c_CheckString)
         {
            c_UserDevices = c_DeviceGroups[u32_ItGroup].GetDevices();
            for (uint32_t u32_ItDev = 0U; u32_ItDev < c_UserDevices.size(); ++u32_ItDev)
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
         // info for user how a valid user_devices.ini should look like
         QString c_ErrorDetails = C_GtGetText::h_GetText(
            ". File must have this structure:<br/>");
         c_ErrorDetails +=
            "<br/>[DeviceTypes]<br/>NumTypes=1<br/>TypeName1=User Nodes<br/><br/>[User Nodes]<br/>DeviceCount="
            "&lt;number_of_user_devices&gt;<br/>Device1=&lt;path_to_user_syde_devdef&gt;<br/>...<br/>";
         orc_Errors.append(static_cast<QString>(C_GtGetText::h_GetText("Could not load file: ")) +
                           orc_Path + c_ErrorDetails);

         osc_write_log_error("Loading from ini file",
                             "File \"" + static_cast<C_SclString>(orc_Path.toStdString().c_str()) +
                             "\" contains no User Nodes.");
      }
   }
   else if (c_FileInfo.completeSuffix().toLower().contains("syde_devdef") == true)
   {
      ors32_DeviceCount++;
      orc_UserDeviceDefPaths.append(orc_Path);
   }
   else
   {
      orc_Errors.append(
         static_cast<QString>(C_GtGetText::h_GetText("File type '%1' not allowed.")).arg(c_FileInfo.completeSuffix()));
      osc_write_log_error("Loading file",
                          "Wrong file suffix \"" + static_cast<stw::scl::C_SclString>(orc_Path.toStdString().c_str()) +
                          "\".");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adds user nodes from the given .syde_devdef-files to the user_devices.ini

   \param[in]       orc_UserDeviceDefPaths  paths to .syde_devdef files
   \param[in,out]   orc_Errors              error messages for GUI Feedback

   \return
   Number of devices that could be added successfully
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdTopologyToolbox::m_AddUserNodesToIni(const QStringList & orc_UserDeviceDefPaths, QStringList & orc_Errors)
{
   int32_t s32_AddDeviceCount = 0;

   if (orc_UserDeviceDefPaths.size() > 0)
   {
      for (int32_t s32_ItPath = 0; s32_ItPath < orc_UserDeviceDefPaths.size(); ++s32_ItPath)
      {
         const int32_t s32_Result = C_OscSystemDefinition::hc_Devices.AddDevice(
            orc_UserDeviceDefPaths[s32_ItPath].toStdString().c_str(),
            "User Nodes",
            "../devices/user_devices.ini");

         if (s32_Result != C_NO_ERR)
         {
            QString c_Error = C_GtGetText::h_GetText("Could not add device: \"") + orc_UserDeviceDefPaths[s32_ItPath] +
                              ("\".<br/>");
            if (s32_Result == C_OVERFLOW)
            {
               c_Error += C_GtGetText::h_GetText("Device does already exist.<br/>");
            }
            orc_Errors.append(c_Error);
         }
         else
         {
            ++s32_AddDeviceCount;
         }
      }
   }
   if (s32_AddDeviceCount > 0)
   {
      this->m_AddUserNodesToToolbox();
   }
   return s32_AddDeviceCount;
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
      std::vector<C_OscDeviceDefinition> c_UserDevices;
      const std::vector<C_OscDeviceGroup> c_DeviceGroups = C_OscSystemDefinition::hc_Devices.GetDeviceGroups();

      QStringList c_ToolboxItems;
      const C_SclString c_CheckString = "User Nodes";

      for (int32_t s32_Counter = 0; s32_Counter < this->mpc_List->count(); ++s32_Counter)
      {
         c_ToolboxItems.append(this->mpc_List->item(s32_Counter)->text());
      }

      // get all user nodes
      for (uint32_t u32_ItGroup = 0U; u32_ItGroup < c_DeviceGroups.size(); ++u32_ItGroup)
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

      for (uint32_t u32_ItDev = 0U; u32_ItDev < c_UserDevices.size(); ++u32_ItDev)
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
int32_t C_SdTopologyToolbox::m_DeleteUserNode(const QPoint & orc_Pos)
{
   int32_t s32_Return = C_NO_ERR;
   int32_t s32_Result = C_NO_ERR;

   if (this->mpc_List != NULL)
   {
      QListWidgetItem * const pc_Item = this->mpc_List->itemAt(orc_Pos);

      std::vector<C_OscDeviceDefinition> c_Devices;
      std::vector<C_OscDeviceDefinition>::iterator c_ItDevice;
      std::vector<C_OscDeviceDefinition>::iterator c_ItEraseDevice;
      std::vector<C_OscDeviceGroup> c_DeviceGroups = C_OscSystemDefinition::hc_Devices.GetDeviceGroups();
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
      for (uint32_t u32_ItNode = 0U; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize(); ++u32_ItNode)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNode(u32_ItNode);

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
         s32_Return = C_OscSystemDefinition::hc_Devices.ChangeDevices(c_Devices,
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
int32_t C_SdTopologyToolbox::m_ClearAllUserNodes()
{
   int32_t s32_Return = C_NO_ERR;
   int32_t s32_Result = C_NO_ERR;

   if (this->mpc_List != NULL)
   {
      std::vector<C_OscDeviceDefinition> c_Devices;
      std::vector<C_OscDeviceGroup> c_DeviceGroups = C_OscSystemDefinition::hc_Devices.GetDeviceGroups();
      c_Devices = c_DeviceGroups[c_DeviceGroups.size() - 1].GetDevices();

      // get all nodes, which are currently used in Network Topology
      for (uint32_t u32_ItNode = 0U; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize(); ++u32_ItNode)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNode(u32_ItNode);

         if (pc_Node != NULL)
         {
            for (uint32_t u32_ItDevice = 0U; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
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
         s32_Return = C_OscSystemDefinition::hc_Devices.ChangeDevices(c_Devices,
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
