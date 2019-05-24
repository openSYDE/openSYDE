//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of recording an NVM image to a file. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAIMAGERECORDWIDGET_H
#define C_SYVDAITPAIMAGERECORDWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QTimer>

#include "stwtypes.h"

#include "C_OgePopUpDialog.h"
#include "C_SyvComDriverDiag.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDaItPaImageRecordWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDaItPaImageRecordWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDaItPaImageRecordWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                         stw_opensyde_gui_logic::C_SyvComDriverDiag & orc_ComDriver,
                                         const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> & orc_ListItemIds,
                                         const QString & orc_ViewName);
   virtual ~C_SyvDaItPaImageRecordWidget();

   void InitText(void);
   void SaveUserSettings(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

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
   C_SyvDaItPaImageRecordWidget & operator =(const C_SyvDaItPaImageRecordWidget &);

   void m_OnBrowse(void);
   QString m_GetValidPath(const QString & orc_Path) const;
   void m_SplitNameAndPath(const QString & orc_FullPath, QString & orc_Name, QString & orc_Path) const;
   void m_FilePathChanged(void);
   void m_ReadClicked(void);
   void m_StartReadElementsOfNode(void);
   void m_PrepareVariablesForParametrization(void);
   void m_ReadElementsOfNode(const QString & orc_Comment);
   stw_types::sint32 m_CreateParameterSetFile(const QString & orc_Comment);
   stw_types::sint32 m_ReadBackElementsOfNodeFromFile(void);
   QString m_GetTextForStep(
      const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> > & orc_DataPoolListsForEachNode,
      const bool oq_IsConfirm) const;
   void m_PrepareConfirmStep(
      const std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> > & orc_DataPoolListsForEachNode);
   void m_WriteCrcOfNodeToFile(void);

   void m_OkClicked(void);
   void m_OnCancel(void);

   void m_ConfirmCheckBoxChanged(void) const;
   void m_ConfirmClicked(void);

   void m_LoadUserSettings(void);

   void m_Timer(void);
   void m_ReportError(const QString & orc_FunctionName, const QString & orc_ErrorText,
                      const stw_types::sint32 os32_ErrorCode);
   void m_ReportErrorNvmSafeReadParameterValues(const stw_types::sint32 os32_ErrorCode);
   stw_opensyde_core::C_OSCParamSetInterpretedFileInfoData m_GetFileInfoData(const QString & orc_Comment) const;

   Ui::C_SyvDaItPaImageRecordWidget * mpc_Ui;

   stw_opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_logic::C_SyvComDriverDiag & mrc_ComDriver;
   const std::vector<stw_opensyde_core::C_OSCNodeDataPoolListElementId> mc_ListItemIds;
   std::vector<stw_types::uint32> mc_AllNodeIndexes;
   std::vector<std::vector<stw_opensyde_core::C_OSCNodeDataPoolListId> > mc_RelevantListsForEachNode;
   QTimer mc_Timer;
   E_Step me_Step;
   stw_types::uint32 mu32_CurrentNode;
   bool mq_RequestPending;
   QString mc_FilePath;
   QString mc_FinishedText;
   const QString mc_ViewName;

   static const QString mhc_FILE_EXTENSION;
   QString m_GetPathForNode(const stw_types::uint32 ou32_NodeIndex, const QString & orc_Path) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
