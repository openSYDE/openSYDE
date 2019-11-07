////----------------------------------------------------------------------------------------------------------------------
///*!
//   \file
//   \brief       Short description
//   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
//*/
////----------------------------------------------------------------------------------------------------------------------
//#ifndef C_COLORSHOWLABEL_H
//#define C_COLORSHOWLABEL_H

///* -- Includes ------------------------------------------------------------------------------------------------------ */
//#include "qdrag.h"
//#include "qlabel.h"

///* -- Namespace ----------------------------------------------------------------------------------------------------- */
//namespace stw_opensyde_gui
//{

///* -- Global Constants ---------------------------------------------------------------------------------------------- */

///* -- Types --------------------------------------------------------------------------------------------------------- */

//class C_ColorShowLabel : public QFrame
//{
//    Q_OBJECT

//public:
//    C_ColorShowLabel(QWidget *parent);
////        : QFrame(parent) {
////        setFrameStyle(QFrame::Panel|QFrame::Sunken);
////        setAcceptDrops(true);
////        mousePressed = false;
////    }
//    void setColor(QColor c) { col = c; }

//Q_SIGNALS:
//    void colorDropped(QRgb);

//protected:
//    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
//    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
//    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
//    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
//#ifndef QT_NO_DRAGANDDROP
//    void dragEnterEvent(QDragEnterEvent *e) Q_DECL_OVERRIDE;
//    void dragLeaveEvent(QDragLeaveEvent *e) Q_DECL_OVERRIDE;
//    void dropEvent(QDropEvent *e) Q_DECL_OVERRIDE;
//#endif

//private:
//    QColor col;
//    bool mousePressed;
//    QPoint pressPos;
//};

///* -- Extern Global Variables --------------------------------------------------------------------------------------- */

//} //end of namespace

//#endif
