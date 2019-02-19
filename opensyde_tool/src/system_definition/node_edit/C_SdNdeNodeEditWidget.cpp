//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for node edit

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     31.01.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
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

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

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

   \param[in]     ou32_NodeIndex    Node index
   \param[in]     osn_TabIndex      Tab index to show
   \param[in,out] opc_parent        Optional pointer to parent

   \created     31.01.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_SdNdeNodeEditWidget::C_SdNdeNodeEditWidget(const uint32 ou32_NodeIndex, const sintn osn_TabIndex,
                                             QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeNodeEditWidget),
   mpc_TabThreeWidget(NULL),
   mpc_ComIfDescriptionWidget(NULL),
   mu32_NodeIndex(ou32_NodeIndex),
   mq_DataChanged(false)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_WidgetApplications->SetNodeIndex(this->mu32_NodeIndex);

   InitStaticNames();

   this->mpc_Ui->pc_NodePropWidget->SetNodeId(this->mu32_NodeIndex);
   this->mpc_Ui->pc_DataPoolEditWidget->SetNode(this->mu32_NodeIndex);

   // configure the dynamic widget for editing the COM datapools of nodes
   this->mpc_TabThreeWidget = new QWidget();
   if (this->mpc_TabThreeWidget != NULL)
   {
      QVBoxLayout * const pc_Layout = new QVBoxLayout();
      pc_Layout->setContentsMargins(21, 11, 4, 0);
      this->mpc_TabThreeWidget->setLayout(pc_Layout);

      this->mpc_ComIfDescriptionWidget = new C_SdBueComIfDescriptionWidget();
      pc_Layout->addWidget(this->mpc_ComIfDescriptionWidget);

      // connecting to signals
      connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigChanged,
              this, &C_SdNdeNodeEditWidget::m_DataChanged);
      connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDataPoolEditWidget::SigChanged,
              this, &C_SdNdeNodeEditWidget::m_DataChanged);
      connect(this->mpc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigChanged,
              this, &C_SdNdeNodeEditWidget::m_DataChanged);
      connect(this->mpc_ComIfDescriptionWidget, &C_SdBueComIfDescriptionWidget::SigErrorChange,
              this, &C_SdNdeNodeEditWidget::SigErrorChange);
      connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDataPoolEditWidget::SigSwitchToBus,
              this, &C_SdNdeNodeEditWidget::m_OnSwitchToBus);
      connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDataPoolEditWidget::SigEdit,
              this, &C_SdNdeNodeEditWidget::m_EditComDataPool);
      connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDataPoolEditWidget::SigSave,
              this, &C_SdNdeNodeEditWidget::SigSave);
      connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDataPoolEditWidget::SigSaveAs,
              this, &C_SdNdeNodeEditWidget::SigSaveAs);
      connect(this->mpc_Ui->pc_TabWidgetPageNavi, &stw_opensyde_gui_elements::C_OgeTawPageNavi::currentChanged,
              this, &C_SdNdeNodeEditWidget::m_CurrentTabChanged);
      connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigNameChanged,
              this, &C_SdNdeNodeEditWidget::SigNameChanged);

      //lint -e{429}  no memory leak because of the parent of pc_Layout and the Qt memory management
   }

   // show the initial tab
   if (osn_TabIndex == 0)
   {
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(0);
   }
   else
   {
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(1);
   }

   //Other connects
   connect(this->mpc_Ui->pc_DataPoolEditWidget, &C_SdNdeDataPoolEditWidget::SigErrorChange,
           this, &C_SdNdeNodeEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigErrorChange, this,
           &C_SdNdeNodeEditWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigReloadDataPools, this,
           &C_SdNdeNodeEditWidget::m_ReloadDataPools);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigCheckDataPoolInteraction, this,
           &C_SdNdeNodeEditWidget::m_CheckDataPoolInteraction);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigOpenDataPool, this,
           &C_SdNdeNodeEditWidget::m_OpenDataPool);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     31.01.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_SdNdeNodeEditWidget::~C_SdNdeNodeEditWidget()
{
   //Store splitter position
   const QList<sintn> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   if (c_Sizes.size() > 0)
   {
      C_UsHandler::h_GetInstance()->SetSdNodeEditSplitterX(c_Sizes.at(0));
   }

   //Other
   if (this->mpc_Ui->pc_TabWidgetPageNavi->count() >= 2)
   {
      // disconnect tab changed because this signal is emitted on delete and therefore leads to crash
      // (m_CurrentTabChanged updates tabs sub widgets which may not exist anymore)
      disconnect(this->mpc_Ui->pc_TabWidgetPageNavi, &stw_opensyde_gui_elements::C_OgeTawPageNavi::currentChanged,
                 this, &C_SdNdeNodeEditWidget::m_CurrentTabChanged);
      // delete the widget if not in the tab with no parent
      delete this->mpc_TabThreeWidget;
   }

   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_ComIfDescriptionWidget and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(0, C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(1, C_GtGetText::h_GetText("Datapools"));

   //Tool tips
}

//-----------------------------------------------------------------------------
/*!
   \brief   Information about the save state of the node. Resets the data changed flag.

   \return
   true     Node was changed
   false    Node was not changed

   \created     10.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdNdeNodeEditWidget::WasChanged(void) const
{
   return this->mq_DataChanged;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Saves the actual data into the core but not in the files

   \created     23.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::Save(void) const
{
   this->mpc_Ui->pc_NodePropWidget->SaveToData();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates specific functionality

   mu32_FLAG_EDIT_NAME           Opens the properties and selects the name for editing
   mu32_FLAG_OPEN_PROPERTIES     Opens the properties

   \param[in]     ou32_Flag       Flag for specific functionality

   \created     02.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::SetFlag(const uint32 ou32_Flag) const
{
   if ((ou32_Flag == mu32_FLAG_EDIT_NAME) ||
       (ou32_Flag == mu32_FLAG_OPEN_PROPERTIES))
   {
      // open the properties
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(0);

      if (ou32_Flag == mu32_FLAG_EDIT_NAME)
      {
         this->mpc_Ui->pc_NodePropWidget->SelectName();
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to open a concrete datapool, datapool list or dataelement

   \param[in] os32_MainIndex          Datapool index (os32_Flag is 0,1,2) or Application index (os32_Flag is 3)
   \param[in] os32_ListIndex          Optional list index (if not used set to -1)
   \param[in] os32_ElementIndex       Optional element index (if not used set to -1)
                                      Index depends of value of os32_Flag
   \param[in] os32_Flag               Optional flag for further information
                                      0: os32_ElementIndex is index of datapool data element without CAN reference
                                      1: os32_ElementIndex is index of datapool data element with associated CAN signal
                                      2: os32_ElementIndex is index of CAN message
                                      3: os32_MainIndex is index of application / data block

   \created     16.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::OpenDetail(const sint32 os32_MainIndex, const sint32 os32_ListIndex,
                                       const sint32 os32_ElementIndex, const sint32 os32_Flag)
{
   if (os32_Flag == 0)
   {
      // open the datapool
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(1);
      this->mpc_Ui->pc_DataPoolEditWidget->OpenDetail(os32_MainIndex, os32_ListIndex, os32_ElementIndex);
   }
   else if ((os32_Flag == 1) ||
            (os32_Flag == 2))
   {
      // open the interface description widget
      this->m_EditComDataPool(static_cast<uint32>(os32_MainIndex), static_cast<uint32>(os32_ListIndex));

      if (os32_Flag == 1)
      {
         this->mpc_ComIfDescriptionWidget->SelectSignal(this->mu32_NodeIndex, static_cast<uint32>(os32_MainIndex),
                                                        static_cast<uint32>(os32_ListIndex),
                                                        static_cast<uint32>(os32_ElementIndex));
      }
      else if (os32_Flag == 2)
      {
         this->mpc_ComIfDescriptionWidget->SelectMessage(this->mu32_NodeIndex, static_cast<uint32>(os32_MainIndex),
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
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(0);
      this->mpc_Ui->pc_WidgetApplications->ShowApplication(os32_MainIndex);
   }
   else
   {
      // nothing to do
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the actual tab index

   \return
   Tab index

   \created     19.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sintn C_SdNdeNodeEditWidget::GetTabIndex(void) const
{
   return this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out] opc_Event Event identification and information

   \created     22.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::showEvent(QShowEvent * const opc_Event)
{
   const sint32 s32_FirstSegmentWidth = C_UsHandler::h_GetInstance()->GetSdNodeEditSplitterX();

   this->mpc_Ui->pc_Splitter->SetFirstSegment(s32_FirstSegmentWidth);
   QWidget::showEvent(opc_Event);
}

//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_DataChanged(void)
{
   this->mq_DataChanged = true;
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in] oru32_BusIndex Bus index
   \param[in] orc_BusName  Bus name

   \created     23.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OnSwitchToBus(const uint32 & oru32_BusIndex, const QString & orc_BusName)
{
   Q_EMIT this->SigSwitchToBus(oru32_BusIndex, orc_BusName);
}

//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_EditComDataPool(const uint32 ou32_DataPoolIndex, const uint32 ou32_ListIndex)
{
   sintn sn_Index;
   const C_OSCNode * pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);
   QString c_Title;

   if (ou32_DataPoolIndex < pc_Node->c_DataPools.size())
   {
      const C_OSCCanProtocol * const pc_Protocol = pc_Node->GetRelatedCANProtocolConst(ou32_DataPoolIndex);
      // build the title for the tab widget

      // each interface are two lists
      c_Title = C_PuiSdUtil::h_GetInterfaceName(C_OSCSystemBus::eCAN, static_cast<uint8>(ou32_ListIndex / 2U)) + " ";

      if (pc_Protocol != NULL)
      {
         switch (pc_Protocol->e_Type)
         {
         case C_OSCCanProtocol::eLAYER2:
            c_Title += "(OSI Layer 2)";
            break;
         case C_OSCCanProtocol::eCAN_OPEN_SAFETY:
            c_Title += "(ECoS)";
            break;
         case C_OSCCanProtocol::eECES:
            c_Title += "(ECeS)";
            break;
         default:
            break;
         }
      }
      c_Title += C_GtGetText::h_GetText(" - COMM Interface Description");

      // add the necessary widget to the tab widget
      sn_Index = this->mpc_Ui->pc_TabWidgetPageNavi->addTab(this->mpc_TabThreeWidget, c_Title);
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(sn_Index);

      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TabWidgetPageNavi->tabBar(),
                                             "LastItemVeryBig",
                                             true);

      // set the data
      if (this->mpc_ComIfDescriptionWidget != NULL)
      {
         this->mpc_ComIfDescriptionWidget->SetNodeDataPool(this->mu32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot for tab change of pc_TabWidgetPageNavi

   Adapt the visibility of the tab options

   The not shown widget in the other tab cause a resize of the tab widget itself.
   This function sets the size policy of all not shown widgets to ignore to achieve that the hided but visible
   widgets have no impact on the current size of the tab widget.
   The changed widget of the current tab must be reseted to the preferred size

   \param[in]     osn_Index         Index of selected tab

   \created     23.01.2019  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CurrentTabChanged(const sintn osn_Index) const
{
   sintn sn_Counter;

   // remove the COM datapool edit widget if it is visible
   if ((osn_Index < 2) &&
       (this->mpc_Ui->pc_TabWidgetPageNavi->count() >= 2))
   {
      this->mpc_Ui->pc_TabWidgetPageNavi->removeTab(2);
      this->mpc_Ui->pc_TabWidgetPageNavi->tabBar()->adjustSize(); // resize to avoid datapool tab takes the space

      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TabWidgetPageNavi->tabBar(),
                                             "LastItemVeryBig",
                                             false);

      this->mpc_Ui->pc_DataPoolEditWidget->UpdateComLists();
   }
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
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger reload of data pools tab

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_ReloadDataPools(void) const
{
   this->mpc_Ui->pc_DataPoolEditWidget->SetNode(this->mu32_NodeIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger check of data pool interaction availability

   \created     04.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_CheckDataPoolInteraction(void) const
{
   this->mpc_Ui->pc_DataPoolEditWidget->CheckDataPoolInteraction();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show data pool properties

   \param[in] ou32_DataPoolIndex Data pool index

   \created     08.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeNodeEditWidget::m_OpenDataPool(const uint32 ou32_DataPoolIndex)
{
   this->OpenDetail(ou32_DataPoolIndex, -1, -1, 0);
}
