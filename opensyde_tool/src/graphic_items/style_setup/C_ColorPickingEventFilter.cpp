////----------------------------------------------------------------------------------------------------------------------
///*!
//   \file
//   \brief       Short description

//   Detailed description (optional). The module shall be described detailed if it is not described completely
//   by short description.

//   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
//*/
////----------------------------------------------------------------------------------------------------------------------

///* -- Includes ------------------------------------------------------------------------------------------------------ */
//#include "precomp_headers.h"

//#include "stwtypes.h"
//#include "C_ColorPickingEventFilter.h"

///* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
//using namespace stw_types;
//using namespace stw_opensyde_gui;

///* -- Module Global Constants --------------------------------------------------------------------------------------- */

///* -- Types --------------------------------------------------------------------------------------------------------- */

///* -- Global Variables ---------------------------------------------------------------------------------------------- */

///* -- Module Global Variables --------------------------------------------------------------------------------------- */

///* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

///* -- Implementation ------------------------------------------------------------------------------------------------ */

////----------------------------------------------------------------------------------------------------------------------
///*! \brief  Short function description

//   Detailed function description (optional). The function shall be described detailed if it is not described completely
//   by short function description and parameter description.


//   \param[in]       ou8_Aa     Detailed input parameter description
//   \param[out]      opu32_Bb   Detailed output parameter description
//   \param[in,out]   opu16_Cc   Detailed input/output parameter description

//   \return
//   Type of return values, e.g. STW error codes

//   \retval   Return value 1   Detailed description of 1st return value
//   \retval   Return value 2   Detailed description of 2nd return value
//*/
////----------------------------------------------------------------------------------------------------------------------
//C_ColorPickingEventFilter::C_ColorPickingEventFilter(C_GiSyColorSelectWidget * csw, QObject * parent) :
//   QObject(parent),
//   m_csw(csw)
//{
//}

////----------------------------------------------------------------------------------------------------------------------
///*! \brief   Default destructor
//*/
////----------------------------------------------------------------------------------------------------------------------
//C_ColorPickingEventFilter::~C_ColorPickingEventFilter(void)
//{
//}
////----------------------------------------------------------------------------------------------------------------------
//bool C_ColorPickingEventFilter::eventFilter(QObject *, QEvent *event)
//{
//    switch (event->type()) {
//    case QEvent::MouseMove:
//        return m_csw->handleColorPickingMouseMove(static_cast<QMouseEvent *>(event));
//    case QEvent::MouseButtonRelease:
//        return m_csw->handleColorPickingMouseButtonRelease(static_cast<QMouseEvent *>(event));
//    case QEvent::KeyPress:
//        return m_csw->handleColorPickingKeyPress(static_cast<QKeyEvent *>(event));
//    default:
//        break;
//    }
//    return false;
//}
