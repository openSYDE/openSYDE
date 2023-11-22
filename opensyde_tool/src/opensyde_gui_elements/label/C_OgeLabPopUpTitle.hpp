//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom label for pop up title (header)

   Custom label for pop up title (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABPOPUPTITLE_HPP
#define C_OGELABPOPUPTITLE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QMouseEvent>
#include <QLabel>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
