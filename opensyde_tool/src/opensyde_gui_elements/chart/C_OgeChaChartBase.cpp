//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base implementation for Qt chart (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OgeChaChartBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace QtCharts;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeChaChartBase::C_OgeChaChartBase(QGraphicsItem * const opc_Parent) :
   QChart(opc_Parent),
   mq_DrawingActive(true)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in] oq_Active Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaChartBase::SetDrawingActive(const bool oq_Active)
{
   this->mq_DrawingActive = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten paint event slot

   Here: Don't paint if not visible

   \param[in,out] opc_Painter Painter
   \param[in,out] opc_Option  Options
   \param[in,out] opc_Widget  Widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeChaChartBase::paint(QPainter * const opc_Painter, const QStyleOptionGraphicsItem * const opc_Option,
                              QWidget * const opc_Widget)
{
   if (this->mq_DrawingActive)
   {
      QChart::paint(opc_Painter, opc_Option, opc_Widget);
   }
}
