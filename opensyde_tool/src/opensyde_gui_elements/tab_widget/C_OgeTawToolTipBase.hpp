//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget with tool tip (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETAWTOOLTIPBASE_H
#define C_OGETAWTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <map>
#include <QTabWidget>
#include "stwtypes.hpp"
#include "C_NagToolTip.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTawToolTipBase :
   public QTabWidget
{
public:
   C_OgeTawToolTipBase(QWidget * const opc_Parent = NULL);
   ~C_OgeTawToolTipBase(void) override;

   void SetToolTipInformation(const uint32_t ou32_Index, const QString & orc_Heading, const QString & orc_Content);

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

   void m_HideToolTip(void);

private:
   class C_ToolTipContent
   {
   public:
      QString c_Heading;
      QString c_Content;
   };

   std::map<int32_t, C_ToolTipContent> mc_ToolTips;
   stw::opensyde_gui::C_NagToolTip * mpc_ToolTip;
   int32_t ms32_CurrentHoveredIndex;

   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
