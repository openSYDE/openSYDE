//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Implementation for drawing element boundary (implementation)

   detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPen>
#include <QGraphicsView>

#include "stwtypes.h"
#include "C_GiBiBoundary.h"
#include "C_OgePopUpDialog.h"
#include "C_GtGetText.h"
#include "C_GiSyBaseWidget.h"
#include "C_GiSyBoundaryWidget.h"
#include "C_PuiSdDataElement.h"
#include "gitypes.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const float64 mf64_ACTION_POINT_OFFSET_BOUNDARY = 8.0;

const float64 C_GiBiBoundary::mhf64_MIN_WIDTH_BOUNDARY = 50.0;
const float64 C_GiBiBoundary::mhf64_MIN_HEIGHT_BOUNDARY = 50.0;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     oru64_ID    Unique ID
   \param[in]     of64_Width  Width of node
   \param[in]     of64_Height Height of node
   \param[in,out] opc_Parent  Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiBoundary::C_GiBiBoundary(const uint64 & oru64_ID, const float64 of64_Width, const float64 of64_Height,
                               QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiBiRectBaseGroup(oru64_ID,
                       mhf64_MIN_WIDTH_BOUNDARY, mhf64_MIN_HEIGHT_BOUNDARY, mf64_ACTION_POINT_OFFSET_BOUNDARY, false,
                       opc_Parent)
{
   //lint -e{1938}  static const is guaranteed preinitialized before main
   const QRectF c_Rect = QRectF(0.0, 0.0,
                                std::max(mhf64_MIN_WIDTH_BOUNDARY, of64_Width),
                                std::max(mhf64_MIN_HEIGHT_BOUNDARY, of64_Height));
   QPen c_Pen;
   QBrush c_Brush;

   //set default colors
   // border
   this->mpc_Rectangle = new C_GiRect(c_Rect);
   c_Pen = this->mpc_Rectangle->pen();
   c_Pen.setColor(QColor(39, 170, 225));
   c_Pen.setJoinStyle(Qt::MiterJoin);
   this->mpc_Rectangle->setPen(c_Pen);

   // background
   c_Brush = this->mpc_Rectangle->brush();
   c_Brush.setColor(QColor(250, 250, 250));
   c_Brush.setStyle(Qt::SolidPattern);
   this->mpc_Rectangle->setBrush(c_Brush);

   // Notify the base class about the boundary as biggest item as orientation. Very important!
   this->m_SetBiggestItem(*this->mpc_Rectangle);

   this->addToGroup(this->mpc_Rectangle);

   // Init z order
   this->setZValue(mf64_ZORDER_INIT_BOUNDARY);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiBiBoundary::~C_GiBiBoundary()
{
   //Deleted via Qt parent mechanism
   this->mpc_Rectangle = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiBiBoundary::type(void) const
{
   return msn_GRAPHICS_ITEM_BOUNDARY;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open style dialog

   \param[in] oq_DarkMode Optional dark mode flag

   \retval true   style dialog was accepted
   \retval false  style dialog was rejected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiBiBoundary::OpenStyleDialog(const bool oq_DarkMode)
{
   bool q_Retval;
   QGraphicsView * const pc_View = this->scene()->views().at(0);

   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
   C_GiSyBaseWidget * const pc_Dialog = new C_GiSyBaseWidget(*c_New, C_GtGetText::h_GetText("Boundary"), oq_DarkMode);
   C_GiSyBoundaryWidget * const pc_SettingsWidget = new C_GiSyBoundaryWidget(*pc_Dialog);

   pc_SettingsWidget->SetBorderColor(this->GetBorderColor());
   pc_SettingsWidget->SetBackgroundColor(this->GetBackgroundColor());
   pc_SettingsWidget->SetBorderWidth(this->GetBorderWidth());

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      this->SetBorderColor(pc_SettingsWidget->GetBorderColor());
      this->SetBackgroundColor(pc_SettingsWidget->GetBackgroundColor());
      this->SetBorderWidth(pc_SettingsWidget->GetBorderWidth());
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
   return q_Retval; //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Copy the style of the provided element

   Warning: Only expected to work if the provided item is of the same type as this element

   \param[in] opc_GuidelineItem Detailed input parameter description
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiBoundary::CopyStyle(const QGraphicsItem * const opc_GuidelineItem)
{
   const C_GiBiBoundary * const pc_Item = dynamic_cast<const C_GiBiBoundary * const>(opc_GuidelineItem);

   if (pc_Item != NULL)
   {
      this->SetBorderColor(pc_Item->GetBorderColor());
      this->SetBackgroundColor(pc_Item->GetBackgroundColor());
      this->SetBorderWidth(pc_Item->GetBorderWidth());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set border width

   \param[in] ors32_Width New line width
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiBoundary::SetBorderWidth(const stw_types::sint32 & ors32_Width)
{
   //Update pen
   QPen c_Pen = this->mpc_Rectangle->pen();

   c_Pen.setWidth(ors32_Width);

   this->mpc_Rectangle->setPen(c_Pen);

   //Redraw/resize rectangle
   this->mpc_Rectangle->Redraw();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set border color

   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiBoundary::SetBorderColor(const QColor & orc_Color)
{
   QPen c_Pen = this->mpc_Rectangle->pen();

   c_Pen.setColor(orc_Color);

   this->mpc_Rectangle->setPen(c_Pen);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set background color

   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiBoundary::SetBackgroundColor(const QColor & orc_Color)
{
   QBrush c_Brush = this->mpc_Rectangle->brush();

   c_Brush.setColor(orc_Color);

   this->mpc_Rectangle->setBrush(c_Brush);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get border width

   \return
   border width
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiBiBoundary::GetBorderWidth() const
{
   return this->mpc_Rectangle->pen().width();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get border color

   \return
   border color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiBiBoundary::GetBorderColor() const
{
   return this->mpc_Rectangle->pen().color();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get background color

   \return
   background color
*/
//----------------------------------------------------------------------------------------------------------------------
QColor C_GiBiBoundary::GetBackgroundColor() const
{
   return this->mpc_Rectangle->brush().color();
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiBiBoundary::m_ResizeUpdateItems(const float64 of64_DiffWidth, const float64 of64_DiffHeight)
{
   Q_UNUSED(of64_DiffWidth)
   Q_UNUSED(of64_DiffHeight)

   this->mpc_Rectangle->update();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load from internal data

   \param[in] orc_Data    Boundary data
   \param[in] oq_DarkMode Optional dark mode flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiBoundary::m_LoadFromData(const C_PuiBsBoundary & orc_Data, const bool oq_DarkMode)
{
   this->LoadBasicData(orc_Data);
   if (oq_DarkMode == true)
   {
      this->SetBackgroundColor(orc_Data.c_UIBackgroundColorDark);
      this->SetBorderColor(orc_Data.c_UIBorderColorDark);
   }
   else
   {
      this->SetBackgroundColor(orc_Data.c_UIBackgroundColorBright);
      this->SetBorderColor(orc_Data.c_UIBorderColorBright);
   }
   this->SetBorderWidth(orc_Data.s32_UIBorderWidth);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update internal data

   \param[in] orc_Data    Boundary data
   \param[in] oq_DarkMode Optional dark mode flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiBiBoundary::m_UpdateData(C_PuiBsBoundary & orc_Data, const bool oq_DarkMode) const
{
   this->UpdateBasicData(orc_Data);

   if (oq_DarkMode == true)
   {
      orc_Data.c_UIBorderColorDark = this->GetBorderColor();
      orc_Data.c_UIBackgroundColorDark = this->GetBackgroundColor();
   }
   else
   {
      orc_Data.c_UIBorderColorBright = this->GetBorderColor();
      orc_Data.c_UIBackgroundColorBright = this->GetBackgroundColor();
   }
   orc_Data.s32_UIBorderWidth = this->GetBorderWidth();
}
