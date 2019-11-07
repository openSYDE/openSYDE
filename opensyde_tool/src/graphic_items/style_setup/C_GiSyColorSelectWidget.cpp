//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog widget for color selection.

   TODO SFI adapt this description if not sufficient anymore:
   Let the user select a color from a predefined list, from recently selected colors or completely free by RGB
   values. Furthermore provide a color picker "pipette" functionality.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_GtGetText.h"
#include "C_GiSyColorSelectWidget.h"
#include "ui_C_GiSyColorSelectWidget.h"
#include "C_OgeWiError.h"
#include <QTextEdit>
#include <QMessageBox>
#include "C_ColorPickingEventFilter.h"
#include "qdialog.h"

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

   \param[in,out] orc_Parent Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorSelectWidget::C_GiSyColorSelectWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_GiSyColorSelectWidget),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   this->InitStaticNames();
   this->InitStandardColors();
   this->InitElements();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   QTextEdit * textEdit = new QTextEdit;
   QTextEdit * textEdit2 = new QTextEdit;
   //   textEdit->setMinimumSize(200, 150);
   this->mpc_Ui->horizontalLayout_5->addWidget(textEdit);
   this->mpc_Ui->horizontalLayout_5->addWidget(textEdit2);

   connect(this->mpc_Ui->pc_PushButtonOk, SIGNAL(clicked()), this, SLOT(m_OkClicked()));
   connect(this->mpc_Ui->pc_PushButtonCancel, SIGNAL(clicked()), this, SLOT(m_CancelClicked()));

   connect(this->mpc_Ui->pc_PushButtonLightBlue, SIGNAL(clicked()), this, SLOT(m_LightBlueClicked()));
   connect(this->mpc_Ui->pc_PushButtonGreen, SIGNAL(clicked()), this, SLOT(m_GreenClicked()));
   connect(this->mpc_Ui->pc_PushButtonGold, SIGNAL(clicked()), this, SLOT(m_GoldClicked()));
   connect(this->mpc_Ui->pc_PushButtonOrange, SIGNAL(clicked()), this, SLOT(m_OrangeClicked()));
   connect(this->mpc_Ui->pc_PushButtonRed, SIGNAL(clicked()), this, SLOT(m_RedClicked()));
   connect(this->mpc_Ui->pc_PushButtonPurple, SIGNAL(clicked()), this, SLOT(m_PurpleClicked()));
   connect(this->mpc_Ui->pc_PushButtonLightGreen, SIGNAL(clicked()), this, SLOT(m_LightGreenClicked()));
   connect(this->mpc_Ui->pc_PushButtonDarkBlueGreen, SIGNAL(clicked()), this, SLOT(m_DarkBlueGreenClicked()));
   connect(this->mpc_Ui->pc_PushButtonLavendel, SIGNAL(clicked()), this, SLOT(m_LavendelClicked()));

   connect(this->mpc_Ui->pc_PushButtonHTML, SIGNAL(clicked()), this, SLOT(m_HMTLClicked()));
   connect(this->mpc_Ui->pc_PushButtonRGBA, SIGNAL(clicked()), this, SLOT(m_RGBAClicked()));

   //   connect(this->mpc_Ui->pc_LineEditColor, SIGNAL(textChanged()), this, SLOT(m_LineEditTextChanged()));
   connect(this->mpc_Ui->pc_LineEditColor, QLineEdit::textChanged, this,
           C_GiSyColorSelectWidget::m_LineEditTextChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyColorSelectWidget::~C_GiSyColorSelectWidget(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Color"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Selection"));
   this->mpc_Ui->pc_LabelHeadingPreviewStandardColors->setText(C_GtGetText::h_GetText("Standard Colors"));
   this->mpc_Ui->pc_LabelHeadingPreviewRecentColors->setText(C_GtGetText::h_GetText("Recent Colors"));
   this->mpc_Ui->pc_PushButtonHTML->setText(C_GtGetText::h_GetText("HTML"));
   this->mpc_Ui->pc_LabelPipeSymbol->setText(C_GtGetText::h_GetText("|"));
   this->mpc_Ui->pc_PushButtonRGBA->setText(C_GtGetText::h_GetText("RGBA"));
//   this->mpc_Ui->pc_LabelNote->setText(C_GtGetText::h_GetText("Cursor at ???, ???. Press ESC to cancel."));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all standard colors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::InitStandardColors(void)
{
   lightBlue = new QColor(109, 166, 232);
   green = new QColor(127, 189, 101);
   gold = new QColor(249, 217, 72);
   orange = new QColor(243, 147, 34);
   red = new QColor(230, 58, 63);
   purple = new QColor(88, 87, 163);
   lightGreen = new QColor(203, 235, 193);
   darkBlueGreen = new QColor(2, 155, 136);
   lavendel = new QColor(237, 237, 242);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static colors
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::InitElements(void) const
{
   this->mpc_Ui->pc_PushButtonLightBlue->setStyleSheet(
      "\
                                                        QPushButton {\
                                                            background-color: " + lightBlue->name() + ";\
                                                            border-width: 0.5px;\
                                                            border-radius: 10px;\
                                                            border-top-color: rgb(87, 106, 164);\
                                                            border-right-color: rgb(99, 147, 203);\
                                                            border-bottom-color: rgb(99, 147, 203);\
                                                            border-left-color: rgb(99, 147, 203);\
                                                        }\
                                                        QPushButton:checked{\
                                                            border-color: white;\
                                                        }\
                                                        ");
   this->mpc_Ui->pc_PushButtonGreen->setStyleSheet(
      "\
                                                    QPushButton {\
                                                        background-color: " + green->name() + ";\
                                                        border-width: 0.5px;\
                                                        border-radius: 10px;\
                                                        border-top-color: rgb(101, 136, 70);\
                                                        border-right-color: rgb(117, 162, 88);\
                                                        border-bottom-color: rgb(117, 162, 88);\
                                                        border-left-color: rgb(117, 162, 88);\
                                                    }\
                                                    QPushButton:checked{\
                                                        border-color: white;\
                                                    }\
                                                    ");
   this->mpc_Ui->pc_PushButtonGold->setStyleSheet(
      "\
                                                   QPushButton {\
                                                       background-color: " + gold->name() + ";\
                                                       border-width: 0.5px;\
                                                       border-radius: 10px;\
                                                       border-top-color: rgb(181, 160, 67);\
                                                       border-right-color: rgb(219, 193, 72);\
                                                       border-bottom-color: rgb(219, 193, 72);\
                                                       border-left-color: rgb(219, 193, 72);\
                                                   }\
                                                   QPushButton:checked{\
                                                       border-color: white;\
                                                   }\
                                                   ");
   this->mpc_Ui->pc_PushButtonOrange->setStyleSheet(
      "\
                                                     QPushButton {\
                                                         background-color: " + orange->name() + ";\
                                                         border-width: 0.5px;\
                                                         border-radius: 10px;\
                                                         border-top-color: rgb(153, 81, 38);\
                                                         border-right-color: rgb(211, 130, 30);\
                                                         border-bottom-color: rgb(211, 130, 30);\
                                                         border-left-color: rgb(211, 130, 30);\
                                                     }\
                                                     QPushButton:checked{\
                                                         border-color: white;\
                                                     }\
                                                     ");
   this->mpc_Ui->pc_PushButtonRed->setStyleSheet(
      "\
                                                  QPushButton {\
                                                      background-color: " + red->name() + ";\
                                                      border-width: 0.5px;\
                                                      border-radius: 10px;\
                                                      border-top-color: rgb(140, 32, 21);\
                                                      border-right-color: rgb(176, 55, 43);\
                                                      border-bottom-color: rgb(192, 59, 45);\
                                                      border-left-color: rgb(176, 55, 43);\
                                                  }\
                                                  QPushButton:checked{\
                                                      border-color: white;\
                                                  }\
                                                  ");
   this->mpc_Ui->pc_PushButtonPurple->setStyleSheet(
      "\
                                                     QPushButton {\
                                                         background-color: " + purple->name() + ";\
                                                         border-width: 0.5px;\
                                                         border-radius: 10px;\
                                                         border-top-color: rgb(42, 40, 76);\
                                                         border-right-color: rgb(63, 61, 125);\
                                                         border-bottom-color: rgb(63, 61, 125);\
                                                         border-left-color: rgb(63, 61, 125);\
                                                     }\
                                                     QPushButton:checked{\
                                                         border-color: white;\
                                                     }\
                                                     ");
   this->mpc_Ui->pc_PushButtonLightGreen->setStyleSheet(
      "\
                                                         QPushButton {\
                                                             background-color: " + lightGreen->name() + ";\
                                                             border-width: 0.5px;\
                                                             border-radius: 10px;\
                                                             border-top-color: rgb(169, 198, 145);\
                                                             border-right-color: rgb(193, 219, 180);\
                                                             border-bottom-color: rgb(193, 219, 180);\
                                                             border-left-color: rgb(193, 219, 180);\
                                                         }\
                                                         QPushButton:checked{\
                                                             border-color: white;\
                                                         }\
                                                         ");
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setStyleSheet(
      "\
                                                            QPushButton {\
                                                                background-color: " + darkBlueGreen->name() + ";\
                                                                border-width: 0.5px;\
                                                                border-radius: 10px;\
                                                                border-top-color: rgb(0, 100, 102);\
                                                                border-right-color: rgb(0, 133, 113);\
                                                                border-bottom-color: rgb(0, 133, 113);\
                                                                border-left-color: rgb(0, 133, 113);\
                                                            }\
                                                            QPushButton:checked{\
                                                                border-color: white;\
                                                            }\
                                                            ");
   this->mpc_Ui->pc_PushButtonLavendel->setStyleSheet(
      "\
                                                       QPushButton {\
                                                           background-color: " + lavendel->name() + ";\
                                                           border-width: 0.5px;\
                                                           border-radius: 10px;\
                                                           border-top-color: rgb(177, 180, 179);\
                                                           border-right-color: rgb(215, 215, 215);\
                                                           border-bottom-color: rgb(215, 215, 215);\
                                                           border-left-color: rgb(215, 215, 215);\
                                                       }\
                                                       QPushButton:checked{\
                                                           border-color: white;\
                                                       }\
                                                       ");
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setStyleSheet(
      "\
                                                              QPushButton {\
                                                                  background-color: white;\
                                                                  border-width: 0.5px;\
                                                                  border-radius: 10px;\
                                                                  border-top-color: rgb(169, 172, 171);\
                                                                  border-right-color: rgb(199, 199, 199);\
                                                                  border-bottom-color: rgb(205, 205, 205);\
                                                                  border-left-color: rgb(199, 199, 199);\
                                                              }   \
                                                              QPushButton:checked{\
                                                                  border-color: lightgrey;\
                                                              }\
                                                              ");
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setStyleSheet(
      "\
                                                             QPushButton {\
                                                                 background-color: white;\
                                                                 border-width: 0.5px;\
                                                                 border-radius: 10px;\
                                                                 border-top-color: rgb(169, 172, 171);\
                                                                 border-right-color: rgb(199, 199, 199);\
                                                                 border-bottom-color: rgb(205, 205, 205);\
                                                                 border-left-color: rgb(199, 199, 199);\
                                                             }\
                                                             QPushButton:checked{\
                                                                 border-color: lightgrey;\
                                                             }\
                                                             ");
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setStyleSheet(
      "\
                                                             QPushButton {\
                                                                 background-color: white;\
                                                                 border-width: 0.5px;\
                                                                 border-radius: 10px;\
                                                                 border-top-color: rgb(169, 172, 171);\
                                                                 border-right-color: rgb(199, 199, 199);\
                                                                 border-bottom-color: rgb(205, 205, 205);\
                                                                 border-left-color: rgb(199, 199, 199);\
                                                             }\
                                                             QPushButton:checked{\
                                                                 border-color: lightgrey;\
                                                             }\
                                                             ");
   this->mpc_Ui->pc_PushButtonHTML->setStyleSheet(
      "\
                                                   QPushButton {\
                                                       background-color: white;\
                                                       border: none;\
                                                       color: rgb(114, 86, 104);\
                                                   }\
                                                   QPushButton:checked{\
                                                       font-weight: bold;\
                                                   }\
                                                   ");
   this->mpc_Ui->pc_LabelPipeSymbol->setStyleSheet(
      "QLabel {\
                                                        color: rgb(114, 86, 104);\
                                                   }\
                                                   ");
   this->mpc_Ui->pc_PushButtonRGBA->setStyleSheet(
      "\
                                                   QPushButton {\
                                                       background-color: white;\
                                                       border: none;\
                                                       color: rgb(114, 86, 104);\
                                                   }\
                                                   QPushButton:checked{\
                                                       font-weight: bold;\
                                                   }\
                                                   ");
   this->mpc_Ui->pc_PushButtonIcon->setIcon(QIcon("://images/IconEditActive.svg"));
   this->mpc_Ui->pc_PushButtonIcon->setStyleSheet("QPushButton { background-color: transparent; }");
   this->mpc_Ui->pc_LineEditColor->setStyleSheet(
      "QLineEdit {\
                                                       background-color: transparent;\
                                                       border: none;\
                                                       color: rgb(114, 86, 104);\
                                                 }\
                                                 ");
   this->mpc_Ui->pc_PushButtonSelectedColor->setStyleSheet(
      "\
                                                           QPushButton {\
                                                               background-color: rgb(230, 58, 62);\
                                                               border-width: 1px;\
                                                               border-radius: 14px;\
                                                               border-top-color: rgb(140, 32, 21);\
                                                               border-right-color: rgb(176, 55, 43);\
                                                               border-bottom-color: rgb(176, 55, 43);\
                                                               border-left-color: rgb(176, 55, 43);\
                                                           }\
                                                           ");
   this->mpc_Ui->pc_LabelNote->setStyleSheet(
      "QLabel {\
                                                  color: rgb(172, 130, 162);\
                                             }\
                                             ");
   m_PushButtonHTML();
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

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
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
void C_GiSyColorSelectWidget::m_OkClicked(void)
{
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
/*! \brief   Slot of blue button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_LightBlueClicked(void)
{
   this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonLightBlue->isChecked())
   {
      this->mpc_Ui->pc_PushButtonLightBlue->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of green button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_GreenClicked(void)
{
   this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonGreen->isChecked())
   {
      this->mpc_Ui->pc_PushButtonGreen->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of gold button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_GoldClicked(void)
{
   this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonGold->isChecked())
   {
      this->mpc_Ui->pc_PushButtonGold->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of orange button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_OrangeClicked(void)
{
   this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonOrange->isChecked())
   {
      this->mpc_Ui->pc_PushButtonOrange->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of red button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RedClicked(void)
{
   this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonRed->isChecked())
   {
      this->mpc_Ui->pc_PushButtonRed->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of purple button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_PurpleClicked(void)
{
   this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonPurple->isChecked())
   {
      this->mpc_Ui->pc_PushButtonPurple->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of light green button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_LightGreenClicked(void)
{
   this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonLightGreen->isChecked())
   {
      this->mpc_Ui->pc_PushButtonLightGreen->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of dark blue green button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_DarkBlueGreenClicked(void)
{
   this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonDarkBlueGreen->isChecked())
   {
      this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of lavendel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_LavendelClicked(void)
{
   this->mpc_Ui->pc_PushButtonLightBlue->setChecked(false);
   this->mpc_Ui->pc_PushButtonGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonGold->setChecked(false);
   this->mpc_Ui->pc_PushButtonOrange->setChecked(false);
   this->mpc_Ui->pc_PushButtonRed->setChecked(false);
   this->mpc_Ui->pc_PushButtonPurple->setChecked(false);
   this->mpc_Ui->pc_PushButtonLightGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonDarkBlueGreen->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr1->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr2->setChecked(false);
   this->mpc_Ui->pc_PushButtonRecentColorNr3->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonLavendel->isChecked())
   {
      this->mpc_Ui->pc_PushButtonLavendel->setChecked(true);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonLavendel->setChecked(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of which color is selected
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiSyColorSelectWidget::m_ChooseSelectedColor(QColor c_Color)
{
   if (this->mpc_Ui->pc_PushButtonLightBlue->isChecked())
   {
      return *lightBlue;
   }
   else if (this->mpc_Ui->pc_PushButtonGreen->isChecked())
   {
      return *green;
   }
   else if (this->mpc_Ui->pc_PushButtonGold->isChecked())
   {
      return *gold;
   }
   else if (this->mpc_Ui->pc_PushButtonOrange->isChecked())
   {
      return *orange;
   }
   else if (this->mpc_Ui->pc_PushButtonRed->isChecked())
   {
      return *red;
   }
   else if (this->mpc_Ui->pc_PushButtonPurple->isChecked())
   {
      return *purple;
   }
   else if (this->mpc_Ui->pc_PushButtonLightGreen->isChecked())
   {
      return *lightGreen;
   }
   else if (this->mpc_Ui->pc_PushButtonDarkBlueGreen->isChecked())
   {
      return *darkBlueGreen;
   }
   else if (this->mpc_Ui->pc_PushButtonLavendel->isChecked())
   {
      return *lavendel;
   }
   else
   {
      // display a new window with an warning message
      QMessageBox::warning(this, "", "No new color chosen!\nThe last selected color was retain!");
      return c_Color;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of html button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_HMTLClicked(void)
{
   this->mpc_Ui->pc_PushButtonRGBA->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonHTML->isChecked())
   {
      m_PushButtonHTML();
   }
   else
   {
      m_PushButtonRGBA();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of rgba button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_RGBAClicked(void)
{
   this->mpc_Ui->pc_PushButtonHTML->setChecked(false);

   if (this->mpc_Ui->pc_PushButtonRGBA->isChecked())
   {
      m_PushButtonRGBA();
   }
   else
   {
      m_PushButtonHTML();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initalize the html button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_PushButtonHTML(void) const
{
   this->mpc_Ui->pc_PushButtonHTML->setChecked(true);
   this->mpc_Ui->pc_LineEditColor->setText(C_GtGetText::h_GetText("#"));
   this->mpc_Ui->pc_LineEditColor->setValidator(new QRegExpValidator(
                                                   QRegExp("#([A-F0-9]{6})"),
                                                   this->mpc_Ui->pc_LineEditColor));
//    m_pickScreenColor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initalize the rgba button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_PushButtonRGBA(void) const
{
   this->mpc_Ui->pc_PushButtonRGBA->setChecked(true);
   this->mpc_Ui->pc_LineEditColor->setText(C_GtGetText::h_GetText(""));
   this->mpc_Ui->pc_LineEditColor->setValidator(new QRegExpValidator(
                                                   QRegExp("([0-9]{3})/([0-9]{3})/([0-9]{3})/([0-9]{3})"),
                                                   this->mpc_Ui->pc_LineEditColor));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the parting line for the rgba
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyColorSelectWidget::m_LineEditTextChanged()
{
   if (this->mpc_Ui->pc_PushButtonRGBA->isChecked())
   {
       switch(this->mpc_Ui->pc_LineEditColor->text().size())
       {
            case 3:
               this->mpc_Ui->pc_LineEditColor->setText(this->mpc_Ui->pc_LineEditColor->text() + "/");
               break;
            case 7:
               this->mpc_Ui->pc_LineEditColor->setText(this->mpc_Ui->pc_LineEditColor->text() + "/");
               break;
            case 11:
               this->mpc_Ui->pc_LineEditColor->setText(this->mpc_Ui->pc_LineEditColor->text() + "/");
               break;
       }
   }
   else if(this->mpc_Ui->pc_PushButtonHTML->isChecked()
           && this->mpc_Ui->pc_LineEditColor->text().size() == 0)
   {
       this->mpc_Ui->pc_LineEditColor->setText(this->mpc_Ui->pc_LineEditColor->text() + "#");
   }
}

//void C_GiSyColorSelectWidget::m_pickScreenColor()
//{
//    Q_Q(C_GiSyColorSelectWidget);
//    if (!colorPickingEventFilter)
//        colorPickingEventFilter = new C_ColorPickingEventFilter();
//    q->installEventFilter(colorPickingEventFilter);
//    // If user pushes Escape, the last color before picking will be restored.
////    beforeScreenColorPicking = cs->currentColor();
//#ifndef QT_NO_CURSOR
//    q->grabMouse(Qt::CrossCursor);
//#else
//    q->grabMouse();
//#endif

//#ifdef Q_OS_WIN32 // excludes WinCE and WinRT
//    // On Windows mouse tracking doesn't work over other processes's windows
//    updateTimer->start(30);

//    // HACK: Because mouse grabbing doesn't work across processes, we have to have a dummy,
//    // invisible window to catch the mouse click, otherwise we will click whatever we clicked
//    // and loose focus.
//    dummyTransparentWindow.show();
//#endif
//    q->grabKeyboard();
//    /* With setMouseTracking(true) the desired color can be more precisely picked up,
//     * and continuously pushing the mouse button is not necessary.
//     */
//    q->setMouseTracking(true);

////    addCusBt->setDisabled(true);
////    buttons->setDisabled(true);
////    screenColorPickerButton->setDisabled(true);

//    const QPoint globalPos = QCursor::pos();
//    q->setCurrentColor(grabScreenColor(globalPos));
////    updateColorLabelText(globalPos);
////    this->mpc_Ui->pc_LabelNote->setText(QColorDialog::tr("Cursor at %1, %2\nPress ESC to cancel")
//    this->mpc_Ui->pc_LabelNote->setText(C_GiSyColorSelectWidget::tr("Cursor at %1, %2\nPress ESC to cancel")
//                                .arg(globalPos.x())
//                                .arg(globalPos.y()));
//}

//void C_GiSyColorSelectWidget::setCurrentColor(const QColor &color,  SetColorMode setColorMode)
//{
//    if (nativeDialogInUse) {
//        platformColorHelper()->setCurrentColor(color);
//        return;
//    }

//    if (setColorMode & ShowColor) {
//        setCurrentRgbColor(color.rgb());
//        setCurrentAlpha(color.alpha());
//    }
//    if (setColorMode & SelectColor)
//        selectColor(color);
//}

///*!
//    \property QColorDialog::currentColor
//    \brief the currently selected color in the dialog
//*/
//void C_GiSyColorSelectWidget::setCurrentColor(const QColor &color)
//{
//    Q_D(C_GiSyColorSelectWidget);
//    d->setCurrentColor(color);
//}

////void C_GiSyColorSelectWidget::updateColorLabelText(const QPoint &globalPos)
////{
////    this->mpc_Ui->pc_LabelNote->setText(QColorDialog::tr("Cursor at %1, %2\nPress ESC to cancel")
////                                .arg(globalPos.x())
////                                .arg(globalPos.y()));
////}

//QColor C_GiSyColorSelectWidget::grabScreenColor(const QPoint &p)
//{
//    const QDesktopWidget * desktop = QApplication::desktop();
//    const QPixmap pixmap = QGuiApplication::screens().at(desktop->screenNumber())->grabWindow(desktop->winId(),
//                                                                                              p.x(), p.y(), 1, 1);
//    QImage i = pixmap.toImage();
//    return i.pixel(0, 0);
//}

//void C_GiSyColorSelectWidget::m_updateColorPicking()
//{
//#ifndef QT_NO_CURSOR
//    Q_Q(C_GiSyColorSelectWidget);
//    static QPoint lastGlobalPos;
//    QPoint newGlobalPos = QCursor::pos();
//    if (lastGlobalPos == newGlobalPos)
//        return;
//    lastGlobalPos = newGlobalPos;

//    if (!q->rect().contains(q->mapFromGlobal(newGlobalPos))) { // Inside the dialog mouse tracking works, handleColorPickingMouseMove will be called
//        updateColorPicking(newGlobalPos);
//#ifdef Q_OS_WIN32
//        dummyTransparentWindow.setPosition(newGlobalPos);
//#endif
//    }
//#endif // ! QT_NO_CURSOR
//}

//void C_GiSyColorSelectWidget::updateColorPicking(const QPoint &globalPos)
//{
//    const QColor color = grabScreenColor(globalPos);
//    // QTBUG-39792, do not change standard, custom color selectors while moving as
//    // otherwise it is not possible to pre-select a custom cell for assignment.
//    setCurrentColor(color, ShowColor);
////    updateColorLabelText(globalPos);
//    this->mpc_Ui->pc_LabelNote->setText(C_GiSyColorSelectWidget::tr("Cursor at %1, %2\nPress ESC to cancel")
//                                .arg(globalPos.x())
//                                .arg(globalPos.y()));
//}

////sets all widgets to display rgb
//void C_GiSyColorSelectWidget::setCurrentRgbColor(QRgb rgb)
//{
//    if (!nativeDialogInUse) {
//        cs->setRgb(rgb);
//        m_newColorTypedIn(rgb);
//    }
//}

////sets all widgets except cs to display rgb
//void C_GiSyColorSelectWidget::m_newColorTypedIn(QRgb rgb)
//{
//    if (!nativeDialogInUse) {
//        int h, s, v;
//        rgb2hsv(rgb, h, s, v);
//        cp->setCol(h, s);
//        lp->setCol(h, s, v);
//    }
//}

//inline void C_GiSyColorSelectWidget::setCurrentAlpha(int a)
//{
//    cs->setCurrentAlpha(a);
//}

//bool C_GiSyColorSelectWidget::selectColor(const QColor &col)
//{
//    QRgb color = col.rgb();
//    // Check standard colors
//    if (standard) {
//        const QRgb * standardColors = C_ColorOptions::standardColors();
//        const QRgb * standardColorsEnd = standardColors + standardColorRows * colorColumns;
//        const QRgb * match = std::find(standardColors, standardColorsEnd, color);
//        if (match != standardColorsEnd) {
//            const int index = int(match - standardColors);
//            const int column = index / standardColorRows;
//            const int row = index % standardColorRows;
////            _q_newStandard(row, column);
//            standard->setCurrent(row, column);
//            standard->setSelected(row, column);
//            standard->setFocus();
//            return true;
//        }
//    }
//    // Check custom colors
//    if (custom) {
//        const QRgb * customColors = C_ColorOptions::customColors();
//        const QRgb * customColorsEnd = customColors + customColorRows * colorColumns;
//        const QRgb * match = std::find(customColors, customColorsEnd, color);
//        if (match != customColorsEnd) {
//            const int index = int(match - customColors);
//            const int column = index / customColorRows;
//            const int row = index % customColorRows;
////            _q_newCustom(row, column);
//            custom->setCurrent(row, column);
//            custom->setSelected(row, column);
//            custom->setFocus();
//            return true;
//        }
//    }
//    return false;
//}

//static inline void rgb2hsv(QRgb rgb, int &h, int &s, int &v)
//{
//    QColor c;
//    c.setRgb(rgb);
//    c.getHsv(&h, &s, &v);
//}

//bool C_GiSyColorSelectWidget::handleColorPickingMouseMove(QMouseEvent *e)
//{
//    // If the cross is visible the grabbed color will be black most of the times
//    cp->setCrossVisible(!cp->geometry().contains(e->pos()));

//    updateColorPicking(e->globalPos());
//    return true;
//}

//bool C_GiSyColorSelectWidget::handleColorPickingMouseButtonRelease(QMouseEvent *e)
//{
//    setCurrentColor(grabScreenColor(e->globalPos()), SetColorAll);
//    releaseColorPicking();
//    return true;
//}

//bool C_GiSyColorSelectWidget::handleColorPickingKeyPress(QKeyEvent *e)
//{
//    Q_Q(C_GiSyColorSelectWidget);
//    if (e->matches(QKeySequence::Cancel)) {
//        releaseColorPicking();
//        q->setCurrentColor(beforeScreenColorPicking);
//    } else if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
//        q->setCurrentColor(grabScreenColor(QCursor::pos()));
//        releaseColorPicking();
//    }
//    e->accept();
//    return true;
//}

//void C_GiSyColorSelectWidget::releaseColorPicking()
//{
////    Q_Q(C_GiSyColorSelectWidget);
////    cp->setCrossVisible(true);
////    q->removeEventFilter(colorPickingEventFilter);
////    q->releaseMouse();
//#ifdef Q_OS_WIN32
//    updateTimer->stop();
//    dummyTransparentWindow.setVisible(false);
//#endif
////    q->releaseKeyboard();
////    q->setMouseTracking(false);
////    lblScreenColorInfo->setText(QLatin1String("\n"));
////    addCusBt->setDisabled(false);
////    buttons->setDisabled(false);
////    screenColorPickerButton->setDisabled(false);
//}

//void C_GiSyColorSelectWidget::setCrossVisible(bool visible)
//{
//    if (crossVisible != visible) {
//        crossVisible = visible;
//        update();
//    }
//}
