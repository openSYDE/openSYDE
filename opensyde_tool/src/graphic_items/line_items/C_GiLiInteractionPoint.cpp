//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Interactable point for line (implementation)

   Interactable point for line

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPen>

#include "C_GiLiInteractionPoint.hpp"
#include "gitypes.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_PointIndex Index of the corresponding point
   \param[in,out] opc_Parent       Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiInteractionPoint::C_GiLiInteractionPoint(const int32_t & ors32_PointIndex, QGraphicsItem * const opc_Parent) :
   QObject(),
   C_GiPointInteraction(opc_Parent),
   C_GiBiCustomMouseItem(),
   ms32_PointIndex(ors32_PointIndex)
{
   this->setFlag(ItemIsMovable);
   this->setFlag(ItemSendsGeometryChanges);

   this->mc_DefaultCursor = static_cast<QCursor>(Qt::SizeBDiagCursor);
   this->C_GiLiInteractionPoint::RestoreDefaultCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiInteractionPoint::~C_GiLiInteractionPoint()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiLiInteractionPoint::type() const
{
   return ms32_GRAPHICS_ITEM_LINEINTERACTION;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set point index

   \param[in] ors32_PointIndex Index of the corresponding point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiInteractionPoint::SetPointIndex(const int32_t & ors32_PointIndex)
{
   this->ms32_PointIndex = ors32_PointIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get point index

   \return   Index of the corresponding point
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiLiInteractionPoint::GetPointIndex(void) const
{
   return this->ms32_PointIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Restore default mouse cursor
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
/*! \brief  Set default mouse cursor

   \param[in] orc_Value New default mouse cursor
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiLiInteractionPoint::SetDefaultCursor(const QCursor & orc_Value)
{
   this->setCursor(orc_Value);
   this->mc_DefaultCursor = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten itemChange event slot

   Here: Signal position change

   \param[in,out] oe_Change Indicator what changed
   \param[in]     orc_Value Value corresponding to change

   \return
   new value
*/
//----------------------------------------------------------------------------------------------------------------------
QVariant C_GiLiInteractionPoint::itemChange(const GraphicsItemChange oe_Change, const QVariant & orc_Value)
{
   QVariant c_Retval = QGraphicsEllipseItem::itemChange(oe_Change, orc_Value);

   switch (oe_Change) //lint !e788 //All other cases handled by call of parent
   {
   case ItemPositionChange:
   case ItemPositionHasChanged:
      Q_EMIT this->SigPointChanged(ms32_PointIndex);
      break;
   default:
      break;
   }
   return c_Retval;
}
