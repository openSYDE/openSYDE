//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog widget for adding a device in can open manager.

   Pop up dialog widget for adding a device in can open manager.

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QIcon>
#include <QImageReader>
#include <QFileDialog>

#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_GtGetText.h"
#include "C_SdNdeCoAddDeviceDialog.h"
#include "ui_C_SdNdeCoAddDeviceDialog.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNode.h"
#include "C_SdUtil.h"
#include "C_PuiUtil.h"
#include "C_PuiProject.h"
#include "C_OgeWiUtil.h"
#include "C_UsHandler.h"
#include "C_OSCCanOpenObjectDictionary.h"
#include "C_OSCCanOpenManagerInfo.h"
#include "C_OSCImportEdsDcf.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_scl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint16 mu16_NODE_IMG_WIDTH = 300;
const QString C_SdNdeCoAddDeviceDialog::mhc_SUFFIX = "eds";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent  Reference to parent
   \param[in] ou32_NodeIndex  Index of current node
   \param[in] ou8_InterfaceId Interface id of current node
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoAddDeviceDialog::C_SdNdeCoAddDeviceDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                   const uint32 ou32_NodeIndex, const uint8 ou8_InterfaceId) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeCoAddDeviceDialog),
   mrc_ParentDialog(orc_Parent),
   mu32_BusIndex(0),
   mu32_NodeIndex(ou32_NodeIndex),
   mu8_InterfaceId(ou8_InterfaceId)
{
   this->mpc_Ui->setupUi(this);

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_LineEditEDSPath->SetDragAndDropActiveForFile(mhc_SUFFIX);

   InitStaticNames();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->mpc_Ui->pc_TedHtmlReport->setReadOnly(true);

   const QIcon c_Icon("://images/system_definition/Image_Grey.svg");
   const QPixmap c_Pic = c_Icon.pixmap(QSize(mu16_NODE_IMG_WIDTH / 2, mu16_NODE_IMG_WIDTH / 2));
   this->mpc_Ui->pc_LabelPic->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
   this->mpc_Ui->pc_LabelPic->setPixmap(c_Pic);

   this->mpc_Ui->pc_LabelFileName->setVisible(false);

   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNode(this->mu32_NodeIndex);
   const C_OSCNodeComInterfaceSettings c_ComInterface = *(pc_Node->c_Properties.GetComInterface(C_OSCSystemBus::eCAN,
                                                                                                this->mu8_InterfaceId));
   if (c_ComInterface.GetBusConnected() == true)
   {
      this->mu32_BusIndex = c_ComInterface.u32_BusIndex;
      this->m_FillUpComboBox(c_ComInterface.u32_BusIndex, pc_Node->c_Properties.c_Name.AsStdString()->c_str());
   }
   else
   {
      this->mpc_Ui->pc_CbxNode->addItem(C_GtGetText::h_GetText("<no Nodes available>"));
   }

   // connect
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeCoAddDeviceDialog::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_SdNdeCoAddDeviceDialog::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonEDSPath, &QPushButton::clicked, this,
           &C_SdNdeCoAddDeviceDialog::m_EDSPathButtonClicked);
   connect(this->mpc_Ui->pc_LineEditEDSPath, &C_OgeLeFilePath::editingFinished,
           this, &C_SdNdeCoAddDeviceDialog::m_OnLoadEDS);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeCoAddDeviceDialog::~C_SdNdeCoAddDeviceDialog(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Add CANopen Device"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Select EDS File"));
   this->mpc_Ui->pc_LabelSelectNode->setText(C_GtGetText::h_GetText("1. Select Node"));
   this->mpc_Ui->pc_LabelSelectEDSFile->setText(C_GtGetText::h_GetText("2. Select EDS File"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Continue"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_TedHtmlReport->setPlainText(C_GtGetText::h_GetText("<EDS file description>"));
}

//-------------------------------------------------------------------------------------------f---------------------------
/*! \brief   Get user selected node.

   \param[out]  oru32_NodeIndex        index of selected node
   \param[out]  oru32_InterfaceIndex   index of interface for selected node
   \param[out]  orc_EdsPath            Selected EDS path

   \return
   C_NO_ERR    everything fine
   C_RANGE     something went wrong
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_SdNdeCoAddDeviceDialog::GetNodeSelection(uint32 & oru32_NodeIndex, uint32 & oru32_InterfaceIndex,
                                                  QString & orc_EdsPath) const
{
   sint32 s32_Return = C_RANGE;
   const sint32 s32_CurrentIndex = this->mpc_Ui->pc_CbxNode->currentIndex();

   if ((s32_CurrentIndex >= 0) && (this->mc_InterfaceIndexes.size() == this->mc_NodeIndexes.size()) &&
       (this->mpc_Ui->pc_CbxNode->currentText().compare(C_GtGetText::h_GetText("<no Nodes available>")) != 0))
   {
      if (s32_CurrentIndex < static_cast<sint32>(this->mc_NodeIndexes.size()))
      {
         oru32_NodeIndex = this->mc_NodeIndexes[s32_CurrentIndex];
         oru32_InterfaceIndex = this->mc_InterfaceIndexes[s32_CurrentIndex];
         orc_EdsPath = this->mpc_Ui->pc_LineEditEDSPath->GetPath();
         s32_Return = C_NO_ERR;
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get EDS file

   \return
   Current EDS file path
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_SdNdeCoAddDeviceDialog::GetEDSFile(void)
{
   return C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditEDSPath->GetPath()).toStdString().c_str();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_ParentDialog.accept();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle EDS path button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::m_EDSPathButtonClicked(void)
{
   //Load user settings value
   QString c_Folder = C_UsHandler::h_GetInstance()->GetProjSdTopologyLastKnownCANopenEDSPath();
   QString c_FilePath = "";
   QFileInfo c_FileInfo;
   const QString c_FilterName = static_cast<QString>(C_GtGetText::h_GetText("EDS file")) + " (*." + mhc_SUFFIX + ")";

   //Replace default path if necessary
   if (c_Folder.compare("") == 0)
   {
      c_Folder = C_PuiProject::h_GetInstance()->GetFolderPath();
   }

   c_FilePath =
      C_OgeWiUtil::h_GetOpenFileName(this, C_GtGetText::h_GetText("Select EDS File"),
                                     c_Folder, c_FilterName, mhc_SUFFIX);

   c_FileInfo.setFile(c_FilePath);
   C_UsHandler::h_GetInstance()->SetProjSdTopologyLastKnownCANopenEDSPath(c_FileInfo.absoluteDir().absolutePath());

   if (c_FilePath != "")
   {
      this->m_SetEDSPath(c_FilePath);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle set full EDS path.

   For appropriate QLineEdit the minimized path is shown.
   Also loads the EDS if possible

   \param[in] orc_New New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::m_SetEDSPath(const QString & orc_New)
{
   this->mpc_Ui->pc_LineEditEDSPath->SetPath(orc_New, C_PuiProject::h_GetInstance()->GetFolderPath());
   this->m_OnLoadEDS();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle load picture

   \param[in] oc_FileInfo  File info for filename and filepath
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::m_LoadPicture(const QFileInfo oc_FileInfo)
{
   const QString c_Name = oc_FileInfo.baseName();
   const QString c_Path = oc_FileInfo.absoluteDir().absolutePath();

   const QList<QByteArray> c_Types = QImageReader::supportedImageFormats();

   for (QList<QByteArray>::const_iterator c_It = c_Types.begin(); c_It != c_Types.end(); ++c_It)
   {
      const QString c_CurrentType(*c_It);
      if (QFile::exists(c_Path + "/" + c_Name + static_cast<QString>(".%1").arg(c_CurrentType)) == true)
      {
         const QIcon c_Icon(c_Path + "/" + c_Name + static_cast<QString>(".%1").arg(c_CurrentType));
         const QPixmap c_Pic = c_Icon.pixmap(QSize(mu16_NODE_IMG_WIDTH / 2, mu16_NODE_IMG_WIDTH / 2));
         this->mpc_Ui->pc_LabelPic->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
         this->mpc_Ui->pc_LabelPic->setPixmap(c_Pic);
         this->mpc_Ui->pc_LabelNoImageAvailable->setVisible(false);
         break;
      }
      else
      {
         const QIcon c_Icon("://images/system_definition/Image_Grey.svg");
         const QPixmap c_Pic = c_Icon.pixmap(QSize(mu16_NODE_IMG_WIDTH / 2, mu16_NODE_IMG_WIDTH / 2));
         this->mpc_Ui->pc_LabelPic->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
         this->mpc_Ui->pc_LabelPic->setPixmap(c_Pic);
         this->mpc_Ui->pc_LabelNoImageAvailable->setVisible(true);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize ComboBox for node selection.

   \param[in] ou32_BusIndex   Index of current bus
   \param[in] oc_NodeName     Name of the current node
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::m_FillUpComboBox(const uint32 ou32_BusIndex, const QString oc_NodeName)
{
   uint8 u8_InterfaceId;

   std::vector<QString> c_NotExistNames;
   std::vector<stw_types::uint32> c_NotExistNodeIndexes;
   std::vector<stw_types::uint32> c_NotExistInterfaceIndexes;
   // get node and interface names
   C_PuiSdHandler::h_GetInstance()->GetOSCSystemDefinitionConst().GetNodeIndexesOfBus(ou32_BusIndex,
                                                                                      this->mc_NodeIndexes,
                                                                                      this->mc_InterfaceIndexes);
   const C_OSCCanOpenManagerInfo * const pc_CANopenManagerInfo =
      C_PuiSdHandler::h_GetInstance()->GetCanOpenManager(this->mu32_NodeIndex, this->mu8_InterfaceId);
   const std::map<C_OSCCanInterfaceId,
                  C_OSCCanOpenManagerDeviceInfo> & rc_Devices = pc_CANopenManagerInfo->c_CanOpenDevices;

   tgl_assert(this->mc_NodeIndexes.size() == this->mc_InterfaceIndexes.size());

   if (this->mc_NodeIndexes.size() == this->mc_InterfaceIndexes.size())
   {
      for (uint32 u32_Counter = 0; u32_Counter < this->mc_NodeIndexes.size(); u32_Counter++)
      {
         tgl_assert(C_PuiSdHandler::h_GetInstance()->TranslateCanInterfaceIndexToId(this->mc_NodeIndexes[u32_Counter],
                                                                                    this->mc_InterfaceIndexes[
                                                                                       u32_Counter],
                                                                                    u8_InterfaceId) == C_NO_ERR);
         const uint32 u32_CurrentNodeIndex = this->mc_NodeIndexes[u32_Counter];
         const C_OSCCanInterfaceId c_CanInterfaceId(this->mc_NodeIndexes[u32_Counter], u8_InterfaceId);
         const std::map<C_OSCCanInterfaceId,
                        C_OSCCanOpenManagerDeviceInfo>::const_iterator c_ItDevice = rc_Devices.find(c_CanInterfaceId);
         if ((c_ItDevice == rc_Devices.end()) && (u32_CurrentNodeIndex != this->mu32_NodeIndex))
         {
            const bool q_NodeHasCANOpen = this->m_CheckIfNodeHasCANopenManager(u32_CurrentNodeIndex);

            if (q_NodeHasCANOpen == false)
            {
               c_NotExistNodeIndexes.push_back(this->mc_NodeIndexes[u32_Counter]);
               c_NotExistInterfaceIndexes.push_back(this->mc_InterfaceIndexes[u32_Counter]);
            }
         }
      }
   }
   // cleanup
   this->mpc_Ui->pc_CbxNode->clear();

   tgl_assert(C_SdUtil::h_GetNames(c_NotExistNodeIndexes, c_NotExistInterfaceIndexes, c_NotExistNames,
                                   false) == C_NO_ERR);

   for (std::vector<QString>::const_iterator c_NodeIt = c_NotExistNames.begin();
        c_NodeIt != c_NotExistNames.end(); ++c_NodeIt)
   {
      if (oc_NodeName.compare(*c_NodeIt) != 0)
      {
         this->mpc_Ui->pc_CbxNode->addItem(*c_NodeIt);
      }
   }

   this->mc_NodeIndexes = c_NotExistNodeIndexes;
   tgl_assert((static_cast<sintn>(this->mc_NodeIndexes.size())) == this->mpc_Ui->pc_CbxNode->count());
   this->mc_InterfaceIndexes = c_NotExistInterfaceIndexes;

   if (this->mpc_Ui->pc_CbxNode->count() == 0)
   {
      this->mpc_Ui->pc_CbxNode->addItem(C_GtGetText::h_GetText("<no Nodes available>"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility function to check if node has already a CANopen Manager

   \param[in] ou32_NodeIndex     node index of device to check
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeCoAddDeviceDialog::m_CheckIfNodeHasCANopenManager(const uint32 ou32_NodeIndex)
{
   bool q_NodeHasCANOpen = false;
   const C_OSCNode * const pc_CurrentNode = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);

   // check if node has already a CANopen protocol by its comm interfaces
   std::vector<C_OSCNodeComInterfaceSettings>::const_iterator c_ComInterfacesIter;
   for (c_ComInterfacesIter = pc_CurrentNode->c_Properties.c_ComInterfaces.begin();
        c_ComInterfacesIter != pc_CurrentNode->c_Properties.c_ComInterfaces.end();
        ++c_ComInterfacesIter)
   {
      // only if we have a connected bus, the device can be part of another CANopen Manager
      if ((c_ComInterfacesIter->GetBusConnected() == true) &&
          (c_ComInterfacesIter->e_InterfaceType == C_OSCSystemBus::E_Type::eCAN))
      {
         // let us get the CANopen Manager of found bus
         const C_OSCNode * const pc_ManagerNodeToCheck =
            C_PuiSdHandler::h_GetInstance()->GetCanOpenManagerNodeOnBus(
               c_ComInterfacesIter->u32_BusIndex);
         if (pc_ManagerNodeToCheck != NULL)
         {
            // Ok, we found a manager on this bus, let's check if the manager has our current device.
            // An additional check if current device is a manager device is not needed here because of
            // u32_CurrentNodeIndex != this->mu32_NodeIndex check earlier.
            std::map<stw_types::uint8, C_OSCCanOpenManagerInfo>::const_iterator c_CanOpenManagersIter;
            for (c_CanOpenManagersIter = pc_ManagerNodeToCheck->c_CanOpenManagers.begin();
                 (c_CanOpenManagersIter != pc_ManagerNodeToCheck->c_CanOpenManagers.end());
                 ++c_CanOpenManagersIter)
            {
               std::map<C_OSCCanInterfaceId,
                        C_OSCCanOpenManagerDeviceInfo>::const_iterator c_CanOpenManagerDeviceInfoIter;
               for (c_CanOpenManagerDeviceInfoIter = c_CanOpenManagersIter->second.c_CanOpenDevices.begin();
                    (c_CanOpenManagerDeviceInfoIter != c_CanOpenManagersIter->second.c_CanOpenDevices.end()) &&
                    (q_NodeHasCANOpen == false);
                    ++c_CanOpenManagerDeviceInfoIter)
               {
                  if (c_CanOpenManagerDeviceInfoIter->first.u32_NodeIndex == ou32_NodeIndex)
                  {
                     q_NodeHasCANOpen = true;
                     break;
                  }
               }
            }
         }
      }
   }

   return q_NodeHasCANOpen;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle loading EDS
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeCoAddDeviceDialog::m_OnLoadEDS(void)
{
   bool q_Invalid = false;
   C_OSCCanOpenObjectDictionary c_CanOpenObjDictionary;
   const C_SCLString c_File =
      C_PuiUtil::h_GetAbsolutePathFromProject(this->mpc_Ui->pc_LineEditEDSPath->GetPath()).toStdString().c_str();
   const QFileInfo c_FileInfo(c_File.AsStdString()->c_str());

   if (c_CanOpenObjDictionary.LoadFromFile(c_File) == C_NO_ERR)
   {
      C_SCLIniFile c_IniFile(c_File);

      if ((c_IniFile.SectionExists("FileInfo") == true) && (c_IniFile.SectionExists("DeviceInfo") == true))
      {
         this->mpc_Ui->pc_TedHtmlReport->setText(C_SdUtil::h_GetEdsFileDetails(
                                                    c_CanOpenObjDictionary).toStdString().c_str());
      }
      else
      {
         q_Invalid = true;
         this->mpc_Ui->pc_TedHtmlReport->setPlainText(C_GtGetText::h_GetText("<No readable file>"));
      }
   }
   else
   {
      q_Invalid = true;
      this->mpc_Ui->pc_TedHtmlReport->setPlainText(C_GtGetText::h_GetText("<EDS file description>"));
   }

   if ((QFile::exists(c_File.AsStdString()->c_str()) == true) && (c_FileInfo.suffix().compare(mhc_SUFFIX) == 0) &&
       (q_Invalid == false))
   {
      this->mpc_Ui->pc_LabelFileName->setVisible(true);
      this->mpc_Ui->pc_LabelFileName->setText(c_FileInfo.baseName());
   }
   else
   {
      this->mpc_Ui->pc_LabelFileName->setVisible(false);
      this->mpc_Ui->pc_LabelFileName->setText("");
   }

   this->m_LoadPicture(c_FileInfo);
}
