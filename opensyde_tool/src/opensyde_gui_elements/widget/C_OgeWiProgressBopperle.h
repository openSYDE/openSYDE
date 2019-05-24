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

   void SetOuterBopperleColor(const QColor & orc_BorderColer, const QColor & orc_InnerColor);
   void SetMainBopperleColor(const QColor & orc_BorderColer, const QColor & orc_InnerColor);
   void HideMainBopperle(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   QBrush mc_BrushOuterCircle;
   QPen mc_PenOuterCircle;
   QBrush mc_BrushMainCircle;
   QPen mc_PenMainCircle;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
