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

#include "stwtypes.h"
#include "C_OgeWiWithToolTip.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpSelectorItemUsageWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDpSelectorItemUsageWidget(QWidget * const opc_Parent = NULL);

   void SetData(const stw_types::uint32 ou32_Size, const stw_types::uint32 ou32_Used,
                const stw_types::uint32 ou32_Reserved, stw_types::uint32 & oru32_PercentageUsed,
                stw_types::uint32 & oru32_PercentageReserved);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QBrush mc_Brush;
   QPen mc_Pen;
   QColor mc_ColorFree;
   QColor mc_ColorReserved;
   bool mq_UsedToFull;
   bool mq_ReservedToFull;
   bool mq_UsedOverReserved;
   stw_types::uint32 mu32_PercentageUsed;
   stw_types::uint32 mu32_PercentageReserved;
   stw_types::uint32 mu32_Size;
   stw_types::uint32 mu32_Used;
   stw_types::uint32 mu32_Reserved;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
