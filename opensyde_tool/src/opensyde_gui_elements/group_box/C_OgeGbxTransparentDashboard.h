//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom transparent group box for dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXTRANSPARENTDASHBOARD_H
#define C_OGEGBXTRANSPARENTDASHBOARD_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeGbxTransparentDashboard :
   public QGroupBox
{
   Q_OBJECT

public:
   C_OgeGbxTransparentDashboard(QWidget * const opc_Parent = NULL);
   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigResized(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
