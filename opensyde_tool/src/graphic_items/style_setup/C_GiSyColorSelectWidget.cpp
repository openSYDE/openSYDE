//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog widget for color selection.

   The user can select a color from a predefined list, from recently selected colors or completely free by
   HEX or RGB values. Furthermore with a color picker "pipette" functionality the user can pick a color from
   the hole screen or screens.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QString>
#include <QRegExpValidator>

#include "C_GiSyColorSelectWidget.hpp"
#include "ui_C_GiSyColorSelectWidget.h"

#include "stwtypes.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_GiSyScreenColorPickingEventFilter.hpp"
#include "C_GiSyColorPicker.hpp"
#include "C_GiSyColorBrightnessPicker.hpp"
#include "C_UtiStyleSheets.hpp"
#include "C_UsHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString mc_PATH_ROUND_BACKGROUND_IMG = ":/images/graphic_items/TransparentRoundBackground.png";
const QString mc_PATH_ROUND_SMALL_BACKGROUND_IMG = ":/images/graphic_items/TransparentRoundSmallBackground.png";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent    Reference to parent
   \param[in]      oc_Color      Color from previous widget
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorSelectWidget::C_GiSyColorSelectWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                 const QColor oc_Color) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_GiSyColorSelectWidget),
   mrc_ParentDialog(orc_Parent),
   mc_PreviousColor(oc_Color),
   mq_ColorPickingActive(false),
   mpc_ScreenColorPickingEventFilter(NULL),
   ms32_NextRecentButton(C_UsHandler::h_GetInstance()->GetNextRecentColorButtonNumber())
{
   const QVector<QColor> c_RecentColors = C_UsHandler::h_GetInstance()->GetRecentColors();

   this->mc_RecentColorNr1 = c_RecentColors.at(0);
   this->mc_RecentColorNr2 = c_RecentColors.at(1);
   this->mc_RecentColorNr3 = c_RecentColors.at(2);
   this->mc_RecentColorNr4 = c_RecentColors.at(3);
   this->mc_RecentColorNr5 = c_RecentColors.at(4);
   this->mc_RecentColorNr6 = c_RecentColors.at(5);

   this->mpc_Ui->setupUi(this);

   this->setFocusPolicy(Qt::StrongFocus);

   this->mpc_UpdateTimer = new QTimer(this);

   this->mc_DummyTransparentWindow.resize(1, 1);
   this->mc_DummyTransparentWindow.setFlags(Qt::Tool | Qt::FramelessWindowHint);

   // Add the color picker to the color select widget
   this->mpc_ColorPicker = new C_GiSyColorPicker(this);
   this->mpc_Ui->pc_horizontalLayout_5->addWidget(mpc_ColorPicker);

   // Add the color brightness picker to the color select widget
   this->mpc_ColorBrightnessPicker = new C_GiSyColorBrightnessPicker(this);
   this->mpc_Ui->pc_horizontalLayout_5->addWidget(mpc_ColorBrightnessPicker);

   // Get the previous color with the alpha value from the line widget
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_PreviousColor.alpha());
   this->m_SetCurrentRgbColor(this->mc_PreviousColor.rgba());

   this->m_InitStaticNames();
   this->m_InitStandardColors();
   this->m_InitElements();
   this->m_InitStandardColorButtons();
   this->m_SetTransparentBackground();

   // Register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   this->m_InitConnects();
   //lint -e{1746} Necessary because needs default parameter and is not recognized as const
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_ScreenColorPickingEventFilter and the Qt memory
// management
C_GiSyColorSelectWidget::~C_GiSyColorSelectWidget(void)
{
   delete this->mpc_Ui;
   delete this->mpc_UpdateTimer;
   delete this->mpc_ColorPicker;
   delete this->mpc_ColorBrightnessPicker;
   delete this->mpc_LabelTransparentColorShower;
   delete this->mpc_LabelTransparentRecentColorNr1;
   delete this->mpc_LabelTransparentRecentColorNr2;
   delete this->mpc_LabelTransparentRecentColorNr3;
   delete this->mpc_LabelTransparentRecentColorNr4;
   delete this->mpc_LabelTransparentRecentColorNr5;
   delete this->mpc_LabelTransparentRecentColorNr6;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of which color is selected

   \return
   QColor   Current color of color shower
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyColorSelectWidget::ChooseSelectedColor(void) const
{
   return this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handling the mouse movement during screen color picking

   \param[in] opc_MouseEvent   Event identification and information

   \return
   bool  If handle color picking mouse move
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSyColorSelectWidget::HandleColorPickingMouseMove(const QMouseEvent * const opc_MouseEvent)
{
   //If the cross is visible the grabbed color will be black most of the times
   //lint -e{730}  Allow the circle is visible (Use clean Qt interface)
   this->mpc_ColorPicker->SetCircleVisible(!mpc_ColorPicker->geometry().contains(opc_MouseEvent->pos()));
   this->m_UpdateColorPicking(opc_MouseEvent->globalPos());
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handling when the mouse button is released during screen color picking

   \param[in] opc_MouseEvent   Event identification and information

   \return
   bool  If handle color picking mouse button release
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSyColorSelectWidget::HandleColorPickingMouseButtonRelease(const QMouseEvent * const opc_MouseEvent)
{
   this->m_SetCurrentRgbColor(m_GrabScreenColor(opc_MouseEvent->globalPos()).rgba());
   this->m_LeaveColorPicking();
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   // Handle all enter key cases manually
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

   // Handle cancel key while color picking on whole screen is active
   if (this->mq_ColorPickingActive == true)
   {
      if (opc_KeyEvent->matches(QKeySequence::Cancel))
      {
         q_CallOrg = false;
         this->m_LeaveColorPicking();
         this->m_SetCurrentRgbColor(mc_ColorBeforeScreenColorPicking.rgba());
      }
      else
      {
         q_CallOrg = true;
      }
   }

   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten show event slot

   Here: Handle specific resizes and moves (from transparent background to buttons)

   \param[in,out] opc_Event Event identification and information, but not used
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::showEvent(QShowEvent * const opc_Event)
{
   this->mpc_LabelTransparentColorShower->resize(this->mpc_Ui->pc_PushButtonColorShower->size());
   this->mpc_LabelTransparentColorShower->move(this->mpc_Ui->pc_PushButtonColorShower->pos());
   this->mpc_LabelTransparentRecentColorNr1->resize(this->mpc_Ui->pc_PushButtonRecentColorNr1->size());
   this->mpc_LabelTransparentRecentColorNr1->move(this->mpc_Ui->pc_PushButtonRecentColorNr1->pos());
   this->mpc_LabelTransparentRecentColorNr2->resize(this->mpc_Ui->pc_PushButtonRecentColorNr2->size());
   this->mpc_LabelTransparentRecentColorNr2->move(this->mpc_Ui->pc_PushButtonRecentColorNr2->pos());
   this->mpc_LabelTransparentRecentColorNr3->resize(this->mpc_Ui->pc_PushButtonRecentColorNr3->size());
   this->mpc_LabelTransparentRecentColorNr3->move(this->mpc_Ui->pc_PushButtonRecentColorNr3->pos());
   this->mpc_LabelTransparentRecentColorNr4->resize(this->mpc_Ui->pc_PushButtonRecentColorNr4->size());
   this->mpc_LabelTransparentRecentColorNr4->move(this->mpc_Ui->pc_PushButtonRecentColorNr4->pos());
   this->mpc_LabelTransparentRecentColorNr5->resize(this->mpc_Ui->pc_PushButtonRecentColorNr5->size());
   this->mpc_LabelTransparentRecentColorNr5->move(this->mpc_Ui->pc_PushButtonRecentColorNr5->pos());
   this->mpc_LabelTransparentRecentColorNr6->resize(this->mpc_Ui->pc_PushButtonRecentColorNr6->size());
   this->mpc_LabelTransparentRecentColorNr6->move(this->mpc_Ui->pc_PushButtonRecentColorNr6->pos());

   QWidget::showEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Color"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Selection"));
   this->mpc_Ui->pc_LabelHeadingPreviewStandardColors->setText(C_GtGetText::h_GetText("Standard Colors"));
   this->mpc_Ui->pc_LabelHeadingPreviewRecentColors->setText(C_GtGetText::h_GetText("Recent Colors"));
   this->mpc_Ui->pc_PushButtonHtml->setText(C_GtGetText::h_GetText("HTML"));
   this->mpc_Ui->pc_LabelPipeSymbol->setText(C_GtGetText::h_GetText("|"));
   this->mpc_Ui->pc_PushButtonRgba->setText(C_GtGetText::h_GetText("RGBA"));
   this->mpc_Ui->pc_LabelNote->setText(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all standard colors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_InitStandardColors(void)
{
   this->mc_LightBlue = QColor(109, 166, 232);
   this->mc_Green = mc_STYLE_GUIDE_COLOR_13;
   this->mc_Gold = QColor(249, 217, 72);
   this->mc_Orange = QColor(248, 179, 27);
   this->mc_Red = mc_STYLE_GUIDE_COLOR_24;
   this->mc_Purple = mc_STYLE_GUIDE_COLOR_7;
   this->mc_LightGreen = mc_STYLE_GUIDE_COLOR_22;
   this->mc_DarkBlueGreen = mc_STYLE_GUIDE_COLOR_26;
   this->mc_Lavendel = mc_STYLE_GUIDE_COLOR_11;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static colors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_InitElements(void) const
{
   this->mpc_Ui->pc_PushButtonLightBlue->SetColors(this->mc_LightBlue, QColor(87, 106, 164), QColor(99, 147, 203));
   this->mpc_Ui->pc_PushButtonLightBlue->SetToolTipInformation(
      C_GtGetText::h_GetText("Light Blue"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_LightBlue.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_LightBlue.red()) +
                              "/" + QString::number(this->mc_LightBlue.green()) +
                              "/" + QString::number(this->mc_LightBlue.blue()) +
                              "/" + QString::number(this->mc_LightBlue.alpha())).
                             toStdString().c_str()));
   this->mpc_Ui->pc_PushButtonGreen->SetColors(this->mc_Green, QColor(101, 136, 70), QColor(117, 162, 88));
   this->mpc_Ui->pc_PushButtonGreen->SetToolTipInformation(
      C_GtGetText::h_GetText("Green"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_Green.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_Green.red()) +
                              "/" + QString::number(this->mc_Green.green()) +
                              "/" + QString::number(this->mc_Green.blue()) +
                              "/" + QString::number(this->mc_Green.alpha()))
                             .toStdString().c_str()));
   this->mpc_Ui->pc_PushButtonGold->SetColors(this->mc_Gold, QColor(181, 160, 67), QColor(219, 193, 72));
   this->mpc_Ui->pc_PushButtonGold->SetToolTipInformation(
      C_GtGetText::h_GetText("Gold"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_Gold.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_Gold.red()) +
                              "/" + QString::number(this->mc_Gold.green()) +
                              "/" + QString::number(this->mc_Gold.blue()) +
                              "/" + QString::number(this->mc_Gold.alpha()))
                             .toStdString().c_str()));
   this->mpc_Ui->pc_PushButtonOrange->SetColors(this->mc_Orange, QColor(153, 81, 38), QColor(211, 130, 30));
   this->mpc_Ui->pc_PushButtonOrange->SetToolTipInformation(
      C_GtGetText::h_GetText("Orange"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_Orange.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_Orange.red()) +
                              "/" + QString::number(this->mc_Orange.green()) +
                              "/" + QString::number(this->mc_Orange.blue()) +
                              "/" + QString::number(this->mc_Orange.alpha()))
                             .toStdString().c_str()));
   this->mpc_Ui->pc_PushButtonRed->SetColors(this->mc_Red, QColor(140, 32, 21), QColor(176, 55, 43));
   this->mpc_Ui->pc_PushButtonRed->SetToolTipInformation(
      C_GtGetText::h_GetText("Red"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_Red.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_Red.red()) +
                              "/" + QString::number(this->mc_Red.green()) +
                              "/" + QString::number(this->mc_Red.blue()) +
                              "/" + QString::number(this->mc_Red.alpha()))
                             .toStdString().c_str()));
   this->mpc_Ui->pc_PushButtonPurple->SetColors(this->mc_Purple, QColor(42, 40, 76), QColor(63, 61, 125));
   this->mpc_Ui->pc_PushButtonPurple->SetToolTipInformation(
      C_GtGetText::h_GetText("Purple"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_Purple.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_Purple.red()) +
                              "/" + QString::number(this->mc_Purple.green()) +
                              "/" + QString::number(this->mc_Purple.blue()) +
                              "/" + QString::number(this->mc_Purple.alpha()))
                             .toStdString().c_str()));
   this->mpc_Ui->pc_PushButtonLightGreen->SetColors(this->mc_LightGreen, QColor(169, 198, 145), QColor(193, 219, 180));
   this->mpc_Ui->pc_PushButtonLightGreen->SetToolTipInformation(
      C_GtGetText::h_GetText("Light Green"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_LightGreen.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_LightGreen.red()) +
                              "/" + QString::number(this->mc_LightGreen.green()) +
                              "/" + QString::number(this->mc_LightGreen.blue()) +
                              "/" + QString::number(this->mc_LightGreen.alpha()))
                             .toStdString().c_str()));
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->SetColors(this->mc_DarkBlueGreen, QColor(0, 100, 102),
                                                       QColor(0, 133, 113));
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->SetToolTipInformation(
      C_GtGetText::h_GetText("Dark Blue Green"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_DarkBlueGreen.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_DarkBlueGreen.red()) +
                              "/" + QString::number(this->mc_DarkBlueGreen.green()) +
                              "/" + QString::number(this->mc_DarkBlueGreen.blue()) +
                              "/" + QString::number(this->mc_DarkBlueGreen.alpha()))
                             .toStdString().c_str()));
   this->mpc_Ui->pc_PushButtonLavendel->SetColors(this->mc_Lavendel, QColor(177, 180, 179), QColor(215, 215, 215));
   this->mpc_Ui->pc_PushButtonLavendel->SetToolTipInformation(
      C_GtGetText::h_GetText("Lavendel"),
      C_GtGetText::h_GetText(("HTML: " + this->mc_Lavendel.name().toUpper() +
                              "\nRGBA: " + QString::number(this->mc_Lavendel.red()) +
                              "/" + QString::number(this->mc_Lavendel.green()) +
                              "/" + QString::number(this->mc_Lavendel.blue()) +
                              "/" + QString::number(this->mc_Lavendel.alpha()))
                             .toStdString().c_str()));

   this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr1, this->mc_RecentColorNr1);
   this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr2, this->mc_RecentColorNr2);
   this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr3, this->mc_RecentColorNr3);
   this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr4, this->mc_RecentColorNr4);
   this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr5, this->mc_RecentColorNr5);
   this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr6, this->mc_RecentColorNr6);

   this->mpc_Ui->pc_PushButtonHtml->setCursor(Qt::PointingHandCursor);
   this->mpc_Ui->pc_LabelPipeSymbol->SetForegroundColor(6);
   this->mpc_Ui->pc_PushButtonRgba->setCursor(Qt::PointingHandCursor);

   this->mpc_Ui->pc_PushButtonIcon->setFocusPolicy(Qt::NoFocus);
   this->mpc_Ui->pc_PushButtonIcon->setIcon(QIcon(":/images/graphic_items/Icon_pipette.svg"));
   this->mpc_Ui->pc_PushButtonIcon->SetToolTipInformation(C_GtGetText::h_GetText("Screen color picking"),
                                                          C_GtGetText::h_GetText(
                                                             "Click on the pipette, then you can pick a color of "
                                                             "your choice from your screen or screens.\n"
                                                             "When you click, a cross will be visible and you can move with "
                                                             "your mouse over the screens.\n"
                                                             "If you have found the right color, click with the cross on it. "
                                                             "This color will be shown on the color shower until "
                                                             "you choose a new one."));
   this->mpc_Ui->pc_LineEditColor->setStyleSheet("QLineEdit {"
                                                 "background-color: rgb(250, 250, 250);"
                                                 "border: none;"
                                                 "color: rgb(86, 86, 104);"
                                                 "selection-background-color: rgb(195, 186, 221);"
                                                 " selection-color: rgb(42, 42, 91);"
                                                 "}");

   this->mpc_Ui->pc_LabelNote->SetForegroundColor(6);

   this->m_RepaintColorShowerButton();
   this->m_HtmlClicked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add all standard color buttons to a vector
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_InitStandardColorButtons(void)
{
   QVector<stw::opensyde_gui_elements::C_OgePubColor *>::iterator pc_ItButton;

   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonLightBlue);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonGreen);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonGold);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonOrange);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonRed);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonPurple);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonLightGreen);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonDarkBlueGreen);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonLavendel);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonRecentColorNr1);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonRecentColorNr2);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonRecentColorNr3);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonRecentColorNr4);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonRecentColorNr5);
   this->mc_StandardColorButtons.push_back(this->mpc_Ui->pc_PushButtonRecentColorNr6);

   for (pc_ItButton = this->mc_StandardColorButtons.begin();
        pc_ItButton != this->mc_StandardColorButtons.end();
        ++pc_ItButton)
   {
      (*pc_ItButton)->setFocusPolicy(Qt::NoFocus);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set a background if the color has a transparency
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_SetTransparentBackground(void)
{
   QPixmap c_Pixmap;

   c_Pixmap.load(mc_PATH_ROUND_BACKGROUND_IMG);
   this->mpc_LabelTransparentColorShower = new QLabel(dynamic_cast<QWidget *>
                                                      (this->mpc_Ui->pc_PushButtonColorShower->parent()));
   this->mpc_LabelTransparentColorShower->setPixmap(c_Pixmap);
   this->mpc_LabelTransparentColorShower->lower();

   c_Pixmap.load(mc_PATH_ROUND_SMALL_BACKGROUND_IMG);
   this->mpc_LabelTransparentRecentColorNr1 = new QLabel(dynamic_cast<QWidget *>
                                                         (this->mpc_Ui->pc_PushButtonRecentColorNr1->parent()));
   this->mpc_LabelTransparentRecentColorNr1->setPixmap(c_Pixmap);
   this->mpc_LabelTransparentRecentColorNr1->lower();
   this->mpc_LabelTransparentRecentColorNr2 = new QLabel(dynamic_cast<QWidget *>
                                                         (this->mpc_Ui->pc_PushButtonRecentColorNr2->parent()));
   this->mpc_LabelTransparentRecentColorNr2->setPixmap(c_Pixmap);
   this->mpc_LabelTransparentRecentColorNr2->lower();
   this->mpc_LabelTransparentRecentColorNr3 = new QLabel(dynamic_cast<QWidget *>
                                                         (this->mpc_Ui->pc_PushButtonRecentColorNr3->parent()));
   this->mpc_LabelTransparentRecentColorNr3->setPixmap(c_Pixmap);
   this->mpc_LabelTransparentRecentColorNr3->lower();
   this->mpc_LabelTransparentRecentColorNr4 = new QLabel(dynamic_cast<QWidget *>
                                                         (this->mpc_Ui->pc_PushButtonRecentColorNr4->parent()));
   this->mpc_LabelTransparentRecentColorNr4->setPixmap(c_Pixmap);
   this->mpc_LabelTransparentRecentColorNr4->lower();
   this->mpc_LabelTransparentRecentColorNr5 = new QLabel(dynamic_cast<QWidget *>
                                                         (this->mpc_Ui->pc_PushButtonRecentColorNr5->parent()));
   this->mpc_LabelTransparentRecentColorNr5->setPixmap(c_Pixmap);
   this->mpc_LabelTransparentRecentColorNr5->lower();
   this->mpc_LabelTransparentRecentColorNr6 = new QLabel(dynamic_cast<QWidget *>
                                                         (this->mpc_Ui->pc_PushButtonRecentColorNr6->parent()));
   this->mpc_LabelTransparentRecentColorNr6->setPixmap(c_Pixmap);
   this->mpc_LabelTransparentRecentColorNr6->lower();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all connects
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_InitConnects(void) const
{
   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_CancelClicked);

   connect(this->mpc_Ui->pc_PushButtonLightBlue, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_LightBlueClicked);
   connect(this->mpc_Ui->pc_PushButtonGreen, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_GreenClicked);
   connect(this->mpc_Ui->pc_PushButtonGold, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_GoldClicked);
   connect(this->mpc_Ui->pc_PushButtonOrange, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_OrangeClicked);
   connect(this->mpc_Ui->pc_PushButtonRed, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_RedClicked);
   connect(this->mpc_Ui->pc_PushButtonPurple, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_PurpleClicked);
   connect(this->mpc_Ui->pc_PushButtonLightGreen, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_LightGreenClicked);
   connect(this->mpc_Ui->pc_PushButtonDarkBlueGreen, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_DarkBlueGreenClicked);
   connect(this->mpc_Ui->pc_PushButtonLavendel, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_LavendelClicked);

   connect(this->mpc_Ui->pc_PushButtonRecentColorNr1, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_RecentColorNr1Clicked);
   connect(this->mpc_Ui->pc_PushButtonRecentColorNr2, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_RecentColorNr2Clicked);
   connect(this->mpc_Ui->pc_PushButtonRecentColorNr3, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_RecentColorNr3Clicked);
   connect(this->mpc_Ui->pc_PushButtonRecentColorNr4, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_RecentColorNr4Clicked);
   connect(this->mpc_Ui->pc_PushButtonRecentColorNr5, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_RecentColorNr5Clicked);
   connect(this->mpc_Ui->pc_PushButtonRecentColorNr6, &QPushButton::clicked,
           this, &C_GiSyColorSelectWidget::m_RecentColorNr6Clicked);

   connect(this->mpc_Ui->pc_PushButtonHtml, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_HtmlClicked);
   connect(this->mpc_Ui->pc_PushButtonRgba, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_RgbaClicked);
   // The screen color picker button
   connect(this->mpc_Ui->pc_PushButtonIcon, &QPushButton::clicked, this, &C_GiSyColorSelectWidget::m_PickScreenColor);

   connect(this->mpc_ColorPicker, &C_GiSyColorPicker::SigNewColor,
           mpc_ColorBrightnessPicker, &C_GiSyColorBrightnessPicker::SetColorWithHs);
   connect(this->mpc_ColorBrightnessPicker, &C_GiSyColorBrightnessPicker::SigNewHsv,
           this, &C_GiSyColorSelectWidget::m_NewHsv);
   connect(this->mpc_ColorBrightnessPicker, &C_GiSyColorBrightnessPicker::SigNewHsv,
           this, &C_GiSyColorSelectWidget::m_UpdateLineEditColor);
   connect(this->mpc_Ui->pc_PushButtonColorShower, &C_GiSyColorShower::SigChangeColor,
           this, &C_GiSyColorSelectWidget::m_RepaintColorShowerButton);

   connect(this->mpc_UpdateTimer, &QTimer::timeout,
           this, &C_GiSyColorSelectWidget::m_UpdatePositionWhileScreenColorPicking);

   connect(this->mpc_Ui->pc_LineEditColor, &QLineEdit::textEdited, this, &C_GiSyColorSelectWidget::m_NewColor);
   connect(this->mpc_Ui->pc_LineEditColor, &QLineEdit::returnPressed,
           this, &C_GiSyColorSelectWidget::m_AutomaticallyFillOutLineEditColor);
   connect(this->mpc_Ui->pc_LineEditColor, &QLineEdit::returnPressed,
           this, &C_GiSyColorSelectWidget::m_OkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the current color to the color shower button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RepaintColorShowerButton(void) const
{
   const int32_t s32_Red = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor().red();
   const int32_t s32_Green = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor().green();
   const int32_t s32_Blue = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor().blue();

   int32_t s32_RedDarker = s32_Red - ((s32_Red * 20) / 100);
   int32_t s32_GreenDarker = s32_Green - ((s32_Green * 20) / 100);
   int32_t s32_BlueDarker = s32_Blue - ((s32_Blue * 20) / 100);

   s32_RedDarker = (s32_RedDarker > 255) ? 255 : s32_RedDarker;
   s32_GreenDarker = (s32_RedDarker > 255) ? 255 : s32_GreenDarker;
   s32_BlueDarker = (s32_RedDarker > 255) ? 255 : s32_BlueDarker;

   this->mpc_Ui->pc_PushButtonColorShower->setStyleSheet("\
                            stw--opensyde_gui--C_GiSyColorShower {\
                                background-color: " +
                                                         this->mpc_Ui->pc_PushButtonColorShower->CurrentColor().name() + ";\
                                border-top-color: rgb(" + QString::number(
                                                            s32_RedDarker) + ", \
                                                      " + QString::number(
                                                            s32_GreenDarker) + ", \
                                                      " + QString::number(
                                                            s32_BlueDarker) + ");\
                                border-right-color: rgb(" + QString::number(
                                                            s32_RedDarker) + ", \
                                                        " + QString::number(
                                                            s32_GreenDarker) + ", \
                                                        " + QString::number(
                                                            s32_BlueDarker) + ");\
                                border-bottom-color: rgb(" + QString::number(
                                                            s32_RedDarker) + ", \
                                                         " + QString::number(
                                                            s32_GreenDarker) + ", \
                                                         " + QString::number(
                                                            s32_BlueDarker) + ");\
                                border-left-color: rgb(" + QString::number(
                                                            s32_RedDarker) + ", \
                                                       " + QString::number(
                                                            s32_GreenDarker) + ", \
                                                       " + QString::number(
                                                            s32_BlueDarker) + ");\
                            }\
                            ");
   QString c_Style = this->mpc_Ui->pc_PushButtonColorShower->styleSheet();
   C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, this->mpc_Ui->pc_PushButtonColorShower->CurrentColor());
   this->mpc_Ui->pc_PushButtonColorShower->setStyleSheet(c_Style);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_OkClicked(void)
{
   this->m_AddRecentColors();
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set all buttons are checked to false except the one clicked

   \param[in,out] orc_Button Reference to the color buttons
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_StandardColorClicked(const C_OgePubColor * const opc_Button)
{
   QVector<stw::opensyde_gui_elements::C_OgePubColor *>::iterator pc_ItButton;
   for (pc_ItButton = this->mc_StandardColorButtons.begin();
        pc_ItButton != this->mc_StandardColorButtons.end();
        ++pc_ItButton)
   {
      if ((*pc_ItButton) != opc_Button)
      {
         (*pc_ItButton)->setChecked(false);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of blue button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_LightBlueClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonLightBlue);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_LightBlue.alpha());
   this->m_NewHsv(this->mc_LightBlue.hue(), this->mc_LightBlue.saturation(), this->mc_LightBlue.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_LightBlue);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of green button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_GreenClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonGreen);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_Green.alpha());
   this->m_NewHsv(this->mc_Green.hue(), this->mc_Green.saturation(), this->mc_Green.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_Green);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of gold button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_GoldClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonGold);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_Gold.alpha());
   this->m_NewHsv(this->mc_Gold.hue(), this->mc_Gold.saturation(), this->mc_Gold.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_Gold);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of orange button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_OrangeClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonOrange);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_Orange.alpha());
   this->m_NewHsv(this->mc_Orange.hue(), this->mc_Orange.saturation(), this->mc_Orange.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_Orange);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of red button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RedClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonRed);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_Red.alpha());
   this->m_NewHsv(this->mc_Red.hue(), this->mc_Red.saturation(), this->mc_Red.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_Red);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of purple button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_PurpleClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonPurple);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_Purple.alpha());
   this->m_NewHsv(this->mc_Purple.hue(), this->mc_Purple.saturation(), this->mc_Purple.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_Purple);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of light green button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_LightGreenClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonLightGreen);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_LightGreen.alpha());
   this->m_NewHsv(this->mc_LightGreen.hue(), this->mc_LightGreen.saturation(), this->mc_LightGreen.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_LightGreen);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of dark blue green button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_DarkBlueGreenClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonDarkBlueGreen);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_DarkBlueGreen.alpha());
   this->m_NewHsv(this->mc_DarkBlueGreen.hue(), this->mc_DarkBlueGreen.saturation(), this->mc_DarkBlueGreen.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_DarkBlueGreen);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of lavendel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_LavendelClicked(void)
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonLavendel);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_Lavendel.alpha());
   this->m_NewHsv(this->mc_Lavendel.hue(), this->mc_Lavendel.saturation(), this->mc_Lavendel.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_Lavendel);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of recent color number one button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RecentColorNr1Clicked()
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonRecentColorNr1);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_RecentColorNr1.alpha());
   this->m_NewHsv(this->mc_RecentColorNr1.hue(), this->mc_RecentColorNr1.saturation(), this->mc_RecentColorNr1.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_RecentColorNr1);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of recent color number two button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RecentColorNr2Clicked()
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonRecentColorNr2);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_RecentColorNr2.alpha());
   this->m_NewHsv(this->mc_RecentColorNr2.hue(), this->mc_RecentColorNr2.saturation(), this->mc_RecentColorNr2.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_RecentColorNr2);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of recent color number three button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RecentColorNr3Clicked()
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonRecentColorNr3);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_RecentColorNr3.alpha());
   this->m_NewHsv(this->mc_RecentColorNr3.hue(), this->mc_RecentColorNr3.saturation(), this->mc_RecentColorNr3.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_RecentColorNr3);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of recent color number four button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RecentColorNr4Clicked()
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonRecentColorNr4);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_RecentColorNr4.alpha());
   this->m_NewHsv(this->mc_RecentColorNr4.hue(), this->mc_RecentColorNr4.saturation(), this->mc_RecentColorNr4.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_RecentColorNr4);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of recent color number five button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RecentColorNr5Clicked()
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonRecentColorNr5);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_RecentColorNr5.alpha());
   this->m_NewHsv(this->mc_RecentColorNr5.hue(), this->mc_RecentColorNr5.saturation(), this->mc_RecentColorNr5.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_RecentColorNr5);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of recent color number six button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RecentColorNr6Clicked()
{
   this->m_StandardColorClicked(this->mpc_Ui->pc_PushButtonRecentColorNr6);
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(this->mc_RecentColorNr6.alpha());
   this->m_NewHsv(this->mc_RecentColorNr6.hue(), this->mc_RecentColorNr6.saturation(), this->mc_RecentColorNr6.value());
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentColor(this->mc_RecentColorNr6);
   this->m_UpdateLineEditColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of html button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_HtmlClicked(void) const
{
   this->mpc_Ui->pc_PushButtonRgba->setChecked(false);
   this->mpc_Ui->pc_PushButtonHtml->setChecked(true);

   this->m_PushButtonHtml();
   this->mpc_Ui->pc_LineEditColor->setText(this->mpc_Ui->pc_PushButtonColorShower->CurrentColor().name().toUpper());
   QFont c_FontHtml;
   c_FontHtml.setBold(true);
   this->mpc_Ui->pc_PushButtonHtml->setFont(c_FontHtml);
   QFont c_FontRgba;
   c_FontRgba.setBold(false);
   this->mpc_Ui->pc_PushButtonRgba->setFont(c_FontRgba);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of rgba button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RgbaClicked(void) const
{
   this->mpc_Ui->pc_PushButtonHtml->setChecked(false);
   this->mpc_Ui->pc_PushButtonRgba->setChecked(true);

   this->m_PushButtonRgba();
   QFont c_FontRgba;
   c_FontRgba.setBold(true);
   this->mpc_Ui->pc_PushButtonRgba->setFont(c_FontRgba);
   QFont c_FontHtml;
   c_FontHtml.setBold(false);
   this->mpc_Ui->pc_PushButtonHtml->setFont(c_FontHtml);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initalize the html button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_PushButtonHtml(void) const
{
   this->mpc_Ui->pc_LineEditColor->setText(this->mpc_Ui->pc_PushButtonColorShower->CurrentColor().name().toUpper());
   this->mpc_Ui->pc_LineEditColor->setValidator(new QRegExpValidator(QRegExp("#([A-Fa-f0-9]{1,6})"),
                                                                     this->mpc_Ui->pc_LineEditColor));
   // set an empty input mask when the html button is checked
   this->mpc_Ui->pc_LineEditColor->setInputMask("");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initalize the rgba button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_PushButtonRgba(void) const
{
   const QColor c_HtmlColor = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();

   this->mpc_Ui->pc_LineEditColor->setText(QString::number(c_HtmlColor.red()) + "/" +
                                           QString::number(c_HtmlColor.green()) + "/" +
                                           QString::number(c_HtmlColor.blue()) + "/" +
                                           QString::number(c_HtmlColor.alpha()));
   // set validator for RGBA input (source: C_OgeLeIpAddress)
   const QString c_Range =
      "(([ 0]+)|"                                     // only spaces or zeros
      "([ 0]*[0-9] *)|"                               // one digit, e.g. "9  " or " 08" or " 1 "; but not " 10"
      "([0-9][0-9] )|([ 0][0-9][0-9])|([0-9] [0-9])|" // two digits, e.g. "99 " or " 25" or "087" or "8 3"
      "(1[0-9][0-9])|([2][0-4][0-9])|(25[0-5]))";     // three digits, e.g. "255" or "179"; but not "678"
   const QRegularExpression c_Regex("^" + c_Range +
                                    "/" + c_Range +
                                    "/" + c_Range +
                                    "/" + c_Range + "$");

   this->mpc_Ui->pc_LineEditColor->setValidator(new QRegularExpressionValidator(c_Regex,
                                                                                this->mpc_Ui->pc_LineEditColor));

   // use a mask in addition to the validator
   this->mpc_Ui->pc_LineEditColor->setInputMask("000/000/000/000");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update the line edit with the current color in hex or rgba spelling-wise
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_UpdateLineEditColor(void) const
{
   const QColor c_CurrentColor = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();

   if (this->mpc_Ui->pc_PushButtonHtml->font().bold() == true)
   {
      this->mpc_Ui->pc_LineEditColor->setText(c_CurrentColor.name().toUpper());
   }
   else if (this->mpc_Ui->pc_PushButtonRgba->font().bold() == true)
   {
      this->mpc_Ui->pc_LineEditColor->setText(QString::number(c_CurrentColor.red()) + "/" +
                                              QString::number(c_CurrentColor.green()) + "/" +
                                              QString::number(c_CurrentColor.blue()) + "/" +
                                              QString::number(c_CurrentColor.alpha()));
   }
   else
   {
      // nothing to do
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   A color is entered in line edit in hex or rgba notation
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_NewColor(void)
{
   QColor c_NewColorTypeIn;

   if (this->mpc_Ui->pc_PushButtonHtml->font().bold() == true)
   {
      if (this->mpc_Ui->pc_LineEditColor->text().isEmpty())
      {
         this->mpc_Ui->pc_LineEditColor->setText("#");
      }
      else
      {
         this->mpc_Ui->pc_LineEditColor->setText(this->mpc_Ui->pc_LineEditColor->text().toUpper());
      }
      c_NewColorTypeIn = static_cast<QColor>(this->mpc_Ui->pc_LineEditColor->text());
   }
   else if (this->mpc_Ui->pc_PushButtonRgba->font().bold() == true)
   {
      const QStringList c_StringColorList = this->mpc_Ui->pc_LineEditColor->text().split("/");
      c_NewColorTypeIn = QColor(c_StringColorList.at(0).toInt(), c_StringColorList.at(1).toInt(),
                                c_StringColorList.at(2).toInt(), c_StringColorList.at(3).toInt());
   }
   else
   {
      // nothing to do
   }
   this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(c_NewColorTypeIn.alpha());
   this->m_NewHsv(c_NewColorTypeIn.hue(), c_NewColorTypeIn.saturation(), c_NewColorTypeIn.value());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   When HTML button is bold and in edit line color the return key is pressed,
             then the length will be checked and the hex color filled in six places.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_AutomaticallyFillOutLineEditColor(void)
{
   if ((this->mpc_Ui->pc_PushButtonHtml->font().bold() == true) &&
       (this->mpc_Ui->pc_LineEditColor->text().length() < 7))
   {
      const QStringList c_StringColorList = this->mpc_Ui->pc_LineEditColor->text().split("");
      switch (this->mpc_Ui->pc_LineEditColor->text().length())
      {
      case 2:
         this->mpc_Ui->pc_LineEditColor->setText((c_StringColorList.at(1) + c_StringColorList.at(2) +
                                                  c_StringColorList.at(2) + c_StringColorList.at(2) +
                                                  c_StringColorList.at(2) + c_StringColorList.at(2) +
                                                  c_StringColorList.at(2)).toUpper());
         break;
      case 3:
         this->mpc_Ui->pc_LineEditColor->setText((c_StringColorList.at(1) + c_StringColorList.at(2) +
                                                  c_StringColorList.at(2) + c_StringColorList.at(2) +
                                                  c_StringColorList.at(3) + c_StringColorList.at(3) +
                                                  c_StringColorList.at(3)).toUpper());
         break;
      case 4:
         this->mpc_Ui->pc_LineEditColor->setText((c_StringColorList.at(1) + c_StringColorList.at(2) +
                                                  c_StringColorList.at(2) + c_StringColorList.at(3) +
                                                  c_StringColorList.at(3) + c_StringColorList.at(4) +
                                                  c_StringColorList.at(4)).toUpper());
         break;
      case 5:
         this->mpc_Ui->pc_LineEditColor->setText((c_StringColorList.at(1) + c_StringColorList.at(2) +
                                                  c_StringColorList.at(3) + c_StringColorList.at(4) +
                                                  c_StringColorList.at(4) + c_StringColorList.at(5) +
                                                  c_StringColorList.at(5)).toUpper());
         break;
      case 6:
         this->mpc_Ui->pc_LineEditColor->setText((c_StringColorList.at(1) + c_StringColorList.at(2) +
                                                  c_StringColorList.at(3) + c_StringColorList.at(4) +
                                                  c_StringColorList.at(5) + c_StringColorList.at(6) +
                                                  c_StringColorList.at(6)).toUpper());
         break;
      default:
         break;
      }

      const QColor c_NewColor = static_cast<QColor>(this->mpc_Ui->pc_LineEditColor->text());
      this->mpc_Ui->pc_PushButtonColorShower->SetCurrentAlpha(c_NewColor.alpha());
      this->m_NewHsv(c_NewColor.hue(), c_NewColor.saturation(), c_NewColor.value());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The current color will be set to the current button

   which is selected by the integer of the mc_NextRecentButton

   \param[in,out]  opc_Button    Button
   \param[in]      oc_Color      Color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_SetRecentColorToRecentButton(C_OgePubColor * const opc_Button,
                                                             const QColor oc_Color) const
{
   const int32_t s32_Red = oc_Color.red();
   const int32_t s32_Green = oc_Color.green();
   const int32_t s32_Blue = oc_Color.blue();

   int32_t s32_RedDark = s32_Red - ((s32_Red * 40) / 100);
   int32_t s32_GreenDark = s32_Green - ((s32_Green * 40) / 100);
   int32_t s32_BlueDark = s32_Blue - ((s32_Blue * 40) / 100);

   int32_t s32_RedDarker = s32_Red - ((s32_Red * 20) / 100);
   int32_t s32_GreenDarker = s32_Green - ((s32_Green * 20) / 100);
   int32_t s32_BlueDarker = s32_Blue - ((s32_Blue * 20) / 100);

   int32_t s32_RedLightDark = s32_Red - ((s32_Red * 15) / 100);
   int32_t s32_GreenLightDark = s32_Green - ((s32_Green * 15) / 100);
   int32_t s32_BlueLightDark = s32_Blue - ((s32_Blue * 15) / 100);

   s32_RedDark = (s32_RedDark > 255) ? 255 : s32_RedDark;
   s32_GreenDark = (s32_GreenDark > 255) ? 255 : s32_GreenDark;
   s32_BlueDark = (s32_BlueDark > 255) ? 255 : s32_BlueDark;
   s32_RedDarker = (s32_RedDarker > 255) ? 255 : s32_RedDarker;
   s32_GreenDarker = (s32_RedDarker > 255) ? 255 : s32_GreenDarker;
   s32_BlueDarker = (s32_RedDarker > 255) ? 255 : s32_BlueDarker;
   s32_RedLightDark = (s32_RedDarker > 255) ? 255 : s32_RedLightDark;
   s32_GreenLightDark = (s32_RedDarker > 255) ? 255 : s32_GreenLightDark;
   s32_BlueLightDark = (s32_RedDarker > 255) ? 255 : s32_BlueLightDark;

   opc_Button->setStyleSheet(
      "stw--opensyde_gui_elements--C_OgePubColor {"
      "background-color: " + oc_Color.name() + ";"
      "border-top-color: rgb(" +
      QString::number(s32_RedDark) + ", " +
      QString::number(s32_GreenDark) + ", " +
      QString::number(s32_BlueDark) + ");"
      "border-right-color: rgb(" +
      QString::number(s32_RedDarker) + ", " +
      QString::number(s32_GreenDarker) + ", " +
      QString::number(s32_BlueDarker) + ");"
      "border-bottom-color: rgb(" +
      QString::number(s32_RedLightDark) + ", " +
      QString::number(s32_GreenLightDark) + ", " +
      QString::number(s32_BlueLightDark) + ");"
      "border-left-color: rgb(" +
      QString::number(s32_RedDarker) + ", " +
      QString::number(s32_GreenDarker) + ", " +
      QString::number(s32_BlueDarker) + ");"
      "}");
   QString c_Style = opc_Button->styleSheet();
   C_UtiStyleSheets::h_SetStyleSheetBackgroundColor(c_Style, oc_Color);
   opc_Button->setStyleSheet(c_Style);

   // choose which button gets the current color
   this->ms32_NextRecentButton = this->ms32_NextRecentButton + 1;
   if (this->ms32_NextRecentButton == 7)
   {
      this->ms32_NextRecentButton = 1;
   }
   //lint -e{1746} Necessary because needs default parameter and is not recognized as const
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   The current color resp the recent color add to the current button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_AddRecentColors(void)
{
   QVector<QColor> c_RecentColorsVector;

   if ((this->mpc_Ui->pc_PushButtonColorShower->CurrentColor() == this->mc_RecentColorNr1) ||
       (this->mpc_Ui->pc_PushButtonColorShower->CurrentColor() == this->mc_RecentColorNr2) ||
       (this->mpc_Ui->pc_PushButtonColorShower->CurrentColor() == this->mc_RecentColorNr3) ||
       (this->mpc_Ui->pc_PushButtonColorShower->CurrentColor() == this->mc_RecentColorNr4) ||
       (this->mpc_Ui->pc_PushButtonColorShower->CurrentColor() == this->mc_RecentColorNr5) ||
       (this->mpc_Ui->pc_PushButtonColorShower->CurrentColor() == this->mc_RecentColorNr6))
   {
      // nothing to do
   }
   else
   {
      switch (this->ms32_NextRecentButton)
      {
      case 1:
         this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr1,
                                              this->mpc_Ui->pc_PushButtonColorShower->CurrentColor());
         this->mc_RecentColorNr1 = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();
         break;
      case 2:
         this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr2,
                                              this->mpc_Ui->pc_PushButtonColorShower->CurrentColor());
         this->mc_RecentColorNr2 = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();
         break;
      case 3:
         this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr3,
                                              this->mpc_Ui->pc_PushButtonColorShower->CurrentColor());
         this->mc_RecentColorNr3 = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();
         break;
      case 4:
         this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr4,
                                              this->mpc_Ui->pc_PushButtonColorShower->CurrentColor());
         this->mc_RecentColorNr4 = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();
         break;
      case 5:
         this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr5,
                                              this->mpc_Ui->pc_PushButtonColorShower->CurrentColor());
         this->mc_RecentColorNr5 = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();
         break;
      case 6:
         this->m_SetRecentColorToRecentButton(this->mpc_Ui->pc_PushButtonRecentColorNr6,
                                              this->mpc_Ui->pc_PushButtonColorShower->CurrentColor());
         this->mc_RecentColorNr6 = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();
         break;
      default:
         break;
      }
   }

   c_RecentColorsVector.push_back(this->mc_RecentColorNr1);
   c_RecentColorsVector.push_back(this->mc_RecentColorNr2);
   c_RecentColorsVector.push_back(this->mc_RecentColorNr3);
   c_RecentColorsVector.push_back(this->mc_RecentColorNr4);
   c_RecentColorsVector.push_back(this->mc_RecentColorNr5);
   c_RecentColorsVector.push_back(this->mc_RecentColorNr6);
   C_UsHandler::h_GetInstance()->SetRecentColors(c_RecentColorsVector);
   C_UsHandler::h_GetInstance()->SetNextRecentColorButtonNumber(this->ms32_NextRecentButton);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets all widgets to display h, s, v

   \param[in]  os32_Hue     Hue
   \param[in]  os32_Sat     Saturation
   \param[in]  os32_Value   Value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_NewHsv(const int32_t os32_Hue, const int32_t os32_Sat, const int32_t os32_Value)
{
   this->mpc_Ui->pc_PushButtonColorShower->SetHsv(os32_Hue, os32_Sat, os32_Value);
   this->mpc_ColorPicker->SetColor(os32_Hue, os32_Sat);
   this->mpc_ColorBrightnessPicker->SetColorWithHsv(os32_Hue, os32_Sat, os32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of icon button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_PickScreenColor(void)
{
   if (mpc_ScreenColorPickingEventFilter == NULL)
   {
      mpc_ScreenColorPickingEventFilter = new C_GiSyScreenColorPickingEventFilter(this);
   }
   this->installEventFilter(mpc_ScreenColorPickingEventFilter);
   // If user pushes escape, the last color before picking will be restored.
   this->mc_ColorBeforeScreenColorPicking = this->mpc_Ui->pc_PushButtonColorShower->CurrentColor();
#ifndef QT_NO_CURSOR
   this->grabMouse(Qt::CrossCursor);
#else
   this->grabMouse();
#endif
   // On Windows mouse tracking doesn't work over other processes's windows
   mpc_UpdateTimer->start(10);
   // HACK: Because mouse grabbing doesn't work across processes, we have to have a dummy,
   // invisible window to catch the mouse click, otherwise we will click whatever we clicked
   // and loose focus.
   mc_DummyTransparentWindow.show();
   this->grabKeyboard();
   // With setMouseTracking(true) the desired color can be more precisely picked up,
   // and continuously pushing the mouse button is not necessary.
   this->setMouseTracking(true);
   this->m_SetCurrentRgbColor(m_GrabScreenColor(QCursor::pos()).rgba());
   this->mpc_Ui->pc_LabelNote->setText(static_cast<QString>("Cursor at %1, %2. Press ESC to cancel")
                                       .arg(QCursor::pos().x())
                                       .arg(QCursor::pos().y()));
   this->mq_ColorPickingActive = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update color while screen color picking is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_UpdatePositionWhileScreenColorPicking(void)
{
#ifndef QT_NO_CURSOR
   static QPoint hc_LastGlobalPosition;
   const QPoint c_NewGlobalPosition = QCursor::pos();
   if (hc_LastGlobalPosition != c_NewGlobalPosition)
   {
      hc_LastGlobalPosition = c_NewGlobalPosition;

      if (!this->rect().contains(this->mapFromGlobal(c_NewGlobalPosition))) // Inside the dialog mouse tracking works,
                                                                            // handleColorPickingMouseMove will be
                                                                            // called
      {
         this->m_UpdateColorPicking(c_NewGlobalPosition);
         mc_DummyTransparentWindow.setPosition(c_NewGlobalPosition);
      }
   }
#endif // ! QT_NO_CURSOR
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update the current color

   \param[in]  orc_GlobalPosition   Global position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_UpdateColorPicking(const QPoint & orc_GlobalPosition)
{
   this->m_SetCurrentRgbColor(m_GrabScreenColor(orc_GlobalPosition).rgba());
   this->mpc_Ui->pc_LabelNote->setText(static_cast<QString>("Cursor at %1, %2. Press ESC to cancel")
                                       .arg(orc_GlobalPosition.x())
                                       .arg(orc_GlobalPosition.y()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets all widgets to display rgb

   \param[in]  ou32_Rgb  RGB color of type QRgb that is equivalent to an unsigned int
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_SetCurrentRgbColor(const uint32_t ou32_Rgb)
{
   const QColor c_RgbColor = this->mpc_Ui->pc_PushButtonColorShower->SetRgb(ou32_Rgb);

   if (this->mpc_Ui->pc_PushButtonHtml->font().bold() == true)
   {
      this->mpc_Ui->pc_LineEditColor->setText(c_RgbColor.name().toUpper());
   }
   else if (this->mpc_Ui->pc_PushButtonRgba->font().bold() == true)
   {
      this->mpc_Ui->pc_LineEditColor->setText(QString::number(c_RgbColor.red()) + "/" +
                                              QString::number(c_RgbColor.green()) + "/" +
                                              QString::number(c_RgbColor.blue()) + "/" +
                                              QString::number(c_RgbColor.alpha()));
   }
   else
   {
      // nothing to do
   }
   this->m_NewColorTypedIn(ou32_Rgb);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Grab the screen color which is selected

   \param[in]  orc_Position   Position

   \return
   Selected color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyColorSelectWidget::m_GrabScreenColor(const QPoint & orc_Position) const
{
   const QDesktopWidget * const pc_Desktop = QApplication::desktop();
   const QPixmap c_Pixmap = QGuiApplication::screens().at(pc_Desktop->screenNumber())->grabWindow(pc_Desktop->winId(),
                                                                                                  orc_Position.x(),
                                                                                                  orc_Position.y(),
                                                                                                  1, 1);

   return c_Pixmap.toImage().pixel(0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sets all widgets except cs to display rgb

   \param[in]  ou32_Rgb  RGB color of type QRgb that is equivalent to an unsigned int
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_NewColorTypedIn(const uint32_t ou32_Rgb)
{
   int32_t s32_Hue;
   int32_t s32_Sat;
   int32_t s32_Value;

   // Convert rgb to hsv and set it to the color
   QColor c_Color;

   c_Color.setRgb(ou32_Rgb);
   c_Color.getHsv(&s32_Hue, &s32_Sat, &s32_Value);

   this->mpc_ColorPicker->SetColor(s32_Hue, s32_Sat);
   this->mpc_ColorBrightnessPicker->SetColorWithHsv(s32_Hue, s32_Sat, s32_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Leave the screen color picking
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_LeaveColorPicking(void)
{
   //lint -e{730}  Allow the circle is visible (Use clean Qt interface)
   this->mpc_ColorPicker->SetCircleVisible(true);
   this->removeEventFilter(mpc_ScreenColorPickingEventFilter);
   this->releaseMouse();
   this->mpc_UpdateTimer->stop();
   this->mc_DummyTransparentWindow.setVisible(false);
   this->releaseKeyboard();
   this->setMouseTracking(false);
   this->mpc_Ui->pc_LabelNote->setText("");
   this->mq_ColorPickingActive = false;
}
