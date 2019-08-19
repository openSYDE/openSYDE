//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Copy paste manager of base scene (implementation)

   Copy paste manager of base scene

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <limits>
#include "C_SebBaseCopyPasteManager.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebBaseCopyPasteManager::C_SebBaseCopyPasteManager(void) :
   mc_OriginalPosition(0.0, 0.0),
   mu32_PasteCounter(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebBaseCopyPasteManager::~C_SebBaseCopyPasteManager(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set new position

   \param[in] orc_Position New position
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_SebBaseCopyPasteManager::GetDiff(const QPointF & orc_Position) const
{
   return orc_Position - this->mc_OriginalPosition;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get paste counter

   \return
   Paste counter value
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_SebBaseCopyPasteManager::GetPasteCounter(void) const
{
   return this->mu32_PasteCounter;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Increment paste counter
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::IncrementPasteCounter(void)
{
   ++this->mu32_PasteCounter;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Evaluate all items to get top left point

   \param[in] opc_Data Data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data)
{
   if (opc_Data != NULL)
   {
      uint32 u32_ItElem;

      this->mc_OriginalPosition.setX(std::numeric_limits<float64>::max());
      this->mc_OriginalPosition.setY(std::numeric_limits<float64>::max());

      for (u32_ItElem = 0; u32_ItElem < opc_Data->c_Boundaries.size(); ++u32_ItElem)
      {
         m_MinToOrgPos(opc_Data->c_Boundaries[u32_ItElem].c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < opc_Data->c_Images.size(); ++u32_ItElem)
      {
         m_MinToOrgPos(opc_Data->c_Images[u32_ItElem].c_UIPosition);
      }

      for (u32_ItElem = 0; u32_ItElem < opc_Data->c_LineArrows.size(); ++u32_ItElem)
      {
         for (uint32 u32_ItConnPoint =
                 0; u32_ItConnPoint < opc_Data->c_LineArrows[u32_ItElem].c_UIInteractionPoints.size();
              ++u32_ItConnPoint)
         {
            m_MinToOrgPos(opc_Data->c_LineArrows[u32_ItElem].c_UIInteractionPoints[u32_ItConnPoint]);
         }
      }

      for (u32_ItElem = 0; u32_ItElem < opc_Data->c_TextElements.size(); ++u32_ItElem)
      {
         m_MinToOrgPos(opc_Data->c_TextElements[u32_ItElem].c_UIPosition);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get minimal point

   \param[in] orc_P1 Point 1
   \param[in] orc_P2 Point 2
*/
//----------------------------------------------------------------------------------------------------------------------
QPointF C_SebBaseCopyPasteManager::mh_Min(const QPointF & orc_P1, const QPointF & orc_P2)
{
   return QPointF(std::min(orc_P1.x(), orc_P2.x()), std::min(orc_P1.y(), orc_P2.y()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Consider this point for original position

   \param[in] orc_P New point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::m_MinToOrgPos(const QPointF & orc_P)
{
   this->mc_OriginalPosition = mh_Min(this->mc_OriginalPosition, orc_P);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle Z value for box

   \param[in]     opc_Item              Original item
   \param[in]     orc_NormalizedZValues Normalized Z values for all copied items
   \param[in,out] orc_Box               Box data to change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::mh_HandleZValueBox(const QGraphicsItem * const opc_Item,
                                                   const QMap<const QGraphicsItem *,
                                                              float64> & orc_NormalizedZValues,
                                                   C_PuiBsBox & orc_Box)
{
   const QMap<const QGraphicsItem *, float64>::const_iterator c_ItItem = orc_NormalizedZValues.find(opc_Item);

   if (c_ItItem != orc_NormalizedZValues.end())
   {
      orc_Box.f64_ZOrder = c_ItItem.value();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle Z value for line

   \param[in]     opc_Item              Original item
   \param[in]     orc_NormalizedZValues Normalized Z values for all copied items
   \param[in,out] orc_Line              Line data to change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::mh_HandleZValueLine(const QGraphicsItem * const opc_Item,
                                                    const QMap<const QGraphicsItem *,
                                                               float64> & orc_NormalizedZValues,
                                                    C_PuiBsLineBase & orc_Line)
{
   const QMap<const QGraphicsItem *, float64>::const_iterator c_ItItem = orc_NormalizedZValues.find(opc_Item);

   if (c_ItItem != orc_NormalizedZValues.end())
   {
      orc_Line.f64_ZOrder = c_ItItem.value();
   }
}
