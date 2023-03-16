//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar of an selector item (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPSELECTORITEMUSAGEWIDGET_H
#define C_SDNDEDPSELECTORITEMUSAGEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QColor>

#include "stwtypes.hpp"
#include "C_OgeWiWithToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpSelectorItemUsageWidget :
   public stw::opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDpSelectorItemUsageWidget(QWidget * const opc_Parent = NULL);

   void SetData(const uint32_t ou32_Size, const uint32_t ou32_Used, const uint32_t ou32_Reserved,
                uint32_t & oru32_PercentageUsed, uint32_t & oru32_PercentageReserved);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QBrush mc_Brush;
   QPen mc_Pen;
   QColor mc_ColorFree;
   QColor mc_ColorReserved;
   bool mq_UsedToFull;
   bool mq_ReservedToFull;
   bool mq_UsedOverReserved;
   uint32_t mu32_PercentageUsed;
   uint32_t mu32_PercentageReserved;
   uint32_t mu32_Size;
   uint32_t mu32_Used;
   uint32_t mu32_Reserved;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
