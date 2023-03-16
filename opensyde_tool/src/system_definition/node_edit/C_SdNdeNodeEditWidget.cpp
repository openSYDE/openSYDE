//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for node edit

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QVBoxLayout>

#include "C_PuiSdUtil.hpp"
#include "constants.hpp"
#include "C_SdNdeNodeEditWidget.hpp"
#include "ui_C_SdNdeNodeEditWidget.h"
#include "C_HeHandler.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscNode.hpp"

#include "C_UsHandler.hpp"
#include "C_GtGetText.hpp"
#include "C_OgeWiUtil.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SdNdeNodeEditWidget::hs32_TAB_INDEX_PROPERTIES = 0;
const int32_t C_SdNdeNodeEditWidget::hs32_TAB_INDEX_DATA_POOL = 1;
const int32_t C_SdNdeNodeEditWidget::hs32_TAB_INDEX_COMM = 2;
const int32_t C_SdNdeNodeEditWidget::hs32_TAB_INDEX_CO_MANAGER = 3;
const int32_t C_SdNdeNodeEditWidget::hs32_TAB_INDEX_HALC = 4;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_NodeIndex   Node index
   \param[in]      os32_TabIndex     Tab index to show
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeNodeEditWidget::C_SdNdeNodeEditWidget(const uint32_t ou32_NodeIndex, const int32_t os32_TabIndex,
                                             QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeNodeEditWidget),
   mu32_NodeIndex(ou32_NodeIndex),
   mq_DataChanged(false),
   mq_SkipLoadUserSettings(true),
   mpc_PropertiesWidget(NULL),
   mpc_DataPoolEditWidget(NULL),
   mpc_ComIfDescriptionWidget(NULL),
   mpc_HalWidget(NULL),
   mpc_CoManagerWidget(NULL)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();

   // connecting to signals
   connect(this->mpc_Ui->pc_TabWidgetPageNavi, &stw::opensyde_gui_elements::C_OgeTawPageNavi::currentChanged,
           this, &C_SdNdeNodeEditWidget::m_CurrentTabChanged);
   connect(this->mpc_Ui->pc_TabWidgetPageNavi, &stw::opensyde_gui_elements::C_OgeTawPageNavi::tabBarClicked,
           this, &C_SdNdeNodeEditWidget::m_TabClicked);

   // show the initial tab
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(os32_TabIndex);
   m_CurrentTabChanged(os32_TabIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeNodeEditWidget::~C_SdNdeNodeEditWidget()
{
   //Store splitter position
   this->m_SaveUserSettings();

   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hs32_TAB_INDEX_PROPERTIES, C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hs32_TAB_INDEX_DATA_POOL, C_GtGetText::h_GetText("Datapools"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hs32_TAB_INDEX_COMM, C_GtGetText::h_GetText("COMM Messages"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hs32_TAB_INDEX_CO_MANAGER, C_GtGetText::h_GetText("CANopen Manager"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hs32_TAB_INDEX_HALC, C_GtGetText::h_GetText("Hardware Configurator"));

   //Tool tips
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the save state of the node. Resets the data changed flag.

   \return
   true     Node was changed
   false    Node was not changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeNodeEditWidget::WasChanged(void) const
{
   return this->mq_DataChanged;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Saves the actual data into the core but not in the files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::Save(void) const
{
   if (this->mpc_PropertiesWidget != NULL)
   {
      this->mpc_PropertiesWidget->SaveToData();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates specific functionality

   mu32_FLAG_EDIT_NAME           Opens the properties and selects the name for editing
   mu32_FLAG_OPEN_PROPERTIES     Opens the properties

   \param[in]  ou32_Flag   Flag for specific functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::SetFlag(const uint32_t ou32_Flag) const
{
   if ((ou32_Flag == mu32_FLAG_EDIT_NAME) ||
       (ou32_Flag == mu32_FLAG_OPEN_PROPERTIES))
   {
      // open the properties
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_PROPERTIES);

      if (ou32_Flag == mu32_FLAG_EDIT_NAME)
      {
         tgl_assert(this->mpc_PropertiesWidget != NULL);
         if (this->mpc_PropertiesWidget != NULL)
         {
            this->mpc_PropertiesWidget->SelectName();
         }
      }
   }
   else if ((ou32_Flag & mu32_FLAG_OPEN_SYSDEF_CANOPENMANAGER) ==
            mu32_FLAG_OPEN_SYSDEF_CANOPENMANAGER)
   {
      // open the CANopen Manager with navigation to one of its manager interface configurations
      // Special case: the interface number of the manager to navigate to in the tree of the manager is part of the flag
      const uint8_t u8_InterfaceNumber = static_cast<uint8_t>(ou32_Flag & (~mu32_FLAG_OPEN_SYSDEF_CANOPENMANAGER));
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_CO_MANAGER);
      tgl_assert(this->mpc_CoManagerWidget != NULL);
      if (this->mpc_CoManagerWidget != NULL)
      {
         this->mpc_CoManagerWidget->OpenManagerConfiguration(u8_InterfaceNumber);
      }
   }
   else if ((ou32_Flag & mu32_FLAG_OPEN_SYSDEF_DEVICENODE_IN_CANOPENMANAGER) ==
            mu32_FLAG_OPEN_SYSDEF_DEVICENODE_IN_CANOPENMANAGER)
   {
      // open the CANopen Manager with navigation to one of its device configurations
      // Special case: the node id of the device to navigate to in the tree of the manager is part of the flag
      const uint32_t u32_DeviceNodeIndex = ou32_Flag & (~mu32_FLAG_OPEN_SYSDEF_DEVICENODE_IN_CANOPENMANAGER);
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_CO_MANAGER);
      tgl_assert(this->mpc_CoManagerWidget != NULL);
      if (this->mpc_CoManagerWidget != NULL)
      {
         this->mpc_CoManagerWidget->OpenDeviceConfiguration(u32_DeviceNodeIndex);
      }
   }
   else
   {
      // Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to open a concrete datapool, datapool list or dataelement

   \param[in]  os32_MainIndex       Datapool index (os32_Flag is 0,1,2) or Application index (os32_Flag is 3) or
                                    HALC domain index (os32_Flag is 4)
   \param[in]  os32_ListIndex       Optional list index (if not used set to -1) or optional HALC channel
                                    index (os32_Flag is 4)
   \param[in]  os32_ElementIndex    Optional element index (if not used set to -1)
                                    Index depends of value of os32_Flag
   \param[in]  os32_Flag            Optional flag for further information
                                    0: os32_ElementIndex is index of datapool data element without CAN reference
                                    1: os32_ElementIndex is index of datapool data element with associated CAN signal
                                    2: os32_ElementIndex is index of CAN message
                                    3: os32_MainIndex is index of application / data block
                                    4: os32_MainIndex is index of HALC domain and os32_ListIndex is index of
                                    HALC channel of domain (optional)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::OpenDetail(const int32_t os32_MainIndex, const int32_t os32_ListIndex,
                                       const int32_t os32_ElementIndex, const int32_t os32_Flag)
{
   if (os32_Flag == 0)
   {
      // open the datapool
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_DATA_POOL);
      tgl_assert(this->mpc_DataPoolEditWidget != NULL);
      if (this->mpc_DataPoolEditWidget != NULL)
      {
         this->mpc_DataPoolEditWidget->OpenDetail(os32_MainIndex, os32_ListIndex, os32_ElementIndex);
      }
   }
   else if ((os32_Flag == 1) ||
            (os32_Flag == 2))
   {
      // open the interface description widget
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_COMM);
      tgl_assert(this->mpc_ComIfDescriptionWidget != NULL);
      if (this->mpc_ComIfDescriptionWidget != NULL)
      {
         if (os32_Flag == 1)
         {
            this->mpc_ComIfDescriptionWidget->SelectSignalSearch(this->mu32_NodeIndex,
                                                                 static_cast<uint32_t>(os32_MainIndex),
                                                                 static_cast<uint32_t>(os32_ListIndex),
                                                                 static_cast<uint32_t>(os32_ElementIndex));
         }
         else if (os32_Flag == 2)
         {
            this->mpc_ComIfDescriptionWidget->SelectMessageSearch(this->mu32_NodeIndex,
                                                                  static_cast<uint32_t>(os32_MainIndex),
                                                                  static_cast<uint32_t>(os32_ListIndex),
                                                                  static_cast<uint32_t>(os32_ElementIndex));
         }
         else
         {
            // nothing to do
         }
      }
   }
   else if (os32_Flag == 3)
   {
      // show the application / data block
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_PROPERTIES);
      tgl_assert(this->mpc_PropertiesWidget != NULL);
      if (this->mpc_PropertiesWidget != NULL)
      {
         this->mpc_PropertiesWidget->ShowApplication(os32_MainIndex);
      }
   }
   else if (os32_Flag == 4)
   {
      // show HALC
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_HALC);
      tgl_assert(this->mpc_HalWidget != NULL);
      if (this->mpc_HalWidget != NULL)
      {
         this->mpc_HalWidget->ShowChannel(os32_MainIndex, os32_ListIndex);
      }
   }
   else if (os32_Flag == 5)
   {
      // show CANopen Manager
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_CO_MANAGER);
      tgl_assert(this->mpc_CoManagerWidget != NULL);
      if (this->mpc_CoManagerWidget != NULL)
      {
         // TODO: Handle CANopen Manager
      }
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Wrapper to call C_SdNdeDbViewWidget::AddFromTsp()
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::AddFromTsp(void)
{
   //Widget necessary
   m_CreatePropertiesTab(true);
   tgl_assert(this->mpc_PropertiesWidget != NULL);
   if (this->mpc_PropertiesWidget != NULL)
   {
      this->mpc_PropertiesWidget->AddFromTsp();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the actual tab index

   \return
   Tab index
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdNdeNodeEditWidget::GetTabIndex(void) const
{
   return this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::showEvent(QShowEvent * const opc_Event)
{
   this->mq_SkipLoadUserSettings = false;
   this->m_LoadUserSettings();
   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hide event slot

   Here: Save splitter position

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::hideEvent(QHideEvent * const opc_Event)
{
   this->m_SaveUserSettings();
   QWidget::hideEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_LoadUserSettings() const
{
   if (this->mpc_PropertiesWidget != NULL)
   {
      this->mpc_PropertiesWidget->LoadUserSettings();
   }
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(C_UsHandler::h_GetInstance()->GetProjLastSysDefNodeTabIndex());

   if (this->mpc_HalWidget != NULL)
   {
      this->mpc_HalWidget->LoadUserSettings();
   }
   if (this->mpc_ComIfDescriptionWidget != NULL)
   {
      this->mpc_ComIfDescriptionWidget->LoadUserSettings();
   }
   if (this->mpc_CoManagerWidget != NULL)
   {
      this->mpc_CoManagerWidget->LoadUserSettings();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_SaveUserSettings() const
{
   if (this->mpc_PropertiesWidget != NULL)
   {
      this->mpc_PropertiesWidget->SaveUserSettings();
   }
   C_UsHandler::h_GetInstance()->SetProjLastSysDefNodeTabIndex(this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex());
   if (this->mpc_HalWidget != NULL)
   {
      this->mpc_HalWidget->SaveUserSettings();
   }
   if (this->mpc_ComIfDescriptionWidget != NULL)
   {
      this->mpc_ComIfDescriptionWidget->SaveUserSettings();
   }
   if (this->mpc_CoManagerWidget != NULL)
   {
      this->mpc_CoManagerWidget->SaveUserSettings();
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_DataChanged(void)
{
   this->mq_DataChanged = true;
   Q_EMIT (this->SigChanged());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal

   \param[in]  oru32_BusIndex    Bus index
   \param[in]  orc_BusName       Bus name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OnSwitchToBus(const uint32_t & oru32_BusIndex, const QString & orc_BusName)
{
   Q_EMIT (this->SigSwitchToBus(oru32_BusIndex, orc_BusName));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open the HALC tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OnSwitchToHalc(void)
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_HALC);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open the COMM messages tab

   \param[in]  ou32_DataPoolIndex   Index of COMM Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OnSwitchToCommMessages(const uint32_t ou32_DataPoolIndex)
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_COMM);
   tgl_assert(this->mpc_ComIfDescriptionWidget != NULL);
   if (this->mpc_ComIfDescriptionWidget != NULL)
   {
      this->mpc_ComIfDescriptionWidget->SetProtocolByDataPool(ou32_DataPoolIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open the CANopen Manager tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OnSwitchToCanOpenManager(void)
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_CO_MANAGER);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for tab change of pc_TabWidgetPageNavi

   Adapt the visibility of the tab options

   The not shown widget in the other tab cause a resize of the tab widget itself.
   This function sets the size policy of all not shown widgets to ignore to achieve that the hided but visible
   widgets have no impact on the current size of the tab widget.
   The changed widget of the current tab must be reseted to the preferred size

   \param[in]  os32_Index   Index of selected tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CurrentTabChanged(const int32_t os32_Index)
{
   int32_t s32_Counter;

   m_CreateTabWidgetsAlways(os32_Index, true);

   //Simple trigger update
   if (this->mpc_PropertiesWidget != NULL)
   {
      this->mpc_PropertiesWidget->UpdateApplications();
   }

   // Adjust the size of the hided but visible widgets of the other tabs
   for (s32_Counter = 0; s32_Counter < this->mpc_Ui->pc_TabWidgetPageNavi->count(); ++s32_Counter)
   {
      if (s32_Counter != os32_Index)
      {
         this->mpc_Ui->pc_TabWidgetPageNavi->widget(s32_Counter)->setSizePolicy(QSizePolicy::Ignored,
                                                                                QSizePolicy::Ignored);
      }
   }
   this->mpc_Ui->pc_TabWidgetPageNavi->widget(os32_Index)->setSizePolicy(QSizePolicy::Preferred,
                                                                         QSizePolicy::Preferred);
   this->mpc_Ui->pc_TabWidgetPageNavi->widget(os32_Index)->adjustSize();

   // The previous call of adjustSize causes some times to a second size change of pc_DataPoolEditWidget
   // This can cause a wrong size of the widget. A further call of adjustSize seems to repair the problem
   // The origin reason why the widget is resized two times with different sizes is unknown
   if (this->mpc_DataPoolEditWidget != NULL)
   {
      this->mpc_DataPoolEditWidget->adjustSize();
   }

   if (os32_Index == hs32_TAB_INDEX_COMM)
   {
      tgl_assert(this->mpc_ComIfDescriptionWidget != NULL);
      if (this->mpc_ComIfDescriptionWidget != NULL)
      {
         this->mpc_ComIfDescriptionWidget->SetInitialFocus();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for tab click of pc_TabWidgetPageNavi

   Navigate back to Datapool overview if a specific NVM or DIAG Datapool content was opened

   \param[in]  os32_Index   Index of selected tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_TabClicked(const int32_t os32_Index) const
{
   if ((this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex() == hs32_TAB_INDEX_DATA_POOL) &&
       (os32_Index == hs32_TAB_INDEX_DATA_POOL))
   {
      // Only relevant if no other tab than the Datapool tab was clicked and no other tab was selected before
      tgl_assert(this->mpc_DataPoolEditWidget != NULL);
      if (this->mpc_DataPoolEditWidget != NULL)
      {
         this->mpc_DataPoolEditWidget->OpenOverview();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for tab change of pc_TabWidgetPageNavi (including inital call)

   Create widgets, if necessary

   \param[in]  os32_Index         Index of selected tab
   \param[in]  oq_AdaptCursor    Adapt cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CreateTabWidgetsAlways(const int32_t os32_Index, const bool oq_AdaptCursor)
{
   if (os32_Index == hs32_TAB_INDEX_PROPERTIES)
   {
      m_CreatePropertiesTab(oq_AdaptCursor);
   }
   else if (os32_Index == hs32_TAB_INDEX_DATA_POOL)
   {
      m_CreateDpTab(oq_AdaptCursor);
   }
   else if (os32_Index == hs32_TAB_INDEX_COMM)
   {
      m_CreateCommTab(oq_AdaptCursor);
   }
   else if (os32_Index == hs32_TAB_INDEX_HALC)
   {
      m_CreateHalTab(oq_AdaptCursor);
   }
   else if (os32_Index == hs32_TAB_INDEX_CO_MANAGER)
   {
      m_CreateCoManagerTab(oq_AdaptCursor);
   }
   else
   {
      //Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for DataPool changed signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OnDpChanged(void)
{
   this->m_ReloadCommMessages();
   this->m_ReloadHalc();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger reload of data pools tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_ReloadDataPools(void) const
{
   if (this->mpc_DataPoolEditWidget != NULL)
   {
      this->mpc_DataPoolEditWidget->SetNode(this->mu32_NodeIndex);
   }
   if (this->mpc_PropertiesWidget != NULL)
   {
      this->mpc_PropertiesWidget->UpdateApplications();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger reload of CANopen Manager tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_ReloadCanOpenConfig(void) const
{
   if (this->mpc_CoManagerWidget != NULL)
   {
      this->mpc_CoManagerWidget->Refresh();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger entire reload of COMM messages

   Necessary if Datapool order or existence has changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_ReloadCommMessages(void) const
{
   if (this->mpc_ComIfDescriptionWidget != NULL)
   {
      this->mpc_ComIfDescriptionWidget->SetNodeId(this->mu32_NodeIndex,
                                                  this->mpc_ComIfDescriptionWidget->GetActProtocol());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reload COMM datapools
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_ReloadCommDatapools() const
{
   this->m_ReloadDataPools();
   this->m_ReloadCommMessages();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger entire reload of HALC

   Necessary if Datapool assignment changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_ReloadHalc(void)
{
   if (this->mpc_HalWidget != NULL)
   {
      this->mpc_HalWidget->SetNode(this->mu32_NodeIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  runs the HALC magician, reloads datapool Tab and HALC Tab Screen
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_HalcLoadedFromTsp(void) const
{
   // run magician
   const int32_t s32_Result = C_PuiSdHandler::h_GetInstance()->HalcGenerateDatapools(this->mu32_NodeIndex);

   tgl_assert((s32_Result == stw::errors::C_NO_ERR) || (s32_Result == stw::errors::C_NOACT));

   // assign HALC Datapools to Data Block if possible
   tgl_assert(C_PuiSdHandler::h_GetInstance()->AssignAllHalcNvmDataPools(this->mu32_NodeIndex) ==
              stw::errors::C_NO_ERR);

   // reload tabs to update GUI
   this->m_ReloadDataPools();
   if (this->mpc_HalWidget != NULL)
   {
      this->mpc_HalWidget->SetNode(this->mu32_NodeIndex);
   }
   if (this->mpc_PropertiesWidget != NULL)
   {
      this->mpc_PropertiesWidget->UpdateApplications();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create properties tab content

   \param[in]  oq_AdaptCursor    Adapt cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CreatePropertiesTab(const bool oq_AdaptCursor)
{
   if (this->mpc_PropertiesWidget == NULL)
   {
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_StartWaitingCursor();
      }
      this->mpc_PropertiesWidget = new C_SdNdeNodePropertiesTabContentWidget();

      connect(this->mpc_PropertiesWidget, &C_SdNdeNodePropertiesTabContentWidget::SigChanged,
              this, &C_SdNdeNodeEditWidget::m_DataChanged);
      connect(this->mpc_PropertiesWidget, &C_SdNdeNodePropertiesTabContentWidget::SigNameChanged,
              this, &C_SdNdeNodeEditWidget::SigNameChanged);
      connect(this->mpc_PropertiesWidget, &C_SdNdeNodePropertiesTabContentWidget::SigBusBitrateClicked,
              this, &C_SdNdeNodeEditWidget::SigSwitchToBusProperties);
      connect(this->mpc_PropertiesWidget, &C_SdNdeNodePropertiesTabContentWidget::SigNodePropChanged, this,
              &C_SdNdeNodeEditWidget::m_ReloadCanOpenConfig);
      connect(this->mpc_PropertiesWidget, &C_SdNdeNodePropertiesTabContentWidget::SigErrorChange, this,
              &C_SdNdeNodeEditWidget::SigErrorChange);
      connect(this->mpc_PropertiesWidget, &C_SdNdeNodePropertiesTabContentWidget::SigOwnedDataPoolsChanged, this,
              &C_SdNdeNodeEditWidget::m_ReloadDataPools);
      connect(this->mpc_PropertiesWidget, &C_SdNdeNodePropertiesTabContentWidget::SigHalcLoadedFromTsp, this,
              &C_SdNdeNodeEditWidget::m_HalcLoadedFromTsp);

      this->mpc_PropertiesWidget->SetNodeIndex(this->mu32_NodeIndex);
      this->mpc_Ui->pc_TabPropertiesLayout->addWidget(this->mpc_PropertiesWidget);
      if (this->mq_SkipLoadUserSettings == false)
      {
         this->mpc_PropertiesWidget->LoadUserSettings();
      }
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_EndWaitingCursor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create dp tab content

   \param[in]  oq_AdaptCursor    Adapt cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CreateDpTab(const bool oq_AdaptCursor)
{
   if (this->mpc_DataPoolEditWidget == NULL)
   {
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_StartWaitingCursor();
      }
      this->mpc_DataPoolEditWidget = new C_SdNdeDpEditWidget();
      this->mpc_DataPoolEditWidget->SetNode(this->mu32_NodeIndex);

      connect(this->mpc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigChanged,
              this, &C_SdNdeNodeEditWidget::m_DataChanged);
      connect(this->mpc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigDataPoolsChanged,
              this, &C_SdNdeNodeEditWidget::m_OnDpChanged);
      connect(this->mpc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSwitchToHalc,
              this, &C_SdNdeNodeEditWidget::m_OnSwitchToHalc);
      connect(this->mpc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSwitchToCanOpen,
              this, &C_SdNdeNodeEditWidget::m_OnSwitchToCanOpenManager);
      connect(this->mpc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSwitchToCommMessages,
              this, &C_SdNdeNodeEditWidget::m_OnSwitchToCommMessages);
      connect(this->mpc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSave,
              this, &C_SdNdeNodeEditWidget::SigSave);
      connect(this->mpc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSaveAs,
              this, &C_SdNdeNodeEditWidget::SigSaveAs);
      connect(this->mpc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigErrorChange,
              this, &C_SdNdeNodeEditWidget::SigErrorChange);

      this->mpc_Ui->pc_TabDatapoolsLayout->addWidget(this->mpc_DataPoolEditWidget);
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_EndWaitingCursor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create comm tab content

   \param[in]  oq_AdaptCursor    Adapt cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CreateCommTab(const bool oq_AdaptCursor)
{
   if (this->mpc_ComIfDescriptionWidget == NULL)
   {
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_StartWaitingCursor();
      }
      this->mpc_ComIfDescriptionWidget = new C_SdBueComIfDescriptionWidget();
      this->mpc_ComIfDescriptionWidget->SetNodeId(this->mu32_NodeIndex, C_OscCanProtocol::eLAYER2);

      connect(this->mpc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigChanged,
              this, &C_SdNdeNodeEditWidget::m_DataChanged);
      connect(this->mpc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigErrorChange,
              this, &C_SdNdeNodeEditWidget::SigErrorChange);
      connect(this->mpc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigSwitchToBusProtocol,
              this, &C_SdNdeNodeEditWidget::SigSwitchToBusProtocol);
      connect(this->mpc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigCommDataPoolAdded, this,
              &C_SdNdeNodeEditWidget::m_ReloadDataPools);
      connect(this->mpc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigErrorChange, this,
              &C_SdNdeNodeEditWidget::m_ReloadDataPools);

      this->mpc_Ui->pc_TabCommMessagesLayout->addWidget(this->mpc_ComIfDescriptionWidget);
      // Not loading the user settings here due to loading it in the showEvent of mpc_ComIfDescriptionWidget itself
      // Problems were occured when loading the user settings for this widget two times
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_EndWaitingCursor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create hal tab content

   \param[in]  oq_AdaptCursor    Adapt cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CreateHalTab(const bool oq_AdaptCursor)
{
   if (this->mpc_HalWidget == NULL)
   {
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_StartWaitingCursor();
      }
      this->mpc_HalWidget = new C_SdNdeHalcWidget();
      this->mpc_HalWidget->SetNode(this->mu32_NodeIndex);

      connect(mpc_HalWidget, &C_SdNdeHalcWidget::SigErrorChange, this,
              &C_SdNdeNodeEditWidget::SigErrorChange);
      connect(mpc_HalWidget, &C_SdNdeHalcWidget::SigHalcDataPoolChanged, this,
              &C_SdNdeNodeEditWidget::m_ReloadDataPools);

      this->mpc_Ui->pc_TabHalcLayout->addWidget(this->mpc_HalWidget);
      if (this->mq_SkipLoadUserSettings == false)
      {
         this->mpc_HalWidget->LoadUserSettings();
      }
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_EndWaitingCursor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create CANopen manager tab content

   \param[in]  oq_AdaptCursor    Adapt cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CreateCoManagerTab(const bool oq_AdaptCursor)
{
   if (this->mpc_CoManagerWidget == NULL)
   {
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_StartWaitingCursor();
      }
      this->mpc_CoManagerWidget = new C_SdNdeCoWidget();
      this->mpc_CoManagerWidget->SetNode(this->mu32_NodeIndex);

      connect(mpc_CoManagerWidget, &C_SdNdeCoWidget::SigErrorChange, this,
              &C_SdNdeNodeEditWidget::SigErrorChange);
      connect(mpc_CoManagerWidget, &C_SdNdeCoWidget::SigCommDatapoolsChanged, this,
              &C_SdNdeNodeEditWidget::m_ReloadCommDatapools);
      connect(this->mpc_CoManagerWidget, &C_SdNdeCoWidget::SigSwitchToDeviceNodeInCoManager,
              this, &C_SdNdeNodeEditWidget::SigSwitchToDeviceNodeInCoManager);
      connect(this->mpc_CoManagerWidget, &C_SdNdeCoWidget::SigSwitchToBusProtocol, this,
              &C_SdNdeNodeEditWidget::SigSwitchToBusProtocol);
      connect(this->mpc_CoManagerWidget, &C_SdNdeCoWidget::SigSwitchToBusProtocolMessage, this,
              &C_SdNdeNodeEditWidget::SigSwitchToBusProtocolMessage);

      this->mpc_Ui->pc_TabCoManagerLayout->addWidget(this->mpc_CoManagerWidget);
      if (this->mq_SkipLoadUserSettings == false)
      {
         this->mpc_CoManagerWidget->LoadUserSettings();
      }
      if (oq_AdaptCursor)
      {
         C_SdNdeNodeEditWidget::mh_EndWaitingCursor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start waiting cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::mh_StartWaitingCursor()
{
   QApplication::setOverrideCursor(Qt::WaitCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  End waiting cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::mh_EndWaitingCursor()
{
   QApplication::restoreOverrideCursor();
}
