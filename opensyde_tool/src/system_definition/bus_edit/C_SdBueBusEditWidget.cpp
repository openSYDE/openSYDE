//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus edit widget (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "constants.hpp"
#include "stwerrors.hpp"
#include "C_SdBueBusEditWidget.hpp"
#include "ui_C_SdBueBusEditWidget.h"

#include "TglUtils.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OscSystemBus.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SdBueBusEditWidget::hs32_TAB_INDEX_PROPERTIES = 0;
const int32_t C_SdBueBusEditWidget::hs32_TAB_INDEX_COMM = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_BusIndex    Bus index
   \param[in]      os32_TabIndex     Tab index to show
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueBusEditWidget::C_SdBueBusEditWidget(const uint32_t ou32_BusIndex, const int32_t os32_TabIndex,
                                           QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueBusEditWidget),
   mu32_BusIndex(ou32_BusIndex),
   mq_DataChanged(false),
   mq_SkipLoadUserSettings(true),
   mpc_BusPropertiesWidget(NULL),
   mpc_WidgetComIfDescr(NULL)
{
   const stw::opensyde_core::C_OscSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOscBus(
      this->mu32_BusIndex);

   mpc_Ui->setupUi(this);

   InitStaticNames();

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_PROPERTIES);

   this->me_BusType = pc_Bus->e_Type;
   if (this->me_BusType == stw::opensyde_core::C_OscSystemBus::eCAN)
   {
      // show the initial tab
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(os32_TabIndex);
      this->m_CreateTabWidgetsAlways(os32_TabIndex, false);
      this->m_TabChanged(os32_TabIndex);
   }
   else
   {
      // Ethernet is not supported for the C_SdBueComIfDescriptionWidget yet
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_PROPERTIES);
      this->m_CreateTabWidgetsAlways(hs32_TAB_INDEX_PROPERTIES, false);
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_TabWidgetPageNavi->removeTab(hs32_TAB_INDEX_COMM);

      stw::opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TabWidgetPageNavi->tabBar(),
                                                                      "OnlyOneItem",
                                                                      true);
   }

   // Connect after setCurrentIndex. The signal will not be sent if the index is not changed, but we need the call
   // in both cases
   connect(this->mpc_Ui->pc_TabWidgetPageNavi, &stw::opensyde_gui_elements::C_OgeTawPageNavi::currentChanged,
           this, &C_SdBueBusEditWidget::m_TabChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(0, C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(1, C_GtGetText::h_GetText("COMM Messages"));

   //Tool tips
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the actual tab index

   \return
   Tab index
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SdBueBusEditWidget::GetTabIndex(void) const
{
   return this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Forward call to import messages from DBC, EDS or DCF file to this bus.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::ImportMessages(void)
{
   // Change to COMM tab
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_COMM);
   tgl_assert(this->mpc_WidgetComIfDescr != NULL);
   if (this->mpc_WidgetComIfDescr != NULL)
   {
      this->mpc_WidgetComIfDescr->ImportMessages();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueBusEditWidget::~C_SdBueBusEditWidget()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the save state of the node. Resets the data changed flag.

   \return
   true     Node was changed
   false    Node was not changed
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueBusEditWidget::WasChanged(void) const
{
   return this->mq_DataChanged;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Saves the actual data into the core but not in the files
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175} intentionally no functionality in this implementation
void C_SdBueBusEditWidget::Save(void) const
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates specific functionality

   mu32_FLAG_EDIT_NAME                    Opens the properties and selects the name for editing
   mu32_FLAG_OPEN_PROPERTIES              Opens the properties
   mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR   Opens the COM interface description

   \param[in]  ou32_Flag   Flag for specific functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::SetFlag(const uint32_t ou32_Flag) const
{
   if ((ou32_Flag == mu32_FLAG_EDIT_NAME) ||
       (ou32_Flag == mu32_FLAG_OPEN_PROPERTIES))
   {
      // open the properties
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_PROPERTIES);

      if (ou32_Flag == mu32_FLAG_EDIT_NAME)
      {
         tgl_assert(this->mpc_BusPropertiesWidget != NULL);
         if (this->mpc_BusPropertiesWidget != NULL)
         {
            this->mpc_BusPropertiesWidget->SelectName();
         }
      }
   }
   else if (ou32_Flag == mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR)
   {
      // open the COM interface description
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_COMM);
   }
   else if ((ou32_Flag & mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR_PROTOCOL) ==
            mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR_PROTOCOL)
   {
      // open the COM interface description with a specific protocol
      // Special case: the protocol type to navigate to in the tabs of the COMM messages is part of the flag
      const C_OscCanProtocol::E_Type e_ProtocolType =
         static_cast<C_OscCanProtocol::E_Type>(ou32_Flag & (~mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR_PROTOCOL));
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_COMM);
      tgl_assert(this->mpc_WidgetComIfDescr != NULL);
      if (this->mpc_WidgetComIfDescr != NULL)
      {
         this->mpc_WidgetComIfDescr->SetProtocol(e_ProtocolType);
      }
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to open a concrete datapool, datapool list or dataelement or the COM interface description

   \param[in]  os32_NodeIndex       Node index (if not used set to -1)
   \param[in]  os32_DataPoolIndex   Datapool index (if not used set to -1)
   \param[in]  os32_ListIndex       Optional list index (if not used set to -1)
   \param[in]  os32_ElementIndex    Optional element index (if not used set to -1)
                                    Index depends of value of os32_Flag
   \param[in]  os32_Flag            Optional flag for further information
                                    0: os32_ElementIndex is index of datapool data element without CAN reference
                                    1: os32_ElementIndex is index of datapool data element with associated CAN signal
                                    2: os32_ElementIndex is index of CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::OpenDetail(const int32_t os32_NodeIndex, const int32_t os32_DataPoolIndex,
                                      const int32_t os32_ListIndex, const int32_t os32_ElementIndex,
                                      const int32_t os32_Flag) const
{
   if (this->me_BusType == stw::opensyde_core::C_OscSystemBus::eCAN)
   {
      // open the interface description widget
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hs32_TAB_INDEX_COMM);
      tgl_assert(this->mpc_WidgetComIfDescr != NULL);
      if (this->mpc_WidgetComIfDescr != NULL)
      {
         if (os32_Flag == 1)
         {
            this->mpc_WidgetComIfDescr->SelectSignalSearch(os32_NodeIndex, static_cast<uint32_t>(os32_DataPoolIndex),
                                                           static_cast<uint32_t>(os32_ListIndex),
                                                           static_cast<uint32_t>(os32_ElementIndex));
         }
         else if (os32_Flag == 2)
         {
            this->mpc_WidgetComIfDescr->SelectMessageSearch(os32_NodeIndex, static_cast<uint32_t>(os32_DataPoolIndex),
                                                            static_cast<uint32_t>(os32_ListIndex),
                                                            static_cast<uint32_t>(os32_ElementIndex));
         }
         else
         {
            // nothing to do
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::m_DataChanged(void)
{
   this->mq_DataChanged = true;
   Q_EMIT this->SigChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for tab change of pc_TabWidgetPageNavi

   The not shown widget in the other tab cause a resize of the tab widget itself.
   This function sets the size policy of all not shown widgets to ignore to achieve that the hided but visible
   widgets have no impact on the current size of the tab widget.
   The changed widget of the current tab must be reseted to the preferred size

   \param[in]  os32_Index   Index of selected tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::m_TabChanged(const int32_t os32_Index)
{
   int32_t s32_Counter;

   m_CreateTabWidgetsAlways(os32_Index, true);

   if (this->mpc_WidgetComIfDescr != NULL)
   {
      //Save
      this->mpc_WidgetComIfDescr->TriggerSaveOfSplitterUserSettings();
   }

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

   if (os32_Index == hs32_TAB_INDEX_COMM)
   {
      tgl_assert(this->mpc_WidgetComIfDescr != NULL);
      if (this->mpc_WidgetComIfDescr != NULL)
      {
         this->mpc_WidgetComIfDescr->SetInitialFocus();
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
void C_SdBueBusEditWidget::m_CreateTabWidgetsAlways(const int32_t os32_Index, const bool oq_AdaptCursor)
{
   if (os32_Index == hs32_TAB_INDEX_PROPERTIES)
   {
      m_CreatePropertiesTab(oq_AdaptCursor);
   }
   else if (os32_Index == hs32_TAB_INDEX_COMM)
   {
      m_CreateCommTab(oq_AdaptCursor);
   }
   else
   {
      //Nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create properties tab content

   \param[in]  oq_AdaptCursor    Adapt cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::m_CreatePropertiesTab(const bool oq_AdaptCursor)
{
   if (this->mpc_BusPropertiesWidget == NULL)
   {
      if (oq_AdaptCursor)
      {
         C_SdBueBusEditWidget::mh_StartWaitingCursor();
      }
      this->mpc_BusPropertiesWidget = new C_SdBueBusEditPropertiesWidget();

      this->mpc_BusPropertiesWidget->SetBusId(this->mu32_BusIndex);

      connect(this->mpc_BusPropertiesWidget, &C_SdBueBusEditPropertiesWidget::SigChanged,
              this, &C_SdBueBusEditWidget::m_DataChanged);
      connect(this->mpc_BusPropertiesWidget, &C_SdBueBusEditPropertiesWidget::SigNameChanged,
              this, &C_SdBueBusEditWidget::SigNameChanged);

      this->mpc_Ui->pc_TabPropertiesLayout->addWidget(this->mpc_BusPropertiesWidget);
      if (oq_AdaptCursor)
      {
         C_SdBueBusEditWidget::mh_EndWaitingCursor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create comm tab content

   \param[in]  oq_AdaptCursor    Adapt cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::m_CreateCommTab(const bool oq_AdaptCursor)
{
   if (this->mpc_WidgetComIfDescr == NULL)
   {
      if (oq_AdaptCursor)
      {
         C_SdBueBusEditWidget::mh_StartWaitingCursor();
      }
      this->mpc_WidgetComIfDescr = new C_SdBueComIfDescriptionWidget();
      this->mpc_WidgetComIfDescr->SetBusId(this->mu32_BusIndex);

      // connecting to signals
      connect(this->mpc_WidgetComIfDescr, &C_SdBueComIfDescriptionWidget::SigChanged,
              this, &C_SdBueBusEditWidget::m_DataChanged);
      connect(this->mpc_WidgetComIfDescr, &C_SdBueComIfDescriptionWidget::SigErrorChange,
              this, &C_SdBueBusEditWidget::SigErrorChange);
      connect(this->mpc_WidgetComIfDescr, &C_SdBueComIfDescriptionWidget::SigSwitchToCoManager,
              this, &C_SdBueBusEditWidget::SigSwitchToCoManager);

      this->mpc_Ui->pc_TabCommMessagesLayout->addWidget(this->mpc_WidgetComIfDescr);
      if (this->mq_SkipLoadUserSettings == false)
      {
         this->mpc_WidgetComIfDescr->TriggerLoadOfSplitterUserSettings();
      }
      if (oq_AdaptCursor)
      {
         C_SdBueBusEditWidget::mh_EndWaitingCursor();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Start waiting cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::mh_StartWaitingCursor()
{
   QApplication::setOverrideCursor(Qt::WaitCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  End waiting cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::mh_EndWaitingCursor()
{
   QApplication::restoreOverrideCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::showEvent(QShowEvent * const opc_Event)
{
   //Necessary to initialize with appropriate size
   if (this->mpc_WidgetComIfDescr != NULL)
   {
      this->mpc_WidgetComIfDescr->TriggerLoadOfSplitterUserSettings();
   }
   QWidget::showEvent(opc_Event);
}
