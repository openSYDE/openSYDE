//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for dashboard tabs (implementation)

   Widget for dashboard tabs.

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFontMetrics>
#include "constants.h"
#include "C_OgeWiDashboardTab.h"
#include "ui_C_OgeWiDashboardTab.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent    Optional pointer to parent
   \param[in]     oq_ShowUndock Optional flag to show/hide undock option
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiDashboardTab::C_OgeWiDashboardTab(QWidget * const opc_Parent, const bool oq_ShowUndock) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiDashboardTab),
   mpc_ActionActivate(NULL),
   mpc_ActionCut(NULL),
   mpc_ActionPaste(NULL),
   mpc_ActionDelete(NULL),
   mpc_ActionClose(NULL),
   mq_Pinned(false),
   mq_Current(false),
   mq_Active(false),
   mq_EditActive(false),
   mq_Interactive(true),
   mq_FinishRenamingActive(false),
   mq_CloseButtonVisible(true),
   mq_ShowUndock(oq_ShowUndock)
{
   this->mpc_Ui->setupUi(this);
   //Before mode
   m_InitContextMenu();
   m_HandleMode();
   this->mpc_Ui->pc_PushButtonClose->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_PushButtonClose->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                           C_GtGetText::h_GetText("Delete Dashboard tab."));

   //Temporary remove of close action until reactivation feature is available
   //connect(this->mpc_Ui->pc_PushButtonClose, &QPushButton::clicked, this, &C_OgeWiDashboardTab::m_CloseAction);
   connect(this->mpc_Ui->pc_PushButtonClose, &QPushButton::clicked, this, &C_OgeWiDashboardTab::m_DeleteAction);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiDashboardTab::~C_OgeWiDashboardTab(void)
{
   delete mpc_Ui;
   //lint -e{1740} Action pointers do not take ownership of qt items
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set text

   \param[in] orc_Text Tab text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::SetText(const QString & orc_Text)
{
   const sintn sn_Offset = 24 + 30 + 6;
   //this->mpc_Ui->pc_PushButtonClose->width() +
   //this->mpc_Ui->pc_HorizontalSpacer1->sizeHint().width() +
   //this->mpc_Ui->pc_HorizontalSpacer2->sizeHint().width()

   const QFontMetrics c_FontMetrics(this->mpc_Ui->pc_LabelName->font());

   this->mpc_Ui->pc_LabelName->setText(orc_Text);
   this->resize(c_FontMetrics.width(orc_Text) + sn_Offset, this->height());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set tooltip of tab label.

   long description of function within several lines

   \param[in]     orc_Heading    tooltip heading
   \param[in]     orc_Content    tooltip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::SetToolTip(const QString & orc_Heading, const QString & orc_Content) const
{
   // show tooltip only if there is any content
   if (orc_Content != "")
   {
      this->mpc_Ui->pc_LabelName->SetToolTipInformation(orc_Heading, orc_Content);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   Here: Enter edit mode

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::mouseDoubleClickEvent(QMouseEvent * const opc_Event)
{
   QWidget::mouseDoubleClickEvent(opc_Event);
   if (this->mq_Interactive == true)
   {
      Q_EMIT this->SigEditPropertiesAction(this);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active flag

   \param[in] oq_Value New flag value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::SetActive(const bool oq_Value)
{
   this->mq_Active = oq_Value;
   m_HandleMode();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set close button visibility

   \param[in] oq_Visibility Close button visibility
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::SetCloseButtonVisibility(const bool oq_Visibility)
{
   this->mpc_Ui->pc_PushButtonClose->setVisible(oq_Visibility && this->mq_Interactive);
   this->mq_CloseButtonVisible = oq_Visibility;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Deactivate additional actions
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::DeactivateAdditionalActions(void)
{
   if (this->mpc_ActionCut != NULL)
   {
      this->mpc_ActionCut->setVisible(false);
   }
   if (this->mpc_ActionPaste != NULL)
   {
      this->mpc_ActionPaste->setVisible(false);
   }
   if (this->mpc_ActionDelete != NULL)
   {
      this->mpc_ActionDelete->setVisible(false);
   }
   if (this->mpc_ActionClose != NULL)
   {
      this->mpc_ActionClose->setVisible(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark mode active

   \param[in] oq_Active Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::SetDarkMode(const bool oq_Active)
{
   C_OgeWiUtil::h_ApplyStylesheetPropertyToItselfAndAllChildren(this, "DarkMode", oq_Active);
   this->mpc_Ui->pc_PushButtonClose->SetDarkMode(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function activate or deactivate interaction

   \param[in] oq_Active Flag if interaction available
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::SetInteractive(const bool oq_Active)
{
   this->mq_Interactive = oq_Active;
   this->mpc_Ui->pc_PushButtonClose->setVisible(oq_Active && this->mq_CloseButtonVisible);
   if (oq_Active == true)
   {
      this->setContextMenuPolicy(Qt::CustomContextMenu);
   }
   else
   {
      this->setContextMenuPolicy(Qt::NoContextMenu);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current flag

   \param[in] oq_Value New flag value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::SetCurrent(const bool oq_Value)
{
   this->mq_Current = oq_Value;
   m_HandleMode();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set pinned flag

   \param[in] oq_Value New flag value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::SetPinned(const bool oq_Value)
{
   this->mq_Pinned = oq_Value;
   m_HandleMode();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle mode change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_HandleMode(void)
{
   if (this->mq_Current == true)
   {
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "State", "Current");
   }
   else
   {
      C_OgeWiUtil::h_ApplyStylesheetProperty(this, "State", "Inactive");
   }
   if (this->mq_Active == true)
   {
      if (this->mpc_ActionActivate != NULL)
      {
         this->mpc_ActionActivate->setText(C_GtGetText::h_GetText("Deactivate"));
      }
      this->mpc_Ui->pc_LabelName->setEnabled(true);
   }
   else
   {
      if (this->mpc_ActionActivate != NULL)
      {
         this->mpc_ActionActivate->setText(C_GtGetText::h_GetText("Activate"));
      }
      this->mpc_Ui->pc_LabelName->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_InitContextMenu(void)
{
   // add all actions
   this->mc_ContextMenu.addAction(C_GtGetText::h_GetText("Edit Properties"), this,
                                  &C_OgeWiDashboardTab::m_EditProperties);

   this->mc_ContextMenu.addSeparator();

   if (this->mq_ShowUndock == true)
   {
      this->mc_ContextMenu.addAction(C_GtGetText::h_GetText("Undock"), this, &C_OgeWiDashboardTab::m_UndockAction);
      this->mc_ContextMenu.addSeparator();
   }

   /* Activate/Deactivate Feature is not supported so far (open issue)
   this->mpc_ActionActivate = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                                "Activate/Deactivate"), this,
                                                             &C_OgeWiDashboardTab::m_ToggleActive);
   this->mc_ContextMenu.addSeparator();
   */

   this->mpc_ActionCut = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                           "Cut"), this, &C_OgeWiDashboardTab::m_CutAction);
   this->mc_ContextMenu.addAction(C_GtGetText::h_GetText("Copy"), this, &C_OgeWiDashboardTab::m_CopyAction);
   this->mpc_ActionPaste = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                             "Paste"), this, &C_OgeWiDashboardTab::m_PasteAction);

   this->mc_ContextMenu.addSeparator();

   //Temporary remove of close action until reactivation feature is available
   //this->mpc_ActionClose = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
   //                                                          "Close"), this, &C_OgeWiDashboardTab::m_CloseAction);
   this->mpc_ActionDelete = this->mc_ContextMenu.addAction(C_GtGetText::h_GetText(
                                                              "Delete"), this, &C_OgeWiDashboardTab::m_DeleteAction);

   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, &C_OgeWiDashboardTab::customContextMenuRequested, this,
           &C_OgeWiDashboardTab::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Toggle active state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_ToggleActive(void)
{
   const bool q_Active = !this->mq_Active;

   this->SetActive(q_Active);
   Q_EMIT this->SigActiveChanged(this, q_Active);
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   this->mc_ContextMenu.popup(this->mapToGlobal(orc_Pos));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal add action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_EditProperties(void)
{
   Q_EMIT this->SigEditPropertiesAction(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal copy action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_CopyAction(void)
{
   Q_EMIT this->SigCopyAction(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal cut action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_CutAction(void)
{
   Q_EMIT this->SigCutAction(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal paste action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_PasteAction(void)
{
   Q_EMIT this->SigPasteAction();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal delete action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_DeleteAction(void)
{
   Q_EMIT this->SigDeleteAction(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal close action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_CloseAction(void)
{
   Q_EMIT this->SigCloseAction(this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal undock action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiDashboardTab::m_UndockAction(void)
{
   Q_EMIT this->SigUndockAction(this);
}
