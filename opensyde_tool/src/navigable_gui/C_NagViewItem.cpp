//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for view item in navigation bar (implementation)

   Widget for view item in navigation bar

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "constants.h"
#include "C_SyvUtil.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_UsHandler.h"
#include "C_NagViewItem.h"
#include "C_PuiSvHandler.h"
#include "ui_C_NagViewItem.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */
const stw_types::sintn C_NagViewItem::hsn_SizeTop = 48;
const stw_types::sintn C_NagViewItem::hsn_SizeSub = 30;
const stw_types::sintn C_NagViewItem::hsn_FixSizeBottom = 18;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     27.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_NagViewItem::C_NagViewItem(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_NagViewItem),
   mq_Active(false),
   mq_IgnoreActiveOnExpand(false),
   mu32_ViewIndex(0UL)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   this->setAttribute(Qt::WA_Hover, true);

   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Active", false);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Dragged", false);

   //Remove debug text
   this->mpc_Ui->pc_LabelExpand->setText("");
   this->mpc_Ui->pc_LabelError->setText("");
   this->mpc_Ui->pc_PushButtonEdit->setText("");
   this->mpc_Ui->pc_PushButtonCopy->setText("");
   this->mpc_Ui->pc_GroupBoxStyle->setTitle("");
   this->mpc_Ui->pc_PushButtonDelete->setText("");

   //Set icons
   this->mpc_Ui->pc_PushButtonEdit->SetSvg("://images/main_page_and_navi_bar/Icon_edit.svg", "",
                                           "://images/main_page_and_navi_bar/Icon_edit_hover.svg");
   this->mpc_Ui->pc_PushButtonCopy->SetSvg("://images/main_page_and_navi_bar/Icon_duplicate.svg", "",
                                           "://images/main_page_and_navi_bar/Icon_duplicate_hover.svg");
   this->mpc_Ui->pc_PushButtonDelete->SetSvg("://images/main_page_and_navi_bar/Icon_delete_navi_bar.svg", "",
                                             "://images/main_page_and_navi_bar/Icon_delete_navi_bar_hover.svg");

   //Style top label
   this->mpc_Ui->pc_LabelHeading->SetFontPixel(14);
   this->mpc_Ui->pc_LabelHeading->SetForegroundColor(12);

   //Initial icons
   this->mpc_Ui->pc_LabelError->SetSvg("");

   //Icons for sub items
   this->mpc_Ui->pc_WidgetSetup->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_WidgetUpdate->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_WidgetDashboard->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_WidgetSetup->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
   this->mpc_Ui->pc_WidgetUpdate->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
   this->mpc_Ui->pc_WidgetDashboard->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));

   //Configure
   this->mpc_Ui->pc_WidgetTopButton->setCheckable(true);

   //Initially hide buttons
   this->mpc_Ui->pc_GroupBoxHoverButtons->setVisible(false);

   //Initially hide line edit
   this->mpc_Ui->pc_LineEditHeading->setVisible(false);

   //Connects
   connect(this->mpc_Ui->pc_LineEditHeading, &stw_opensyde_gui_elements::C_OgeLeNavigation::editingFinished, this,
           &C_NagViewItem::m_OnNameEditFinished);
   connect(this->mpc_Ui->pc_LineEditHeading, &stw_opensyde_gui_elements::C_OgeLeNavigation::SigEscape, this,
           &C_NagViewItem::m_OnNameEditCancelled);
   connect(this->mpc_Ui->pc_WidgetTopButton, &stw_opensyde_gui_elements::C_OgePubNavigationHover::toggled, this,
           &C_NagViewItem::m_SetExpanded);
   connect(this->mpc_Ui->pc_PushButtonCopy, &stw_opensyde_gui_elements::C_OgePubSvgIconOnly::clicked, this,
           &C_NagViewItem::m_OnDuplicate);
   connect(this->mpc_Ui->pc_PushButtonDelete, &stw_opensyde_gui_elements::C_OgePubSvgIconOnly::clicked, this,
           &C_NagViewItem::m_OnDelete);
   connect(this->mpc_Ui->pc_PushButtonEdit, &stw_opensyde_gui_elements::C_OgePubSvgIconOnly::clicked, this,
           &C_NagViewItem::m_OnEditButton);
   connect(this->mpc_Ui->pc_WidgetSetup, &stw_opensyde_gui_elements::C_OgePubNavigationHover::clicked, this,
           &C_NagViewItem::m_OnSetupClicked);
   connect(this->mpc_Ui->pc_WidgetUpdate, &stw_opensyde_gui_elements::C_OgePubNavigationHover::clicked, this,
           &C_NagViewItem::m_OnUpdateClicked);
   connect(this->mpc_Ui->pc_WidgetDashboard, &stw_opensyde_gui_elements::C_OgePubNavigationHover::clicked, this,
           &C_NagViewItem::m_OnDashboardClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     27.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_NagViewItem::~C_NagViewItem(void)
{
   delete this->mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load all user settings

   \created     08.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::LoadUserSettings(void)
{
   bool q_NewVal;

   //Always initially expand active view
   if (this->mq_Active == true)
   {
      q_NewVal = true;
   }
   else
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         const C_UsSystemView c_ViewUserSettings = C_UsHandler::h_GetInstance()->GetProjSvSetupView(pc_View->GetName());
         q_NewVal = c_ViewUserSettings.GetNavigationExpandedStatus();
      }
      else
      {
         q_NewVal = false;
      }
   }
   this->mq_IgnoreActiveOnExpand = true;
   this->m_SetExpanded(q_NewVal);
   this->mq_IgnoreActiveOnExpand = false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Save all user settings

   \created     08.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::SaveUserSettings(void) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      C_UsHandler::h_GetInstance()->SetProjSvNavigationExpandedStatus(pc_View->GetName(), this->m_IsExpanded());
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update view name

   \created     03.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::UpdateName(void) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const QString c_Name = QString(C_GtGetText::h_GetText("View #%1 - %2")).arg(this->mu32_ViewIndex + 1UL).arg(
         pc_View->GetName());
      this->mpc_Ui->pc_LabelHeading->setText(c_Name);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger update of decoration role

   \created     30.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::UpdateDeco(void) const
{
   QString c_Error;

   if (C_SyvUtil::h_CheckViewSetupError(this->mu32_ViewIndex, c_Error) == true)
   {
      //Error
      this->mpc_Ui->pc_LabelError->SetSvg("://images/Error_iconV2.svg");
      this->mpc_Ui->pc_LabelError->SetToolTipInformation("", c_Error, C_NagToolTip::eERROR);
   }
   else
   {
      this->mpc_Ui->pc_LabelError->SetSvg("");
      this->mpc_Ui->pc_LabelError->SetToolTipInformation("", "");
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     06.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::InitStaticNames(void) const
{
   const QString c_Padding = " ";

   //Add one space as padding
   this->mpc_Ui->pc_WidgetSetup->setText(c_Padding + C_GtGetText::h_GetText("Setup"));
   this->mpc_Ui->pc_WidgetUpdate->setText(c_Padding + C_GtGetText::h_GetText("Update"));
   this->mpc_Ui->pc_WidgetDashboard->setText(c_Padding + C_GtGetText::h_GetText("Dashboards"));

   //Tool tips
   this->mpc_Ui->pc_PushButtonCopy->SetToolTipInformation(C_GtGetText::h_GetText("Duplicate"),
                                                          C_GtGetText::h_GetText("Add a copy of this view"));
   this->mpc_Ui->pc_PushButtonEdit->SetToolTipInformation(C_GtGetText::h_GetText("Rename"),
                                                          C_GtGetText::h_GetText("Edit view name"));
   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete"),
                                                            C_GtGetText::h_GetText("Delete this view from the project"));
   this->mpc_Ui->pc_WidgetSetup->SetToolTipInformation(C_GtGetText::h_GetText("Setup"),
                                                       C_GtGetText::h_GetText(
                                                          "Edit your current setup of available nodes"));
   this->mpc_Ui->pc_WidgetUpdate->SetToolTipInformation(C_GtGetText::h_GetText("Update"),
                                                        C_GtGetText::h_GetText(
                                                           "Update software on your current setup of available nodes"));
   this->mpc_Ui->pc_WidgetDashboard->SetToolTipInformation(C_GtGetText::h_GetText("Dashboards"),
                                                           C_GtGetText::h_GetText(
                                                              "Interact (visualize, parametrize) with your current setup of available nodes"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set dragged state

   \param[in] oq_Active Flag if drag is active

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::SetDragged(const bool oq_Active)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Dragged", oq_Active);
   //Deactivate hovering while dragging
   this->SetHovered(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set index

   \param[in] ou32_ViewIndex New view index

   \created     27.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::Init(const uint32 ou32_ViewIndex)
{
   this->mu32_ViewIndex = ou32_ViewIndex;
   this->UpdateName();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set active state

   \param[in] oq_Active    New active state
   \param[in] os32_SubMode New active sub mode

   \created     30.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::SetActive(const bool oq_Active, const sint32 os32_SubMode)
{
   this->mq_Active = oq_Active;
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "Active", oq_Active);
   if (oq_Active == true)
   {
      switch (os32_SubMode)
      {
      case 0:
         this->mpc_Ui->pc_WidgetSetup->SetActive(true);
         this->mpc_Ui->pc_WidgetUpdate->SetActive(false);
         this->mpc_Ui->pc_WidgetDashboard->SetActive(false);
         this->mpc_Ui->pc_WidgetSetup->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletListActive.svg"));
         this->mpc_Ui->pc_WidgetUpdate->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         this->mpc_Ui->pc_WidgetDashboard->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         break;
      case 1:
         this->mpc_Ui->pc_WidgetSetup->SetActive(false);
         this->mpc_Ui->pc_WidgetUpdate->SetActive(true);
         this->mpc_Ui->pc_WidgetDashboard->SetActive(false);
         this->mpc_Ui->pc_WidgetSetup->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         this->mpc_Ui->pc_WidgetUpdate->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletListActive.svg"));
         this->mpc_Ui->pc_WidgetDashboard->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         break;
      case 2:
         this->mpc_Ui->pc_WidgetSetup->SetActive(false);
         this->mpc_Ui->pc_WidgetUpdate->SetActive(false);
         this->mpc_Ui->pc_WidgetDashboard->SetActive(true);
         this->mpc_Ui->pc_WidgetSetup->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         this->mpc_Ui->pc_WidgetUpdate->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         this->mpc_Ui->pc_WidgetDashboard->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletListActive.svg"));
         break;
      default:
         this->mpc_Ui->pc_WidgetSetup->SetActive(false);
         this->mpc_Ui->pc_WidgetUpdate->SetActive(false);
         this->mpc_Ui->pc_WidgetDashboard->SetActive(false);
         this->mpc_Ui->pc_WidgetSetup->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         this->mpc_Ui->pc_WidgetUpdate->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         this->mpc_Ui->pc_WidgetDashboard->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
         break;
      }
   }
   else
   {
      this->mpc_Ui->pc_WidgetSetup->SetActive(false);
      this->mpc_Ui->pc_WidgetUpdate->SetActive(false);
      this->mpc_Ui->pc_WidgetDashboard->SetActive(false);
      this->mpc_Ui->pc_WidgetSetup->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
      this->mpc_Ui->pc_WidgetUpdate->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
      this->mpc_Ui->pc_WidgetDashboard->setIcon(QIcon("://images/main_page_and_navi_bar/IconBulletList.svg"));
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set hovered state

   \param[in] oq_Hovered Current hovered state

   \created     27.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::SetHovered(const bool oq_Hovered) const
{
   //Order is important due to hover event handling
   this->mpc_Ui->pc_GroupBoxHoverButtons->setVisible(oq_Hovered);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get size hint (handled expanded or not expanded size)

   \return
   Size hint (handled expanded or not expanded size)

   \created     26.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_NagViewItem::sizeHint(void) const
{
   QSize c_Retval = QWidget::sizeHint();
   sintn sn_Height = C_NagViewItem::hsn_SizeTop;

   if (this->m_IsExpanded() == true)
   {
      sn_Height += (3 * C_NagViewItem::hsn_SizeSub) + C_NagViewItem::hsn_FixSizeBottom;
   }

   c_Retval.setWidth(this->width());
   c_Retval.setHeight(sn_Height);
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle hover

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     27.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_NagViewItem::event(QEvent * const opc_Event)
{
   bool q_Retval = QWidget::event(opc_Event);

   if (opc_Event->type() == QEvent::Leave)
   {
      this->SetHovered(false);
   }
   else if (opc_Event->type() == QEvent::Enter)
   {
      this->SetHovered(true);
   }
   else
   {
      //No handling required
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten paint event slot

   Here: draw background
   (Not automatically drawn in any QWidget derivative)

   \param[in,out] opc_Event Event identification and information

   \created     17.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::paintEvent(QPaintEvent * const opc_Event)
{
   stw_opensyde_gui_logic::C_OgeWiUtil::h_DrawBackground(this);

   QWidget::paintEvent(opc_Event);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check if currently expanded

   \return
   True  Expanded
   False Not expanded

   \created     14.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_NagViewItem::m_IsExpanded(void) const
{
   return this->mpc_Ui->pc_WidgetDashboard->isVisible();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle name edit finished

   \created     14.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnNameEditFinished(void)
{
   bool q_Found = false;

   // avoid second "editingFinished" signal when line edit looses focus (occurs on enter key press)
   disconnect(this->mpc_Ui->pc_LineEditHeading, &stw_opensyde_gui_elements::C_OgeLeNavigation::editingFinished, this,
              &C_NagViewItem::m_OnNameEditFinished);

   this->mpc_Ui->pc_LabelHeading->setVisible(true);
   this->mpc_Ui->pc_LineEditHeading->setVisible(false);

   connect(this->mpc_Ui->pc_LineEditHeading, &stw_opensyde_gui_elements::C_OgeLeNavigation::editingFinished, this,
           &C_NagViewItem::m_OnNameEditFinished);

   //Check new name
   for (uint32 u32_ItView = 0; u32_ItView < C_PuiSvHandler::h_GetInstance()->GetViewCount(); ++u32_ItView)
   {
      //Skip the current name
      if (u32_ItView != this->mu32_ViewIndex)
      {
         const C_PuiSvData * const pc_ViewData = C_PuiSvHandler::h_GetInstance()->GetView(u32_ItView);
         if (pc_ViewData != NULL)
         {
            if (pc_ViewData->GetName().compare(this->mpc_Ui->pc_LineEditHeading->text()) == 0)
            {
               q_Found = true;
            }
         }
      }
   }
   if (q_Found == true)
   {
      stw_opensyde_gui_elements::C_OgeWiCustomMessage
         c_ImportWarnings(this, stw_opensyde_gui_elements::C_OgeWiCustomMessage::eERROR);
      c_ImportWarnings.SetHeading(C_GtGetText::h_GetText("View rename"));
      c_ImportWarnings.SetDescription(
         QString(C_GtGetText::h_GetText("The view name \"%1\" is already used. Please choose another name.")).
         arg(this->mpc_Ui->pc_LineEditHeading->text()));
      c_ImportWarnings.Execute();
   }
   else
   {
      Q_EMIT this->SigSetName(this, this->mpc_Ui->pc_LineEditHeading->text());
      this->UpdateName();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle name edit cancel

   \created     22.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnNameEditCancelled(void)
{
   disconnect(this->mpc_Ui->pc_LineEditHeading, &stw_opensyde_gui_elements::C_OgeLeNavigation::editingFinished, this,
              &C_NagViewItem::m_OnNameEditFinished);
   this->mpc_Ui->pc_LabelHeading->setVisible(true);
   this->mpc_Ui->pc_LineEditHeading->setVisible(false);
   //Don't allow accepting the current input
   connect(this->mpc_Ui->pc_LineEditHeading, &stw_opensyde_gui_elements::C_OgeLeNavigation::editingFinished, this,
           &C_NagViewItem::m_OnNameEditFinished);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new expanded state

   \param[in] oq_Expanded New expanded state

   \created     14.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_SetExpanded(const bool oq_Expanded)
{
   //If external trigger
   this->mpc_Ui->pc_WidgetTopButton->setChecked(oq_Expanded);
   this->mpc_Ui->pc_WidgetSetup->setVisible(oq_Expanded);
   this->mpc_Ui->pc_WidgetUpdate->setVisible(oq_Expanded);
   this->mpc_Ui->pc_WidgetDashboard->setVisible(oq_Expanded);
   if (oq_Expanded == true)
   {
      this->mpc_Ui->pc_LabelExpand->SetSvg("://images/main_page_and_navi_bar/Icon_close_list.svg");
   }
   else
   {
      this->mpc_Ui->pc_LabelExpand->SetSvg("://images/main_page_and_navi_bar/Icon_open_list.svg");
   }
   m_OnExpand();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle expand action

   \created     26.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnExpand(void)
{
   Q_EMIT this->SigExpanded(this);
   //Check if is expanded NOW (after action was executed)
   if (((this->mq_Active == false) && (this->mq_IgnoreActiveOnExpand == false)) && (this->m_IsExpanded() == true))
   {
      QString c_SubMode;
      QString c_SubSubMode;
      C_SyvUtil::h_GetViewDisplayName(this->mu32_ViewIndex, ms32_SUBMODE_SYSVIEW_SETUP, c_SubMode, c_SubSubMode);
      //Also select this one
      Q_EMIT this->SigSelect(this, ms32_SUBMODE_SYSVIEW_SETUP, c_SubMode, c_SubSubMode);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle delete action

   \created     30.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnDelete(void)
{
   Q_EMIT this->SigDelete(this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle start drag action

   \created     02.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnStartDrag(void)
{
   Q_EMIT this->SigStartDrag(this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle duplicate action

   \created     30.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnDuplicate(void)
{
   Q_EMIT this->SigDuplicate(this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle edit button action

   \created     30.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnEditButton(void) const
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      this->mpc_Ui->pc_LabelHeading->setVisible(false);
      this->mpc_Ui->pc_LineEditHeading->setVisible(true);
      //Configure text edit
      this->mpc_Ui->pc_LineEditHeading->setText(pc_View->GetName());
      this->mpc_Ui->pc_LineEditHeading->setFocus();
      this->mpc_Ui->pc_LineEditHeading->selectAll();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle setup item click

   \created     13.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnSetupClicked(void)
{
   QString c_SubMode;
   QString c_SubSubMode;

   C_SyvUtil::h_GetViewDisplayName(this->mu32_ViewIndex, ms32_SUBMODE_SYSVIEW_SETUP, c_SubMode, c_SubSubMode);
   Q_EMIT this->SigSelect(this, ms32_SUBMODE_SYSVIEW_SETUP, c_SubMode, c_SubSubMode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle update item click

   \created     13.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnUpdateClicked(void)
{
   QString c_SubMode;
   QString c_SubSubMode;

   C_SyvUtil::h_GetViewDisplayName(this->mu32_ViewIndex, ms32_SUBMODE_SYSVIEW_UPDATE, c_SubMode, c_SubSubMode);
   Q_EMIT this->SigSelect(this, ms32_SUBMODE_SYSVIEW_UPDATE, c_SubMode, c_SubSubMode);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle dashboard item click

   \created     13.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagViewItem::m_OnDashboardClicked(void)
{
   QString c_SubMode;
   QString c_SubSubMode;

   C_SyvUtil::h_GetViewDisplayName(this->mu32_ViewIndex, ms32_SUBMODE_SYSVIEW_DASHBOARD, c_SubMode, c_SubSubMode);
   Q_EMIT this->SigSelect(this, ms32_SUBMODE_SYSVIEW_DASHBOARD, c_SubMode, c_SubSubMode);
}
