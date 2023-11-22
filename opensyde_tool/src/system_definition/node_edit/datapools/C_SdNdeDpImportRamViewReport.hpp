//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Popup dialog widget to show the report of an RAMView project import.
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPIMPORTRAMVIEWREPORT_HPP
#define C_SDNDEDPIMPORTRAMVIEWREPORT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"
#include "C_SclStringList.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_PuiSdNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpImportRamViewReport;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpImportRamViewReport :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpImportRamViewReport(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                         const QString & orc_ProjectPath,
                                         const stw::opensyde_core::C_OscNodeDataPool & orc_DataPool,
                                         const scl::C_SclStringList & orc_ImportInformation);
   ~C_SdNdeDpImportRamViewReport(void) override;

   void InitStaticNames(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeDpImportRamViewReport * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_FillReport(const QString & orc_ProjectPath, const stw::opensyde_core::C_OscNodeDataPool & orc_DataPool,
                     const scl::C_SclStringList & orc_ImportInformation);

   //Avoid call
   C_SdNdeDpImportRamViewReport(const C_SdNdeDpImportRamViewReport &);
   C_SdNdeDpImportRamViewReport & operator =(const C_SdNdeDpImportRamViewReport &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
