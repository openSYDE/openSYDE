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
//#include "C_ColorShowLabel.h"
//#include "C_GiSyColorSelectWidget.h"

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
//C_ColorShowLabel::C_ColorShowLabel(C_GiSyColorSelectWidget *parent) : QFrame(parent) {
//    setFrameStyle(QFrame::Panel|QFrame::Sunken);
//    setAcceptDrops(true);
//    mousePressed = false;
//}

////----------------------------------------------------------------------------------------------------------------------
///*! \brief   Default destructor
//*/
////----------------------------------------------------------------------------------------------------------------------
//C_ColorShowLabel::~C_ColorShowLabel()
//{
//   delete this;
//}

//void C_ColorShowLabel::paintEvent(QPaintEvent *e)
//{
//    QPainter p(this);
//    drawFrame(&p);
//    p.fillRect(contentsRect()&e->rect(), col);
//}

//void C_ColorShowLabel::mousePressEvent(QMouseEvent *e)
//{
//    mousePressed = true;
//    pressPos = e->pos();
//}

//void C_ColorShowLabel::mouseMoveEvent(QMouseEvent *e)
//{
//#ifdef QT_NO_DRAGANDDROP
//    Q_UNUSED(e);
//#else
//    if (!mousePressed)
//        return;
//    if ((pressPos - e->pos()).manhattanLength() > QApplication::startDragDistance()) {
//        QMimeData *mime = new QMimeData;
//        mime->setColorData(col);
//        QPixmap pix(30, 20);
//        pix.fill(col);
//        QPainter p(&pix);
//        p.drawRect(0, 0, pix.width() - 1, pix.height() - 1);
//        p.end();
//        QDrag *drg = new QDrag(this);
//        drg->setMimeData(mime);
//        drg->setPixmap(pix);
//        mousePressed = false;
//        drg->start();
//    }
//#endif
//}

//#ifndef QT_NO_DRAGANDDROP
//void C_ColorShowLabel::dragEnterEvent(QDragEnterEvent *e)
//{
//    if (qvariant_cast<QColor>(e->mimeData()->colorData()).isValid())
//        e->accept();
//    else
//        e->ignore();
//}

//void C_ColorShowLabel::dragLeaveEvent(QDragLeaveEvent *)
//{
//}

//void C_ColorShowLabel::dropEvent(QDropEvent *e)
//{
//    QColor color = qvariant_cast<QColor>(e->mimeData()->colorData());
//    if (color.isValid()) {
//        col = color;
//        repaint();
//        emit colorDropped(col.rgb());
//        e->accept();
//    } else {
//        e->ignore();
//    }
//}
//#endif // QT_NO_DRAGANDDROP

//void C_ColorShowLabel::mouseReleaseEvent(QMouseEvent *)
//{
//    if (!mousePressed)
//        return;
//    mousePressed = false;
//}
