//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing points as index indicator (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QPainter>

#include "stwtypes.hpp"
#include "C_OgeWiBopperle.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_OgeWiBopperle::mhs32_POINT_SIZE = 5;
const int32_t C_OgeWiBopperle::mhs32_POINT_DISTANCE = 10;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiBopperle::C_OgeWiBopperle(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   ms32_CountIndex(1),
   ms32_CurrentIndex(0)
{
   this->m_AdaptSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the maximum count of index points

   \param[in]  os32_Count  Maximum count of index points
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiBopperle::SetCountIndex(const int32_t os32_Count)
{
   this->ms32_CountIndex = os32_Count;
   this->m_AdaptSize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the current index point

   \param[in]  os32_Index  Current index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiBopperle::SetCurrentIndex(const int32_t os32_Index)
{
   this->ms32_CurrentIndex = os32_Index;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the colors of the points

   \param[in]  orc_ColorActive      Color of the active point
   \param[in]  orc_ColorInactive    Color of the inactive point
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiBopperle::SetColor(const QColor & orc_ColorActive, const QColor & orc_ColorInactive)
{
   this->mc_ColorActive = orc_ColorActive;
   this->mc_ColorInactive = orc_ColorInactive;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden paint event

   Draws the background element

   \param[in,out]  opc_Event  Pointer to paint event
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiBopperle::paintEvent(QPaintEvent * const opc_Event)
{
   QPainter c_Painter(this);
   int32_t s32_Counter;
   int32_t s32_CoordHorizontal = mhs32_POINT_DISTANCE;

   // set the initial color
   c_Painter.setPen(Qt::transparent);
   c_Painter.setBrush(this->mc_ColorInactive);

   for (s32_Counter = 0; s32_Counter < this->ms32_CountIndex; ++s32_Counter)
   {
      if (s32_Counter == this->ms32_CurrentIndex)
      {
         // the selected index. Change the color
         c_Painter.setBrush(this->mc_ColorActive);
      }

      // draw the circle
      c_Painter.drawEllipse(s32_CoordHorizontal, (mhs32_POINT_SIZE / 2), mhs32_POINT_SIZE, mhs32_POINT_SIZE);
      s32_CoordHorizontal += (mhs32_POINT_DISTANCE + mhs32_POINT_SIZE);

      if (s32_Counter == this->ms32_CurrentIndex)
      {
         // change the color back
         c_Painter.setBrush(this->mc_ColorInactive);
      }
   }

   QWidget::paintEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiBopperle::m_AdaptSize()
{
   const int32_t s32_CountIndex = this->ms32_CountIndex;

   this->setMinimumSize(((mhs32_POINT_SIZE * s32_CountIndex) + (mhs32_POINT_DISTANCE * (s32_CountIndex + 1))),
                        (mhs32_POINT_SIZE * 2));
   this->setMaximumSize(((mhs32_POINT_SIZE * s32_CountIndex) + (mhs32_POINT_DISTANCE * (s32_CountIndex + 1))),
                        (mhs32_POINT_SIZE * 2));
}
