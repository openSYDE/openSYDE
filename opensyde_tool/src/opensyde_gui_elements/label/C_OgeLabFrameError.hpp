//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label for error frame label (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABFRAMEERROR_H
#define C_OGELABFRAMEERROR_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QFontMetrics>
#include "stwtypes.hpp"
#include "C_OgeLabToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabFrameError :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   C_OgeLabFrameError(QWidget * const opc_Parent = NULL);

   void SetForegroundColor(const int32_t os32_Value);
   void SetCompleteText(const QString & orc_Text, const QString & orc_Tooltip,
                        const stw::opensyde_gui::C_NagToolTip::E_Type oe_TooltipType);
   QSize sizeHint(void) const override;
   QSize minimumSizeHint(void) const override;

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void showEvent(QShowEvent * const opc_Event) override;

private:
   QString mc_Text;

   void m_OnSizeChange(const QSize * const opc_SizeToUse = NULL);
   static QFontMetrics mh_GetFontMetrics(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
