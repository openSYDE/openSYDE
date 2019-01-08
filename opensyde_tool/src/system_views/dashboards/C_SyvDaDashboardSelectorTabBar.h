//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Specific tabbar implementation for drag functionality of C_SyvDaDashboardSelectorTabWidget

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     28.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDADASHBOARDSELECTORTABBAR_H
#define C_SYVDADASHBOARDSELECTORTABBAR_H

/* -- Includes ------------------------------------------------------------- */
#include <QTabBar>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaDashboardSelectorTabBar :
   public QTabBar
{
   Q_OBJECT

public:
   C_SyvDaDashboardSelectorTabBar(QWidget * const opc_Parent = NULL);
   ~C_SyvDaDashboardSelectorTabBar();

   void SetScreenshot(const QPixmap & orc_Screenshot);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigTabTearOff(const stw_types::sint32 os32_Index, const QPoint & orc_Pos);
   void SigNeedScreenshot(const stw_types::sint32 os32_Index);
   void SigReselect(const stw_types::sint32 os32_Index);
   void SigResizeHappened(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvDaDashboardSelectorTabBar(const C_SyvDaDashboardSelectorTabBar &);
   C_SyvDaDashboardSelectorTabBar & operator =(const C_SyvDaDashboardSelectorTabBar &);

   bool mq_MouseClicked;
   QPoint mc_LastMouseClickPosition;
   bool mq_TearOff;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
