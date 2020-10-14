//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus edit widget (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "constants.h"
#include "stwerrors.h"
#include "C_SdBueBusEditWidget.h"
#include "ui_C_SdBueBusEditWidget.h"

#include "C_PuiSdHandler.h"
#include "C_OgeWiUtil.h"
#include "C_OSCSystemBus.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::sintn C_SdBueBusEditWidget::hsn_TabIndexProperties = 0;
const stw_types::sintn C_SdBueBusEditWidget::hsn_TabIndexComm = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_BusIndex     Bus index
   \param[in]     osn_TabIndex      Tab index to show
   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueBusEditWidget::C_SdBueBusEditWidget(const uint32 ou32_BusIndex, const sintn osn_TabIndex,
                                           QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueBusEditWidget),
   mu32_BusIndex(ou32_BusIndex),
   mq_DataChanged(false)
{
   const stw_opensyde_core::C_OSCSystemBus * const pc_Bus = C_PuiSdHandler::h_GetInstance()->GetOSCBus(
      this->mu32_BusIndex);

   mpc_Ui->setupUi(this);

   InitStaticNames();

   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexProperties);

   this->mpc_Ui->pc_BusPropertiesWidget->SetBusId(this->mu32_BusIndex);

   connect(this->mpc_Ui->pc_BusPropertiesWidget, &C_SdBueBusEditPropertiesWidget::SigChanged,
           this, &C_SdBueBusEditWidget::m_DataChanged);
   connect(this->mpc_Ui->pc_BusPropertiesWidget, &C_SdBueBusEditPropertiesWidget::SigNameChanged,
           this, &C_SdBueBusEditWidget::SigNameChanged);

   this->me_BusType = pc_Bus->e_Type;
   if (this->me_BusType == stw_opensyde_core::C_OSCSystemBus::eCAN)
   {
      this->mpc_Ui->pc_WidgetComIfDescr->SetBusId(this->mu32_BusIndex);

      // connecting to signals
      connect(this->mpc_Ui->pc_WidgetComIfDescr, &C_SdBueComIfDescriptionWidget::SigChanged,
              this, &C_SdBueBusEditWidget::m_DataChanged);
      connect(this->mpc_Ui->pc_WidgetComIfDescr, &C_SdBueComIfDescriptionWidget::SigErrorChange,
              this, &C_SdBueBusEditWidget::SigErrorChange);

      // show the initial tab
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(osn_TabIndex);
      this->m_TabChanged(osn_TabIndex);
   }
   else
   {
      // Ethernet is not supported for the C_SdBueComIfDescriptionWidget yet
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexProperties);
      //lint -e{1938}  static const is guaranteed preinitialized before main
      this->mpc_Ui->pc_TabWidgetPageNavi->removeTab(hsn_TabIndexComm);

      stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TabWidgetPageNavi->tabBar(),
                                                                     "OnlyOneItem",
                                                                     true);
   }

   // Connect after setCurrentIndex. The signal will not be sent if the index is not changed, but we need the call
   // in both cases
   connect(this->mpc_Ui->pc_TabWidgetPageNavi, &stw_opensyde_gui_elements::C_OgeTawPageNavi::currentChanged,
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
sintn C_SdBueBusEditWidget::GetTabIndex(void) const
{
   return this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Forward call to import messages from DBC, EDS or DCF file to this bus.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::ImportMessages(void) const
{
   this->mpc_Ui->pc_WidgetComIfDescr->ImportMessages();
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
void C_SdBueBusEditWidget::Save(void) const
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates specific functionality

   mu32_FLAG_EDIT_NAME                    Opens the properties and selects the name for editing
   mu32_FLAG_OPEN_PROPERTIES              Opens the properties
   mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR   Opens the COM interface description

   \param[in]     ou32_Flag       Flag for specific functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::SetFlag(const uint32 ou32_Flag) const
{
   if ((ou32_Flag == mu32_FLAG_EDIT_NAME) ||
       (ou32_Flag == mu32_FLAG_OPEN_PROPERTIES))
   {
      // open the properties
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexProperties);

      if (ou32_Flag == mu32_FLAG_EDIT_NAME)
      {
         this->mpc_Ui->pc_BusPropertiesWidget->SelectName();
      }
   }
   else if (ou32_Flag == mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR)
   {
      // open the COM interface description
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexComm);
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to open a concrete datapool, datapool list or dataelement or the COM interface description

   \param[in] os32_NodeIndex          Node index (if not used set to -1)
   \param[in] os32_DataPoolIndex      Datapool index (if not used set to -1)
   \param[in] os32_ListIndex          Optional list index (if not used set to -1)
   \param[in] os32_ElementIndex       Optional element index (if not used set to -1)
                                      Index depends of value of os32_Flag
   \param[in] os32_Flag               Optional flag for further information
                                      0: os32_ElementIndex is index of datapool data element without CAN reference
                                      1: os32_ElementIndex is index of datapool data element with associated CAN signal
                                      2: os32_ElementIndex is index of CAN message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::OpenDetail(const sint32 os32_NodeIndex, const sint32 os32_DataPoolIndex,
                                      const sint32 os32_ListIndex, const sint32 os32_ElementIndex,
                                      const sint32 os32_Flag) const
{
   if (this->me_BusType == stw_opensyde_core::C_OSCSystemBus::eCAN)
   {
      // open the interface description widget
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexComm);

      if (os32_Flag == 1)
      {
         this->mpc_Ui->pc_WidgetComIfDescr->SelectSignalSearch(os32_NodeIndex, static_cast<uint32>(os32_DataPoolIndex),
                                                               static_cast<uint32>(os32_ListIndex),
                                                               static_cast<uint32>(os32_ElementIndex));
      }
      else if (os32_Flag == 2)
      {
         this->mpc_Ui->pc_WidgetComIfDescr->SelectMessageSearch(os32_NodeIndex, static_cast<uint32>(os32_DataPoolIndex),
                                                                static_cast<uint32>(os32_ListIndex),
                                                                static_cast<uint32>(os32_ElementIndex));
      }
      else
      {
         // nothing to do
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

   \param[in]     osn_Index         Index of selected tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::m_TabChanged(const sintn osn_Index) const
{
   sintn sn_Counter;

   if (osn_Index == 2L)
   {
      //Restore
      this->mpc_Ui->pc_WidgetComIfDescr->TriggerLoadOfSplitterUserSettings();
   }
   else
   {
      //Save
      this->mpc_Ui->pc_WidgetComIfDescr->TriggerSaveOfSplitterUserSettings();
   }

   for (sn_Counter = 0; sn_Counter < this->mpc_Ui->pc_TabWidgetPageNavi->count(); ++sn_Counter)
   {
      if (sn_Counter != osn_Index)
      {
         this->mpc_Ui->pc_TabWidgetPageNavi->widget(sn_Counter)->setSizePolicy(QSizePolicy::Ignored,
                                                                               QSizePolicy::Ignored);
      }
   }
   this->mpc_Ui->pc_TabWidgetPageNavi->widget(osn_Index)->setSizePolicy(QSizePolicy::Preferred,
                                                                        QSizePolicy::Preferred);
   this->mpc_Ui->pc_TabWidgetPageNavi->widget(osn_Index)->adjustSize();

   if (osn_Index == hsn_TabIndexComm)
   {
      this->mpc_Ui->pc_WidgetComIfDescr->SetInitialFocus();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueBusEditWidget::showEvent(QShowEvent * const opc_Event)
{
   //Necessary to initialize with appropriate size
   this->mpc_Ui->pc_WidgetComIfDescr->TriggerLoadOfSplitterUserSettings();
   QWidget::showEvent(opc_Event);
}
