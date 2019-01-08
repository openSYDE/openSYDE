//-----------------------------------------------------------------------------
/*!
   \file
   \brief       SVG icon only push button (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEPUBSVGICONONLY_H
#define C_OGEPUBSVGICONONLY_H

/* -- Includes ------------------------------------------------------------- */
#include <QSvgRenderer>
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgePubSvgIconOnly :
   public C_OgePubToolTipBase
{
public:
   C_OgePubSvgIconOnly(QWidget * const opc_Parent = NULL);
   ~C_OgePubSvgIconOnly(void);

   void SetSvg(const QString & orc_PathEnabled, const QString & orc_PathDisabled = "",
               const QString & orc_PathHovered = "", const QString & orc_PathCheckedEnabled = "",
               const QString & orc_PathCheckedDisabled = "", const QString & orc_PathCheckedHovered = "");

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

   bool mq_Hovered;
   QSvgRenderer * mpc_SvgRendererEnabled;
   QSvgRenderer * mpc_SvgRendererDisabeld;
   QSvgRenderer * mpc_SvgRendererHovered;
   QSvgRenderer * mpc_SvgRendererCheckedEnabled;
   QSvgRenderer * mpc_SvgRendererCheckedDisabeld;
   QSvgRenderer * mpc_SvgRendererCheckedHovered;

private:
   //Avoid call
   C_OgePubSvgIconOnly(const C_OgePubSvgIconOnly &);
   C_OgePubSvgIconOnly & operator =(const C_OgePubSvgIconOnly &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
