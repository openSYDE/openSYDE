//-----------------------------------------------------------------------------
/*!
   \internal   (todo: remove this line for header files)
   \file
   \brief       Bus edit widget (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "constants.h"
#include "C_SdBueBusEditWidget.h"

#include "ui_C_SdBueBusEditWidget.h"

#include "C_PuiSdHandler.h"
#include "C_OgeWiUtil.h"
#include "C_OSCSystemBus.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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

   \param[in]     ou32_BusIndex     Bus index
   \param[in]     osn_TabIndex      Tab index to show
   \param[in,out] opc_parent Optional pointer to parent

   \created     01.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
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

   this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(0);

   this->mpc_Ui->pc_BusPropertiesWidget->SetBusId(this->mu32_BusIndex);

   connect(this->mpc_Ui->pc_BusPropertiesWidget, &C_SdBueBusEditPropertiesWidget::SigChanged,
           this, &C_SdBueBusEditWidget::m_DataChanged);
   connect(this->mpc_Ui->pc_BusPropertiesWidget, &C_SdBueBusEditPropertiesWidget::SigNameChanged,
           this, &C_SdBueBusEditWidget::SigNameChanged);

   this->me_BusTye = pc_Bus->e_Type;
   if (this->me_BusTye == stw_opensyde_core::C_OSCSystemBus::eCAN)
   {
      stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TabWidgetPageNavi->tabBar(),
                                                                     "LastItemBig", true);

      this->mpc_Ui->pc_WidgetComIfDescr->SetBusId(this->mu32_BusIndex);

      // connecting to signals
      connect(this->mpc_Ui->pc_WidgetComIfDescr, &C_SdBueComIfDescriptionWidget::SigChanged,
              this, &C_SdBueBusEditWidget::m_DataChanged);
      connect(this->mpc_Ui->pc_WidgetComIfDescr, &C_SdBueComIfDescriptionWidget::SigErrorChange,
              this, &C_SdBueBusEditWidget::SigErrorChange);

      // show the initial tab
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(osn_TabIndex);
   }
   else
   {
      // Ethernet is not supported for the C_SdBueComIfDescriptionWidget yet
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(0);
      this->mpc_Ui->pc_TabWidgetPageNavi->removeTab(1);

      stw_opensyde_gui_logic::C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_TabWidgetPageNavi->tabBar(),
                                                                     "OnlyOneItem",
                                                                     true);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     27.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueBusEditWidget::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(0, C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_TabWidgetPageNavi->setTabText(1, C_GtGetText::h_GetText("COMM Interface Description"));

   //Tool tips
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the actual tab index

   \return
   Tab index

   \created     19.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sintn C_SdBueBusEditWidget::GetTabIndex(void) const
{
   return this->mpc_Ui->pc_TabWidgetPageNavi->currentIndex();
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     01.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueBusEditWidget::~C_SdBueBusEditWidget()
{
   delete mpc_Ui;
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
bool C_SdBueBusEditWidget::WasChanged(void) const
{
   return this->mq_DataChanged;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Saves the actual data into the core but not in the files

   \created     23.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueBusEditWidget::Save(void) const
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Activates specific functionality

   mu32_FLAG_EDIT_NAME                    Opens the properties and selects the name for editing
   mu32_FLAG_OPEN_PROPERTIES              Opens the properties
   mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR   Opens the COM interface description

   \param[in]     ou32_Flag       Flag for specific functionality

   \created     02.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueBusEditWidget::SetFlag(const uint32 ou32_Flag) const
{
   if ((ou32_Flag == mu32_FLAG_EDIT_NAME) ||
       (ou32_Flag == mu32_FLAG_OPEN_PROPERTIES))
   {
      // open the properties
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(0);

      if (ou32_Flag == mu32_FLAG_EDIT_NAME)
      {
         this->mpc_Ui->pc_BusPropertiesWidget->SelectName();
      }
   }
   else if (ou32_Flag == mu32_FLAG_OPEN_SYSDEF_BUS_COMIFDESCR)
   {
      // open the COM interface description
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(1);
   }
   else
   {
      // nothing to do
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to open a concrete datapool, datapool list or dataelement or the COM interface description

   \param[in] os32_NodeIndex          Node index (if not used set to -1)
   \param[in] os32_DataPoolIndex      Datapool index (if not used set to -1)
   \param[in] os32_ListIndex          Optional list index (if not used set to -1)
   \param[in] os32_ElementIndex       Optional element index (if not used set to -1)
                                      Index depends of value of os32_Flag
   \param[in] os32_Flag               Optional flag for further information
                                      0: os32_ElementIndex is index of datapool data element without CAN reference
                                      1: os32_ElementIndex is index of datapool data element with associated CAN signal
                                      2: os32_ElementIndex is index of CAN message

   \created     10.05.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueBusEditWidget::OpenDetail(const sint32 os32_NodeIndex, const sint32 os32_DataPoolIndex,
                                      const sint32 os32_ListIndex, const sint32 os32_ElementIndex,
                                      const sint32 os32_Flag) const
{
   if (this->me_BusTye == stw_opensyde_core::C_OSCSystemBus::eCAN)
   {
      // open the interface description widget
      this->mpc_Ui->pc_TabWidgetPageNavi->setCurrentIndex(1);

      if (os32_Flag == 1)
      {
         this->mpc_Ui->pc_WidgetComIfDescr->SelectSignal(os32_NodeIndex, static_cast<uint32>(os32_DataPoolIndex),
                                                         static_cast<uint32>(os32_ListIndex),
                                                         static_cast<uint32>(os32_ElementIndex));
      }
      else if (os32_Flag == 2)
      {
         this->mpc_Ui->pc_WidgetComIfDescr->SelectMessage(os32_NodeIndex, static_cast<uint32>(os32_DataPoolIndex),
                                                          static_cast<uint32>(os32_ListIndex),
                                                          static_cast<uint32>(os32_ElementIndex));
      }
      else
      {
         // nothing to do
      }
   }
}

//-----------------------------------------------------------------------------
void C_SdBueBusEditWidget::m_DataChanged(void)
{
   this->mq_DataChanged = true;
   Q_EMIT this->SigChanged();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Getter to access UI information of this widget.

   \return
   const pointer of UI functionality

   \created     06.06.2018  STW/D.Pohl
*/
//-----------------------------------------------------------------------------
const Ui::C_SdBueBusEditWidget * C_SdBueBusEditWidget::GetUiWidget() const
{
   return this->mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten show event slot

   Here: Load splitter position

   \param[in,out] opc_Event Event identification and information

   \created     07.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueBusEditWidget::showEvent(QShowEvent * const opc_Event)
{
   //Necessary to initialize with appropriate size
   this->mpc_Ui->pc_WidgetComIfDescr->TriggerLoadOfSplitterUserSettings();
   QWidget::showEvent(opc_Event);
}
