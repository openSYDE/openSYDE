//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar of an selector item (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.02.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLSELECTORITEMUSAGEWIDGET_H
#define C_SDNDEDATAPOOLSELECTORITEMUSAGEWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QColor>

#include "stwtypes.h"
#include "C_OgeWiWithToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolSelectorItemUsageWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolSelectorItemUsageWidget(QWidget * const opc_Parent = NULL);

   void SetData(const stw_types::uint32 ou32_Size, const stw_types::uint32 ou32_Used,
                const stw_types::uint32 ou32_Reserved, stw_types::uint32 & oru32_PercentageUsed,
                stw_types::uint32 & oru32_PercentageReserved);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

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

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
