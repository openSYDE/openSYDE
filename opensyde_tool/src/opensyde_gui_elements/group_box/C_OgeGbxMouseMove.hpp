//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom label to make mouse move possible (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXMOUSEMOVE_H
#define C_OGEGBXMOUSEMOVE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMouseEvent>
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxMouseMove :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxMouseMove(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMousePress(QMouseEvent * const opc_Event);
   void SigMouseRelease(QMouseEvent * const opc_Event);
   void SigMouseMove(QMouseEvent * const opc_Event);

protected:
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
