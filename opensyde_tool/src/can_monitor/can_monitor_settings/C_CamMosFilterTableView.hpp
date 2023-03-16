//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View (display) component for filter items table view (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSFILTERTABLEVIEW_H
#define C_CAMMOSFILTERTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblViewToolTipBase.hpp"
#include "C_CamTblDelegate.hpp"
#include "C_CamMosFilterTableModel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosFilterTableView :
   public C_TblViewToolTipBase
{
   Q_OBJECT

public:
   C_CamMosFilterTableView(QWidget * const opc_Parent = NULL);
   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDeleteKeyPressed(void);

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   C_CamTblDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
