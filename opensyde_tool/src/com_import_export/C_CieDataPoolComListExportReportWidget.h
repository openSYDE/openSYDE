//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for export report to DBC file (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     18.06.2018  STW/D.Pohl (copied and adapted from data pool list import)
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CIEDATAPOOLCOMLISTEXPORTREPORTWIDGET_H
#define C_CIEDATAPOOLCOMLISTEXPORTREPORTWIDGET_H

#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_CieExportDbc.h"

namespace Ui
{
class C_CieDataPoolComListExportReportWidget;
}
/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_CieDataPoolComListExportReportWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDataPoolComListExportReportWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                   QString & orc_FilePath);
   ~C_CieDataPoolComListExportReportWidget(void);

   void InitStaticNames(void) const;
   stw_types::sint32 SetMessageData(const std::map<stw_scl::C_SCLString, stw_scl::C_SCLString> & orc_NodeMapping,
                                    const stw_opensyde_gui_logic::C_CieExportDbc::C_ExportStatistic & orc_ExportStatistic, const stw_scl::C_SCLStringList & orc_Warnings);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_CieDataPoolComListExportReportWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   std::map<stw_scl::C_SCLString, stw_scl::C_SCLString> mc_NodeMapping;
   stw_opensyde_gui_logic::C_CieExportDbc::C_ExportStatistic mc_ExportStatistic;
   stw_scl::C_SCLStringList mc_Warnings;
   const QString mc_FilePath;
   static const QString mhc_HTML_TABLE_HEADER_START;
   static const QString mhc_HTML_TABLE_DATA_START;

   void m_OkClicked(void);
   stw_types::sint32 m_BuildReport(void);

   //Avoid call
   C_CieDataPoolComListExportReportWidget(const C_CieDataPoolComListExportReportWidget &);
   C_CieDataPoolComListExportReportWidget & operator =(const C_CieDataPoolComListExportReportWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
