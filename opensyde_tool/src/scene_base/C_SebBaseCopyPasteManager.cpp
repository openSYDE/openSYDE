//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Copy paste manager of base scene (implementation)

   Copy paste manager of base scene

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.05.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <limits>
#include "C_SebBaseCopyPasteManager.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     10.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SebBaseCopyPasteManager::C_SebBaseCopyPasteManager(void) :
   mc_OriginalPosition(0.0, 0.0),
   mu32_PasteCounter(0)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     10.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SebBaseCopyPasteManager::~C_SebBaseCopyPasteManager(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new position

   \param[in] orc_Position New position

   \created     14.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QPointF C_SebBaseCopyPasteManager::GetDiff(const QPointF & orc_Position) const
{
   return orc_Position - this->mc_OriginalPosition;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get paste counter

   \return
   Paste counter value

   \created     14.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
uint32 C_SebBaseCopyPasteManager::GetPasteCounter(void) const
{
   return this->mu32_PasteCounter;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Increment paste counter

   \created     14.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::IncrementPasteCounter(void)
{
   ++this->mu32_PasteCounter;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Evaluate all items to get top left point

   \param[in] opc_Data Data

   \created     24.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get minimal point

   \param[in] orc_P1 Point 1
   \param[in] orc_P2 Point 2

   \created     14.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QPointF C_SebBaseCopyPasteManager::mh_Min(const QPointF & orc_P1, const QPointF & orc_P2)
{
   return QPointF(std::min(orc_P1.x(), orc_P2.x()), std::min(orc_P1.y(), orc_P2.y()));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Consider this point for original position

   \param[in] orc_P New point

   \created     14.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SebBaseCopyPasteManager::m_MinToOrgPos(const QPointF & orc_P)
{
   this->mc_OriginalPosition = mh_Min(this->mc_OriginalPosition, orc_P);
}
