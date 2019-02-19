//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for upper toolbar with dynamic buttons

   Toolbar offers dynamic buttons for specific functions.
   This widget is designed in a ui file.

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>

#include "stwtypes.h"

#include "C_NagToolBarWidget.h"
#include "ui_C_NagToolBarWidget.h"

#include "C_GtGetText.h"
#include "C_HeHandler.h"
#include "C_OgeWiUtil.h"
#include "constants.h"
#include "C_OgeWiCustomMessage.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;
using namespace stw_types;

/* -- Module Global Constants ---------------------------------------------- */
const stw_types::sintn C_NagToolBarWidget::mhsn_SPACER_SIZE = 24;

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

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_NagToolBarWidget::C_NagToolBarWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_NagToolBarWidget),
   mpc_ActUseCaseWidget(NULL),
   mq_FocusInSearchResultWidget(false),
   mq_ShowSearch(false)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_LabelSearchImg->SetSvg("://images/IconSearch.svg");
   this->mpc_Ui->pc_BtnCancelSearch->setIcon(QIcon(":images/CheckBoxDisabled.png"));

   //init mc_SearchResultWidget
   this->mc_SearchResultWidget.setVisible(false);
   this->mpc_Ui->pc_BtnCancelSearch->setVisible(false);

   this->SetDarkTheme();
   this->InitText();

   //Help
   this->mpc_Ui->pc_BtnHelp->setIconSize(mc_ICON_SIZE_24);
   this->mpc_Ui->pc_BtnHelp->SetToolTipInformation(C_GtGetText::h_GetText("Help"),
                                                   C_GtGetText::h_GetText("Open user manual for help."));
   connect(this->mpc_Ui->pc_BtnHelp, &QPushButton::clicked, this, &C_NagToolBarWidget::m_TriggerHelp);

   //search connects
   connect(this->mpc_Ui->pc_BtnCancelSearch, &QPushButton::clicked, this, &C_NagToolBarWidget::m_CancelSearch);
   // Forwarding the signal
   connect(&this->mc_SearchResultWidget, &C_NagToolBarSearchResults::SigChangeMode, this,
           &C_NagToolBarWidget::m_ChangeMode);
   connect(&this->mc_SearchResultWidget, &C_NagToolBarSearchResults::SigOpenDetail, this,
           &C_NagToolBarWidget::SigOpenDetail);
   connect(&this->mc_SearchResultWidget, &C_NagToolBarSearchResults::SigHide, this,
           &C_NagToolBarWidget::m_Hide);
   connect(&this->mc_SearchResultWidget, &C_NagToolBarSearchResults::SigFocusOut, this,
           &C_NagToolBarWidget::m_FocusOut);

   this->mpc_Ui->pc_GroupBoxSearch->installEventFilter(this);
   this->mpc_Ui->pc_LineEditSearch->installEventFilter(this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_NagToolBarWidget::~C_NagToolBarWidget()
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because mpc_ActUseCaseWidget is not created and removed here
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initializes all visible strings on the widget

   \created     07.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::InitText(void) const
{
   this->mpc_Ui->pc_LineEditSearch->setPlaceholderText(C_GtGetText::h_GetText("Search in SYSTEM DEFINITION"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Configures the function buttons

   \created     12.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void stw_opensyde_gui::C_NagToolBarWidget::ConfigureButtons(C_NagUseCaseWidget * const opc_UseCaseWidget)
{
   QVector<C_NagToolBarButtonProperties> c_VecFuncs;
   QVector<C_NagToolBarButtonProperties>::const_iterator pc_ItFuncName;
   sintn sn_LayoutIndex  = 0;

   this->ResetButtons();

   this->mpc_ActUseCaseWidget = opc_UseCaseWidget;

   connect(this->mpc_ActUseCaseWidget, &C_NagUseCaseWidget::SigEnableUserInputFunc,
           this, &C_NagToolBarWidget::m_EnableBtn);
   connect(this->mpc_ActUseCaseWidget, &C_NagUseCaseWidget::SigShowUserInputFunc,
           this, &C_NagToolBarWidget::m_ShowBtn);
   connect(this->mpc_ActUseCaseWidget, &C_NagUseCaseWidget::SigShowBackgroundOfUserInputFunc,
           this, &C_NagToolBarWidget::m_ShowBackgroundOfBtn);
   connect(this->mpc_ActUseCaseWidget, &C_NagUseCaseWidget::SigSetIconForUserInputFunc,
           this, &C_NagToolBarWidget::m_SetIconForBtn);
   connect(this->mpc_ActUseCaseWidget, &C_NagUseCaseWidget::SigSetToolTipForUserInputFunc,
           this, &C_NagToolBarWidget::m_SetToolTipForBtn);

   // get function names
   c_VecFuncs = this->mpc_ActUseCaseWidget->GetVecToolBarFuncNames();

   // save configuration
   for (pc_ItFuncName = c_VecFuncs.begin(); pc_ItFuncName != c_VecFuncs.end(); ++pc_ItFuncName)
   {
      // create all necessary elements
      C_OgeGbxTransparent * const pc_GroupBox = new C_OgeGbxTransparent(this);
      QHBoxLayout * const pc_GroupBoxLayout = new QHBoxLayout();
      C_OgePubToolBar * const pc_Button = new C_OgePubToolBar(NULL, static_cast<uint32>(this->mc_VecButtons.count()));

      // configure the button
      pc_Button->setMinimumSize(77 + static_cast<sintn>(mh_GetButtonStringWidth((*pc_ItFuncName).c_ButtonText)), 40);
      //Add space because of spacing issue between icon and text
      pc_Button->setText(QString(' ') + (*pc_ItFuncName).c_ButtonText);
      pc_Button->SetToolTipInformation((*pc_ItFuncName).c_ToolTipHeading, (*pc_ItFuncName).c_ToolTipContent);
      //Fix minimum size depending on text current space only
      connect(pc_Button, &C_OgePubToolBar::SigClickedWithIndex, this, &C_NagToolBarWidget::m_BtnClicked);

      // save the items, which we need later
      this->mc_VecButtons.push_back(pc_Button);
      this->mc_VecGroupBox.push_back(pc_GroupBox);

      // add all elements to the layout
      pc_GroupBoxLayout->insertWidget(0, pc_Button);
      pc_GroupBoxLayout->insertSpacing(1, mhsn_SPACER_SIZE);

      // configure the layout
      pc_GroupBoxLayout->setSpacing(0);
      pc_GroupBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);
      pc_GroupBoxLayout->setStretch(0, 1);
      pc_GroupBoxLayout->setStretch(1, 0);
      pc_GroupBoxLayout->setContentsMargins(0, 0, 0, 0);
      pc_GroupBox->setLayout(pc_GroupBoxLayout);

      // add the groupbox to the main layout
      this->mpc_Ui->pc_HorizontalLayout->insertWidget(sn_LayoutIndex, pc_GroupBox);
      ++sn_LayoutIndex;
      //lint -e{429}  no memory leak because of parent of pc_GroupBox and pc_GroupBoxLayout and the Qt memory management
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Disables and hides all function buttons

   \created     12.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::ResetButtons(void)
{
   QVector<C_OgePubToolBar *>::iterator pc_ItButton;
   QVector<QWidget *>::iterator pc_ItGroupBox;

   // remove all buttons and its groupboxes
   for (pc_ItButton = this->mc_VecButtons.begin(); pc_ItButton != this->mc_VecButtons.end(); ++pc_ItButton)
   {
      // prepare the button for removing
      disconnect(*pc_ItButton, &C_OgePubToolBar::SigClickedWithIndex, this, &C_NagToolBarWidget::m_BtnClicked);
   }

   for (pc_ItGroupBox = this->mc_VecGroupBox.begin(); pc_ItGroupBox != this->mc_VecGroupBox.end(); ++pc_ItGroupBox)
   {
      this->mpc_Ui->pc_HorizontalLayout->removeWidget(*pc_ItGroupBox);

      // clean up
      delete (*pc_ItGroupBox);
   }

   this->mc_VecButtons.clear();
   this->mc_VecGroupBox.clear();
   this->mc_VecGroupBoxVisible.clear();

   // remove the pointer of the actual use case widget
   this->mpc_ActUseCaseWidget = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set top widget

   Necessary because of top level scroll area

   \param[in,out] opc_Widget Top widget

   \created     11.04.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::SetTopWidget(QWidget * const opc_Widget)
{
   this->mc_SearchResultWidget.setParent(opc_Widget);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Hide all registered buttons and saves the actual visible state for restoring

   \created     25.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::HideAllButtonsAndStoreState(void)
{
   QVector<QWidget *>::iterator pc_VecGroupBox;

   // hide all buttons and save their current visible state
   this->mc_VecGroupBoxVisible.clear();

   for (pc_VecGroupBox = this->mc_VecGroupBox.begin(); pc_VecGroupBox != this->mc_VecGroupBox.end(); ++pc_VecGroupBox)
   {
      this->mc_VecGroupBoxVisible.push_back((*pc_VecGroupBox)->isVisible());
      (*pc_VecGroupBox)->setVisible(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Restore the visible state of all buttons which are stored by the previous call of HideAllButtons

   \created     25.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::RestoreAllButtonsVisibility(void)
{
   if (this->mc_VecGroupBox.size() == this->mc_VecGroupBoxVisible.size())
   {
      sintn sn_Counter;
      for (sn_Counter = 0; sn_Counter < this->mc_VecGroupBox.size(); ++sn_Counter)
      {
         this->mc_VecGroupBox[sn_Counter]->setVisible(this->mc_VecGroupBoxVisible[sn_Counter]);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Showing the search

   \param[in]     oq_Active      Flag for showing search

   \created     05.07.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::ShowSearch(const bool oq_Active)
{
   this->mq_ShowSearch = oq_Active;
   this->mpc_Ui->pc_GroupBoxSearch->setVisible(oq_Active);
   //only hide search result widget
   if (oq_Active == false)
   {
      this->mc_SearchResultWidget.setVisible(oq_Active);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set search focus

   \created     15.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::Search(void) const
{
   this->mpc_Ui->pc_LineEditSearch->setFocus();
   this->mpc_Ui->pc_LineEditSearch->selectAll();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten event filter slot

   \param[in,out] opc_Object Sender object information
   \param[in,out] opc_Event  Event identification and information

   \return
   true     the event will be filtered
   false    the event will not be filtered

   \created     15.03.2017  STW/S.Singer
*/
//-----------------------------------------------------------------------------
bool C_NagToolBarWidget::eventFilter(QObject * const opc_Object, QEvent * const opc_Event)
{
   bool q_FilterEvent = false;

   if (opc_Object == this->mpc_Ui->pc_GroupBoxSearch)
   {
      if (opc_Event->type() == QEvent::Move)
      {
         //reposition search widget on window move
         this->m_RepositionSearchTree();
      }
   }
   else if (opc_Object == this->mpc_Ui->pc_LineEditSearch)
   {
      if (opc_Event->type() == QEvent::KeyRelease)
      {
         if (this->mpc_Ui->pc_LineEditSearch->text().isEmpty() == true)
         {
            //hide search widget and cancle button if there is nothing to search
            this->mpc_Ui->pc_BtnCancelSearch->setVisible(false);
            this->mc_SearchResultWidget.hide();
         }
         else
         {
            //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
            QKeyEvent * const pc_KeyEvent = dynamic_cast<QKeyEvent *>(opc_Event);
            if (pc_KeyEvent != NULL)
            {
               if (pc_KeyEvent->key() != static_cast<sintn>(Qt::Key_Escape))
               {
                  if (this->mpc_Ui->pc_LineEditSearch->text() == "openSYDE")
                  {
                     //never type openSYDE into openSYDE ;-)
                     C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::E_Type::eERROR);

                     c_MessageBox.SetHeading(C_GtGetText::h_GetText("Do not do this!"));
                     c_MessageBox.SetDescription(C_GtGetText::h_GetText("Never type openSYDE into openSYDE! ;-)"));
                     c_MessageBox.SetOKButtonText(C_GtGetText::h_GetText("Sorry, I'll never do this again"));

                     c_MessageBox.Execute();
                  }

                  //show on every key press except Escape (Escape hides the search window on keypress)
                  this->m_ShowSearchTree();
               }
            }

            //show cancel button if the search text is not empty
            this->mpc_Ui->pc_BtnCancelSearch->setVisible(true);
         }
         //start the search (on empty search string the results will be deleted)
         this->mc_SearchResultWidget.StartSearch(this->mpc_Ui->pc_LineEditSearch->text());
      }
      else if (opc_Event->type() == QEvent::KeyPress)
      {
         //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
         QKeyEvent * const pc_KeyEvent = dynamic_cast<QKeyEvent *>(opc_Event);
         if (pc_KeyEvent != NULL)
         {
            if (pc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Escape))
            {
               if (this->mc_SearchResultWidget.isVisible() == true)
               {
                  //Escape hides the search window on keypress (reason for keypress: instant hide)
                  this->mc_SearchResultWidget.hide();
               }
               else
               {
                  //if search result widget is allredy hidden, the escape click = cancel click
                  this->mpc_Ui->pc_BtnCancelSearch->click();
               }
               //Accept to not accidentally trigger something else
               q_FilterEvent = true;
            }
            else if (pc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Down))
            {
               if ((this->mc_SearchResultWidget.isVisible() == true) &&
                   (this->mc_SearchResultWidget.HasResults() == true))
               {
                  this->mq_FocusInSearchResultWidget = true;
                  this->mc_SearchResultWidget.SetSearchResultFocus();
               }
            }
            else
            {
               // nothing to do, just an other key event
            }
         }
      }
      else if (opc_Event->type() == QEvent::MouseButtonPress)
      {
         if (this->mpc_Ui->pc_LineEditSearch->text().isEmpty() == false)
         {
            //show current search results on mouse click if the search text ist not empty
            this->m_ShowSearchTree();
         }
      }
      else if (opc_Event->type() == QEvent::FocusOut)
      {
         if (this->mq_FocusInSearchResultWidget == false)
         {
            //hide search widget on every focus out event. Normally if the user clicks on an other ui object.
            if (this->mc_SearchResultWidget.HasChildFocus() == false)
            {
               this->mc_SearchResultWidget.hide();
            }
         }
      }
      else if (opc_Event->type() == QEvent::FocusIn)
      {
         if (this->mpc_Ui->pc_LineEditSearch->text().isEmpty() == false)
         {
            // update the search result
            this->mc_SearchResultWidget.StartSearch(this->mpc_Ui->pc_LineEditSearch->text());
         }
      }
      else
      {
         // nothing to do, just an other event
      }
   }
   else
   {
      // nothing to do, just an other event
   }

   //if true: the event is filtered and is not handled furter. False is default.
   return q_FilterEvent;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot function for button

   \created     12.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_BtnClicked(const uint32 ou32_Index)
{
   if (this->mpc_ActUseCaseWidget != NULL)
   {
      this->mpc_ActUseCaseWidget->UserInputFunc(ou32_Index);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Trigger help

   \created     04.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_TriggerHelp(void)
{
   //lint -e{10,48,64,746,1013,1055} Will be defined via moc compiler, PC lint unable to handle this construct
   stw_opensyde_gui_logic::C_HeHandler::GetInstance().CallSpecificHelpPage(this->metaObject()->className());
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_ShowBtn(const uint32 ou32_Index, const bool oq_Show) const
{
   const sintn sn_Index = static_cast<sintn>(ou32_Index);

   if (this->mc_VecGroupBox.size() > sn_Index)
   {
      // change the state of the button
      this->mc_VecGroupBox.at(sn_Index)->setVisible(oq_Show);
   }
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_EnableBtn(const uint32 ou32_Index, const bool oq_Enable) const
{
   const sintn sn_Index = static_cast<sintn>(ou32_Index);

   if (this->mc_VecButtons.size() > sn_Index)
   {
      // hide the button
      this->mc_VecButtons.at(static_cast<sintn>(sn_Index))->setEnabled(oq_Enable);
   }
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_ShowBackgroundOfBtn(const uint32 ou32_Index, const bool oq_Enable) const
{
   const sintn sn_Index = static_cast<sintn>(ou32_Index);

   if (this->mc_VecButtons.size() > sn_Index)
   {
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mc_VecButtons.at(static_cast<sintn>(sn_Index)), "Background",
                                             oq_Enable);
   }
   if ((oq_Enable == false) && (ou32_Index == 1U))
   {
      QIcon c_IconEdit("://images/IconEditActive.svg");
      c_IconEdit.addPixmap(QPixmap("://src/images/IconEditDisabled.svg"), QIcon::Disabled);
      this->m_SetIconForBtn(1U, c_IconEdit);
   }
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_SetIconForBtn(const uint32 ou32_Index, const QIcon & orc_Icon) const
{
   const sintn sn_Index = static_cast<sintn>(ou32_Index);

   if (this->mc_VecButtons.size() > sn_Index)
   {
      // set the new icon
      this->mc_VecButtons.at(static_cast<sintn>(sn_Index))->setIcon(orc_Icon);
      this->mc_VecButtons.at(static_cast<sintn>(sn_Index))->setIconSize(mc_ICON_SIZE_24);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Updates the tool tip of the button

   \param[in]     ou32_Index            Index of button
   \param[in]     orc_ToolTipHeading    New tool tip heading
   \param[in]     orc_ToolTipContent    New tool tip content

   \created     06.02.2019  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_SetToolTipForBtn(const uint32 ou32_Index, const QString & orc_ToolTipHeading,
                                            const QString & orc_ToolTipContent) const
{
   const sintn sn_Index = static_cast<sintn>(ou32_Index);

   if (this->mc_VecButtons.size() > sn_Index)
   {
      // set the new icon
      this->mc_VecButtons.at(static_cast<sintn>(sn_Index))->SetToolTipInformation(orc_ToolTipHeading,
                                                                                  orc_ToolTipContent);
   }
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_ShowSearchTree(void)
{
   this->mc_SearchResultWidget.show();
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_RepositionSearchTree()
{
   QPoint c_ParentPos = this->mpc_Ui->pc_GroupBoxSearch->mapToGlobal(QPoint(0, 0));

   this->mc_SearchResultWidget.move(0, 0);

   c_ParentPos = this->mc_SearchResultWidget.mapFromGlobal(c_ParentPos);

   this->mc_SearchResultWidget.move(c_ParentPos.x(), c_ParentPos.y() + this->mpc_Ui->pc_GroupBoxSearch->height());
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_CancelSearch(void)
{
   //cancel search
   this->mpc_Ui->pc_LineEditSearch->setText("");
   this->mpc_Ui->pc_BtnCancelSearch->setVisible(false);
   this->mc_SearchResultWidget.ClearResult();
   this->mc_SearchResultWidget.hide();
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_ChangeMode(const sint32 os32_Mode, const sint32 os32_SubMode, const uint32 ou32_Index,
                                      const QString & orc_Name, const QString & orc_SubSubItemName,
                                      const uint32 ou32_Flag)
{
   this->mc_SearchResultWidget.hide();
   Q_EMIT this->SigChangeMode(os32_Mode, os32_SubMode, ou32_Index, orc_Name, orc_SubSubItemName, ou32_Flag);
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_Hide(void)
{
   this->mpc_Ui->pc_LineEditSearch->setFocus();
   this->mq_FocusInSearchResultWidget = false;
   this->mc_SearchResultWidget.hide();
}

//-----------------------------------------------------------------------------
void C_NagToolBarWidget::m_FocusOut(void)
{
   if (this->mpc_Ui->pc_LineEditSearch->hasFocus() == false)
   {
      this->mq_FocusInSearchResultWidget = false;
      this->mc_SearchResultWidget.hide();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get button string width

   \param[in] orc_Text Button text

   \return
   Button string width

   \created     29.03.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_NagToolBarWidget::mh_GetButtonStringWidth(const QString & orc_Text)
{
   sint32 s32_Retval;
   QFont c_Font("Segoe UI Semibold");

   c_Font.setPixelSize(13);

   if (orc_Text.compare("") == 0)
   {
      s32_Retval = 0;
   }
   else
   {
      const QFontMetrics c_Metric(c_Font);
      s32_Retval = c_Metric.width(orc_Text);
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the dark theme of the widget

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::SetDarkTheme(void)
{
   QPalette c_Pal;

   this->mpc_Ui->pc_BtnHelp->SetCustomIcons("://images/main_page_and_navi_bar/Icon_help_bright.svg",
                                            "://images/main_page_and_navi_bar/Icon_help_dark_Hovered.svg",
                                            "://images/main_page_and_navi_bar/Icon_help_dark_Clicked.svg",
                                            "://images/main_page_and_navi_bar/Icon_help_dark_Disabled.svg");
   c_Pal = this->palette();
   c_Pal.setColor(QPalette::Window, mc_STYLE_GUIDE_COLOR_2);
   c_Pal.setColor(QPalette::Base, mc_STYLE_GUIDE_COLOR_2);
   c_Pal.setColor(QPalette::Text, mc_STYLE_GUIDE_COLOR_9);
   c_Pal.setColor(QPalette::ButtonText, mc_STYLE_GUIDE_COLOR_10);
   c_Pal.setColor(QPalette::BrightText, mc_STYLE_GUIDE_COLOR_12);
   this->setPalette(c_Pal);
   //this->mpc_Ui->pc_LineEditSearch->SetDarkTheme();
   this->mpc_Ui->pc_LineEditSearch->setVisible(false);
   this->mpc_Ui->pc_LabelSearchImg->setVisible(false);
   this->mpc_Ui->pc_GroupBoxSearch->setVisible(false);
   this->mc_SearchResultWidget.setVisible(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the light theme of the widget

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagToolBarWidget::SetLightTheme(void)
{
   QPalette c_Pal;

   this->mpc_Ui->pc_BtnHelp->SetCustomIcons("://images/main_page_and_navi_bar/Icon_help_dark.svg",
                                            "://images/main_page_and_navi_bar/Icon_help_dark_Hovered.svg",
                                            "://images/main_page_and_navi_bar/Icon_help_dark_Clicked.svg",
                                            "://images/main_page_and_navi_bar/Icon_help_dark_Disabled.svg");
   c_Pal = this->palette();
   c_Pal.setColor(QPalette::Window, mc_STYLE_GUIDE_COLOR_0);
   c_Pal.setColor(QPalette::Base, mc_STYLE_GUIDE_COLOR_0);
   c_Pal.setColor(QPalette::Text, mc_STYLE_GUIDE_COLOR_4);
   c_Pal.setColor(QPalette::ButtonText, mc_STYLE_GUIDE_COLOR_4);
   c_Pal.setColor(QPalette::BrightText, mc_STYLE_GUIDE_COLOR_10);
   this->setPalette(c_Pal);
   //this->mpc_Ui->pc_LineEditSearch->SetLightTheme();
   this->mpc_Ui->pc_LineEditSearch->setVisible(true);
   this->mpc_Ui->pc_LabelSearchImg->setVisible(true);
   this->mpc_Ui->pc_GroupBoxSearch->setVisible(this->mq_ShowSearch);
   //only hide search result widget
   if (this->mq_ShowSearch == false)
   {
      this->mc_SearchResultWidget.setVisible(this->mq_ShowSearch);
   }
}
