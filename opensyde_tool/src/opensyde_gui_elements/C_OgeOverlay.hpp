//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Overlay for background to concentrate focus on widget in foreground (header)

   Overlay for background to concentrate focus on widget in foreground (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEOVERLAY_H
#define C_OGEOVERLAY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QDialog>
#include <QPaintEvent>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeOverlay :
   public QDialog
{
   Q_OBJECT

public:
   C_OgeOverlay(QWidget * const opc_Parent = NULL, QWidget * const opc_EffectTargetChild = NULL);
   static void h_SetTopWidget(QWidget * const opc_TopWidget);
   static const QWidget * h_GetTopWidget(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   static QWidget * mhpc_TopWidget; ///< Most top widget to gray out this most top widgets and all sub widgets when this
                                    // overlay is activated
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
