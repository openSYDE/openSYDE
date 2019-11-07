////----------------------------------------------------------------------------------------------------------------------
///*!
//   \file
//   \brief       Short description
//   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
//*/
////----------------------------------------------------------------------------------------------------------------------
//#ifndef C_COLORPICKINGEVENTFILTER_H
//#define C_COLORPICKINGEVENTFILTER_H

///* -- Includes ------------------------------------------------------------------------------------------------------ */
//#include <QObject>
//#include "C_GiSyColorSelectWidget.h"

///* -- Namespace ----------------------------------------------------------------------------------------------------- */
//namespace stw_opensyde_gui
//{

///* -- Global Constants ---------------------------------------------------------------------------------------------- */

///* -- Types --------------------------------------------------------------------------------------------------------- */
//// Event filter to be installed on the dialog while in color-picking mode.
//class C_ColorPickingEventFilter :
//   public QObject
//{
//    Q_OBJECT

//public:
//    explicit C_ColorPickingEventFilter(stw_opensyde_gui::C_GiSyColorSelectWidget *csw, QObject *parent = 0);

//    bool eventFilter(QObject *, QEvent *event) Q_DECL_OVERRIDE;
////    {
////        switch (event->type()) {
////        case QEvent::MouseMove:
////            return m_csw->handleColorPickingMouseMove(static_cast<QMouseEvent *>(event));
////        case QEvent::MouseButtonRelease:
////            return m_csw->handleColorPickingMouseButtonRelease(static_cast<QMouseEvent *>(event));
////        case QEvent::KeyPress:
////            return m_csw->handleColorPickingKeyPress(static_cast<QKeyEvent *>(event));
////        default:
////            break;
////        }
////        return false;
////    }

//private:
//    C_GiSyColorSelectWidget *m_csw;
//};

///* -- Extern Global Variables --------------------------------------------------------------------------------------- */
//} //end of namespace

//#endif
