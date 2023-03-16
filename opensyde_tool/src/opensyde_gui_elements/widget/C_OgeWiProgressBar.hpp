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

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
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
   void SetSpacingWidth(const int32_t os32_SpacingWidth);

   void SetProgress(const uint32_t ou32_Percentage, const bool oq_Full = false);
   uint32_t GetProgress(void) const;

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QBrush mc_Brush;
   QPen mc_Pen;
   QColor mc_ColorFree;
   QColor mc_ColorReserved;
   QColor mc_ColorTooMuch;
   int32_t ms32_SpacingWidth; // space between free bar and reserved bar

   uint32_t mu32_Percentage;
   bool mq_Full;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
