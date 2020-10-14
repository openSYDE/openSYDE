//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard data element update mode node header (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "TGLUtils.h"
#include "constants.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_SyvDaPeUpdateModeNodeHeader.h"
#include "ui_C_SyvDaPeUpdateModeNodeHeader.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32 C_SyvDaPeUpdateModeNodeHeader::mhu32_HeaderHeight = 40;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_ViewIndex   View index
   \param[in]      ou32_NodeIndex   Node index
   \param[in,out]  opc_Item         Optional pointer to item
   \param[in,out]  opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeUpdateModeNodeHeader::C_SyvDaPeUpdateModeNodeHeader(const stw_types::uint32 ou32_ViewIndex,
                                                             const stw_types::uint32 ou32_NodeIndex,
                                                             QTreeWidgetItem * const opc_Item,
                                                             QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeUpdateModeNodeHeader),
   mpc_TreeWidgetItem(opc_Item),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_NodeIndex(ou32_NodeIndex)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(this->mu32_NodeIndex);

   mpc_Ui->setupUi(this);

   //Init buttons
   RegisterExpandOrCollapse(false);

   //Resize
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->setMinimumHeight(C_SyvDaPeUpdateModeNodeHeader::mhu32_HeaderHeight);
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->setMaximumHeight(C_SyvDaPeUpdateModeNodeHeader::mhu32_HeaderHeight);

   //init
   this->InitStaticNames();

   //Add deactivated button state
   this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconOpenList.svg",
                                                     "://images/IconOpenListHovered.svg",
                                                     "://images/IconOpenListClicked.svg",
                                                     "://images/IconOpenListDisabledBright.svg");

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelCount, "Valid", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LabelCountNumber, "Valid", true);

   tgl_assert(pc_Node != NULL);
   if (pc_Node != NULL)
   {
      //Translation: 1: Node name
      this->mpc_Ui->pc_LabelListName->setText(QString(C_GtGetText::h_GetText("Node - %1")).arg(pc_Node->c_Properties.
                                                                                               c_Name.c_str()));
   }
   this->UpdateCount();

   //signals & slots
   connect(this->mpc_Ui->pc_PushButtonExpand, &QPushButton::toggled, this,
           &C_SyvDaPeUpdateModeNodeHeader::m_OnPushButtonExpandClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeUpdateModeNodeHeader::~C_SyvDaPeUpdateModeNodeHeader(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeNodeHeader::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelCount->setText(C_GtGetText::h_GetText("Used Transmissions:"));
   this->mpc_Ui->pc_LabelCount->SetToolTipInformation(C_GtGetText::h_GetText("Used Transmissions"),
                                                      C_GtGetText::h_GetText(
                                                         "Number of used cyclic or event driven transmissions. "
                                                         "\nThese transmissions will be registered on the server during dashboard connect."));
   this->mpc_Ui->pc_LabelCountNumber->SetToolTipInformation(C_GtGetText::h_GetText("Used Transmissions"),
                                                            C_GtGetText::h_GetText(
                                                               "Number of used cyclic or event driven transmissions."
                                                               "\nThese transmissions will be registered on the server during dashboard connect."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update count value

   \param[in]  os32_Count  <  0: Actual count is calculated from system view data
                           >= 0: Valid count assumed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeNodeHeader::UpdateCount(const sint32 os32_Count) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   tgl_assert(pc_View != NULL);
   if (pc_View != NULL)
   {
      uint32 u32_Cur;
      if (os32_Count >= 0)
      {
         u32_Cur = static_cast<uint32>(os32_Count);
      }
      else
      {
         u32_Cur = pc_View->CountCyclicTransmissions(this->mu32_NodeIndex);
      }
      //Translation: 1 = Current number of used transmissions, 2 = Allowed number of used transmissions
      this->mpc_Ui->pc_LabelCountNumber->setText(QString(C_GtGetText::h_GetText("%1")).arg(u32_Cur));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register expand or collapse

   \param[in]  orq_Expanded   true:  expanded
                              false: collapsed
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeNodeHeader::RegisterExpandOrCollapse(const bool & orq_Expanded) const
{
   this->mpc_Ui->pc_PushButtonExpand->setChecked(orq_Expanded);
   if (orq_Expanded == true)
   {
      this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconCloseList.svg",
                                                        "://images/IconArrowBottomHovered.svg",
                                                        "://images/IconArrowBottomClicked.svg",
                                                        "://images/IconArrowBottomDisabledBright.svg");
   }
   else
   {
      this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconOpenList.svg",
                                                        "://images/IconOpenListHovered.svg",
                                                        "://images/IconOpenListClicked.svg",
                                                        "://images/IconOpenListDisabledBright.svg");
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Here: Trigger manual expand

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeNodeHeader::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   QWidget::mouseDoubleClickEvent(opc_Event);
   m_OnPushButtonExpandClicked(!this->mpc_Ui->pc_PushButtonExpand->isChecked());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle event push button expand clicked

   \param[in]  oq_Checked  Buttons new check state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeUpdateModeNodeHeader::m_OnPushButtonExpandClicked(const bool oq_Checked)
{
   //Resize expanded
   if (oq_Checked == true)
   {
      this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconCloseList.svg",
                                                        "://images/IconArrowBottomHovered.svg",
                                                        "://images/IconArrowBottomClicked.svg",
                                                        "://images/IconArrowBottomDisabledBright.svg");

      Q_EMIT (this->SigExpand(this->mpc_TreeWidgetItem, true));
   }
   else
   {
      this->mpc_Ui->pc_PushButtonExpand->SetCustomIcons("://images/IconOpenList.svg",
                                                        "://images/IconOpenListHovered.svg",
                                                        "://images/IconOpenListClicked.svg",
                                                        "://images/IconOpenListDisabledBright.svg");

      Q_EMIT (this->SigExpand(this->mpc_TreeWidgetItem, false));
   }
}
