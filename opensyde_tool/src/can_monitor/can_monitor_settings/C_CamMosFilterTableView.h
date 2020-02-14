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
#include "C_TblViewToolTipBase.h"
#include "C_CamTblDelegate.h"
#include "C_CamMosFilterTableModel.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   C_CamTblDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
