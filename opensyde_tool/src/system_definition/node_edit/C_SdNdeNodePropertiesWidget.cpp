//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for node properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QCheckBox>
#include <QSpinBox>
#include <QFileInfo>

#include "C_Uti.hpp"
#include "C_PuiSdUtil.hpp"
#include "stwerrors.hpp"
#include "C_SdNdeStwFlashloaderOptions.hpp"
#include "C_SdNdeNodePropertiesWidget.hpp"
#include "ui_C_SdNdeNodePropertiesWidget.h"
#include "C_GtGetText.hpp"
#include "C_OscUtils.hpp"
#include "C_SdNdeComIfSettingsTableDelegate.hpp"
#include "C_PuiSdHandler.hpp"
#include "TglUtils.hpp"
#include "C_SdUtil.hpp"
#include "C_OscNodeProperties.hpp"
#include "C_OgeChxTristateTransparentError.hpp"
#include "constants.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgeChxTristate.hpp"
#include "C_OscNodeComInterfaceSettings.hpp"
#include "C_SdNdeIpAddressConfigurationWidget.hpp"
#include "C_OgeLabNodePropComIfTable.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeNodeEditWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::scl;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint16_t mu16_NODE_IMG_WIDTH = 300;

const uint8_t mu8_FL_INDEX_OS = 0;
const uint8_t mu8_FL_INDEX_STW = 1;
const uint8_t mu8_FL_INDEX_NOSUPPORT = 2;

const int32_t C_SdNdeNodePropertiesWidget::mhs32_PR_INDEX_DISABLED = 0;
const int32_t C_SdNdeNodePropertiesWidget::mhs32_PR_INDEX_ENABLED = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeNodePropertiesWidget::C_SdNdeNodePropertiesWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeNodePropertiesWidget),
   mu32_NodeIndex(0),
   mu32_BusIndex(0),
   mc_BusName("")
{
   QPixmap c_ImgLogo;
   QSizePolicy c_SizePolicy;

   // init UI
   mpc_Ui->setupUi(this);

   //style of subnode information
   this->mpc_Ui->pc_LabSubNodeTitle->SetBackgroundColor(0);
   this->mpc_Ui->pc_LabSubNodeTitle->SetForegroundColor(4);
   this->mpc_Ui->pc_LabSubNodeTitle->SetFontPixel(13);
   this->mpc_Ui->pc_LabSubNodeName->SetBackgroundColor(11);
   this->mpc_Ui->pc_LabSubNodeName->SetForegroundColor(1);
   this->mpc_Ui->pc_LabSubNodeName->SetFontPixel(13);

   //load STW logo
   c_ImgLogo.load("://images/STW_Logo_Dark.png");
   c_ImgLogo = c_ImgLogo.scaled((c_ImgLogo.width() / 22), (c_ImgLogo.height() / 22),
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
   this->mpc_Ui->pc_StwLogo->setPixmap(c_ImgLogo);

   //Options buttons
   this->mpc_Ui->pc_PushButtonFlashloaderOptions->SetCustomIcon("://images/SettingsIcon.svg",
                                                                "://images/SettingsIconDisabled.svg");
   this->mpc_Ui->pc_PushButtonFlashloaderOptions->setIconSize(mc_ICON_SIZE_24);

   //set explicit min width to allow the layout to cut off parts of the button (necessary to free space for smaller
   // screen resolutions)
   this->mpc_Ui->pc_PushButtonFlashloaderOptions->setMinimumWidth(52);

   //retain size when hidden
   c_SizePolicy = this->mpc_Ui->pc_PushButtonFlashloaderOptions->sizePolicy();
   c_SizePolicy.setRetainSizeWhenHidden(true);
   this->mpc_Ui->pc_PushButtonFlashloaderOptions->setSizePolicy(c_SizePolicy);

   InitStaticNames();

   //table setups
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(static_cast<int32_t> (
                                                                      C_SdNdeComIfSettingsTableDelegate::eINTERFACE))->
   setTextAlignment(static_cast<int32_t> (Qt::AlignLeft));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(static_cast<int32_t> (
                                                                      C_SdNdeComIfSettingsTableDelegate::eCONNECTION))->
   setTextAlignment(static_cast<int32_t> (Qt::AlignLeft));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(static_cast<int32_t> (
                                                                      C_SdNdeComIfSettingsTableDelegate::eNODEID))->
   setTextAlignment(static_cast<int32_t> (Qt::AlignHCenter));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(static_cast<int32_t> (
                                                                      C_SdNdeComIfSettingsTableDelegate::eIPADDRESS))->
   setTextAlignment(static_cast<int32_t> (Qt::AlignHCenter));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(static_cast<int32_t> (
                                                                      C_SdNdeComIfSettingsTableDelegate::eUPDATE))->
   setTextAlignment(static_cast<int32_t> (Qt::AlignHCenter));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(static_cast<int32_t> (
                                                                      C_SdNdeComIfSettingsTableDelegate::eROUTING))->
   setTextAlignment(static_cast<int32_t> (Qt::AlignHCenter));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(static_cast<int32_t> (
                                                                      C_SdNdeComIfSettingsTableDelegate::eDIAGNOSTIC))->
   setTextAlignment(static_cast<int32_t> (Qt::AlignHCenter));

   //set min column width (necessary for "Linked to..." strech column
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->setMinimumSectionSize(150);

   //setup column size mode and size
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->setSectionResizeMode(static_cast<int32_t> (
                                                                                          C_SdNdeComIfSettingsTableDelegate
                                                                                          ::eINTERFACE),
                                                                                       QHeaderView::Fixed);

   this->mpc_Ui->pc_TableWidgetComIfSettings->setColumnWidth(static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::
                                                                                   eINTERFACE), 150);

   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->setSectionResizeMode(static_cast<int32_t> (
                                                                                          C_SdNdeComIfSettingsTableDelegate
                                                                                          ::eCONNECTION),
                                                                                       QHeaderView::Stretch);

   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->setSectionResizeMode(static_cast<int32_t> (
                                                                                          C_SdNdeComIfSettingsTableDelegate
                                                                                          ::eNODEID),
                                                                                       QHeaderView::Fixed);
   this->mpc_Ui->pc_TableWidgetComIfSettings->setColumnWidth(static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::
                                                                                   eNODEID), 150);

   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->setSectionResizeMode(static_cast<int32_t> (
                                                                                          C_SdNdeComIfSettingsTableDelegate
                                                                                          ::eIPADDRESS),
                                                                                       QHeaderView::Fixed);
   this->mpc_Ui->pc_TableWidgetComIfSettings->setColumnWidth(static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::
                                                                                   eIPADDRESS), 150);

   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->setSectionResizeMode(static_cast<int32_t> (
                                                                                          C_SdNdeComIfSettingsTableDelegate
                                                                                          ::eUPDATE),
                                                                                       QHeaderView::Fixed);
   this->mpc_Ui->pc_TableWidgetComIfSettings->setColumnWidth(static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::
                                                                                   eUPDATE), 150);

   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->setSectionResizeMode(static_cast<int32_t> (
                                                                                          C_SdNdeComIfSettingsTableDelegate
                                                                                          ::eROUTING),
                                                                                       QHeaderView::Fixed);
   this->mpc_Ui->pc_TableWidgetComIfSettings->setColumnWidth(static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::
                                                                                   eROUTING), 150);

   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->setSectionResizeMode(static_cast<int32_t> (
                                                                                          C_SdNdeComIfSettingsTableDelegate
                                                                                          ::eDIAGNOSTIC),
                                                                                       QHeaderView::Fixed);
   this->mpc_Ui->pc_TableWidgetComIfSettings->setColumnWidth(static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::
                                                                                   eDIAGNOSTIC), 150);

   //Name restriction
   this->mpc_Ui->pc_LineEditNodeName->setMaxLength(ms32_C_ITEM_MAX_CHAR_COUNT);

   // connects
   connect(this->mpc_Ui->pc_LineEditNodeName, &QLineEdit::textChanged, this,
           &C_SdNdeNodePropertiesWidget::m_CheckNodeName);
   connect(this->mpc_Ui->pc_LineEditNodeName, &QLineEdit::editingFinished, this,
           &C_SdNdeNodePropertiesWidget::m_TrimNodeName);
   connect(this->mpc_Ui->pc_TableWidgetComIfSettings, &QTableWidget::cellChanged, this,
           &C_SdNdeNodePropertiesWidget::m_CheckComInterface);

   connect(this->mpc_Ui->pc_TableWidgetComIfSettings, &QTableWidget::cellClicked, this,
           &C_SdNdeNodePropertiesWidget::m_HandleCellClick);
   connect(this->mpc_Ui->pc_PushButtonFlashloaderOptions, &C_OgePubOptions::clicked, this,
           &C_SdNdeNodePropertiesWidget::m_FlashloaderOptions);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxProtocol,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SdNdeNodePropertiesWidget::m_SupportedProtocolChange);

   // see m_BusBitrateClicked for details
   this->mc_Timer.setSingleShot(true);
   this->mc_Timer.setInterval(100);
   connect(&this->mc_Timer, &QTimer::timeout, this, &C_SdNdeNodePropertiesWidget::m_OpenBus);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeNodePropertiesWidget::~C_SdNdeNodePropertiesWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::InitStaticNames(void) const
{
   QString c_InterfaceString;
   QString c_ConnectString;

   const int32_t s32_COL_INTERFACE = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eINTERFACE);
   const int32_t s32_COL_CONNECTION = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eCONNECTION);
   const int32_t s32_COL_NODE_ID = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eNODEID);
   const int32_t s32_COL_IP_ADDRESS = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eIPADDRESS);
   const int32_t s32_COL_UPDATE = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eUPDATE);
   const int32_t s32_COL_ROUTING = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eROUTING);
   const int32_t s32_COL_DIAGNOSTIC = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eDIAGNOSTIC);

   this->mpc_Ui->pc_LabSubNodeTitle->setText(C_GtGetText::h_GetText("Sub-Node"));
   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_LabelConfiguration->setText(C_GtGetText::h_GetText("Configuration"));
   this->mpc_Ui->pc_LabelProtocol->setText(C_GtGetText::h_GetText("Protocol Support"));
   this->mpc_Ui->pc_LabelProgramming->setText(C_GtGetText::h_GetText("Programming Support"));
   this->mpc_Ui->pc_LabelComIfSettings->setText(C_GtGetText::h_GetText("Communication Interfaces Settings"));

   this->mpc_Ui->pc_ComboBoxProtocol->addItem(C_GtGetText::h_GetText("openSYDE"));
   this->mpc_Ui->pc_ComboBoxProtocol->addItem(C_GtGetText::h_GetText("KEFEX"));
   this->mpc_Ui->pc_ComboBoxProtocol->addItem(C_GtGetText::h_GetText("None"));

   this->mpc_Ui->pc_ComboBoxProgramming->addItem(C_GtGetText::h_GetText("Disabled"));
   this->mpc_Ui->pc_ComboBoxProgramming->addItem(C_GtGetText::h_GetText("Enabled"));

   //Add space because of spacing issue between icon and text
   this->mpc_Ui->pc_PushButtonFlashloaderOptions->setText(static_cast<QString>(' ') +
                                                          C_GtGetText::h_GetText("STW Flashloader Settings"));

   //table column text
   //fake padding with Spaces. No other solution known so far
   c_InterfaceString = "          ";
   c_InterfaceString.append(C_GtGetText::h_GetText("Interface"));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(s32_COL_INTERFACE)->setText(c_InterfaceString);

   //fake padding with Spaces. No other solution known so far
   c_ConnectString = "  ";
   c_ConnectString.append(C_GtGetText::h_GetText("Linked to..."));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(s32_COL_CONNECTION)->setText(c_ConnectString);
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(s32_COL_NODE_ID)->setText(C_GtGetText::h_GetText(
                                                                                                "Node ID"));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(s32_COL_IP_ADDRESS)->setText(C_GtGetText::h_GetText(
                                                                                                   "IP Address"));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(s32_COL_UPDATE)->setText(C_GtGetText::h_GetText(
                                                                                               "Usable for Update"));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(s32_COL_ROUTING)->setText(C_GtGetText::h_GetText(
                                                                                                "Usable for Routing"));
   this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeaderItem(s32_COL_DIAGNOSTIC)->setText(C_GtGetText::h_GetText(
                                                                                                   "Usable for Dashboard"));

   this->mpc_Ui->pc_TextEditComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));

   //Tool tips
   this->mpc_Ui->pc_LabSubNodeTitle->SetToolTipInformation(C_GtGetText::h_GetText("Sub-Node"),
                                                           C_GtGetText::h_GetText("Name of the Sub-Node."));

   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText("Name"),
                                                     C_GtGetText::h_GetText(
                                                        "Symbolic node name. Unique within Network Topology.\n"
                                                        "\nFollowing C naming conventions are required:"
                                                        "\n - must not be empty"
                                                        "\n - must not start with digits"
                                                        "\n - only alphanumeric characters and \"_\""
                                                        "\n - should not be longer than 31 characters"));

   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this node."));

   this->mpc_Ui->pc_LabelProgramming->SetToolTipInformation(C_GtGetText::h_GetText("Programming Support"),
                                                            C_GtGetText::h_GetText(
                                                               "This property shows if the device is user programmable."
                                                               "\nDefined in read only *.syde_devdef file."
                                                               "\n\nIf enabled, the source code generation feature can "
                                                               "be activated for Data Blocks ."));

   this->mpc_Ui->pc_LabelProtocol->SetToolTipInformation(C_GtGetText::h_GetText("Protocol Support"),
                                                         C_GtGetText::h_GetText(
                                                            "Type of flashloader and diagnostic server.\n"
                                                            "Options:\n"
                                                            "   - openSYDE: openSYDE Server and openSYDE Flashloader support\n"
                                                            "   - KEFEX: KEFEX server and STW Flashloader support\n"
                                                            "   - none: no STW protocol support (e.g.: 3rd party node)\n"
                                                            "\nSupported protocols defined in read only "
                                                            "*.syde_devdef file."));

   this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipHeadingAt(s32_COL_INTERFACE, Qt::Horizontal,
                                                                  C_GtGetText::h_GetText("Interface"),
                                                                  C_GtGetText::h_GetText(
                                                                     "Name of communication interface (CAN/ETHERNET)."));

   this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipHeadingAt(s32_COL_CONNECTION, Qt::Horizontal,
                                                                  C_GtGetText::h_GetText("Linked to..."),
                                                                  C_GtGetText::h_GetText(
                                                                     "Name of bus to which the interface is linked to."));

   this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipHeadingAt(s32_COL_NODE_ID, Qt::Horizontal,
                                                                  C_GtGetText::h_GetText("Node ID"),
                                                                  C_GtGetText::h_GetText(
                                                                     "Node ID is unique on connected bus. The ID is "
                                                                     "used for addressing in the communication "
                                                                     "protocol. \nThis property is configured for all "
                                                                     "connected interfaces on device while \"Device "
                                                                     "configuration\" (SYSTEM COMMISSIONING/Setup)."));

   this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipHeadingAt(s32_COL_IP_ADDRESS, Qt::Horizontal,
                                                                  C_GtGetText::h_GetText("IP Address"),
                                                                  C_GtGetText::h_GetText(
                                                                     "IP address settings: IP address and subnet mask"
                                                                     "\nThese properties are configured for all "
                                                                     "connected interfaces on device while \"Device "
                                                                     "configuration\" (SYSTEM COMMISSIONING/Setup)"));

   this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipHeadingAt(s32_COL_UPDATE, Qt::Horizontal,
                                                                  C_GtGetText::h_GetText("Usable for Update"),
                                                                  C_GtGetText::h_GetText(
                                                                     "If enabled, the interface is usable for System Update. "
                                                                     "(SYSTEM COMMISSIONING - Update)"
                                                                     "\n\nThis property is just "
                                                                     "a configuration for openSYDE PC tool, "
                                                                     "it is NOT configured on device."));

   this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipHeadingAt(s32_COL_DIAGNOSTIC, Qt::Horizontal,
                                                                  C_GtGetText::h_GetText("Usable for Dashboard"),
                                                                  C_GtGetText::h_GetText(
                                                                     "If enabled, the interface is usable for Dashboard "
                                                                     "(Access of Datapool data elements via diagnostic protocol). "
                                                                     "(SYSTEM COMMISSIONING - Dashboards)"
                                                                     "\n\nThis property is just "
                                                                     "a configuration for openSYDE PC tool, "
                                                                     "it is NOT configured on device."));

   this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipHeadingAt(s32_COL_ROUTING, Qt::Horizontal,
                                                                  C_GtGetText::h_GetText("Usable for Routing"),
                                                                  C_GtGetText::h_GetText(
                                                                     "If enabled, the interface is usable for Routing. "
                                                                     "\nAttention: This property is intended as additive "
                                                                     "feature in addition to \"System Update\" and \"Dashboards\" "
                                                                     "\nUse cases: SYSTEM COMMISSIONING - Update "
                                                                     "SYSTEM COMMISSIONING - Dashboards.\n\nThis "
                                                                     "property is just a configuration for openSYDE tool, "
                                                                     "it is NOT configured on device."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selects the node name in the text edit for fast editing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::SelectName(void) const
{
   this->mpc_Ui->pc_LineEditNodeName->setFocus();
   this->mpc_Ui->pc_LineEditNodeName->selectAll();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Node ID setter

   Sets the private node id of widget

   \param[in]  ou32_NodeIndex    new node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::SetNodeId(const uint32_t ou32_NodeIndex)
{
   this->mu32_NodeIndex = ou32_NodeIndex;

   //load node data
   this->m_LoadFromData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load node information

   Load node information from core data using node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_LoadFromData(void)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   //Disconnects for RegisterChange
   disconnect(this->mpc_Ui->pc_LineEditNodeName, &QLineEdit::editingFinished, this,
              &C_SdNdeNodePropertiesWidget::m_RegisterNameChange);
   disconnect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
              &C_SdNdeNodePropertiesWidget::m_RegisterChange);
   disconnect(this->mpc_Ui->pc_TableWidgetComIfSettings, &QTableWidget::cellChanged, this,
              &C_SdNdeNodePropertiesWidget::m_RegisterErrorChange);
   disconnect(this->mpc_Ui->pc_TableWidgetComIfSettings, &QTableWidget::cellChanged, this,
              &C_SdNdeNodePropertiesWidget::m_CheckComInterface);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxProtocol,
              static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
              &C_SdNdeNodePropertiesWidget::m_SupportedProtocolChange);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      QPixmap c_ImgNode;
      QString c_ComIfName;
      QString c_BusName;
      const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
      const uint32_t u32_SubDeviceIndex = pc_Node->u32_SubDeviceIndex;
      tgl_assert(pc_DevDef != NULL);

      //create delegate
      this->mpc_Ui->pc_TableWidgetComIfSettings->setItemDelegate(new C_SdNdeComIfSettingsTableDelegate(this,
                                                                                                       this->
                                                                                                       mu32_NodeIndex));

      if (pc_DevDef != NULL)
      {
         tgl_assert(u32_SubDeviceIndex < pc_DevDef->c_SubDevices.size());
         if (u32_SubDeviceIndex < pc_DevDef->c_SubDevices.size())
         {
            QFileInfo c_FileInfo;
            bool q_FileExists;
            bool q_StwFlashloaderActive = false;
            uint32_t u32_NodeSquadIndex;

            //node name
            this->mpc_Ui->pc_LineEditNodeName->setText(C_PuiSdUtil::h_GetNodeBaseNameOrName(this->mu32_NodeIndex));

            //sub node info
            if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(this->mu32_NodeIndex,
                                                                                u32_NodeSquadIndex) == C_NO_ERR)
            {
               //squad node
               this->mpc_Ui->pc_SubNodeInfoWidget->setVisible(true);
               this->mpc_Ui->pc_LabSubNodeName->setText(C_PuiSdUtil::h_GetSubNodeDeviceName(this->mu32_NodeIndex));
            }
            else
            {
               //no squad node
               this->mpc_Ui->pc_SubNodeInfoWidget->setVisible(false);
            }

            //comment
            this->mpc_Ui->pc_TextEditComment->setText(pc_Node->c_Properties.c_Comment.c_str());

            //protocol
            if ((pc_DevDef->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeEthernet == true) ||
                (pc_DevDef->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeCan == true) ||
                (pc_DevDef->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderStwCan == true))
            {
               if (pc_Node->c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_OPEN_SYDE)
               {
                  //openSYDE
                  this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(mu8_FL_INDEX_OS);
               }
               else if (pc_Node->c_Properties.e_FlashLoader == C_OscNodeProperties::eFL_STW)
               {
                  //STW
                  this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(mu8_FL_INDEX_STW);
                  q_StwFlashloaderActive = true;
               }
               else
               {
                  //having "none" as flashloader is supported by the device definition structures;
                  // but not by the UI (yet)
                  tgl_assert(false);
               }

               if (((pc_DevDef->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeEthernet == true) ||
                    (pc_DevDef->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderOpenSydeCan == true)) &&
                   (pc_DevDef->c_SubDevices[u32_SubDeviceIndex].q_FlashloaderStwCan == true))
               {
                  // Hybrid node. Both variants are possible.
                  this->mpc_Ui->pc_ComboBoxProtocol->setEnabled(true);
                  // No support is not supported on node with protocol support
                  this->mpc_Ui->pc_ComboBoxProtocol->removeItem(mu8_FL_INDEX_NOSUPPORT);
               }
            }
            else
            {
               //not supported
               this->mpc_Ui->pc_ComboBoxProtocol->setCurrentIndex(mu8_FL_INDEX_NOSUPPORT);
            }

            //activate STW flashloader button (options)
            this->mpc_Ui->pc_PushButtonFlashloaderOptions->setVisible(q_StwFlashloaderActive);

            //programming
            if (pc_DevDef->c_SubDevices[u32_SubDeviceIndex].q_ProgrammingSupport == true)
            {
               this->mpc_Ui->pc_ComboBoxProgramming->setCurrentIndex(C_SdNdeNodePropertiesWidget::mhs32_PR_INDEX_ENABLED);
            }
            else
            {
               this->mpc_Ui->pc_ComboBoxProgramming->setCurrentIndex(
                  C_SdNdeNodePropertiesWidget::mhs32_PR_INDEX_DISABLED);
            }

            //load device picture
            c_FileInfo.setFile(pc_DevDef->c_ImagePath.c_str());
            q_FileExists = (c_FileInfo.exists() && c_FileInfo.isFile());

            //check if file exists
            if (q_FileExists == true)
            {
               c_ImgNode.load(pc_DevDef->c_ImagePath.c_str());
               c_ImgNode = c_ImgNode.scaled(mu16_NODE_IMG_WIDTH, c_ImgNode.height(), //second parameter is not relevant
                                            Qt::KeepAspectRatio, Qt::SmoothTransformation);
               this->mpc_Ui->pc_LabelNoImageAvailable->setVisible(false);
               this->mpc_Ui->pc_DatapoolTypeImage->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            }
            else
            {
               //no image available
               const QIcon c_Icon("://images/system_definition/Image_Grey.svg");
               c_ImgNode = c_Icon.pixmap(QSize(mu16_NODE_IMG_WIDTH / 2, mu16_NODE_IMG_WIDTH / 2));
               this->mpc_Ui->pc_LabelNoImageAvailable->setVisible(true);
               this->mpc_Ui->pc_DatapoolTypeImage->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
            }

            this->mpc_Ui->pc_DatapoolTypeImage->setPixmap(c_ImgNode);

            //load device type name
            this->mpc_Ui->pc_LabelNodeType->setText(pc_DevDef->GetDisplayName().c_str());

            //clear table
            this->mpc_Ui->pc_TableWidgetComIfSettings->setRowCount(0);

            //insert can+ethernet count of rows
            this->mpc_Ui->pc_TableWidgetComIfSettings->setRowCount(static_cast<int32_t> (pc_DevDef->u8_NumCanBusses) +
                                                                   static_cast<int32_t> (pc_DevDef->u8_NumEthernetBusses));

            {
               //set new table max size (Header = 38 + row*40)
               const int32_t s32_MaxHeight = 38 + (this->mpc_Ui->pc_TableWidgetComIfSettings->rowCount() * 40);
               //Minimum is slightly off the optimal value (3 rows are visible)
               const int32_t s32_MinHeight = 38 +
                                             (std::min(3, this->mpc_Ui->pc_TableWidgetComIfSettings->rowCount()) * 40);
               this->mpc_Ui->pc_TableWidgetComIfSettings->setMinimumHeight(s32_MinHeight);
               this->mpc_Ui->pc_TableWidgetComIfSettings->setMaximumHeight(s32_MaxHeight);
            }

            //insert com ifs
            for (uint8_t u8_ComIfCnt = 0;
                 u8_ComIfCnt <
                 (static_cast<int32_t> (pc_DevDef->u8_NumCanBusses) +
                  static_cast<int32_t> (pc_DevDef->u8_NumEthernetBusses));
                 ++u8_ComIfCnt)
            {
               bool q_InterfaceIsConnected = false;

               const int32_t s32_COL_INTERFACE = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eINTERFACE);
               const int32_t s32_COL_CONNECTION = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eCONNECTION);
               const int32_t s32_COL_NODE_ID = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eNODEID);
               const int32_t s32_COL_IP_ADDRESS = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eIPADDRESS);
               const int32_t s32_COL_UPDATE = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eUPDATE);
               const int32_t s32_COL_ROUTING = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eROUTING);
               const int32_t s32_COL_DIAGNOSTIC = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eDIAGNOSTIC);
               bool q_IsUpdateAvailable;
               bool q_IsRoutingAvailable;
               bool q_IsDiagnosisAvailable;
               bool q_IsRoutingEnabled = true;
               if (u8_ComIfCnt < pc_DevDef->u8_NumCanBusses)
               {
                  q_IsUpdateAvailable =
                     pc_DevDef->c_SubDevices[u32_SubDeviceIndex].IsUpdateAvailable(C_OscSystemBus::eCAN);
                  q_IsRoutingAvailable = pc_Node->IsRoutingAvailable(C_OscSystemBus::eCAN);
                  q_IsDiagnosisAvailable = pc_DevDef->c_SubDevices[u32_SubDeviceIndex].IsDiagnosisAvailable(
                     C_OscSystemBus::eCAN);
               }
               else
               {
                  q_IsUpdateAvailable = pc_DevDef->c_SubDevices[u32_SubDeviceIndex].IsUpdateAvailable(
                     C_OscSystemBus::eETHERNET);
                  q_IsRoutingAvailable = pc_Node->IsRoutingAvailable(C_OscSystemBus::eETHERNET);
                  q_IsDiagnosisAvailable = pc_DevDef->c_SubDevices[u32_SubDeviceIndex].IsDiagnosisAvailable(
                     C_OscSystemBus::eETHERNET);
               }

               QString c_IpAddressString;

               /**********************************************************************************************************/
               //INTERFACE
               this->mpc_Ui->pc_TableWidgetComIfSettings->setCellWidget(
                  u8_ComIfCnt, s32_COL_INTERFACE, new C_OgeChxTristateTransparentError(this));
               //disable
               this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt,
                                                                     s32_COL_INTERFACE)->setEnabled(false);

               //set name
               if (u8_ComIfCnt < pc_DevDef->u8_NumCanBusses)
               {
                  //its a CAN interface
                  c_ComIfName =
                     C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eCAN, static_cast<uint8_t>(u8_ComIfCnt));
               }
               else
               {
                  //its an Ethernet interface
                  c_ComIfName =
                     C_PuiSdUtil::h_GetInterfaceName(C_OscSystemBus::eETHERNET,
                                                     static_cast<uint8_t>(u8_ComIfCnt -
                                                                          static_cast<int32_t> (pc_DevDef->
                                                                                                u8_NumCanBusses)));
               }

               dynamic_cast<C_OgeChxTristateTransparentError *> (this->mpc_Ui->pc_TableWidgetComIfSettings->
                                                                 cellWidget(
                                                                    u8_ComIfCnt,
                                                                    s32_COL_INTERFACE))->setText(c_ComIfName);

               /**********************************************************************************************************/
               //CONNECTED TO
               this->mpc_Ui->pc_TableWidgetComIfSettings->setCellWidget(u8_ComIfCnt, s32_COL_CONNECTION,
                                                                        new C_OgeLabNodePropComIfTable(this));

               //set bus name
               if (pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].GetBusConnected() == true)
               {
                  const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
                     pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].u32_BusIndex);
                  //get bus name
                  if (pc_Bus != NULL)
                  {
                     c_BusName = pc_Bus->c_Name.c_str();

                     if (pc_Bus->e_Type == C_OscSystemBus::eCAN)
                     {
                        // add the bitrate
                        c_BusName += " (" + QString::number(pc_Bus->u64_BitRate / 1000ULL) + " kbit/s)";
                     }

                     // Is this bus usable for routing
                     q_IsRoutingEnabled = pc_Bus->q_UseableForRouting;
                  }
               }
               else
               {
                  c_BusName = "-"; // "not connected";
               }

               dynamic_cast<QLabel *> (this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(
                                          u8_ComIfCnt, s32_COL_CONNECTION))->setText(c_BusName);

               if (c_BusName != "-")
               {
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt, s32_COL_CONNECTION)->setEnabled(
                     true);
                  C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TableWidgetComIfSettings
                                                         ->cellWidget(u8_ComIfCnt, s32_COL_CONNECTION),
                                                         "COMIF_TableCell_Hyperlink", true);
               }
               //if the node isn't connected to a bus, the link to bus definition shall not work
               else
               {
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt, s32_COL_CONNECTION)->setEnabled(
                     false);
               }

               /**********************************************************************************************************/
               //NODE ID
               this->mpc_Ui->pc_TableWidgetComIfSettings->setItem(u8_ComIfCnt, s32_COL_NODE_ID,
                                                                  new QTableWidgetItem(""));
               //align center
               this->mpc_Ui->pc_TableWidgetComIfSettings->item(u8_ComIfCnt, s32_COL_NODE_ID)
               ->setTextAlignment(static_cast<int32_t> (Qt::AlignCenter));
               //set node value
               this->mpc_Ui->pc_TableWidgetComIfSettings->item(u8_ComIfCnt, s32_COL_NODE_ID)->
               setText(QString::number(pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].u8_NodeId));

               this->mpc_Ui->pc_TableWidgetComIfSettings->item(u8_ComIfCnt, s32_COL_NODE_ID)->setFlags(
                  Qt::ItemIsEnabled | Qt::ItemIsEditable);

               /**********************************************************************************************************/
               //IP Address
               this->mpc_Ui->pc_TableWidgetComIfSettings->setCellWidget(u8_ComIfCnt, s32_COL_IP_ADDRESS,
                                                                        new C_OgeLabNodePropComIfTable(this));

               //set IP Address
               if (u8_ComIfCnt >= static_cast<int32_t> (pc_DevDef->u8_NumCanBusses))
               {
                  c_IpAddressString.append(QString::number(pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].c_Ip.
                                                           au8_IpAddress[0]));
                  c_IpAddressString.append(".");
                  c_IpAddressString.append(QString::number(pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].c_Ip.
                                                           au8_IpAddress[1]));
                  c_IpAddressString.append(".");
                  c_IpAddressString.append(QString::number(pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].c_Ip.
                                                           au8_IpAddress[2]));
                  c_IpAddressString.append(".");
                  c_IpAddressString.append(QString::number(pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].c_Ip.
                                                           au8_IpAddress[3]));
               }
               else
               {
                  c_IpAddressString = "-";
               }

               dynamic_cast<QLabel *> (this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(
                                          u8_ComIfCnt, s32_COL_IP_ADDRESS))->setText(c_IpAddressString);

               dynamic_cast<QLabel *> (this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(
                                          u8_ComIfCnt, s32_COL_IP_ADDRESS))->setAlignment(Qt::AlignCenter);

               //apply enabled/disabled style
               //if CAN, disable per default
               if (u8_ComIfCnt < pc_DevDef->u8_NumCanBusses)
               {
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt,
                                                                        s32_COL_IP_ADDRESS)->setEnabled(false);
               }
               else
               {
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt,
                                                                        s32_COL_IP_ADDRESS)->setEnabled(true);
                  //apply special style
                  C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TableWidgetComIfSettings
                                                         ->cellWidget(u8_ComIfCnt,
                                                                      s32_COL_IP_ADDRESS), "COMIF_TableCell_Hyperlink",
                                                         true);
               }

               //hide the ip address column for devices without Ethernet interfaces
               if (pc_DevDef->u8_NumEthernetBusses == 0)
               {
                  this->mpc_Ui->pc_TableWidgetComIfSettings->horizontalHeader()->hideSection(static_cast<int32_t> (
                                                                                                C_SdNdeComIfSettingsTableDelegate
                                                                                                ::eIPADDRESS));
               }

               /**********************************************************************************************************/
               // UPDATE
               this->mpc_Ui->pc_TableWidgetComIfSettings->setCellWidget(u8_ComIfCnt, s32_COL_UPDATE, new C_OgeChxTristate(
                                                                           this));
               //set node value
               if (q_IsUpdateAvailable == true)
               {
                  //defensive move
                  if ((pc_DevDef->u8_NumCanBusses) > 0)
                  {
                     // if Protocol Cbx is KEFEX and we got ETH interfaces update shall be disabled
                     if ((q_StwFlashloaderActive == true) && (u8_ComIfCnt >= (pc_DevDef->u8_NumCanBusses)))
                     {
                        this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt,
                                                                              s32_COL_UPDATE)->setEnabled(false);

                        dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                          ->cellWidget(u8_ComIfCnt, s32_COL_UPDATE))
                        ->setChecked(false);
                     }
                     else
                     {
                        dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                          ->cellWidget(u8_ComIfCnt, s32_COL_UPDATE))
                        ->setChecked(pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].q_IsUpdateEnabled);
                     }
                  }
                  else
                  {
                     tgl_assert((pc_DevDef->u8_NumCanBusses - 1) <= 0);
                  }
               }
               else
               {
                  //disable
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt,
                                                                        s32_COL_UPDATE)->setEnabled(false);
               }
               //connect to RegisterChange
               connect(dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                         ->cellWidget(u8_ComIfCnt,
                                                                      s32_COL_UPDATE)), &QCheckBox::stateChanged, this,
                       &C_SdNdeNodePropertiesWidget::m_RegisterChange);

               /**********************************************************************************************************/
               // ROUTING
               this->mpc_Ui->pc_TableWidgetComIfSettings->setCellWidget(
                  u8_ComIfCnt, s32_COL_ROUTING, new C_OgeChxTristate(
                     this));

               if (q_IsRoutingAvailable == true)
               {
                  //set node value
                  dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                    ->cellWidget(u8_ComIfCnt, s32_COL_ROUTING))
                  ->setChecked(pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].q_IsRoutingEnabled);

                  if (q_IsRoutingEnabled == false)
                  {
                     // Routing for the bus has been disabled
                     this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt, s32_COL_ROUTING)->
                     setEnabled(false);
                  }
               }

               else
               {
                  //disable
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt, s32_COL_ROUTING)->
                  setEnabled(false);
               }

               //connect to RegisterChange
               connect(dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                         ->cellWidget(u8_ComIfCnt,
                                                                      s32_COL_ROUTING)), &QCheckBox::stateChanged, this,
                       &C_SdNdeNodePropertiesWidget::m_RegisterChange);

               /**********************************************************************************************************/
               // DIAGNOSTIC
               this->mpc_Ui->pc_TableWidgetComIfSettings->setCellWidget(u8_ComIfCnt, s32_COL_DIAGNOSTIC, new C_OgeChxTristate(
                                                                           this));

               if (q_IsDiagnosisAvailable == true)
               {
                  //set node value
                  dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                    ->cellWidget(u8_ComIfCnt, s32_COL_DIAGNOSTIC))
                  ->setChecked(pc_Node->c_Properties.c_ComInterfaces[u8_ComIfCnt].q_IsDiagnosisEnabled);
               }
               else
               {
                  //disable
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u8_ComIfCnt, s32_COL_DIAGNOSTIC)->
                  setEnabled(false);
               }

               //connect to RegisterChange
               connect(dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                         ->cellWidget(u8_ComIfCnt,
                                                                      s32_COL_DIAGNOSTIC)), &QCheckBox::stateChanged, this,
                       &C_SdNdeNodePropertiesWidget::m_RegisterChange);

               //hide rows if they are not connected (necessary for sub nodes)

               //check if interface is connected
               if (u8_ComIfCnt >= static_cast<int32_t> (pc_DevDef->u8_NumCanBusses))
               {
                  //ETH
                  q_InterfaceIsConnected =
                     pc_DevDef->c_SubDevices[u32_SubDeviceIndex].IsConnected(C_OscSystemBus::eETHERNET,
                                                                             static_cast<uint8_t>(u8_ComIfCnt
                                                                                                  -
                                                                                                  static_cast<int32_t>
                                                                                                  (
                                                                                                     pc_DevDef->
                                                                                                     u8_NumCanBusses)));
               }
               else
               {
                  //CAN
                  q_InterfaceIsConnected =
                     pc_DevDef->c_SubDevices[u32_SubDeviceIndex].IsConnected(C_OscSystemBus::eCAN,
                                                                             static_cast<uint8_t>(u8_ComIfCnt));
               }

               //hide row if not connected
               if (q_InterfaceIsConnected == false)
               {
                  this->mpc_Ui->pc_TableWidgetComIfSettings->hideRow(u8_ComIfCnt);
               }
            }
         }
      }
   }

   // Initial check of the data. Row is irrelevant because all rows will be checked of the column.
   this->m_CheckComInterface(0U, static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::eNODEID));

   //connects for RegisterChange
   connect(this->mpc_Ui->pc_LineEditNodeName, &QLineEdit::editingFinished, this,
           &C_SdNdeNodePropertiesWidget::m_RegisterNameChange);
   connect(this->mpc_Ui->pc_TextEditComment, &QTextEdit::textChanged, this,
           &C_SdNdeNodePropertiesWidget::m_RegisterChange);
   connect(this->mpc_Ui->pc_TableWidgetComIfSettings, &QTableWidget::cellChanged, this,
           &C_SdNdeNodePropertiesWidget::m_RegisterErrorChange);
   connect(this->mpc_Ui->pc_TableWidgetComIfSettings, &QTableWidget::cellChanged, this,
           &C_SdNdeNodePropertiesWidget::m_CheckComInterface);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxProtocol,
           static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_SdNdeNodePropertiesWidget::m_SupportedProtocolChange);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save ui data to node

   Is called from outside
      - on system definition save
      - on page change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::SaveToData(void)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
      const uint32_t u32_SubDeviceIndex = pc_Node->u32_SubDeviceIndex;
      tgl_assert(pc_DevDef != NULL);

      if (pc_DevDef != NULL)
      {
         tgl_assert(u32_SubDeviceIndex < pc_DevDef->c_SubDevices.size());
         if (u32_SubDeviceIndex < pc_DevDef->c_SubDevices.size())
         {
            QString c_Name;
            QString c_Comment;
            C_OscNodeProperties::E_DiagnosticServerProtocol e_DiagnosticServer;
            C_OscNodeProperties::E_FlashLoaderProtocol e_FlashLoader;
            std::vector<uint8_t> c_NodeIds;
            std::vector<bool> c_UpdateFlags;
            std::vector<bool> c_RoutingFlags;
            std::vector<bool> c_DiagnosisFlags;
            const int32_t s32_COL_NODE_ID = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eNODEID);
            const int32_t s32_COL_UPDATE = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eUPDATE);
            const int32_t s32_COL_ROUTING = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eROUTING);
            const int32_t s32_COL_DIAGNOSTIC = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eDIAGNOSTIC);

            //save data

            //name
            //Only accept new name if not in conflict
            if (C_PuiSdHandler::h_GetInstance()->CheckNodeNameAvailable(
                   this->mpc_Ui->pc_LineEditNodeName->text().toStdString().c_str(), &this->mu32_NodeIndex, NULL))
            {
               c_Name = this->mpc_Ui->pc_LineEditNodeName->text();
            }
            else
            {
               //Restore previous name
               uint32_t u32_NodeSquadIndex;

               if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(this->mu32_NodeIndex,
                                                                                   u32_NodeSquadIndex) == C_NO_ERR)
               {
                  //squad node
                  const stw::opensyde_core::C_OscNodeSquad * const pc_NodeSquad =
                     C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(u32_NodeSquadIndex);
                  if (pc_NodeSquad != NULL)
                  {
                     //name (base name)
                     c_Name = pc_NodeSquad->c_BaseName.c_str();
                  }
               }
               else
               {
                  //name
                  c_Name = pc_Node->c_Properties.c_Name.c_str();
               }
            }

            //comment
            c_Comment = this->mpc_Ui->pc_TextEditComment->toPlainText();

            switch (this->mpc_Ui->pc_ComboBoxProtocol->currentIndex())
            {
            case mu8_FL_INDEX_OS:
               e_FlashLoader = C_OscNodeProperties::eFL_OPEN_SYDE;
               e_DiagnosticServer = C_OscNodeProperties::eDS_OPEN_SYDE;
               break;
            case mu8_FL_INDEX_STW:
               e_FlashLoader = C_OscNodeProperties::eFL_STW;
               e_DiagnosticServer = C_OscNodeProperties::eDS_KEFEX;
               break;
            default:
               //Not supported
               e_FlashLoader = C_OscNodeProperties::eFL_NONE;
               e_DiagnosticServer = C_OscNodeProperties::eDS_NONE;
               break;
            }

            //com interface settings
            for (uint16_t u16_ComIfCnt = 0;
                 (u16_ComIfCnt < pc_Node->c_Properties.c_ComInterfaces.size()) &&
                 (u16_ComIfCnt < this->mpc_Ui->pc_TableWidgetComIfSettings->rowCount());
                 ++u16_ComIfCnt)
            {
               bool q_NewValue;
               const C_OscNodeComInterfaceSettings & rc_CurInterface =
                  pc_Node->c_Properties.c_ComInterfaces[u16_ComIfCnt];
               const bool q_IsUpdateAvailable = pc_DevDef->c_SubDevices[u32_SubDeviceIndex].IsUpdateAvailable(
                  rc_CurInterface.e_InterfaceType);
               const bool q_IsRoutingAvailable = pc_Node->IsRoutingAvailable(rc_CurInterface.e_InterfaceType);
               const bool q_IsDiagnosisAvailable = pc_DevDef->c_SubDevices[u32_SubDeviceIndex].IsDiagnosisAvailable(
                  rc_CurInterface.e_InterfaceType);
               //node id
               c_NodeIds.push_back(static_cast<uint8_t> ((this->mpc_Ui->pc_TableWidgetComIfSettings->item(u16_ComIfCnt,
                                                                                                          s32_COL_NODE_ID)
                                                          ->
                                                          text()
                                                          .toInt())));

               //update
               if (q_IsUpdateAvailable == true)
               {
                  //set state only if the cell is enabled (connected + available)
                  if (this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u16_ComIfCnt,
                                                                            s32_COL_UPDATE)->isEnabled() == true)
                  {
                     q_NewValue =
                        dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                          ->cellWidget(u16_ComIfCnt, s32_COL_UPDATE))->isChecked();
                  }
                  else
                  {
                     q_NewValue = false;
                  }
               }
               else
               {
                  q_NewValue = false;
               }
               c_UpdateFlags.push_back(q_NewValue);

               //routing
               if (q_IsRoutingAvailable == true)
               {
                  //set state only if the cell is enabled (connected + available)
                  if (this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u16_ComIfCnt,
                                                                            s32_COL_ROUTING)->isEnabled() == true)
                  {
                     q_NewValue =
                        dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                          ->cellWidget(u16_ComIfCnt, s32_COL_ROUTING))->isChecked();
                  }
                  else
                  {
                     q_NewValue = false;
                  }
               }
               else
               {
                  q_NewValue = false;
               }
               c_RoutingFlags.push_back(q_NewValue);

               //diagnosis
               if (q_IsDiagnosisAvailable == true)
               {
                  //set state only if the cell is enabled (connected + available)
                  if (this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u16_ComIfCnt,
                                                                            s32_COL_DIAGNOSTIC)->isEnabled() == true)
                  {
                     q_NewValue =
                        dynamic_cast<C_OgeChxTristate *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                                                          ->cellWidget(u16_ComIfCnt, s32_COL_DIAGNOSTIC))->isChecked();
                  }
                  else
                  {
                     q_NewValue = false;
                  }
               }
               else
               {
                  q_NewValue = false;
               }
               c_DiagnosisFlags.push_back(q_NewValue);
            }

            //save new node
            C_PuiSdHandler::h_GetInstance()->SetOscNodePropertiesDetailed(this->mu32_NodeIndex, c_Name, c_Comment,
                                                                          e_DiagnosticServer, e_FlashLoader, c_NodeIds,
                                                                          c_UpdateFlags, c_RoutingFlags,
                                                                          c_DiagnosisFlags);

            //send signal SigNodePropChanged (trigger to adapt canopen config)
            Q_EMIT (this->SigNodePropChanged());
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Reacts on changing protocol

   Adapts the visibility of the STW flashloader options button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_SupportedProtocolChange(void)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   // Update the visibility of the STW flashloader specific option button
   this->mpc_Ui->pc_PushButtonFlashloaderOptions->setVisible(
      this->mpc_Ui->pc_ComboBoxProtocol->currentIndex() == mu8_FL_INDEX_STW);

   // Save the data
   this->m_RegisterChange();

   // Update the com interface routing settings in the table
   if (pc_Node != NULL)
   {
      const C_OscDeviceDefinition * const pc_DevDef = pc_Node->pc_DeviceDefinition;
      const C_OscNodeProperties c_NodeProp = pc_Node->c_Properties;

      tgl_assert(pc_DevDef != NULL);
      if (pc_DevDef != NULL)
      {
         for (uint16_t u16_ComIfCnt = 0U;
              u16_ComIfCnt <
              (static_cast<uint16_t>(pc_DevDef->u8_NumCanBusses) +
               static_cast<uint16_t>(pc_DevDef->u8_NumEthernetBusses));
              ++u16_ComIfCnt)
         {
            const C_OscNodeComInterfaceSettings & rc_CurInterface = pc_Node->c_Properties.c_ComInterfaces[u16_ComIfCnt];
            const int32_t s32_COL_ROUTING = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eROUTING);
            const int32_t s32_COL_UPDATE = static_cast<int32_t>(C_SdNdeComIfSettingsTableDelegate::eUPDATE);
            const bool q_IsRoutingAvailable = pc_Node->IsRoutingAvailable(rc_CurInterface.e_InterfaceType);

            C_OgeChxTristate * const pc_Tristate =
               dynamic_cast<C_OgeChxTristate *>(this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u16_ComIfCnt,
                                                                                                      s32_COL_ROUTING));
            C_OgeChxTristate * const pc_TristateUpdate =
               dynamic_cast<C_OgeChxTristate *>(this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u16_ComIfCnt,
                                                                                                      s32_COL_UPDATE));

            if (pc_Tristate != NULL)
            {
               this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u16_ComIfCnt, s32_COL_ROUTING)->setEnabled(
                  q_IsRoutingAvailable);

               // if KEFEX is set update shall be disabled for eth interfaces
               if ((rc_CurInterface.e_InterfaceType == C_OscSystemBus::eETHERNET) &&
                   (c_NodeProp.e_FlashLoader == C_OscNodeProperties::eFL_STW))
               {
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u16_ComIfCnt,
                                                                        s32_COL_UPDATE)->setEnabled(false);
                  pc_Tristate->setChecked(false);
                  pc_TristateUpdate->setChecked(false);
               }
               // if protocol switches to opensyde enable and check update for eth interfaces again
               else
               {
                  this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(u16_ComIfCnt, s32_COL_UPDATE)->setEnabled(true);
                  pc_TristateUpdate->setChecked(true);
               }

               if (q_IsRoutingAvailable == true)
               {
                  //set node value
                  pc_Tristate->setChecked(rc_CurInterface.q_IsRoutingEnabled);
               }
               else
               {
                  // Setting is disabled, so it has to be unchecked too
                  pc_Tristate->setChecked(false);
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check node name

   - check input
   - show/hide invalid icon
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_CheckNodeName(void)
{
   //check
   const C_SclString c_Text = this->mpc_Ui->pc_LineEditNodeName->text().toStdString().c_str();
   const bool q_NameIsUnique = C_PuiSdHandler::h_GetInstance()->CheckNodeNameAvailable(c_Text, &this->mu32_NodeIndex,
                                                                                       NULL);
   const bool q_NameIsValid = C_OscUtils::h_CheckValidCeName(c_Text);

   //set invalid text property
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditNodeName, "Valid", q_NameIsUnique && q_NameIsValid);

   if ((q_NameIsUnique == true) && (q_NameIsValid == true))
   {
      this->mpc_Ui->pc_LineEditNodeName->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                               C_GtGetText::h_GetText(""),
                                                               C_NagToolTip::eDEFAULT);
   }
   else
   {
      const QString c_Heading = C_GtGetText::h_GetText("Node Name");
      QString c_Content;
      if (q_NameIsUnique == false)
      {
         c_Content += C_GtGetText::h_GetText("- is already in use\n");
      }
      if (q_NameIsValid == false)
      {
         c_Content += C_GtGetText::h_GetText("- is empty or contains invalid characters.\n");
      }
      this->mpc_Ui->pc_LineEditNodeName->SetToolTipInformation(c_Heading, c_Content, C_NagToolTip::eERROR);
   }

   Q_EMIT this->SigChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trim node name

   Remove whitespaces at the beginning and end of the string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_TrimNodeName(void) const
{
   this->mpc_Ui->pc_LineEditNodeName->setText(this->mpc_Ui->pc_LineEditNodeName->text().trimmed());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register Change

   Function where ui elements register a change. Change will be sent via a signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_RegisterChange(void)
{
   SaveToData();
   //signal
   Q_EMIT this->SigChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Register error change

   Function where ui elements register an error change. Change will be sent via a signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_RegisterErrorChange()
{
   m_RegisterChange();
   //signal
   Q_EMIT this->SigErrorChange();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register name change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_RegisterNameChange(void)
{
   //This function can somehow be called twice, so ... let's avoid that!
   static bool hq_InProgress = false;

   if (hq_InProgress == false)
   {
      std::vector<stw::scl::C_SclString> c_ExistingNames;
      hq_InProgress = true;

      if (C_PuiSdHandler::h_GetInstance()->CheckNodeNameAvailable(
             this->mpc_Ui->pc_LineEditNodeName->text().toStdString().c_str(),
             &this->mu32_NodeIndex,
             &c_ExistingNames) == false)
      {
         const QString c_Description =
            static_cast<QString>(C_GtGetText::h_GetText(
                                    "A node with the name \"%1\" already exists. Choose another name.")).
            arg(this->mpc_Ui->pc_LineEditNodeName->text());
         QString c_Details;
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
         c_Message.SetHeading(C_GtGetText::h_GetText("Node naming"));
         c_Message.SetDescription(c_Description);
         c_Details.append(C_GtGetText::h_GetText("Used node names:\n"));
         for (uint32_t u32_ItExistingName = 0UL; u32_ItExistingName < c_ExistingNames.size(); ++u32_ItExistingName)
         {
            const C_SclString & rc_Name = c_ExistingNames[u32_ItExistingName];
            c_Details.append(static_cast<QString>("\"%1\"\n").arg(rc_Name.c_str()));
         }
         c_Message.SetDetails(c_Details);
         c_Message.SetCustomMinHeight(220, 350);
         c_Message.Execute();
         //Restore previous name
         {
            const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
            if (pc_Node != NULL)
            {
               uint32_t u32_NodeSquadIndex;

               if (C_PuiSdHandler::h_GetInstance()->GetNodeSquadIndexWithNodeIndex(this->mu32_NodeIndex,
                                                                                   u32_NodeSquadIndex) == C_NO_ERR)
               {
                  //squad node
                  const stw::opensyde_core::C_OscNodeSquad * const pc_NodeSquad =
                     C_PuiSdHandler::h_GetInstance()->GetOscNodeSquadConst(u32_NodeSquadIndex);
                  if (pc_NodeSquad != NULL)
                  {
                     //name (base name)
                     this->mpc_Ui->pc_LineEditNodeName->setText(pc_NodeSquad->c_BaseName.c_str());
                  }
               }
               else
               {
                  //name
                  this->mpc_Ui->pc_LineEditNodeName->setText(pc_Node->c_Properties.c_Name.c_str());
               }
            }
         }
      }
      else
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
         m_TrimNodeName();
         m_RegisterChange();

         if (pc_Node != NULL)
         {
            Q_EMIT (this->SigNameChanged(C_GtGetText::h_GetText(
                                            "NETWORK TOPOLOGY"), pc_Node->c_Properties.c_Name.c_str(), false));
         }
      }
      hq_InProgress = false; //lint !e838 its static and could be used on strange second call
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check com interface id

   - check input
   - valid/invalid text

   \param[in]  ou32_Row       Row
   \param[in]  ou32_Column    Column
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_CheckComInterface(const uint32_t ou32_Row, const uint32_t ou32_Column) const
{
   const int32_t s32_COL_NODE_ID = static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::eNODEID);
   const int32_t s32_COL_IP = static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::eIPADDRESS);

   Q_UNUSED(ou32_Row)

   //node id or ip change?
   if ((ou32_Column == s32_COL_NODE_ID) || (ou32_Column == s32_COL_IP))
   {
      int32_t s32_RowCounter; // Equals com interface number.

      bool q_IdValid;
      bool q_IpValid;

      for (s32_RowCounter = 0U; s32_RowCounter < this->mpc_Ui->pc_TableWidgetComIfSettings->model()->rowCount();
           ++s32_RowCounter)
      {
         //get node id from cell
         const uint8_t u8_NodeId =
            static_cast<uint8_t> (this->mpc_Ui->pc_TableWidgetComIfSettings->item(s32_RowCounter,
                                                                                  s32_COL_NODE_ID)->text().toInt());

         //get ip address from cell
         const QString c_Ip =
            dynamic_cast<QLabel *>(this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(s32_RowCounter,
                                                                                         s32_COL_IP))
            ->text();
         //convert string into vector for core logic
         const QStringList c_IpBytesTmp = c_Ip.split('.');
         std::vector<int32_t> c_IpBytes;
         for (int32_t s32_It = 0; s32_It < c_IpBytesTmp.size(); ++s32_It)
         {
            c_IpBytes.push_back(static_cast<int32_t>(c_IpBytesTmp[s32_It].toInt()));
         }

         //check node id and ip address of current row for conflicts
         this->m_GetInterfaceStatus(this->mu32_NodeIndex, s32_RowCounter, u8_NodeId, c_IpBytes, q_IdValid, q_IpValid);

         //handle coloring of cell, when properties are conflicting
         this->m_HandlePropertyConflict(s32_RowCounter, s32_COL_NODE_ID, s32_COL_IP, q_IdValid, q_IpValid);

         //handle error icon and tool-tips with already used properties
         this->m_HandleErrorFeedback(s32_RowCounter, q_IdValid, q_IpValid);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Wrapper for core logic. Checks for id and ip collision on given interface.

   \param[in]    ou32_NodeIndex        Node index
   \param[in]    os32_InterfaceIndex   Interface index
   \param[in]    ou8_NodeId            Node id to investigate
   \param[in]    orc_Ip                Ip address to investigate
   \param[out]   orq_IdValid           Storage for result of node id check
   \param[out]   orq_IpValid           Storage for result of ip address check

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_GetInterfaceStatus(const uint32_t ou32_NodeIndex, const int32_t os32_InterfaceIndex,
                                                       const uint8_t ou8_NodeId, const std::vector<int32_t> & orc_Ip,
                                                       bool & orq_IdValid, bool & orq_IpValid) const
{
   //check if node id is valid
   orq_IdValid =
      C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().CheckInterfaceIsAvailable(
         ou32_NodeIndex,
         os32_InterfaceIndex,
         ou8_NodeId);

   //check if ip is valid
   orq_IpValid =
      C_PuiSdHandler::h_GetInstance()->GetOscSystemDefinitionConst().CheckIpAddressIsValid(
         ou32_NodeIndex,
         os32_InterfaceIndex, orc_Ip);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function marks specified cell content red if a property is invalid (id/ip collision)

   \param[in]      os32_Row      Table row
   \param[in]      os32_ColumnId Table column for node id
   \param[in]      os32_ColumnIp Table column for ip address
   \param[in]      oq_IdValid    Status of node id. (false: conflict!)
   \param[in]      oq_IpValid    Status of ip address. (false: conflict!)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_HandlePropertyConflict(const int32_t os32_Row, const int32_t os32_ColumnId,
                                                           const int32_t os32_ColumnIp, const bool oq_IdValid,
                                                           const bool oq_IpValid) const
{
   if (oq_IdValid == false)
   {
      //invalid
      this->mpc_Ui->pc_TableWidgetComIfSettings->model()->setData(
         this->mpc_Ui->pc_TableWidgetComIfSettings->model()->index(os32_Row, os32_ColumnId),
         mc_STYLE_GUIDE_COLOR_24, static_cast<int32_t> (Qt::ForegroundRole));
   }
   else
   {
      //valid
      this->mpc_Ui->pc_TableWidgetComIfSettings->model()->setData(
         this->mpc_Ui->pc_TableWidgetComIfSettings->model()->index(os32_Row, os32_ColumnId),
         mc_STYLE_GUIDE_COLOR_6, static_cast<int32_t> (Qt::ForegroundRole));
   }

   if (oq_IpValid == false)
   {
      //invalid
      this->mpc_Ui->pc_TableWidgetComIfSettings->model()->setData(
         this->mpc_Ui->pc_TableWidgetComIfSettings->model()->index(os32_Row, os32_ColumnIp),
         mc_STYLE_GUIDE_COLOR_24, static_cast<int32_t> (Qt::ForegroundRole));
   }
   else
   {
      //valid
      this->mpc_Ui->pc_TableWidgetComIfSettings->model()->setData(
         this->mpc_Ui->pc_TableWidgetComIfSettings->model()->index(os32_Row, os32_ColumnIp),
         mc_STYLE_GUIDE_COLOR_6, static_cast<int32_t> (Qt::ForegroundRole));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handles different error cases, sets tool-tip and error icon and prepares content for tool-tip

   \param[in]   os32_InterfaceIndex    Interface index
   \param[in]   oq_IdValid             True: Conflicting ID
   \param[in]   oq_IpValid             True: Conflicting IP
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_HandleErrorFeedback(const int32_t os32_InterfaceIndex, const bool oq_IdValid,
                                                        const bool oq_IpValid) const
{
   bool q_ShowIcon = false;

   if ((oq_IdValid == true) && (oq_IpValid == true))
   {
      this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipAt(os32_InterfaceIndex,
                                                              static_cast<uint32_t> (
                                                                 C_SdNdeComIfSettingsTableDelegate
                                                                 ::eINTERFACE), "", "",
                                                              C_NagToolTip::eDEFAULT);
   }
   else
   {
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
      if (pc_Node != NULL)
      {
         if (os32_InterfaceIndex < static_cast<int32_t>(pc_Node->c_Properties.c_ComInterfaces.size()))
         {
            const C_OscNodeComInterfaceSettings & rc_Interface =
               pc_Node->c_Properties.c_ComInterfaces[os32_InterfaceIndex];
            if (rc_Interface.GetBusConnected() == true)
            {
               const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
                  rc_Interface.u32_BusIndex);
               if (pc_Bus != NULL)
               {
                  QString c_TooltipContent;

                  //only ids are conflicting
                  if ((oq_IdValid == false) && (oq_IpValid == true))
                  {
                     q_ShowIcon = true;
                     const std::vector<uint32_t> c_UsedIds = C_SdUtil::h_GetUsedNodeIdsForBusUniqueAndSortedAscending(
                        rc_Interface.u32_BusIndex, this->mu32_NodeIndex, static_cast<int32_t>(os32_InterfaceIndex));
                     const QString c_Heading = C_GtGetText::h_GetText("Interface: Node ID invalid");
                     c_TooltipContent = C_SdUtil::h_InitUsedIdsString(c_UsedIds,
                                                                      pc_Bus->c_Name.c_str(),
                                                                      C_GtGetText::h_GetText("bus"));
                     this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipAt(os32_InterfaceIndex,
                                                                             static_cast<uint32_t> (
                                                                                C_SdNdeComIfSettingsTableDelegate
                                                                                ::eINTERFACE), c_Heading,
                                                                             c_TooltipContent, C_NagToolTip::eERROR);
                  }
                  //only ips are conflicting
                  else if ((oq_IpValid == false) && (oq_IdValid == true))
                  {
                     q_ShowIcon = true;
                     const std::vector<std::vector<uint8_t> > c_Ips = C_SdUtil::h_GetAllUsedIpAddressesForBus(
                        rc_Interface.u32_BusIndex, this->mu32_NodeIndex,
                        static_cast<int32_t>(os32_InterfaceIndex));
                     const QString c_Heading = C_GtGetText::h_GetText("Interface: IP Address invalid");
                     c_TooltipContent = C_SdUtil::h_InitUsedIpsString(c_Ips,
                                                                      pc_Bus->c_Name.c_str(),
                                                                      C_GtGetText::h_GetText("bus"));

                     this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipAt(os32_InterfaceIndex,
                                                                             static_cast<uint32_t> (
                                                                                C_SdNdeComIfSettingsTableDelegate
                                                                                ::eINTERFACE), c_Heading,
                                                                             c_TooltipContent, C_NagToolTip::eERROR);
                  }
                  //both id and ip are conflicting
                  else if ((oq_IdValid == false) && (oq_IpValid == false))
                  {
                     q_ShowIcon = true;
                     const std::vector<uint32_t> c_UsedIds = C_SdUtil::h_GetUsedNodeIdsForBusUniqueAndSortedAscending(
                        rc_Interface.u32_BusIndex, this->mu32_NodeIndex, static_cast<int32_t>(os32_InterfaceIndex));
                     const std::vector<std::vector<uint8_t> > c_Ips = C_SdUtil::h_GetAllUsedIpAddressesForBus(
                        rc_Interface.u32_BusIndex, this->mu32_NodeIndex, static_cast<int32_t>(os32_InterfaceIndex));
                     const QString c_Heading = C_GtGetText::h_GetText("Interface: Property invalid");
                     c_TooltipContent = C_SdUtil::h_InitUsedIdsString(c_UsedIds,
                                                                      pc_Bus->c_Name.c_str(),
                                                                      C_GtGetText::h_GetText("bus"));
                     c_TooltipContent += "\n";
                     c_TooltipContent += C_SdUtil::h_InitUsedIpsString(c_Ips,
                                                                       pc_Bus->c_Name.c_str(),
                                                                       C_GtGetText::h_GetText("bus"));

                     this->mpc_Ui->pc_TableWidgetComIfSettings->SetToolTipAt(os32_InterfaceIndex,
                                                                             static_cast<uint32_t> (
                                                                                C_SdNdeComIfSettingsTableDelegate
                                                                                ::eINTERFACE), c_Heading,
                                                                             c_TooltipContent, C_NagToolTip::eERROR);
                  }
                  else
                  {
                     //nothing to do here
                  }
               }
            }
         }
      }
   }
   //handle invalid icon
   dynamic_cast<QCheckBox *> (this->mpc_Ui->pc_TableWidgetComIfSettings
                              ->cellWidget(os32_InterfaceIndex,
                                           static_cast<int32_t>
                                           (C_SdNdeComIfSettingsTableDelegate::
                                            eINTERFACE)))
   ->setChecked(q_ShowIcon);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle cell click

   Decides which column in Communication Interfaces Settings Table was clicked and which action to perform after click.
   If a IP-Address is clicked, a pop up to edit the IP-Address will show up.
   If a Bus-Bitrate was clicked, Tool jumps to "Edit Bus Properties" Screen.

   \param[in]  ou32_Row       Table Row
   \param[in]  ou32_Column    Table Column
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_HandleCellClick(const uint32_t ou32_Row, const uint32_t ou32_Column)
{
   const int32_t s32_COL_IP_ADDRESS = static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::eIPADDRESS);
   const int32_t s32_COL_BUS_BITRATE = static_cast<int32_t> (C_SdNdeComIfSettingsTableDelegate::eCONNECTION);
   const bool q_EnabledCellIp = this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(ou32_Row,
                                                                                      s32_COL_IP_ADDRESS)->isEnabled();
   const bool q_EnabledCellBus = this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(ou32_Row,
                                                                                       s32_COL_BUS_BITRATE)->isEnabled();

   if ((ou32_Column == s32_COL_IP_ADDRESS) && (q_EnabledCellIp == true))
   {
      this->m_IpAddressClick(ou32_Row);
   }
   else if ((ou32_Column == s32_COL_BUS_BITRATE) && (q_EnabledCellBus == true))
   {
      this->m_BusBitrateClick(ou32_Row, ou32_Column);
   }
   else
   {
      //nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   m_IpAddressClick

   open Pop Up for clicked IP Cell

   \param[in]  ou32_Row    Row
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_IpAddressClick(const uint32_t ou32_Row)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      //Set parent for better hierarchy handling via window manager
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this->parentWidget(), this->parentWidget());
      C_SdNdeIpAddressConfigurationWidget * const pc_Dialog =
         new C_SdNdeIpAddressConfigurationWidget(*c_New, this->mu32_NodeIndex, ou32_Row);
      const QSize c_SIZE(600, 416);

      Q_UNUSED(pc_Dialog)

      //Resize
      c_New->SetSize(c_SIZE);

      if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
      {
         //refresh table
         this->m_LoadFromData();
      }
      //Hide overlay after dialog is not relevant anymore
      if (c_New.isNull() == false)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_New and pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  collects info about the bus, which got clicked in table and sends signal to change screen. Timer is needed

 *          to delay the signal, that a cell was clicked and the screen should change to bus properties. Without this
 *          opensyde crashes, because of unexpected behaviour of the table. It seems that the table still wants to
 *          perform actions after signal is out and screen is changed (while parent widget is already deleted).

   \param[in]  ou32_Row       Row
   \param[in]  ou32_Column    Column
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_BusBitrateClick(const uint32_t ou32_Row, const uint32_t ou32_Column)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   const C_OscNodeProperties c_Prop = pc_Node->c_Properties;

   std::vector<C_OscNodeComInterfaceSettings> c_ComInterfaces = c_Prop.c_ComInterfaces;

   for (uint32_t u32_It = 0; u32_It < c_ComInterfaces.size(); ++u32_It)
   {
      //check to which buses the node is connected
      if (pc_Node->c_Properties.c_ComInterfaces[u32_It].GetBusConnected() == true)
      {
         const uint32_t u32_BusIndex = c_ComInterfaces[u32_It].u32_BusIndex;
         //get name of connected bus
         const C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
            pc_Node->c_Properties.c_ComInterfaces[u32_It].u32_BusIndex);
         const QString c_BusName = pc_Bus->c_Name.c_str();

         //get label text of the cell, that was clicked

         const QLabel * const pc_CellLabel =
            dynamic_cast<QLabel *> (this->mpc_Ui->pc_TableWidgetComIfSettings->cellWidget(ou32_Row, ou32_Column));

         if (pc_CellLabel != NULL)
         {
            const QString c_Text = pc_CellLabel->text();
            // do we have a match? change screen
            if (c_Text.contains(c_BusName) == true)
            {
               this->mu32_BusIndex = u32_BusIndex;
               this->mc_BusName = c_BusName;
               this->mc_Timer.start();
               break;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for Timer::timeout to walk around the screen change on table-click.

 *          See m_BusBitrateClicked for details.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_OpenBus(void)
{
   Q_EMIT (this->SigBusBitrateClicked(this->mu32_BusIndex, this->mc_BusName));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of flashloader options button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesWidget::m_FlashloaderOptions(void) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);

   if (pc_Node != NULL)
   {
      const C_OscNodeStwFlashloaderSettings c_Copy = pc_Node->c_Properties.c_StwFlashloaderSettings;
      //Set parent for better hierarchy handling via window manager
      const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this->parentWidget(), this->parentWidget());
      C_SdNdeStwFlashloaderOptions * const pc_Dialog = new C_SdNdeStwFlashloaderOptions(*c_New, this->mu32_NodeIndex);

      Q_UNUSED(pc_Dialog)

      //Resize
      c_New->SetSize(QSize(780, 480));

      if (c_New->exec() != static_cast<int32_t>(QDialog::Accepted))
      {
         //Revert
         C_PuiSdHandler::h_GetInstance()->SetStwFlashloaderSettings(this->mu32_NodeIndex, c_Copy);
      }
      //Hide overlay after dialog is not relevant anymore
      if (c_New.isNull() == false)
      {
         c_New->HideOverlay();
         c_New->deleteLater();
      }
   } //lint !e429  //no memory leak because of the parent of pc_New and pc_Dialog and the Qt memory management
}
