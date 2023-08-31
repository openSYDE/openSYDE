//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for Filter list items (implementation)

   Widget for Filter list items including a checkbox, filter name, edit button and remove button.
   Item here means "list entry", this may be confusing because a "filter item" also is used for
   sub-structure of a filter (which is more like a filter package). See C_CamProFilterData for more info.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CamMosFilterItemWidget.hpp"
#include "ui_C_CamMosFilterItemWidget.h"

#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_Uti.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_CamMosFilterPopup.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_Filter  Filter data
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosFilterItemWidget::C_CamMosFilterItemWidget(const C_CamProFilterData & orc_Filter, QWidget * const opc_Parent) :
   stw::opensyde_gui_elements::C_OgeWiOnlyBackground(opc_Parent),
   mpc_Ui(new Ui::C_CamMosFilterItemWidget),
   mq_ButtonPressed(false)
{
   this->mpc_Ui->setupUi(this);

   // label style
   this->mpc_Ui->pc_LabFilter->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabFilter->SetForegroundColor(0);
   this->mpc_Ui->pc_LabFilter->SetFontPixel(13);

   // initialize filter
   this->mc_Filter = orc_Filter;

   // buttons
   this->mpc_Ui->pc_PbEdit->SetSvg("://images/IconEdit.svg", "://images/IconEditDisabled.svg",
                                   "://images/IconEditHovered.svg");
   this->mpc_Ui->pc_PbRemove->SetSvg("://images/IconClearGray.svg", "://images/IconClearGrayDisabled.svg",
                                     "://images/IconClearGrayHovered.svg");
   // hide buttons on start
   this->mpc_Ui->pc_PbEdit->setVisible(false);
   this->mpc_Ui->pc_PbRemove->setVisible(false);

   // check box and label (use extra label for better tooltip handling)
   this->mpc_Ui->pc_CheckBox->setChecked(mc_Filter.q_Enabled);
   this->m_SetFilterNameElided();

   // tool tips
   this->mpc_Ui->pc_PbEdit->SetToolTipInformation(C_GtGetText::h_GetText("Edit"),
                                                  C_GtGetText::h_GetText("Edit filter properties."));
   this->mpc_Ui->pc_PbRemove->SetToolTipInformation(C_GtGetText::h_GetText("Delete"),
                                                    C_GtGetText::h_GetText("Delete this filter."));
   this->mpc_Ui->pc_CheckBox->SetToolTipInformation(C_GtGetText::h_GetText("Enable Receive Filter"),
                                                    C_GtGetText::h_GetText(
                                                       "Check to stop or pass messages as described in the filter "
                                                       "items or uncheck to remove filtering."));
   this->m_CreateTooltipInformation();

   // connects
   connect(this->mpc_Ui->pc_PbEdit, &C_OgePubSvgIconOnly::clicked, this, &C_CamMosFilterItemWidget::m_OnEdit);
   connect(this->mpc_Ui->pc_PbRemove, &C_OgePubSvgIconOnly::clicked, this, &C_CamMosFilterItemWidget::m_OnRemove);
   connect(this->mpc_Ui->pc_PbEdit, &C_OgePubSvgIconOnly::pressed, this, &C_CamMosFilterItemWidget::m_ButtonPressed);
   connect(this->mpc_Ui->pc_PbRemove, &C_OgePubSvgIconOnly::pressed, this, &C_CamMosFilterItemWidget::m_ButtonPressed);
   connect(this->mpc_Ui->pc_PbEdit, &C_OgePubSvgIconOnly::released, this, &C_CamMosFilterItemWidget::m_ButtonReleased);
   connect(this->mpc_Ui->pc_PbRemove, &C_OgePubSvgIconOnly::released, this,
           &C_CamMosFilterItemWidget::m_ButtonReleased);
   connect(this->mpc_Ui->pc_CheckBox, &QCheckBox::toggled, this, &C_CamMosFilterItemWidget::m_OnChxToggle);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosFilterItemWidget::~C_CamMosFilterItemWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create tooltip from information of filter data handling.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::m_CreateTooltipInformation(void)
{
   QString c_Content = mc_Filter.c_Comment + "\n\n";

   QList<C_CamProFilterItemData> c_FilterItems = mc_Filter.c_FilterItems;

   c_Content += static_cast<QString>(C_GtGetText::h_GetText("Number of Filter Items: %1")).arg(c_FilterItems.size());
   c_Content += "\n";

   // show information of first few filter items (maximum 3)
   for (int32_t s32_Pos = 0; (s32_Pos < c_FilterItems.size()) && (s32_Pos < 3); s32_Pos++)
   {
      const C_CamProFilterItemData & rc_FilterItem = c_FilterItems[s32_Pos];
      QString c_Temp = "";

      c_Content += "\n";
      c_Content += static_cast<QString>(C_GtGetText::h_GetText("Filter Item %1\n")).arg(s32_Pos + 1);

      // enabled flag
      c_Content += "   ";
      c_Content += C_GtGetText::h_GetText("Enabled: ");
      if (rc_FilterItem.q_Enabled == true)
      {
         c_Content += C_GtGetText::h_GetText("Yes\n");
      }
      else
      {
         c_Content += C_GtGetText::h_GetText("No\n");
      }

      // type
      c_Content += "   ";
      c_Content += C_GtGetText::h_GetText("Type: ");
      if (rc_FilterItem.e_Type == C_CamProFilterItemData::eID_SINGLE)
      {
         c_Content += C_GtGetText::h_GetText("CAN ID\n");
      }
      else
      {
         c_Content += C_GtGetText::h_GetText("CAN ID Range\n");
      }

      // mode
      c_Content += "   ";
      c_Content += C_GtGetText::h_GetText("Mode: ");
      if (rc_FilterItem.q_PassFilter == true)
      {
         c_Content += C_GtGetText::h_GetText("Pass\n");
      }
      else
      {
         c_Content += C_GtGetText::h_GetText("Stop\n");
      }

      // details
      c_Content += "   ";
      c_Content += C_GtGetText::h_GetText("Details: ");

      // add "x" to ID if extended
      if (static_cast<bool>(rc_FilterItem.u8_ExtendedId) ==  true)
      {
         c_Temp = "x";
      }
      if (rc_FilterItem.e_Type == C_CamProFilterItemData::eID_SINGLE)
      {
         c_Content += C_GtGetText::h_GetText("ID 0x");
         c_Content += static_cast<QString>("%1").arg(rc_FilterItem.u32_StartId, 0, 16).toUpper();
         c_Content += c_Temp;
         c_Content += "\n";
      }
      else
      {
         c_Content += C_GtGetText::h_GetText("From 0x");
         c_Content += static_cast<QString>("%1").arg(rc_FilterItem.u32_StartId, 0, 16).toUpper();
         c_Content += c_Temp;
         c_Content += C_GtGetText::h_GetText(" to 0x");
         c_Content += static_cast<QString>("%1").arg(rc_FilterItem.u32_EndId, 0, 16).toUpper();
         c_Content += c_Temp;
         c_Content += "\n";
      }
   }

   if (c_FilterItems.size() > 3)
   {
      c_Content += "\n";
      c_Content += C_GtGetText::h_GetText("Click edit to see all filter items...");
   }

   this->SetToolTipInformation(mc_Filter.c_Name, c_Content.trimmed());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for edit button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::m_OnEdit()
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_CamMosFilterPopup * const pc_Dialog = new C_CamMosFilterPopup(this->mc_Filter, *c_New);

   //Resize
   c_New->SetSize(QSize(700, 820));

   // Update settings on accept
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      // check if there are any changes
      C_CamProFilterData c_FilterDataNew = pc_Dialog->GetFilterData();
      c_FilterDataNew.q_Enabled = this->mc_Filter.q_Enabled; // enabled flag of whole filter is not set in dialog
      uint32_t u32_HashValueOld = 0xFFFFFFFFUL;
      uint32_t u32_HashValueNew = 0xFFFFFFFFUL;
      this->mc_Filter.CalcHash(u32_HashValueOld);
      c_FilterDataNew.CalcHash(u32_HashValueNew);

      if (u32_HashValueNew != u32_HashValueOld)
      {
         // note: we also started the dialog with data from mc_Filter, so unedited properties did not change

         // inform filter widget (that than handles data handling)
         Q_EMIT (this->SigUpdateFilter(this, this->mc_Filter, c_FilterDataNew));

         // update intern filter (after emitting signal!)
         this->mc_Filter = c_FilterDataNew;

         // adapt item widget
         this->m_CreateTooltipInformation();
         this->m_SetFilterNameElided();
      }
   }
   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for remove button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::m_OnRemove()
{
   // inform list about remove
   Q_EMIT (this->SigRemoveFilter(this));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for checkbox toggle.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::m_OnChxToggle(const bool & orq_Checked)
{
   // inform list about check box toggle
   Q_EMIT (this->SigEnableFilter(this, orq_Checked));

   // update filter data
   this->mc_Filter.q_Enabled = orq_Checked;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for pressed signal of any button

   We need to know when a button of the item was pressed, but is not released already. The signal clicked will be
   send normally when pressed and released is sent.
   In case of an active tool tip of the button and clicking the button, the leave event C_CamMosFilterItemWidget will be
   caused before the released signal of the button and the group of the buttons will be set invisible for a short time.
   This prevents the sending of the clicked signal of the button.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::m_ButtonPressed(void)
{
   this->mq_ButtonPressed = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for released signal of any button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::m_ButtonReleased(void)
{
   this->mq_ButtonPressed = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set filter name and elide with ... if too long for current widget size.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::m_SetFilterNameElided(void) const
{
   const QFontMetrics c_Metric = QFontMetrics(C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13));

   this->mpc_Ui->pc_LabFilter->setText(c_Metric.elidedText(this->mc_Filter.c_Name, Qt::ElideRight,
                                                           this->mpc_Ui->pc_LabFilter->width() - 44));
   // padding 44 for 2 buttons a 16px and 2x padding a 6px
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Modified function to "m_OnEdit" to add the filter message items to the existing filter

   \param[in]       oc_CanMsgId     List of Can Message Id's for corresponding message
   \param[in]      oc_CanMsgXtd   List of Extended format for corresponding message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::m_OnAddToExistingFilter(const QList<int32_t> oc_CanMsgId,
                                                       const QList<uint8_t> oc_CanMsgXtd)
{
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_CamMosFilterPopup * const pc_Dialog = new C_CamMosFilterPopup(this->mc_Filter, *c_New);

   //Resize
   c_New->SetSize(QSize(700, 820));
   pc_Dialog->SetAddFilterItem(oc_CanMsgId, oc_CanMsgXtd);
   // Update settings on accept
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      // check if there are any changes
      C_CamProFilterData c_FilterDataNew = pc_Dialog->GetFilterData();
      c_FilterDataNew.q_Enabled = this->mc_Filter.q_Enabled; // enabled flag of whole filter is not set in dialog
      uint32_t u32_HashValueOld = 0xFFFFFFFFUL;
      uint32_t u32_HashValueNew = 0xFFFFFFFFUL;
      this->mc_Filter.CalcHash(u32_HashValueOld);
      c_FilterDataNew.CalcHash(u32_HashValueNew);

      if (u32_HashValueNew != u32_HashValueOld)
      {
         // note: we also started the dialog with data from mc_Filter, so unedited properties did not change

         // inform filter widget (that than handles data handling)
         Q_EMIT (this->SigUpdateFilter(this, this->mc_Filter, c_FilterDataNew));

         // update intern filter (after emitting signal!)
         this->mc_Filter = c_FilterDataNew;

         // adapt item widget
         this->m_CreateTooltipInformation();
         this->m_SetFilterNameElided();
      }
   }
   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
} //lint !e429  no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Setter to add filter items to existing filter

   \param[in]       oc_CanMsgId     List of Can Message Id's for corresponding message
   \param[in]      oc_CanMsgXtd   List of Extended format for corresponding message

*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::SetAddFilterToExistingFilter(const QList<int32_t> oc_CanMsgId,
                                                            const QList<uint8_t> oc_CanMsgXtd)
{
   C_CamMosFilterItemWidget::m_OnAddToExistingFilter(oc_CanMsgId, oc_CanMsgXtd);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden event function.

    Here: Show or hide buttons; elide text if necessary.

   \param[in,out]    opc_Event   Event identification and information

   \retval  True   Event was recognized and processed
   \retval  False  Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_CamMosFilterItemWidget::event(QEvent * const opc_Event)
{
   if (this->isEnabled() == true)
   {
      if ((opc_Event->type() == QEvent::Leave) &&
          (this->mq_ButtonPressed == false))
      {
         // Set only invisible when no button of this item is pressed. See function m_ButtonPressed for
         // a detailed description
         this->mpc_Ui->pc_PbEdit->setVisible(false);
         this->mpc_Ui->pc_PbRemove->setVisible(false);
         this->SetBackgroundColor(-1);
      }
      else if (opc_Event->type() == QEvent::Enter)
      {
         this->mpc_Ui->pc_PbEdit->setVisible(true);
         this->mpc_Ui->pc_PbRemove->setVisible(true);
         this->SetBackgroundColor(3);
      }
      else if (opc_Event->type() == QEvent::Resize)
      {
         // elide text if necessary
         this->m_SetFilterNameElided();
      }
      else
      {
         // nothing todo
      }
   }
   return C_OgeWiOnlyBackground::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Here: open filter configuration dialog on double click.

   \param[in,out] opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterItemWidget::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   C_OgeWiOnlyBackground::mouseDoubleClickEvent(opc_Event);
   this->m_OnEdit();
}
