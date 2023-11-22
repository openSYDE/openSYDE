//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view base class for link interaction wrapping (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLVIEWINTERACTION_HPP
#define C_TBLVIEWINTERACTION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblViewToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

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
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;
   bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;

private:
   void m_HandleGlobalMousePos(const QPoint & orc_GlobalPos);
   void m_CheckItemClicked(const QModelIndex & orc_Index) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
