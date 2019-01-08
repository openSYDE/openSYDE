//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for toolbox of system definition

   Offers devices in different lists (C_SdTopologyListWidget) as icons.

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.08.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QLabel>
#include "TGLFile.h"
#include "constants.h"
#include "CSCLIniFile.h"
#include "C_OSCDeviceDefinitionFiler.h"
#include "C_SdTopologyToolbox.h"
#include "C_OgeLabToolboxHeadingGroup.h"
#include "C_GtGetText.h"
#include "ui_C_SdTopologyToolbox.h"
#include "C_SebToolboxUtil.h"
#include "C_OSCSystemDefinition.h"

#include "stwtypes.h"
#include "stwerrors.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;
using namespace stw_types;
using namespace stw_tgl;
using namespace stw_scl;
using namespace stw_errors;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     01.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdTopologyToolbox::C_SdTopologyToolbox(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdTopologyToolbox)
{
   mpc_Ui->setupUi(this);
   this->mpc_Ui->pc_ListWidgetSearch->setVisible(false);
   this->mpc_Ui->pc_BlankWidgetSearch->setVisible(false);
   this->mpc_Ui->pc_BlankTopWidgetSearch->setVisible(false);
   this->mpc_Ui->pc_LabelSearchNotFound->setVisible(false);
   this->mpc_Ui->pc_LabelSearchNotFound->setText(C_GtGetText::h_GetText("No results found"));

   InitStaticNames();
   m_FillToolboxStatic();
   m_FillToolboxDynamic();

   // set the 'nodes' tab initially
   this->mpc_Ui->pc_TabWidget->setCurrentIndex(0);

   // The search list shall use the whole free area of the widget
   this->mpc_Ui->pc_ListWidgetSearch->SetMaximumHeightAdaption(false);

   mpc_Ui->pc_ListWidgetDrawing->setDropIndicatorShown(false);
   mpc_Ui->pc_ListWidgetSearch->setDropIndicatorShown(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     01.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdTopologyToolbox::~C_SdTopologyToolbox()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for a search entry change

   If the search text is empty, the search was finished.
   If the search text is not empty, the search result has to be updated.

   \param[in] opc_Text   Updated search text

   \created     09.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdTopologyToolbox::SearchChanged(const QString & orc_Text)
{
   this->mpc_Ui->pc_ListWidgetSearch->clear();
   QList<QListWidgetItem *> c_ListItems;
   QList<QListWidgetItem *>::const_iterator c_ItItem;
   bool q_NoItemsFound = true;

   if (orc_Text == "")
   {
      // no active search
      this->mpc_Ui->pc_ListWidgetSearch->setVisible(false);
      this->mpc_Ui->pc_BlankWidgetSearch->setVisible(false);
      this->mpc_Ui->pc_BlankTopWidgetSearch->setVisible(false);
      this->mpc_Ui->pc_LabelSearchNotFound->setVisible(false);
      this->mpc_Ui->pc_TabWidget->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_TabWidget->setVisible(false);
      this->mpc_Ui->pc_ListWidgetSearch->setVisible(true);
      this->mpc_Ui->pc_BlankWidgetSearch->setVisible(true);
      this->mpc_Ui->pc_BlankTopWidgetSearch->setVisible(true);
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

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     28.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdTopologyToolbox::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(0, C_GtGetText::h_GetText("Nodes"));
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(1, C_GtGetText::h_GetText("Buses"));
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(2, C_GtGetText::h_GetText("Meta"));
   this->mpc_Ui->pc_LabelFreeElements->setText(C_GtGetText::h_GetText("Meta Information Elements"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten enter event slot

   To get the correct cursor back from the resizing implementation of
   C_OgeWiHover.

   \param[in,out] opc_Event Event identification and information

   \created     04.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdTopologyToolbox::enterEvent(QEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   this->setCursor(Qt::ArrowCursor);
   this->parentWidget()->setMouseTracking(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten leave event slot

   To get the correct cursor back from the resizing implementation of
   C_OgeWiHover.

   \param[in,out] opc_Event Event identification and information

   \created     04.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdTopologyToolbox::leaveEvent(QEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   this->parentWidget()->setMouseTracking(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Fill toolbox with dynamic content

   \created     12.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdTopologyToolbox::m_FillToolboxDynamic(void)
{
   C_SdTopologyListWidget * pc_List = NULL;
   QListWidgetItem * pc_Item;

   std::vector<C_OSCDeviceDefinition> c_Devices;
   std::vector<C_OSCDeviceGroup> c_DeviceGroups = C_OSCSystemDefinition::hc_Devices.GetDeviceGroups();
   for (uint32 u32_ItDeviceGroup = 0; u32_ItDeviceGroup < c_DeviceGroups.size(); ++u32_ItDeviceGroup)
   {
      c_Devices = c_DeviceGroups[u32_ItDeviceGroup].GetDevices();
      pc_List = C_SebToolboxUtil::h_AddNewList(
         c_DeviceGroups[u32_ItDeviceGroup].GetGroupName().c_str(),
         this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
         this);
      if (pc_List != NULL)
      {
         QIcon c_Icon;
         c_Icon.addPixmap(QPixmap("://images/system_definition/PreviewNode.svg"), QIcon::Normal);
         c_Icon.addPixmap(QPixmap("://images/system_definition/PreviewNode.svg"), QIcon::Selected);
         for (uint32 u32_ItDevice = 0; u32_ItDevice < c_Devices.size(); ++u32_ItDevice)
         {
            const C_OSCDeviceDefinition & rc_Device = c_Devices[u32_ItDevice];
            const QString c_DeviceName = rc_Device.c_DeviceName.c_str();
            //Tooltip
            const QString c_DeviceDescription = QString(rc_Device.c_DeviceDescription.c_str()) + "\n \n" +
                                                QString(C_GtGetText::h_GetText("Available buses: CAN=%1, Ethernet=%2\n"))
                                                .arg(rc_Device.u8_NumCanBusses).arg(rc_Device.u8_NumEthernetBusses) +
                                                QString(C_GtGetText::h_GetText("NVM-Size: %1 Bytes")).arg(
               rc_Device.u32_UserEepromSizeBytes);
            pc_List->addItem(rc_Device.GetDisplayName().c_str());
            pc_Item = pc_List->item(pc_List->count() - 1);
            pc_Item->setData(msn_USER_ROLE_ADDITIONAL_INFORMATION, c_DeviceName);
            //Toolbox icon
            pc_Item->setIcon(c_Icon);
            //Tooltip
            pc_Item->setData(msn_USER_ROLE_TOOL_TIP_HEADING, c_DeviceName);
            pc_Item->setData(msn_USER_ROLE_TOOL_TIP_CONTENT, c_DeviceDescription);
         }
      }
   }

   // add a final spacer
   C_SebToolboxUtil::h_AddFinalSpacer(this->mpc_Ui->pc_VerticalLayout1, pc_List);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Fill toolbox with static content

   \created     12.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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
