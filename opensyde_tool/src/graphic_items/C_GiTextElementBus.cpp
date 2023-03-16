//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text element graphics item for showing bus name (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>
#include <QGraphicsSceneHoverEvent>

#include "gitypes.hpp"
#include "constants.hpp"

#include "C_GtGetText.hpp"
#include "C_GiTextElementBus.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_Uti.hpp"
#include "C_SdUtil.hpp"
#include "C_PuiSdUtil.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]       ors32_Index          Index of data element in system definition
   \param[in]       oru64_Id             Unique ID
   \param[in,out]   opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiTextElementBus::C_GiTextElementBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                                       QGraphicsItem * const opc_Parent) :
   C_GiBiTextElement(oru64_Id, false, opc_Parent),
   C_PuiSdDataElement(ors32_Index, C_PuiSdDataElement::eTEXT_ELEMENT_BUS),
   C_GiBiCustomToolTip(),
   mq_ErrorIconHovered(false),
   mu32_BusIndex(0U),
   mq_ErrorState(false),
   mq_ErrorResized(false),
   mpc_SvgRenderer(NULL)
{
   QFont c_Font;
   const QString c_SvgString("://images/Error_iconV2.svg");

   this->mpc_SvgRenderer = new QSvgRenderer(c_SvgString);

   // Set default font
   //lint -e{1938} //we don't create global objects of this class; no race conditions can occur
   c_Font = mc_STYLE_GUIDE_FONT_REGULAR_16;
   //Convert point to pixel
   c_Font.setPixelSize(c_Font.pointSize());
   this->mpc_TextItem->setFont(c_Font);
   this->mpc_TextItem->setDefaultTextColor(mc_STYLE_GUIDE_COLOR_4);

   this->setAcceptHoverEvents(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiTextElementBus::~C_GiTextElementBus()
{
   delete mpc_SvgRenderer;
   mpc_SvgRenderer = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the associated bus index

   \param[in]     ou32_BusIndex     Index of associated bus
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::SetBusIndex(const uint32_t ou32_BusIndex)
{
   this->mu32_BusIndex = ou32_BusIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the associated bus index

   \return
   Index of associated bus
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_GiTextElementBus::GetBusIndex(void) const
{
   return this->mu32_BusIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiTextElementBus::type(void) const
{
   return ms32_GRAPHICS_ITEM_TEXTELEMENT_BUS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for initially loading internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::LoadData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_BusTextElements.size() > static_cast<uint32_t>(this->ms32_Index))
   {
      C_PuiSdTextElementBus & rc_UiTextElementData =
         C_PuiSdHandler::h_GetInstance()->c_BusTextElements[static_cast<uint32_t>(this->ms32_Index)];

      this->m_LoadTextElementData(&rc_UiTextElementData, false);

      this->mu32_BusIndex = rc_UiTextElementData.u32_BusIndex;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::UpdateData(void)
{
   if (C_PuiSdHandler::h_GetInstance()->c_BusTextElements.size() > static_cast<uint32_t>(this->ms32_Index))
   {
      C_PuiSdTextElementBus * const pc_Item = &C_PuiSdHandler::h_GetInstance()->c_BusTextElements[this->ms32_Index];

      this->m_UpdateTextElementData(pc_Item, false);

      pc_Item->u32_BusIndex = this->mu32_BusIndex;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::DeleteData(void)
{
   C_PuiSdHandler::h_GetInstance()->c_BusTextElements.erase(C_PuiSdHandler::h_GetInstance()->c_BusTextElements.begin() +
                                                            ms32_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle tooltip content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::GenerateHint(void)
{
   if ((this->mq_ErrorIconHovered == true) && (this->ms32_Index >= 0))
   {
      QString c_Content;
      C_SdUtil::h_GetErrorToolTipBus(this->ms32_Index, c_Content);
      this->SetDefaultToolTipHeading(C_GtGetText::h_GetText("Invalid Bus Content"));
      this->SetDefaultToolTipContent(c_Content);
      this->SetDefaultToolTipType(C_NagToolTip::eERROR);
   }
   else
   {
      this->SetDefaultToolTipHeading("");
      this->SetDefaultToolTipContent("");
      this->SetDefaultToolTipType(C_NagToolTip::eDEFAULT);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set font color

   Saving the set color in case of switch to disabled look

   \param[in] orc_Color New color
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::SetFontColor(const QColor & orc_Color)
{
   this->mc_OriginalColor = orc_Color;

   C_GiBiTextElement::SetFontColor(this->mc_OriginalColor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the actual error state for showing the error icon

   \param[in]     oq_Error      Actual error state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::SetErrorState(const bool oq_Error)
{
   this->mq_ErrorState = oq_Error;

   if ((oq_Error == true) &&
       (this->mq_ErrorResized == false))
   {
      QSizeF c_Size;
      c_Size.setWidth(this->GetSize().width() + 20.0);
      c_Size.setHeight(this->GetSize().height());
      this->ApplySizeChange(this->pos(), c_Size);

      // do it only once
      this->mq_ErrorResized = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set item disabled look flag

   \param[in] oq_Disabled Flag if item is disabled
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::SetDisabledLook(const bool oq_Disabled)
{
   if (oq_Disabled == true)
   {
      this->mpc_TextItem->setDefaultTextColor(C_Uti::h_GetDisabledColorLook(this->mc_OriginalColor));
   }
   else
   {
      this->mpc_TextItem->setDefaultTextColor(this->mc_OriginalColor);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Option
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                               QWidget * const opc_Widget)
{
   C_GiBiTextElement::paint(opc_Painter, opc_Option, opc_Widget);

   if ((this->mq_ErrorState == true) && (this->mpc_SvgRenderer != NULL))
   {
      //      opc_Painter->drawPixmap(static_cast<int32_t>(this->boundingRect().width()) - 55, -6, this->mc_ErrorIcon);
      this->mpc_SvgRenderer->render(opc_Painter, m_GetErrorIconRect());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do mouse move tool tip handling

   \param[in] orc_ScenePos Scene pos
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::HandleMouseMoveToolTip(const QPointF & orc_ScenePos)
{
   this->mq_ErrorIconHovered = false;

   if (this->mq_ErrorState == true)
   {
      if (m_GetErrorIconRect().contains(this->mapFromScene(orc_ScenePos)) == true)
      {
         this->mq_ErrorIconHovered = true;
      }
   }
   if (this->mq_ErrorIconHovered == false)
   {
      Q_EMIT this->SigHideToolTip();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Apply new Z value

   \param[in] of64_ZetValue New Z value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::SetZetValueCustom(const float64_t of64_ZetValue)
{
   C_GiBiTextElement::SetZetValueCustom(of64_ZetValue);
   //Apply to data
   this->UpdateData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten hover leave event slot

   Here: Hide tool tip

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiTextElementBus::hoverLeaveEvent(QGraphicsSceneHoverEvent * const opc_Event)
{
   Q_EMIT this->SigHideToolTip();

   C_GiBiTextElement::hoverLeaveEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get rect for error icon

   \return
   Rect for error icon
*/
//----------------------------------------------------------------------------------------------------------------------
QRectF C_GiTextElementBus::m_GetErrorIconRect(void) const
{
   return QRectF(QPointF(static_cast<float64_t>(this->boundingRect().width()) - 55.0, -6.0), QSizeF(24.0, 24.0));
}
