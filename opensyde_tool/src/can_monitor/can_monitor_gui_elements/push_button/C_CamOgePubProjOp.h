//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Push button styled for can monitor title bar (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CamOgePubProjOp_H
#define C_CamOgePubProjOp_H

/* -- Includes ------------------------------------------------------------- */
#include "C_OgePubToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgePubProjOp :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubProjOp(QWidget * const opc_Parent = NULL);

   void SetSvg(const QString & orc_Path);
   void SetIconOnly(const bool oq_IconOnly);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_PaintEvent);
   //lint -restore

private:
   QString mc_SvgIcon;
   bool mq_IconOnly;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
