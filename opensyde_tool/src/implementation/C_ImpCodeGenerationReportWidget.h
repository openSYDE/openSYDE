//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for code generation report (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.10.2018  STW/G.Landsgesell (copied and adapted from DBC export report)
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_IMPCODEGENERATIONREPORTWIDGET_H
#define C_IMPCODEGENERATIONREPORTWIDGET_H

#include <QWidget>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"

namespace Ui
{
class C_ImpCodeGenerationReportWidget;
}
/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_ImpCodeGenerationReportWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_ImpCodeGenerationReportWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_ImpCodeGenerationReportWidget(void);

   void InitStaticNames(void) const;
   void CreateReport(const std::list<QString> & orc_ExportedFiles) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_ImpCodeGenerationReportWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   const QString mc_FilePath;
   static const QString mhc_HTML_TABLE_HEADER_START;
   static const QString mhc_HTML_TABLE_DATA_START;

   void m_OkClicked(void);

   //Avoid call
   C_ImpCodeGenerationReportWidget(const C_ImpCodeGenerationReportWidget &);
   C_ImpCodeGenerationReportWidget & operator =(const C_ImpCodeGenerationReportWidget &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
