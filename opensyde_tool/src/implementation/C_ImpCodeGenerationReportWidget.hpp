//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for file generation report (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_IMPCODEGENERATIONREPORTWIDGET_HPP
#define C_IMPCODEGENERATIONREPORTWIDGET_HPP

#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"

namespace Ui
{
class C_ImpCodeGenerationReportWidget;
}
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_ImpCodeGenerationReportWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_ImpCodeGenerationReportWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_ImpCodeGenerationReportWidget(void) override;

   class C_ReportData ///< Class to put together all report-relevant information of one Data Block
   {
   public:
      QString c_NodeName;           ///< Node name where the Data Block belongs to
      QString c_AppName;            ///< Data Block name
      QString c_CodeGeneratorPath;  ///< Path of used file generator
      uint16_t u16_CodeVersion;     ///< File structure version
      QString c_Directory;          ///< Directory where generated files were saved at
      QStringList c_GeneratedFiles; ///< List absolute file paths of all generated files
   };

   void InitStaticNames(void) const;
   void CreateReport(const std::vector<C_ReportData> & orc_ExportInfo) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_ImpCodeGenerationReportWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   static const QString mhc_HTML_TABLE_DATA_START;

   void m_OkClicked(void);

   //Avoid call
   C_ImpCodeGenerationReportWidget(const C_ImpCodeGenerationReportWidget &);
   C_ImpCodeGenerationReportWidget & operator =(const C_ImpCodeGenerationReportWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
