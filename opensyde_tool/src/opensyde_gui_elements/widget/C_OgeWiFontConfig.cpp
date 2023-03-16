//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom font configuration widget (implementation)

   Custom font configuration widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_GtGetText.hpp"
#include "C_OgeWiFontConfig.hpp"
#include "ui_C_OgeWiFontConfig.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_8 = 0;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_9 = 1;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_10 = 2;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_11 = 3;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_12 = 4;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_14 = 5;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_16 = 6;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_18 = 7;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_20 = 8;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_22 = 9;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_24 = 10;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_26 = 11;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_28 = 12;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_36 = 13;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_48 = 14;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_72 = 15;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_100 = 16;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_200 = 17;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_300 = 18;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_400 = 19;
const int32_t C_OgeWiFontConfig::mhs32_INDEX_SIZE_500 = 20;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiFontConfig::C_OgeWiFontConfig(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiFontConfig)
{
   QFont c_Font;

   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_PushButtonItalic->setCheckable(true);
   c_Font = this->mpc_Ui->pc_PushButtonItalic->font();
   c_Font.setItalic(true);
   this->mpc_Ui->pc_PushButtonItalic->setFont(c_Font);

   this->mpc_Ui->pc_PushButtonBold->setCheckable(true);
   c_Font = this->mpc_Ui->pc_PushButtonBold->font();
   c_Font.setBold(true);
   this->mpc_Ui->pc_PushButtonBold->setFont(c_Font);

   this->mpc_Ui->pc_PushButtonUnderline->setCheckable(true);
   c_Font = this->mpc_Ui->pc_PushButtonUnderline->font();
   c_Font.setUnderline(true);
   this->mpc_Ui->pc_PushButtonUnderline->setFont(c_Font);

   this->mpc_Ui->pc_ComboBoxSize->addItem("8");
   this->mpc_Ui->pc_ComboBoxSize->addItem("9");
   this->mpc_Ui->pc_ComboBoxSize->addItem("10");
   this->mpc_Ui->pc_ComboBoxSize->addItem("11");
   this->mpc_Ui->pc_ComboBoxSize->addItem("12");
   this->mpc_Ui->pc_ComboBoxSize->addItem("14");
   this->mpc_Ui->pc_ComboBoxSize->addItem("16");
   this->mpc_Ui->pc_ComboBoxSize->addItem("18");
   this->mpc_Ui->pc_ComboBoxSize->addItem("20");
   this->mpc_Ui->pc_ComboBoxSize->addItem("22");
   this->mpc_Ui->pc_ComboBoxSize->addItem("24");
   this->mpc_Ui->pc_ComboBoxSize->addItem("26");
   this->mpc_Ui->pc_ComboBoxSize->addItem("28");
   this->mpc_Ui->pc_ComboBoxSize->addItem("36");
   this->mpc_Ui->pc_ComboBoxSize->addItem("48");
   this->mpc_Ui->pc_ComboBoxSize->addItem("72");
   this->mpc_Ui->pc_ComboBoxSize->addItem("100");
   this->mpc_Ui->pc_ComboBoxSize->addItem("200");
   this->mpc_Ui->pc_ComboBoxSize->addItem("300");
   this->mpc_Ui->pc_ComboBoxSize->addItem("400");
   this->mpc_Ui->pc_ComboBoxSize->addItem("500");

   InitStaticNames();

   //Connect update
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxSize, static_cast<void (QComboBox::*)(
                                                         int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_OgeWiFontConfig::m_TriggerUpdate);
   connect(this->mpc_Ui->pc_FontComboBox, &QFontComboBox::currentFontChanged, this,
           &C_OgeWiFontConfig::m_TriggerUpdate);
   connect(this->mpc_Ui->pc_PushButtonBold, &QPushButton::clicked, this, &C_OgeWiFontConfig::m_TriggerUpdate);
   connect(this->mpc_Ui->pc_PushButtonItalic, &QPushButton::clicked, this, &C_OgeWiFontConfig::m_TriggerUpdate);
   connect(this->mpc_Ui->pc_PushButtonUnderline, &QPushButton::clicked, this, &C_OgeWiFontConfig::m_TriggerUpdate);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiFontConfig::~C_OgeWiFontConfig(void)
{
   delete mpc_Ui;
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiFontConfig::InitStaticNames(void) const
{
   //Translation: italic
   this->mpc_Ui->pc_PushButtonItalic->setText(C_GtGetText::h_GetText("I"));
   //Translation: bold
   this->mpc_Ui->pc_PushButtonBold->setText(C_GtGetText::h_GetText("B"));
   //Translation: underline
   this->mpc_Ui->pc_PushButtonUnderline->setText(C_GtGetText::h_GetText("U"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply font

   \param[in] orc_Font New font
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiFontConfig::SetFont(const QFont & orc_Font)
{
   //Disconnect update
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   disconnect(this->mpc_Ui->pc_ComboBoxSize, static_cast<void (QComboBox::*)(
                                                            int32_t)>(&QComboBox::currentIndexChanged), this,
              &C_OgeWiFontConfig::m_TriggerUpdate);
   disconnect(this->mpc_Ui->pc_FontComboBox, &QFontComboBox::currentFontChanged, this,
              &C_OgeWiFontConfig::m_TriggerUpdate);
   disconnect(this->mpc_Ui->pc_PushButtonBold, &QPushButton::toggled, this, &C_OgeWiFontConfig::m_TriggerUpdate);
   disconnect(this->mpc_Ui->pc_PushButtonItalic, &QPushButton::toggled, this, &C_OgeWiFontConfig::m_TriggerUpdate);
   disconnect(this->mpc_Ui->pc_PushButtonUnderline, &QPushButton::toggled, this, &C_OgeWiFontConfig::m_TriggerUpdate);

   //Apply new font
   this->mpc_Ui->pc_PushButtonItalic->setChecked(orc_Font.italic());
   this->mpc_Ui->pc_PushButtonBold->setChecked(orc_Font.bold());
   this->mpc_Ui->pc_PushButtonUnderline->setChecked(orc_Font.underline());
   this->mpc_Ui->pc_FontComboBox->setCurrentFont(orc_Font);
   this->mpc_Ui->pc_ComboBoxSize->setCurrentIndex(m_GetSizeIndex(orc_Font));

   //Reconnect update
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxSize, static_cast<void (QComboBox::*)(
                                                         int32_t)>(&QComboBox::currentIndexChanged), this,
           &C_OgeWiFontConfig::m_TriggerUpdate);
   connect(this->mpc_Ui->pc_FontComboBox, &QFontComboBox::currentFontChanged, this,
           &C_OgeWiFontConfig::m_TriggerUpdate);
   connect(this->mpc_Ui->pc_PushButtonBold, &QPushButton::toggled, this, &C_OgeWiFontConfig::m_TriggerUpdate);
   connect(this->mpc_Ui->pc_PushButtonItalic, &QPushButton::toggled, this, &C_OgeWiFontConfig::m_TriggerUpdate);
   connect(this->mpc_Ui->pc_PushButtonUnderline, &QPushButton::toggled, this, &C_OgeWiFontConfig::m_TriggerUpdate);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger update of font
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiFontConfig::m_TriggerUpdate(void)
{
   QFont c_Font = this->mpc_Ui->pc_FontComboBox->currentFont();

   this->m_GetSize(c_Font);
   c_Font.setBold(this->mpc_Ui->pc_PushButtonBold->isChecked());
   c_Font.setItalic(this->mpc_Ui->pc_PushButtonItalic->isChecked());
   c_Font.setUnderline(this->mpc_Ui->pc_PushButtonUnderline->isChecked());

   Q_EMIT this->SigFontUpdate(c_Font);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get size index

   \param[in] orc_Font Current font

   \return
   Size combo box index
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OgeWiFontConfig::m_GetSizeIndex(const QFont & orc_Font) const
{
   int32_t s32_Retval = -1;
   int32_t s32_Size;

   if (orc_Font.pointSize() > 0)
   {
      s32_Size = orc_Font.pointSize();
   }
   else
   {
      s32_Size = orc_Font.pixelSize();
   }

   switch (s32_Size)
   {
   case 9:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_9;
      break;
   case 10:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_10;
      break;
   case 11:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_11;
      break;
   case 12:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_12;
      break;
   case 14:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_14;
      break;
   case 16:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_16;
      break;
   case 18:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_18;
      break;
   case 20:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_20;
      break;
   case 22:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_22;
      break;
   case 24:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_24;
      break;
   case 26:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_26;
      break;
   case 28:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_28;
      break;
   case 36:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_36;
      break;
   case 48:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_48;
      break;
   case 72:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_72;
      break;
   case 100:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_100;
      break;
   case 200:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_200;
      break;
   case 300:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_300;
      break;
   case 400:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_400;
      break;
   case 500:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_500;
      break;
   case 8: // chosen as default
   default:
      s32_Retval = C_OgeWiFontConfig::mhs32_INDEX_SIZE_8;
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update font size

   \param[in,out] orc_Font Font to update
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiFontConfig::m_GetSize(QFont & orc_Font) const
{
   switch (this->mpc_Ui->pc_ComboBoxSize->currentIndex())
   {
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_9:
      orc_Font.setPixelSize(9);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_10:
      orc_Font.setPixelSize(10);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_11:
      orc_Font.setPixelSize(11);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_12:
      orc_Font.setPixelSize(12);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_14:
      orc_Font.setPixelSize(14);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_16:
      orc_Font.setPixelSize(16);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_18:
      orc_Font.setPixelSize(18);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_20:
      orc_Font.setPixelSize(20);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_22:
      orc_Font.setPixelSize(22);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_24:
      orc_Font.setPixelSize(24);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_26:
      orc_Font.setPixelSize(26);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_28:
      orc_Font.setPixelSize(28);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_36:
      orc_Font.setPixelSize(36);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_48:
      orc_Font.setPixelSize(48);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_72:
      orc_Font.setPixelSize(72);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_100:
      orc_Font.setPixelSize(100);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_200:
      orc_Font.setPixelSize(200);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_300:
      orc_Font.setPixelSize(300);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_400:
      orc_Font.setPixelSize(400);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_500:
      orc_Font.setPixelSize(500);
      break;
   case C_OgeWiFontConfig::mhs32_INDEX_SIZE_8: // chosen as default
   default:
      orc_Font.setPixelSize(8);
      break;
   }
}
