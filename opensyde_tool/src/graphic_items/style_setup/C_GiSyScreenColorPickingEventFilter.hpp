//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Event filter to be installed on the color-select-widget while in screen-color-picking mode.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GISYSCREENCOLORPICKINGEVENTFILTER_H
#define C_GISYSCREENCOLORPICKINGEVENTFILTER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_GiSyColorSelectWidget;

class C_GiSyScreenColorPickingEventFilter :
   public QObject
{
   Q_OBJECT

public:
   explicit C_GiSyScreenColorPickingEventFilter(C_GiSyColorSelectWidget * const opc_ColorSelectWidget,
                                                QObject * const opc_Parent = NULL);
   ~C_GiSyScreenColorPickingEventFilter(void) override;
   bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;

private:
   C_GiSyColorSelectWidget * mpc_ColorSelectWidget;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
