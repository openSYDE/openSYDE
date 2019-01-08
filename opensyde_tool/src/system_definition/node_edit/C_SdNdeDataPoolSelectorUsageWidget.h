//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing an usage bar with multiple elements (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.02.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLSELECTORUSAGEWIDGET_H
#define C_SDNDEDATAPOOLSELECTORUSAGEWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <vector>

#include "stwtypes.h"

#include "C_NagToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeDataPoolSelectorUsageWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolSelectorUsageWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDataPoolSelectorUsageWidget(void);

   stw_types::uint32 SetUsage(const stw_types::uint32 ou32_Size, const std::vector<stw_types::uint32> & orc32_VecUsed,
                              std::vector<QString> & orc_VecNames);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigHideOtherToolTips(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

private:
   QBrush mc_Brush;
   QPen mc_Pen;
   QColor mc_ColorFree;
   C_NagToolTip * mpc_ToolTip;
   std::vector<QColor> mc_ColorsUsed;
   stw_types::uint32 mu32_TotalUsed;
   stw_types::uint32 mu32_TotalPercentage;
   stw_types::uint32 mu32_Size;
   std::vector<QString> mc_VecNames;
   std::vector<stw_types::uint32> mc_Used;
   std::vector<stw_types::uint32> mc_Percentages;
   std::vector<stw_types::float32> mc_PercentagesFloat;
   std::vector<stw_types::sintn> mc_Distances;

   void m_UpdateTooltip(const stw_types::sintn osn_MouseX);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
