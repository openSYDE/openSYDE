//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Copy paste manager of base scene (implementation)

   Copy paste manager of base scene

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include "C_SebBaseCopyPasteManager.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebBaseCopyPasteManager::C_SebBaseCopyPasteManager(void) :
   mc_OriginalPosition(0.0, 0.0),
   mu32_PasteCounter(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebBaseCopyPasteManager::~C_SebBaseCopyPasteManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get point difference

   \param[in] orc_Position New position

   \return
   Point difference
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_SebBaseCopyPasteManager::GetDiff(const QPointF & orc_Position) const
{
   return orc_Position - this->mc_OriginalPosition;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get paste counter

   \return
   Paste counter value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SebBaseCopyPasteManager::GetPasteCounter(void) const
{
   return this->mu32_PasteCounter;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Increment paste counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::IncrementPasteCounter(void)
{
   ++this->mu32_PasteCounter;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Evaluate all items to get top left point

   \param[in] opc_Data Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data)
{
   if (opc_Data != NULL)
   {
      uint32_t u32_ItElem;

      this->mc_OriginalPosition.setX(std::numeric_limits<float64_t>::max());
      this->mc_OriginalPosition.setY(std::numeric_limits<float64_t>::max());

      for (u32_ItElem = 0; u32_ItElem < opc_Data->c_Boundaries.size(); ++u32_ItElem)
      {
         m_MinToOrgPos(opc_Data->c_Boundaries[u32_ItElem].c_UiPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < opc_Data->c_Images.size(); ++u32_ItElem)
      {
         m_MinToOrgPos(opc_Data->c_Images[u32_ItElem].c_UiPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < opc_Data->c_LineArrows.size(); ++u32_ItElem)
      {
         for (uint32_t u32_ItConnPoint =
                 0; u32_ItConnPoint < opc_Data->c_LineArrows[u32_ItElem].c_UiInteractionPoints.size();
              ++u32_ItConnPoint)
         {
            m_MinToOrgPos(opc_Data->c_LineArrows[u32_ItElem].c_UiInteractionPoints[u32_ItConnPoint]);
         }
      }

      for (u32_ItElem = 0; u32_ItElem < opc_Data->c_TextElements.size(); ++u32_ItElem)
      {
         m_MinToOrgPos(opc_Data->c_TextElements[u32_ItElem].c_UiPosition);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get minimal point

   \param[in] orc_P1 Point 1
   \param[in] orc_P2 Point 2

   \return
   Minimal point
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_SebBaseCopyPasteManager::mh_Min(const QPointF & orc_P1, const QPointF & orc_P2)
{
   return QPointF(std::min(orc_P1.x(), orc_P2.x()), std::min(orc_P1.y(), orc_P2.y()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Consider this point for original position

   \param[in] orc_Point New point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::m_MinToOrgPos(const QPointF & orc_Point)
{
   this->mc_OriginalPosition = mh_Min(this->mc_OriginalPosition, orc_Point);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle Z value for box

   \param[in]     opc_Item              Original item
   \param[in]     orc_NormalizedZetValues Normalized Z values for all copied items
   \param[in,out] orc_Box               Box data to change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::mh_HandleZetValueBox(const QGraphicsItem * const opc_Item,
                                                     const QMap<const QGraphicsItem *,
                                                                float64_t> & orc_NormalizedZetValues,
                                                     C_PuiBsBox & orc_Box)
{
   const QMap<const QGraphicsItem *, float64_t>::const_iterator c_ItItem = orc_NormalizedZetValues.find(opc_Item);

   if (c_ItItem != orc_NormalizedZetValues.end())
   {
      orc_Box.f64_ZetOrder = c_ItItem.value();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle Z value for line

   \param[in]     opc_Item              Original item
   \param[in]     orc_NormalizedZetValues Normalized Z values for all copied items
   \param[in,out] orc_Line              Line data to change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::mh_HandleZetValueLine(const QGraphicsItem * const opc_Item,
                                                      const QMap<const QGraphicsItem *,
                                                                 float64_t> & orc_NormalizedZetValues,
                                                      C_PuiBsLineBase & orc_Line)
{
   const QMap<const QGraphicsItem *, float64_t>::const_iterator c_ItItem = orc_NormalizedZetValues.find(opc_Item);

   if (c_ItItem != orc_NormalizedZetValues.end())
   {
      orc_Line.f64_ZetOrder = c_ItItem.value();
   }
}
