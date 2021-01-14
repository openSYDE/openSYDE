//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a progress status as point with configurable colors. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIPROGRESSBOPPERLE_H
#define C_OGEWIPROGRESSBOPPERLE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QPen>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiProgressBopperle :
   public QWidget
{
   Q_OBJECT

public:
   C_OgeWiProgressBopperle(QWidget * const opc_Parent = NULL);

   void SetMainBopperleColor(const QColor & orc_BorderColor, const QColor & orc_InnerColor);
   void HideMainBopperle(void);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QBrush mc_BrushOuterCircle;
   QPen mc_PenOuterCircle;
   QBrush mc_BrushMainCircle;
   QPen mc_PenMainCircle;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
