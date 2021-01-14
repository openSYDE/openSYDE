//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom label to make mouse move possible (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OgeGbxMouseMove_H
#define C_OgeGbxMouseMove_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMouseEvent>
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
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
   virtual void mousePressEvent(QMouseEvent * const opc_Event);
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event);
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
