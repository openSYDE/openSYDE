//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for project file table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPFILETABLEVIEW_H
#define C_POPFILETABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PopFileTableModel.h"
#include "C_PopFileTableDelegate.h"
#include "C_TblViewToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopFileTableView :
   public stw_opensyde_gui::C_TblViewToolTipBase
{
   Q_OBJECT

public:
   C_PopFileTableView(QWidget * const opc_Parent = NULL);
   void UpdateData(const std::vector<QString> & orc_RecentFilePaths,
                   const std::vector<stw_opensyde_core::C_OSCProject> & orc_RecentProjects);
   stw_types::sint32 ConvertRowToFile(const stw_types::sint32 & ors32_Row, QString & orc_FilePath) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   virtual void mouseReleaseEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

private:
   stw_opensyde_gui_logic::C_PopFileTableModel mc_Model;
   stw_opensyde_gui_logic::C_PopFileTableDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
