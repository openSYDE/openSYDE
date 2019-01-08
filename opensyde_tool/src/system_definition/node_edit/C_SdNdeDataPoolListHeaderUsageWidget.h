//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar on datapool list header (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     29.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTHEADERUSAGEWIDGET_H
#define C_SDNDEDATAPOOLLISTHEADERUSAGEWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QBrush>
#include <QPen>
#include <QColor>

#include "stwtypes.h"

#include "C_OgeWiProgressBar.h"

#include "C_NagToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolListHeaderUsageWidget :
   public stw_opensyde_gui_elements::C_OgeWiProgressBar
{
   Q_OBJECT

public:
   C_SdNdeDataPoolListHeaderUsageWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDataPoolListHeaderUsageWidget(void);

   stw_types::uint32 SetUsage(const stw_types::uint32 ou32_Size, const stw_types::uint32 ou32_Used);

   void SetToolTipActive(const bool oq_Active);
   void SetToolTipWidgetName(const QString & orc_Value);
   void SetErrorToolTip(const QString & orc_Heading, const QString & orc_Content);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   bool mq_ActivateToolTip;
   C_NagToolTip * mpc_ToolTip;
   stw_types::uint32 mu32_Size;
   stw_types::uint32 mu32_Used;
   bool mq_ErrorToolTip;
   QString mc_ToolTipErrorHeading;
   QString mc_ToolTipErrorContent;
   QString mc_ToolTipHeading;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
