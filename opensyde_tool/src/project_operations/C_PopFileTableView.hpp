//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for project file table (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_POPFILETABLEVIEW_HPP
#define C_POPFILETABLEVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_PopFileTableModel.hpp"
#include "C_PopFileTableDelegate.hpp"
#include "C_TblViewToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_PopFileTableView :
   public stw::opensyde_gui::C_TblViewToolTipBase
{
   Q_OBJECT

public:
   C_PopFileTableView(QWidget * const opc_Parent = NULL);
   void UpdateData(const std::vector<QString> & orc_RecentFilePaths,
                   const std::vector<stw::opensyde_core::C_OscProject> & orc_RecentProjects);
   int32_t ConvertRowToFile(const int32_t & ors32_Row, QString & orc_FilePath) const;

protected:
   void mouseMoveEvent(QMouseEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;
   void leaveEvent(QEvent * const opc_Event) override;
   void mouseReleaseEvent(QMouseEvent * const opc_Event) override;

private:
   stw::opensyde_gui_logic::C_PopFileTableModel mc_Model;
   stw::opensyde_gui_logic::C_PopFileTableDelegate mc_Delegate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
