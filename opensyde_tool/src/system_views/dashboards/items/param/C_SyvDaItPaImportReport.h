//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for param widget configuration import report (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAIMPORTREPORT_H
#define C_SYVDAITPAIMPORTREPORT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include "C_OSCParamSetInterpretedNode.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItPaImportReport;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaImportReport :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItPaImportReport(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                    const std::vector<stw_opensyde_core::C_OSCParamSetInterpretedNode> & orc_Data,
                                    const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ElementIds,
                                    const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                                    const stw_types::uint32 ou32_ValidLayers, const QString & orc_Path);
   ~C_SyvDaItPaImportReport(void);

   void InitStaticNames(void) const;
   void GetOutput(std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_OutputListIds,
                  std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> & orc_OutputContent) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SyvDaItPaImportReport * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const std::vector<stw_opensyde_core::C_OSCParamSetInterpretedNode> & mrc_Data;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & mrc_ElementIds;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const QString & mrc_Path;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const stw_opensyde_core::C_OSCNodeDataPoolListElementId & mrc_Id;
   const stw_types::uint32 mu32_ValidLayers;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_OutputListIds;
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> mc_OutputContent;
   static const QString mhc_HTML_TABLE_HEADER_START;
   static const QString mhc_HTML_TABLE_DATA_START;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_SetReport(void);
   void m_HandleSourceFileInformation(QString & orc_Text) const;
   void m_HandleParsing(QString & orc_Text);
   void m_PrepareTableContent(QString & orc_TableApplyContent, QString & orc_TableMismatchContent,
                              QString & orc_TableRemainContent, stw_types::uint32 & oru32_TableApplyCount,
                              stw_types::uint32 & oru32_TableMismatchCount, stw_types::uint32 & oru32_TableRemainCount);
   void m_AppendTableEntry(QString & orc_TableContent, stw_types::uint32 & oru32_TableCount,
                           const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id, const QString & orc_Value,
                           const QString & orc_Reason) const;
   QString m_GetFilter(const stw_types::uint32 ou32_Value) const;

   //Avoid call
   C_SyvDaItPaImportReport(const C_SyvDaItPaImportReport &);
   C_SyvDaItPaImportReport & operator =(const C_SyvDaItPaImportReport &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
