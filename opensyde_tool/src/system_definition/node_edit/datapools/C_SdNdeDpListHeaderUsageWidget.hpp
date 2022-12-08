//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar on datapool list header (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTHEADERUSAGEWIDGET_H
#define C_SDNDEDPLISTHEADERUSAGEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QBrush>
#include <QPen>
#include <QColor>

#include "stwtypes.hpp"

#include "C_OgeWiProgressBar.hpp"

#include "C_NagToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListHeaderUsageWidget :
   public stw::opensyde_gui_elements::C_OgeWiProgressBar
{
   Q_OBJECT

public:
   C_SdNdeDpListHeaderUsageWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDpListHeaderUsageWidget(void);

   uint32_t SetUsage(const uint32_t ou32_Size, const uint32_t ou32_Used);

   void SetToolTipWidgetName(const QString & orc_Value);
   void SetErrorToolTip(const QString & orc_Heading, const QString & orc_Content);

protected:
   bool event(QEvent * const opc_Event) override;

private:
   bool mq_ActivateToolTip;
   C_NagToolTip * mpc_ToolTip;
   uint32_t mu32_Size;
   uint32_t mu32_Used;
   bool mq_ErrorToolTip;
   QString mc_ToolTipErrorHeading;
   QString mc_ToolTipErrorContent;
   QString mc_ToolTipHeading;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
