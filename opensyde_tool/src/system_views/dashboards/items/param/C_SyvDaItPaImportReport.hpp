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
#include "C_OgePopUpDialog.hpp"
#include "C_OscParamSetInterpretedNode.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvDaItPaImportReport;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaImportReport :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItPaImportReport(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                    const std::vector<stw::opensyde_core::C_OscParamSetInterpretedNode> & orc_Data,
                                    const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ElementIds,
                                    const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                                    const uint32_t ou32_ValidLayers, const QString & orc_Path);
   ~C_SyvDaItPaImportReport(void) override;

   void InitStaticNames(void) const;
   void GetOutput(std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_OutputListIds,
                  std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> & orc_OutputContent) const;
   void GetFloatRangeCheckResults(std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_InvalidValueIds,
                                  std::vector<QString> & orc_InvalidValues, std::vector<QString> & orc_NewValues) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvDaItPaImportReport * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const std::vector<stw::opensyde_core::C_OscParamSetInterpretedNode> & mrc_Data;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & mrc_ElementIds;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const QString & mrc_Path;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const stw::opensyde_core::C_OscNodeDataPoolListElementId & mrc_Id;
   const uint32_t mu32_ValidLayers;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_OutputListIds;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> mc_OutputContent;
   std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_FloatRangeCheckInvalidValueIds;
   std::vector<QString> mc_FloatRangeCheckInvalidValues;
   std::vector<QString> mc_FloatRangeCheckNewValues;
   static const QString mhc_HTML_TABLE_HEADER_START;
   static const QString mhc_HTML_TABLE_DATA_START;

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_SetReport(void);
   void m_HandleSourceFileInformation(QString & orc_Text) const;
   void m_HandleParsing(QString & orc_Text);
   void m_PrepareTableContent(QString & orc_TableApplyContent, QString & orc_TableMismatchContent,
                              QString & orc_TableRemainContent, uint32_t & oru32_TableApplyCount,
                              uint32_t & oru32_TableMismatchCount, uint32_t & oru32_TableRemainCount);
   void m_AppendTableEntry(QString & orc_TableContent, uint32_t & oru32_TableCount,
                           const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id, const QString & orc_Value,
                           const QString & orc_Reason) const;
   QString m_GetFilter(const uint32_t ou32_Value) const;

   //Avoid call
   C_SyvDaItPaImportReport(const C_SyvDaItPaImportReport &);
   C_SyvDaItPaImportReport & operator =(const C_SyvDaItPaImportReport &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
