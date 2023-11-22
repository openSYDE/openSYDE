//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom transparent group box for dashboard (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEGBXTRANSPARENTDASHBOARD_HPP
#define C_OGEGBXTRANSPARENTDASHBOARD_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGroupBox>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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
   void resizeEvent(QResizeEvent * const opc_Event) override;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
