//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of recording an NVM image to a file. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAIMAGERECORDWIDGET_HPP
#define C_SYVDAITPAIMAGERECORDWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QTimer>

#include "stwtypes.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_SyvComDriverDiag.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaItPaImageRecordWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItPaImageRecordWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItPaImageRecordWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                         stw::opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver,
                                         const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> & orc_ListItemIds,
                                         const QString & orc_ViewName);
   ~C_SyvDaItPaImageRecordWidget() override;

   void InitText(void);
   void SaveUserSettings(void) const;
   static stw::opensyde_core::C_OscParamSetInterpretedFileInfoData h_GetFileInfoData(const QString & orc_Comment);

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   enum E_Step
   {
      eSELECTFILE,
      eCREATEPARAMETERSETFILE,
      eREADBACKFROMFILE,
      eCONFIRM,
      eWRITECRC,
      eFINISHED
   };

   //Avoid call
   C_SyvDaItPaImageRecordWidget(const C_SyvDaItPaImageRecordWidget &);
   C_SyvDaItPaImageRecordWidget & operator =(const C_SyvDaItPaImageRecordWidget &) &;

   void m_OnBrowse(void);
   void m_SplitNameAndPath(const QString & orc_FullPath, QString & orc_Name, QString & orc_Path) const;
   void m_FilePathChanged(void);
   void m_ReadClicked(void);
   void m_StartReadElementsOfNode(void);
   void m_PrepareVariablesForParametrization(void);
   void m_ReadElementsOfNode(const QString & orc_Comment);
   int32_t m_CreateParameterSetFile(const QString & orc_Comment);
   int32_t m_ReadBackElementsOfNodeFromFile(void);
   QString m_GetTextForStep(
      const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> > & orc_DataPoolListsForEachNode,
      const bool oq_IsConfirm) const;
   void m_PrepareConfirmStep(
      const std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> > & orc_DataPoolListsForEachNode);
   void m_WriteCrcOfNodeToFile(void);

   void m_OkClicked(void);
   void m_OnCancel(void);

   void m_ConfirmCheckBoxChanged(void) const;
   void m_ConfirmClicked(void);

   void m_LoadUserSettings(void);

   void m_Timer(void);
   void m_ReportError(const QString & orc_FunctionName, const QString & orc_ErrorText, const int32_t os32_ErrorCode);
   void m_ReportErrorNvmSafeReadParameterValues(const int32_t os32_ErrorCode);

   Ui::C_SyvDaItPaImageRecordWidget * mpc_Ui;

   stw::opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_logic::C_SyvComDriverDiag & mrc_ComDriver;
   const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElementId> mc_ListItemIds;
   std::vector<uint32_t> mc_AllNodeIndexes;
   std::vector<std::vector<stw::opensyde_core::C_OscNodeDataPoolListId> > mc_RelevantListsForEachNode;
   QTimer mc_Timer;
   E_Step me_Step;
   uint32_t mu32_CurrentNode;
   bool mq_RequestPending;
   QString mc_FilePath;
   QString mc_FinishedText;
   const QString mc_ViewName;

   static const QString mhc_FILE_EXTENSION;
   QString m_GetPathForNode(const uint32_t ou32_NodeIndex, const QString & orc_Path) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
