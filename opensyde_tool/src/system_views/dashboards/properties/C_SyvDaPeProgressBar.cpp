//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard progress bar properties (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"

#include "TglUtils.hpp"
#include "C_SyvDaPeProgressBar.hpp"
#include "ui_C_SyvDaPeProgressBar.h"

#include "C_GiSvDaProgressBarBase.hpp"
#include "C_GtGetText.hpp"
#include "C_GiProgressBarUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE1 = 0;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE2 = 1;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE3 = 2;

const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_LEFT = 0;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_RIGHT = 1;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_TOP = 0;
const int32_t C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_BOTTOM = 1;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent    Parent widget
   \param[in]      oq_DarkMode   Flag for dark mode
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeProgressBar::C_SyvDaPeProgressBar(C_SyvDaPeBase & orc_Parent, const bool oq_DarkMode) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeProgressBar),
   mrc_ParentDialog(orc_Parent),
   mq_DarkMode(oq_DarkMode)
{
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   InitStaticNames();

   this->m_TypeChanged();

   //Deactivate debug text
   this->mpc_Ui->pc_CheckBoxMinMax->setText("");

   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType,
           static_cast<void (QComboBox::*)(int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeProgressBar::m_TypeChanged);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxAlignment,
           static_cast<void (QComboBox::*)(int32_t)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeProgressBar::m_UpdatePreview);
   connect(this->mpc_Ui->pc_CheckBoxMinMax, &QCheckBox::toggled, this, &C_SyvDaPeProgressBar::m_UpdatePreview);
   connect(&this->mrc_ParentDialog, &C_SyvDaPeBase::SigRefresh, this, &C_SyvDaPeProgressBar::m_UpdatePreview);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeProgressBar::~C_SyvDaPeProgressBar(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::InitStaticNames(void) const
{
   this->mpc_Ui->pc_LabelShowMinMax->setText(C_GtGetText::h_GetText("Show Min / Max"));
   this->mpc_Ui->pc_LabelStyle->setText(C_GtGetText::h_GetText("Style"));
   this->mpc_Ui->pc_LabelAlignment->setText(C_GtGetText::h_GetText("Alignment"));

   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 1"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 2"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("Type 3"));

   //Tool tips
   this->mpc_Ui->pc_LabelStyle->SetToolTipInformation(C_GtGetText::h_GetText("Style"),
                                                      C_GtGetText::h_GetText(
                                                         "Different options for styles specific to this widget"));
   this->mpc_Ui->pc_LabelAlignment->SetToolTipInformation(C_GtGetText::h_GetText("Alignment"),
                                                          C_GtGetText::h_GetText(
                                                             "Option for different alignments of the value label"));
   this->mpc_Ui->pc_LabelShowMinMax->SetToolTipInformation(C_GtGetText::h_GetText("Show Min / Max"),
                                                           C_GtGetText::h_GetText(
                                                              "Option to display the minimum and maximum value next to the progress bar or hide them entirely"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get show minimum and maximum flag

   \return
   Current show minimum and maximum flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPeProgressBar::GetShowMinMax(void) const
{
   return this->mpc_Ui->pc_CheckBoxMinMax->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type

   \return
   Current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbProgressBar::E_Type C_SyvDaPeProgressBar::GetType(void) const
{
   C_PuiSvDbProgressBar::E_Type e_Retval;

   switch (this->mpc_Ui->pc_ComboBoxType->currentIndex())
   {
   case C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE2:
      e_Retval = C_PuiSvDbProgressBar::eTYPE_2;
      break;
   case C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE3:
      e_Retval = C_PuiSvDbProgressBar::eTYPE_3;
      break;
   default:
      e_Retval = C_PuiSvDbProgressBar::eTYPE_1;
      break;
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get alignment

   \return
   C_PuiSvDbProgressBar::E_Alignment - Current alignment
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbProgressBar::E_Alignment C_SyvDaPeProgressBar::GetAlignment(void) const
{
   C_PuiSvDbProgressBar::E_Alignment e_Retval;

   if (this->mpc_Ui->pc_ComboBoxType->currentIndex() == C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE1)
   {
      if (this->mpc_Ui->pc_ComboBoxAlignment->currentIndex() == C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_BOTTOM)
      {
         e_Retval = C_PuiSvDbProgressBar::eBOTTOM;
      }
      else
      {
         e_Retval = C_PuiSvDbProgressBar::eTOP;
      }
   }
   else
   {
      if (this->mpc_Ui->pc_ComboBoxAlignment->currentIndex() == C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_LEFT)
      {
         e_Retval = C_PuiSvDbProgressBar::eLEFT;
      }
      else
      {
         e_Retval = C_PuiSvDbProgressBar::eRIGHT;
      }
   }

   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set show minimum and maximum flag

   \param[in]  oq_Value    New show minimum and maximum flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::SetShowMinMax(const bool oq_Value) const
{
   this->mpc_Ui->pc_CheckBoxMinMax->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type

   \param[in]  oe_Type  New type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::SetType(const C_PuiSvDbProgressBar::E_Type oe_Type)
{
   switch (oe_Type)
   {
   case C_PuiSvDbProgressBar::eTYPE_1:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE1);
      break;
   case C_PuiSvDbProgressBar::eTYPE_2:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE2);
      break;
   case C_PuiSvDbProgressBar::eTYPE_3:
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_STYLE_TYPE3);
      break;
   default:
      tgl_assert(false);
      break;
   }

   this->m_TypeChanged();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set alignment

   \param[in]  oe_Alignment   New alignment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::SetAlignment(const C_PuiSvDbProgressBar::E_Alignment oe_Alignment) const
{
   switch (oe_Alignment)
   {
   case C_PuiSvDbProgressBar::eTOP:
      this->mpc_Ui->pc_ComboBoxAlignment->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_TOP);
      break;
   case C_PuiSvDbProgressBar::eLEFT:
      this->mpc_Ui->pc_ComboBoxAlignment->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_LEFT);
      break;
   case C_PuiSvDbProgressBar::eRIGHT:
      this->mpc_Ui->pc_ComboBoxAlignment->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_RIGHT);
      break;
   case C_PuiSvDbProgressBar::eBOTTOM:
      this->mpc_Ui->pc_ComboBoxAlignment->setCurrentIndex(C_SyvDaPeProgressBar::mhs32_INDEX_ALIGNMENT_BOTTOM);
      break;
   default:
      tgl_assert(false);
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::m_TypeChanged(void)
{
   // Special case: Options for alignment depends of selection of type

   this->m_FillAlignmentComboBox(this->mpc_Ui->pc_ComboBoxType->currentIndex());
   this->m_UpdatePreview();
}

//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::m_FillAlignmentComboBox(const int32_t os32_Type) const
{
   this->mpc_Ui->pc_ComboBoxAlignment->clear();

   if (os32_Type == mhs32_INDEX_STYLE_TYPE1)
   {
      this->mpc_Ui->pc_ComboBoxAlignment->addItem(C_GtGetText::h_GetText("Top"));
      this->mpc_Ui->pc_ComboBoxAlignment->addItem(C_GtGetText::h_GetText("Bottom"));
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxAlignment->addItem(C_GtGetText::h_GetText("Left"));
      this->mpc_Ui->pc_ComboBoxAlignment->addItem(C_GtGetText::h_GetText("Right"));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update of preview click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::m_UpdatePreview(void)
{
   // Constants
   const float32_t f32_SCENE_WIDTH = 260.0F;
   const float32_t f32_SCENE_HEIGHT = 160.0F;
   const QSizeF c_SCENE_SIZE(f32_SCENE_WIDTH, f32_SCENE_HEIGHT);
   const int32_t s32_DEFAULT_VALUE = 50;

   C_GiSvDaProgressBarBase * const pc_Item = new C_GiSvDaProgressBarBase(0UL, 0UL, -1L, 0ULL, NULL);

   QSizeF c_ItemSize;
   QPointF c_ItemPosition;

   m_GetProgressBarPreviewSizeLocation(c_SCENE_SIZE, &c_ItemSize,
                                       &c_ItemPosition);

   pc_Item->SetValuePe(s32_DEFAULT_VALUE);
   pc_Item->SetDisplayStyle(this->mrc_ParentDialog.GetTheme(), this->mq_DarkMode);
   pc_Item->UpdateTypePe(this->GetType(), this->GetAlignment(), this->GetShowMinMax());
   pc_Item->ApplySizeChange(c_ItemPosition, c_ItemSize);

   // clear old scene
   this->mrc_ParentDialog.GetPreviewScene()->clear();

   // add item
   this->mrc_ParentDialog.GetPreviewScene()->addItem(pc_Item);
   this->mrc_ParentDialog.GetPreviewScene()->clearSelection();
} //lint !e429  //no memory leak because of the parent of pc_Item, the call of addItem and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get type 1, 2, or 3 progress bar preview size and position

   Therefore, the type (eTYPE_1, eTYPE_2, eTYPE_3) and the alignment (C_PuiSvDbProgressBar::eTOP,
   C_PuiSvDbProgressBar::eBOTTOM, C_PuiSvDbProgressBar::eLEFT, C_PuiSvDbProgressBar::eRIGHT) has to
   be set correctly

   \param[in]       orc_SceneSize               Scene size of the preview scene
   \param[in,out]   opf32_ProgressBarRecSize    Size of the progress bar preview in scene
   \param[in,out]   opf32_ProgressBarPosition   Position of the progress bar preview in scene

*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeProgressBar::m_GetProgressBarPreviewSizeLocation(const QSizeF & orc_SceneSize,
                                                               QSizeF * const opc_ProgressBarRecSize,
                                                               QPointF * const opc_ProgressBarPosition)
{
   //Constants
   const bool q_ShowMinMax = GetShowMinMax();
   const C_PuiSvDbProgressBar::E_Type e_ProgressBarType = this->GetType();

   const C_PuiSvDbProgressBar::E_Alignment e_ProgressBarAlignment = this->GetAlignment();
   const QPoint c_ZERO_POSITION = QPoint(0, 0);

   //Variables
   uint32_t u32_TemporaryRectangularWidth;
   uint32_t u32_TemporaryRectangularHeight;

   float32_t f32_ProgressBarXposition = .0F;
   float32_t f32_ProgressBarYposition = .0F;

   QMarginsF c_Margins;
   QRect c_Rectangular;

   if (e_ProgressBarType == C_PuiSvDbProgressBar::E_Type::eTYPE_1)
   {
      //Constants
      const QString c_MinimumValue = "0";
      const QString c_MaximumValue = "100";
      const QString c_Unit = "";
      const float32_t f32_ARROW_OFFSET = 15.0F;

      u32_TemporaryRectangularWidth = 200;
      u32_TemporaryRectangularHeight = 50;

      c_Rectangular = QRect(c_ZERO_POSITION.x(),
                            c_ZERO_POSITION.y(),
                            u32_TemporaryRectangularWidth,
                            u32_TemporaryRectangularHeight);

      c_Margins = C_GiProgressBarUtil::h_GetType1Margins(c_Rectangular, q_ShowMinMax,
                                                         c_MaximumValue,
                                                         c_MinimumValue,
                                                         c_Unit,
                                                         e_ProgressBarAlignment,
                                                         mf32_MinimumFontSize,
                                                         f32_ARROW_OFFSET);
   }
   else
   {
      u32_TemporaryRectangularWidth = 125;
      u32_TemporaryRectangularHeight = 115;

      c_Rectangular = QRect(c_ZERO_POSITION.x(),
                            c_ZERO_POSITION.y(),
                            u32_TemporaryRectangularWidth,
                            u32_TemporaryRectangularHeight);

      if (e_ProgressBarType == C_PuiSvDbProgressBar::E_Type::eTYPE_2)
      {
         c_Margins = C_GiProgressBarUtil::h_GetType2Margins(c_Rectangular,
                                                            q_ShowMinMax,
                                                            e_ProgressBarAlignment,
                                                            mf32_MinimumFontSize);
      }
      else
      {
         c_Margins = C_GiProgressBarUtil::h_GetType3Margins(c_Rectangular,
                                                            q_ShowMinMax,
                                                            e_ProgressBarAlignment,
                                                            mf32_MinimumFontSize);
      }
   }

   const float32_t f32_ProgressBarHeight = static_cast<float32_t>(c_Rectangular.height()) -
                                           (static_cast<float32_t>(c_Margins.bottom()) +
                                            static_cast<float32_t>(c_Margins.top()));
   const float32_t f32_ProgressBarWidth = static_cast<float32_t>(c_Rectangular.width()) -
                                          (static_cast<float32_t>(c_Margins.left()) +
                                           static_cast<float32_t>(c_Margins.right()));
   const QSizeF c_ProgressBarSize(f32_ProgressBarWidth, f32_ProgressBarHeight);

   if (e_ProgressBarAlignment == C_PuiSvDbProgressBar::eTOP)
   {
      f32_ProgressBarXposition = (static_cast<float32_t>(orc_SceneSize.width()) / mf32_HalfModifier) -
                                 (static_cast<float32_t>(c_ProgressBarSize.width()) / mf32_HalfModifier);
      f32_ProgressBarYposition = ((static_cast<float32_t>(orc_SceneSize.height()) / mf32_HalfModifier) -
                                  (static_cast<float32_t>(c_ProgressBarSize.height()) / mf32_HalfModifier)) -
                                 static_cast<float32_t>(c_Margins.top());
   }
   else if (e_ProgressBarAlignment == C_PuiSvDbProgressBar::eBOTTOM)
   {
      f32_ProgressBarXposition = (static_cast<float32_t>(orc_SceneSize.width()) / mf32_HalfModifier) -
                                 (static_cast<float32_t>(c_ProgressBarSize.width()) / mf32_HalfModifier);
      f32_ProgressBarYposition = (static_cast<float32_t>(orc_SceneSize.height()) / mf32_HalfModifier) -
                                 (static_cast<float32_t>(c_ProgressBarSize.height()) / mf32_HalfModifier);
   }
   else if (e_ProgressBarAlignment == C_PuiSvDbProgressBar::eLEFT)
   {
      f32_ProgressBarXposition = ((static_cast<float32_t>(orc_SceneSize.width()) / mf32_HalfModifier) -
                                  (static_cast<float32_t>(c_ProgressBarSize.width()) / mf32_HalfModifier)) -
                                 static_cast<float32_t>(c_Margins.left());
      f32_ProgressBarYposition = ((static_cast<float32_t>(orc_SceneSize.height()) / mf32_HalfModifier) -
                                  (static_cast<float32_t>(c_ProgressBarSize.height()) / mf32_HalfModifier)) -
                                 static_cast<float32_t>(c_Margins.top());
   }
   else if (e_ProgressBarAlignment == C_PuiSvDbProgressBar::eRIGHT)
   {
      f32_ProgressBarXposition = (static_cast<float32_t>(orc_SceneSize.width()) / mf32_HalfModifier) -
                                 (static_cast<float32_t>(c_ProgressBarSize.width()) / mf32_HalfModifier);
      f32_ProgressBarYposition = ((static_cast<float32_t>(orc_SceneSize.height()) / mf32_HalfModifier) -
                                  (static_cast<float32_t>(c_ProgressBarSize.height()) / mf32_HalfModifier)) -
                                 static_cast<float32_t>(c_Margins.top());
   }
   else
   {
      //nothing else to do
   }

   // Set final values
   opc_ProgressBarRecSize->setWidth(static_cast<float32_t>(u32_TemporaryRectangularWidth));
   opc_ProgressBarRecSize->setHeight(static_cast<float32_t>(u32_TemporaryRectangularHeight));

   opc_ProgressBarPosition->setX(f32_ProgressBarXposition);
   opc_ProgressBarPosition->setY(f32_ProgressBarYposition);
}
