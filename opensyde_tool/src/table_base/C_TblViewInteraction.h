//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Table view base class for link interaction wrapping (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     13.12.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_TBLVIEWINTERACTION_H
#define C_TBLVIEWINTERACTION_H

/* -- Includes ------------------------------------------------------------- */
#include "C_TblViewToolTipBase.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_TblViewInteraction :
   public C_TblViewToolTipBase
{
   Q_OBJECT

public:
   C_TblViewInteraction(QWidget * const opc_Parent = NULL);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigLinkClicked(const QModelIndex & orc_Index) const;

protected:
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   virtual bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;

private:
   void m_HandleGlobalMousePos(const QPoint & orc_GlobalPos);
   void m_CheckItemClicked(const QModelIndex & orc_Index) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
