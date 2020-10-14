//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for node edit

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QVBoxLayout>

#include "C_PuiSdUtil.h"
#include "constants.h"
#include "C_SdNdeNodeEditWidget.h"
#include "ui_C_SdNdeNodeEditWidget.h"
#include "C_HeHandler.h"
#include "C_PuiSdHandler.h"
#include "C_OSCNode.h"

#include "C_UsHandler.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const stw_types::sintn C_SdNdeNodeEditWidget::hsn_TabIndexProperties = 0;
const stw_types::sintn C_SdNdeNodeEditWidget::hsn_TabIndexDataPool = 1;
const stw_types::sintn C_SdNdeNodeEditWidget::hsn_TabIndexComm = 2;
const stw_types::sintn C_SdNdeNodeEditWidget::hsn_TabIndexHalc = 3;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_NodeIndex   Node index
   \param[in]      osn_TabIndex     Tab index to show
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeNodeEditWidget::C_SdNdeNodeEditWidget(const uint32 ou32_NodeIndex, const sintn osn_TabIndex,
                                             QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeNodeEditWidget),
   mu32_NodeIndex(ou32_NodeIndex),
   mq_DataChanged(false)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_WidgetApplications->SetNodeIndex(this->mu32_NodeIndex);

   InitStaticNames();

   // splitter
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 0);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 1);

   this->mpc_Ui->pc_NodePropWidget->SetNodeId(this->mu32_NodeIndex);
   this->mpc_Ui->pc_DataPoolEditWidget->SetNode(this->mu32_NodeIndex);
   this->mpc_Ui->pc_TabHalc->SetNode(this->mu32_NodeIndex);
   this->mpc_Ui->pc_ComIfDescriptionWidget->SetNodeId(this->mu32_NodeIndex, C_OSCCanProtocol::eLAYER2);

   // connecting to signals
   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigChanged,
           this, &C_SdNdeNodeEditWidget::m_DataChanged);
   connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigChanged,
           this, &C_SdNdeNodeEditWidget::m_DataChanged);
   connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigDataPoolsChanged,
           this, &C_SdNdeNodeEditWidget::m_ReloadCommMessages);
   connect(this->mpc_Ui->pc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigChanged,
           this, &C_SdNdeNodeEditWidget::m_DataChanged);
   connect(this->mpc_Ui->pc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigErrorChange,
           this, &C_SdNdeNodeEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigSwitchToBus,
           this, &C_SdNdeNodeEditWidget::m_OnSwitchToBus);
   connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSwitchToHalc,
           this, &C_SdNdeNodeEditWidget::m_OnSwitchToHalc);
   connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSwitchToCommMessages,
           this, &C_SdNdeNodeEditWidget::m_OnSwitchToCommMessages);
   connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSave,
           this, &C_SdNdeNodeEditWidget::SigSave);
   connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigSaveAs,
           this, &C_SdNdeNodeEditWidget::SigSaveAs);
   connect(this->mpc_Ui->pc_TabWidgetPageNavi, &stw_opensyde_gui_elements::C_OgeTawPageNavi::currentChanged,
           this, &C_SdNdeNodeEditWidget::m_CurrentTabChanged);
   connect(this->mpc_Ui->pc_TabWidgetPageNavi, &stw_opensyde_gui_elements::C_OgeTawPageNavi::tabBarClicked,
           this, &C_SdNdeNodeEditWidget::m_TabClicked);
   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigNameChanged,
           this, &C_SdNdeNodeEditWidget::SigNameChanged);
   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigBusBitrateClicked,
           this, &C_SdNdeNodeEditWidget::SigSwitchToBusProperties);

   // show the initial tab
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(osn_TabIndex);

   //Other connects
   connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDpEditWidget::SigErrorChange,
           this, &C_SdNdeNodeEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigErrorChange, this,
           &C_SdNdeNodeEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_TabHalc, &C_SdNdeHalcWidget::SigErrorChange, this,
           &C_SdNdeNodeEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_TabHalc, &C_SdNdeHalcWidget::SigHalcDataPoolChanged, this,
           &C_SdNdeNodeEditWidget::m_ReloadDataPools);
   connect(this->mpc_Ui->pc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigCommDataPoolAdded, this,
           &C_SdNdeNodeEditWidget::m_ReloadDataPools);
   connect(this->mpc_Ui->pc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigErrorChange, this,
           &C_SdNdeNodeEditWidget::m_ReloadDataPools);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigOpenDataPool, this,
           &C_SdNdeNodeEditWidget::m_OpenDataPool);
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
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hsn_TabIndexProperties, C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hsn_TabIndexDataPool, C_GtGetText::h_GetText("Datapools"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hsn_TabIndexComm, C_GtGetText::h_GetText("COMM Messages"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(hsn_TabIndexHalc, C_GtGetText::h_GetText("Hardware Configurator"));

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
   this->mpc_Ui->pc_NodePropWidget->SaveToData();

   if (this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex() == hsn_TabIndexHalc)
   {
      this->mpc_Ui->pc_TabHalc->Save();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates specific functionality

   mu32_FLAG_EDIT_NAME           Opens the properties and selects the name for editing
   mu32_FLAG_OPEN_PROPERTIES     Opens the properties

   \param[in]  ou32_Flag   Flag for specific functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::SetFlag(const uint32 ou32_Flag) const
{
   if ((ou32_Flag == mu32_FLAG_EDIT_NAME) ||
       (ou32_Flag == mu32_FLAG_OPEN_PROPERTIES))
   {
      // open the properties
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexProperties);

      if (ou32_Flag == mu32_FLAG_EDIT_NAME)
      {
         this->mpc_Ui->pc_NodePropWidget->SelectName();
      }
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
void C_SdNdeNodeEditWidget::OpenDetail(const sint32 os32_MainIndex, const sint32 os32_ListIndex,
                                       const sint32 os32_ElementIndex, const sint32 os32_Flag) const
{
   if (os32_Flag == 0)
   {
      // open the datapool
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexDataPool);
      this->mpc_Ui->pc_DataPoolEditWidget->OpenDetail(os32_MainIndex, os32_ListIndex, os32_ElementIndex);
   }
   else if ((os32_Flag == 1) ||
            (os32_Flag == 2))
   {
      // open the interface description widget
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexComm);

      if (os32_Flag == 1)
      {
         this->mpc_Ui->pc_ComIfDescriptionWidget->SelectSignalSearch(this->mu32_NodeIndex,
                                                                     static_cast<uint32>(os32_MainIndex),
                                                                     static_cast<uint32>(os32_ListIndex),
                                                                     static_cast<uint32>(os32_ElementIndex));
      }
      else if (os32_Flag == 2)
      {
         this->mpc_Ui->pc_ComIfDescriptionWidget->SelectMessageSearch(this->mu32_NodeIndex,
                                                                      static_cast<uint32>(os32_MainIndex),
                                                                      static_cast<uint32>(os32_ListIndex),
                                                                      static_cast<uint32>(os32_ElementIndex));
      }
      else
      {
         // nothing to do
      }
   }
   else if (os32_Flag == 3)
   {
      // show the application / data block
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexProperties);
      this->mpc_Ui->pc_WidgetApplications->ShowApplication(os32_MainIndex);
   }
   else if (os32_Flag == 4)
   {
      // show HALC
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexHalc);
      this->mpc_Ui->pc_TabHalc->ShowChannel(os32_MainIndex, os32_ListIndex);
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the actual tab index

   \return
   Tab index
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_SdNdeNodeEditWidget::GetTabIndex(void) const
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
   const sint32 s32_FirstSegmentWidth = C_UsHandler::h_GetInstance()->GetSdNodeEditSplitterX();

   this->mpc_Ui->pc_Splitter->SetFirstSegment(s32_FirstSegmentWidth);
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(C_UsHandler::h_GetInstance()->GetProjLastSysDefNodeTabIndex());
   this->mpc_Ui->pc_TabHalc->LoadUserSettings();
   this->mpc_Ui->pc_ComIfDescriptionWidget->LoadUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_SaveUserSettings() const
{
   const QList<sintn> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   if (c_Sizes.size() > 0)
   {
      C_UsHandler::h_GetInstance()->SetSdNodeEditSplitterX(c_Sizes.at(0));
   }
   C_UsHandler::h_GetInstance()->SetProjLastSysDefNodeTabIndex(this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex());
   this->mpc_Ui->pc_TabHalc->SaveUserSettings();
   this->mpc_Ui->pc_ComIfDescriptionWidget->SaveUserSettings();
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
void C_SdNdeNodeEditWidget::m_OnSwitchToBus(const uint32 & oru32_BusIndex, const QString & orc_BusName)
{
   Q_EMIT (this->SigSwitchToBus(oru32_BusIndex, orc_BusName));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open the HALC tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OnSwitchToHalc(void) const
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexHalc);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open the COMM messages tab

   \param[in]  ou32_DataPoolIndex   Index of COMM Datapool
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OnSwitchToCommMessages(const uint32 ou32_DataPoolIndex) const
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(hsn_TabIndexComm);
   this->mpc_Ui->pc_ComIfDescriptionWidget->SetProtocolByDataPool(ou32_DataPoolIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for tab change of pc_TabWidgetPageNavi

   Adapt the visibility of the tab options

   The not shown widget in the other tab cause a resize of the tab widget itself.
   This function sets the size policy of all not shown widgets to ignore to achieve that the hided but visible
   widgets have no impact on the current size of the tab widget.
   The changed widget of the current tab must be reseted to the preferred size

   \param[in]  osn_Index   Index of selected tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CurrentTabChanged(const sintn osn_Index) const
{
   sintn sn_Counter;

   //Simple trigger update
   this->mpc_Ui->pc_WidgetApplications->UpdateApplications();

   // Adjust the size of the hided but visible widgets of the other tabs
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

   // The previous call of adjustSize causes some times to a second size change of pc_DataPoolEditWidget
   // This can cause a wrong size of the widget. A further call of adjustSize seems to repair the problem
   // The origin reason why the widget is resized two times with different sizes is unknown
   this->mpc_Ui->pc_DataPoolEditWidget->adjustSize();

   if (osn_Index == hsn_TabIndexComm)
   {
      this->mpc_Ui->pc_ComIfDescriptionWidget->SetInitialFocus();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for tab click of pc_TabWidgetPageNavi

   Navigate back to Datapool overview if a specific NVM or DIAG Datapool content was opened

   \param[in]  osn_Index   Index of selected tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_TabClicked(const sintn osn_Index) const
{
   if ((this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex() == hsn_TabIndexDataPool) &&
       (osn_Index == hsn_TabIndexDataPool))
   {
      // Only relevant if no other tab than the Datapool tab was clicked and no other tab was selected before
      this->mpc_Ui->pc_DataPoolEditWidget->OpenOverview();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger reload of data pools tab
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_ReloadDataPools(void) const
{
   this->mpc_Ui->pc_DataPoolEditWidget->SetNode(this->mu32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger entire reload of COMM messages

   Necessary if Datapool order or existence has changed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_ReloadCommMessages(void) const
{
   this->mpc_Ui->pc_ComIfDescriptionWidget->SetNodeId(this->mu32_NodeIndex,
                                                      this->mpc_Ui->pc_ComIfDescriptionWidget->GetActProtocol());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show data pool properties

   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OpenDataPool(const uint32 ou32_DataPoolIndex) const
{
   this->OpenDetail(ou32_DataPoolIndex, -1, -1, 0);
}
