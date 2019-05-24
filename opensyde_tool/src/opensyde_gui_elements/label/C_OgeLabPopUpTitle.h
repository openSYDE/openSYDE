//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom label for pop up title (header)

   Custom label for pop up title (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABPOPUPTITLE_H
#define C_OGELABPOPUPTITLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QMouseEvent>
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabPopUpTitle :
   public QLabel
{
   Q_OBJECT

public:
   C_OgeLabPopUpTitle(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigMousePress(QMouseEvent * const opc_Event);
   void SigMouseRelease(QMouseEvent * const opc_Event);
   void SigMouseMove(QMouseEvent * const opc_Event);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mousePressEvent(QMouseEvent * const opc_Event);
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event);
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event);
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
