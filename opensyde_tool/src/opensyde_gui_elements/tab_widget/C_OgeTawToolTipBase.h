//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tab widget with tool tip (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETAWTOOLTIPBASE_H
#define C_OGETAWTOOLTIPBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <map>
#include <QTabWidget>
#include "stwtypes.h"
#include "C_NagToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTawToolTipBase :
   public QTabWidget
{
public:
   C_OgeTawToolTipBase(QWidget * const opc_Parent = NULL);
   ~C_OgeTawToolTipBase(void);

   void SetToolTipInformation(const stw_types::uint32 ou32_Index, const QString & orc_Heading,
                              const QString & orc_Content);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

private:
   class C_ToolTipContent
   {
   public:
      QString c_Heading;
      QString c_Content;
   };

   std::map<stw_types::sint32, C_ToolTipContent> mc_ToolTips;
   stw_opensyde_gui::C_NagToolTip * mpc_ToolTip;
   stw_types::sint32 ms32_CurrentHoveredIndex;

   void m_HideTooltip(void);
   void m_HandleMouseMoveToolTip(const QPoint & orc_GlobalPos);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
