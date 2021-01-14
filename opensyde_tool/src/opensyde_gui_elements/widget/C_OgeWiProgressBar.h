//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with minimal progress bar. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEWIPROGRESSBAR_H
#define C_OGEWIPROGRESSBAR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QPen>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeWiProgressBar :
   public QWidget
{
   Q_OBJECT

public:
   C_OgeWiProgressBar(QWidget * const opc_Parent = NULL);

   void SetColorFree(const QColor & orc_Color);
   void SetColorReserved(const QColor & orc_Color);
   void SetColorTooMuch(const QColor & orc_Color);
   void SetSpacingWidth(const stw_types::sint32 os32_SpacingWidth);

   void SetProgress(const stw_types::uint32 ou32_Percentage, const bool oq_Full = false);
   stw_types::uint32 GetProgress(void) const;

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QBrush mc_Brush;
   QPen mc_Pen;
   QColor mc_ColorFree;
   QColor mc_ColorReserved;
   QColor mc_ColorTooMuch;
   stw_types::sint32 ms32_SpacingWidth; // space between free bar and reserved bar

   stw_types::uint32 mu32_Percentage;
   bool mq_Full;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
