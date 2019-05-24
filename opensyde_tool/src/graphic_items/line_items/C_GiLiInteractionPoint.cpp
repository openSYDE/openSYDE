//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interactable point for line (implementation)

   Interactable point for line

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QPen>

#include "C_GiLiInteractionPoint.h"
#include "gitypes.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_PointIndex Index of the corresponding point
   \param[in,out] opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiInteractionPoint::C_GiLiInteractionPoint(const sint32 & ors32_PointIndex, QGraphicsItem * const opc_Parent) :
   C_GiPointInteraction(opc_Parent),
   ms32_PointIndex(ors32_PointIndex)
{
   this->setFlag(ItemIsMovable);
   this->setFlag(ItemSendsGeometryChanges);

   this->mc_DefaultCursor = QCursor(Qt::SizeBDiagCursor);
   this->RestoreDefaultCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiInteractionPoint::~C_GiLiInteractionPoint()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiLiInteractionPoint::type() const
{
   return msn_GRAPHICS_ITEM_LINEINTERACTION;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set point index

   \param[in] ors32_PointIndex Index of the corresponding point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiInteractionPoint::SetPointIndex(const stw_types::sint32 & ors32_PointIndex)
{
   this->ms32_PointIndex = ors32_PointIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get point index

   \return   Index of the corresponding point
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_GiLiInteractionPoint::GetPointIndex(void) const
{
   return this->ms32_PointIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Restore default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiInteractionPoint::RestoreDefaultCursor(void)
{
   this->setCursor(this->mc_DefaultCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Change mouse cursor temporarily

   \param[in] orc_TemporaryCursor New mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiInteractionPoint::SetTemporaryCursor(const QCursor & orc_TemporaryCursor)
{
   this->setCursor(orc_TemporaryCursor);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set default mouse cursor

   \param[in] orc_Value New default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiInteractionPoint::SetDefaultCursor(const QCursor & orc_Value)
{
   this->setCursor(orc_Value);
   this->mc_DefaultCursor = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten itemChange event slot

   Here: Signal position change

   \param[in,out] oe_Change Indicator what changed
   \param[in]     orc_Value Value corresponding to change
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiLiInteractionPoint::itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value)
{
   QVariant c_Retval = QGraphicsEllipseItem::itemChange(oe_Change, orc_Value);

   switch (oe_Change)
   {
   case ItemPositionChange:
   case ItemPositionHasChanged:
      Q_EMIT this->PointChanged(ms32_PointIndex);
      break;
   default:
      break;
   } //lint !e788 //All other cases handled by call of parent
   return c_Retval;
}
