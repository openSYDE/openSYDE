//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to handle data pool selections for data blocks (implementation)

   Widget to handle data pool selections for data blocks

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeDbSelectDataPools.hpp"
#include "ui_C_SdNdeDbSelectDataPools.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_NodeIndex               Active node index
   \param[in]     os32_SkipApplicationIndex    Application index to not display as used
   \param[in]     orc_UsedDataPoolIndicesIndex Data pools to always display as used
   \param[in,out] orc_Parent                   Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbSelectDataPools::C_SdNdeDbSelectDataPools(const uint32_t ou32_NodeIndex,
                                                   const int32_t os32_SkipApplicationIndex,
                                                   const std::vector<uint32_t> & orc_UsedDataPoolIndicesIndex,
                                                   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDbSelectDataPools),
   mrc_ParentDialog(orc_Parent),
   mu32_NodeIndex(ou32_NodeIndex),
   mc_UsedDataPoolIndices(orc_UsedDataPoolIndicesIndex)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   //Deactivate debug string
   this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setTitle("");
   this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setTitle("");

   //configure view
   this->mpc_Ui->pc_TreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
   this->mpc_Ui->pc_TreeView->SetUseInternalExpandedItems(false);
   this->mpc_Ui->pc_TreeView->InitSd(ou32_NodeIndex, os32_SkipApplicationIndex, orc_UsedDataPoolIndicesIndex);

   //initially expand all
   this->mpc_Ui->pc_TreeView->expandAll();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   //After tree was filled
   m_UpdateSelection(0);
   m_HandleHiding();

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeDbSelectDataPools::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_SdNdeDbSelectDataPools::m_CancelClicked);
   connect(this->mpc_Ui->pc_TreeView, &C_TblTreDataElementView::SigAccept, this,
           &C_SdNdeDbSelectDataPools::m_OkClicked);
   connect(this->mpc_Ui->pc_LineEditSearch, &C_OgeLeProperties::textChanged, this,
           &C_SdNdeDbSelectDataPools::m_OnSearch);
   connect(this->mpc_Ui->pc_TreeView, &C_TblTreDataElementView::SigSelectionChanged, this,
           &C_SdNdeDbSelectDataPools::m_UpdateSelection);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDbSelectDataPools::~C_SdNdeDbSelectDataPools(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbSelectDataPools::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Owned Datapool"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Selection"));
   this->mpc_Ui->pc_LineEditSearch->setPlaceholderText(C_GtGetText::h_GetText("Filter"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Select Datapool"));
   this->mpc_Ui->pc_LabelSearchNoElementsFound->setText(C_GtGetText::h_GetText("No data elements found."));
   this->mpc_Ui->pc_LabelInitialDataElementsNoElements->setText(C_GtGetText::h_GetText("No Datapools."));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected data elements

   \return
   Current selected data elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_PuiSvDbNodeDataPoolListElementId> C_SdNdeDbSelectDataPools::GetSelectedDataPools(void) const
{
   return this->mpc_Ui->pc_TreeView->GetSelectedDataElements();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbSelectDataPools::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OkClicked();
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
void C_SdNdeDbSelectDataPools::m_OkClicked(void)
{
   // Check COMM datapools: maximum one datapool per protocol allowed
   if (this->m_IsCommDatapoolSelectionValid() == true)
   {
      this->mrc_ParentDialog.accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbSelectDataPools::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle view hiding / replacement
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbSelectDataPools::m_HandleHiding(void) const
{
   if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_TreeView->setVisible(false);
      if (this->mpc_Ui->pc_LineEditSearch->text().isEmpty() == true)
      {
         this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
         this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(true);
      }
      else
      {
         this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(true);
         this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(false);
      }
   }
   else
   {
      this->mpc_Ui->pc_TreeView->setVisible(true);
      this->mpc_Ui->pc_GroupBoxSearchNoElementsFound->setVisible(false);
      this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Filter for string

   \param[in] orc_Text String
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbSelectDataPools::m_OnSearch(const QString & orc_Text) const
{
   this->mpc_Ui->pc_TreeView->Search(orc_Text);
   if (orc_Text.isEmpty() == true)
   {
      m_HandleHiding();
      m_UpdateSelection(this->mpc_Ui->pc_TreeView->GetSelectedDataElements().size());
   }
   else
   {
      m_HandleHiding();
      if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
      {
         this->mpc_Ui->pc_LabelSelection->setVisible(false);
      }
      else
      {
         m_UpdateSelection(this->mpc_Ui->pc_TreeView->GetSelectedDataElements().size());
      }
      this->mpc_Ui->pc_GroupBoxInitialDataElementNoElements->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update number of selected items

   \param[in] os32_SelectionCount Number of selected items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDbSelectDataPools::m_UpdateSelection(const int32_t os32_SelectionCount) const
{
   if (this->mpc_Ui->pc_TreeView->IsEmpty() == true)
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_LabelSelection->setVisible(true);
      if (os32_SelectionCount > 0)
      {
         if (os32_SelectionCount > 1)
         {
            this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText(
                                                                             "%1 selected Datapools")).arg(
                                                        os32_SelectionCount));
         }
         else
         {
            this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText("1 selected Datapool")));
         }
      }
      else
      {
         this->mpc_Ui->pc_LabelSelection->setText(static_cast<QString>(C_GtGetText::h_GetText("No selected Datapool")));
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if COMM Datapool selection is  valid.

   For each protocol the selected Datapool must be unique, i.e. maximum one Datapool per protocol is allowed.
   Check all owned and selected Datapools for protocol duplicate.

   \retval   true    Selection is valid
   \retval   false   Selection is invalid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDbSelectDataPools::m_IsCommDatapoolSelectionValid(void)
{
   bool q_Return = true;
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mu32_NodeIndex);
   QString c_Details = "";

   std::map<C_OscCanProtocol::E_Type, uint32_t> c_Counter;
   std::map<C_OscCanProtocol::E_Type, uint32_t>::const_iterator c_It;

   if (pc_Node != NULL)
   {
      // put already used and newly selected datapools together
      // note: in fact we only need to check COMM datapools, but differentiating also needs to check every DP (for type)
      std::vector<uint32_t> c_DatapoolIndices = this->mc_UsedDataPoolIndices;
      const std::vector<C_PuiSvDbNodeDataPoolListElementId> & rc_SelectedDatapoolElements =
         this->mpc_Ui->pc_TreeView->GetSelectedDataElements();
      for (uint32_t u32_It = 0UL; u32_It < rc_SelectedDatapoolElements.size(); ++u32_It)
      {
         const C_PuiSvDbNodeDataPoolListElementId & rc_CurItem = rc_SelectedDatapoolElements[u32_It];
         c_DatapoolIndices.push_back(rc_CurItem.u32_DataPoolIndex);
      }

      // check all already owned and newly selected datapools
      for (uint32_t u32_It = 0UL; u32_It < c_DatapoolIndices.size(); ++u32_It)
      {
         const C_OscCanProtocol * const pc_Protocol = pc_Node->GetRelatedCanProtocolConst(c_DatapoolIndices[u32_It]);
         if (pc_Protocol != NULL)
         {
            c_Counter[pc_Protocol->e_Type] += 1;
         }
      }
   }

   for (c_It = c_Counter.begin(); c_It != c_Counter.end(); ++c_It)
   {
      if (c_It->second > 1)
      {
         // more than one COMM Datapool of a protocol found
         c_Details += "\n" + C_PuiSdUtil::h_ConvertProtocolTypeToString(c_It->first);
         q_Return = false;
      }
   }

   // inform user if not unique
   if (q_Return == false)
   {
      c_Details.prepend(
         C_GtGetText::h_GetText("There is more than one Datapool selected for the following protocol type(s): "));
      C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
      c_Message.SetHeading(C_GtGetText::h_GetText("COMM Datapool selection"));
      c_Message.SetDescription(C_GtGetText::h_GetText("Select only one COMM Datapool for each of Layer 2, "
                                                      "ECeS and ECoS."));
      c_Message.SetDetails(c_Details);
      c_Message.SetCustomMinHeight(230, 270);
      c_Message.Execute();
   }

   return q_Return;
}
