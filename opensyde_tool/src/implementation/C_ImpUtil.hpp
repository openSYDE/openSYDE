//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for use case implementation functionality (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_IMPUTIL_H
#define C_IMPUTIL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <windows.h>
#include <QWidget>
#include <QString>
#include "stwtypes.hpp"
#include "C_OscNodeApplication.hpp"
#include "C_ImpCodeGenerationReportWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_ImpUtil
{
public:
   C_ImpUtil(void);

   static void h_ExportCodeAll(QWidget * const opc_Parent);
   static void h_ExportCode(const std::vector<uint32_t> & orc_NodeIndices,
                            const std::vector< std::vector<uint32_t> > & orc_AppIndicesPerNode,
                            QWidget * const opc_Parent);
   static void h_ExportCodeNodes(const std::vector<uint32_t> & orc_NodeIndices, QWidget * const opc_Parent);
   static int32_t h_OpenIde(const QString & orc_IdeExeCall);
   static QString h_GetSydeCoderCePath(void);
   static QString h_GetAbsoluteGeneratedDir(const stw::opensyde_core::C_OscNodeApplication & orc_Application,
                                            const stw::scl::C_SclString & orc_NodeName);
   static QString h_GetDefaultGeneratedDir(const stw::scl::C_SclString & orc_ApplicationName,
                                           const stw::scl::C_SclString & orc_NodeName);
   static bool h_CheckProjForCodeGeneration(QWidget * const opc_Parent);
   static QString h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
                                              const QString & orc_AbsoluteReferenceDir);
   static QStringList h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QStringList & orc_Paths,
                                                  const QString & orc_AbsoluteReferenceDir);
   static QString h_FormatSourceFileInfoForReport(const QString & orc_FilePath, const QString & orc_ReadContent);

private:
   class C_HandleData
   {
   public:
      uint32_t u32_ProcessId;
      HWND pc_WindowHandle;
   };

   static void mh_GetExistingApplicationHandle(const std::wstring & orc_ExeName, std::vector<HWND> & orc_Windows);
   static int32_t mh_ExecuteCodeGenerator(const QString & orc_NodeName, const QString & orc_AppName,
                                          const QString & orc_ExportFolder, QStringList & orc_ExportedFiles,
                                          const QString & orc_CodeGenerator, const bool & orq_Erase);
   static int32_t mh_ExportCodeNode(const uint32_t ou32_NodeIndex, const std::vector<uint32_t> & orc_AppIndices,
                                    std::vector<stw::opensyde_gui::C_ImpCodeGenerationReportWidget::C_ReportData> & orc_ExportInfo, const bool & orq_Erase);
   //lint -e{8080} //using type expected by the library for API compatibility
   static WINBOOL CALLBACK mh_EnumWindowsCallback(HWND opc_Handle, const LPARAM ox_LoParam);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
