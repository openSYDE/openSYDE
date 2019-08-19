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
#include "stwtypes.h"
#include "C_OSCNodeApplication.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_ImpUtil
{
public:
   C_ImpUtil(void);

   static void h_ExportCodeAll(QWidget * const opc_Parent);
   static void h_ExportCode(const std::vector<stw_types::uint32> & orc_NodeIndices,
                            const std::vector< std::vector<stw_types::uint32> > & orc_AppIndicesPerNode,
                            QWidget * const opc_Parent);
   static void h_ExportCodeNodes(const std::vector<stw_types::uint32> & orc_NodeIndices, QWidget * const opc_Parent);
   static stw_types::sint32 h_OpenIDE(const QString & orc_IdeExeCall);
   static QString h_GetSydeCoderCPath(void);
   static QString h_GetAbsoluteGeneratedDir(const stw_opensyde_core::C_OSCNodeApplication * const opc_Application,
                                            const stw_scl::C_SCLString & orc_NodeName);
   static bool h_CheckProjForCodeGeneration(QWidget * const opc_Parent);
   static QString h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
                                              const QString & orc_AbsoluteReferenceDir);
   static QStringList h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QStringList & orc_Paths,
                                                  const QString & orc_AbsoluteReferenceDir);

private:
   struct T_HandleData
   {
      stw_types::uint32 u32_ProcessId;
      HWND pc_WindowHandle;
   };

   static void mh_GetExistingApplicationHandle(const std::wstring & orc_ExeName, std::vector<HWND> & orc_Windows);
   static stw_types::sint32 mh_ExecuteCodeGenerator(const QString & orc_NodeName, const QString & orc_AppName,
                                                    const QString & orc_ExportFolder,
                                                    std::vector<QString> & orc_ExportedFiles,
                                                    const QString & orc_CodeGenerator, const bool & orq_Erase);
   static stw_types::sint32 mh_ExportCodeNode(const stw_types::uint32 ou32_NodeIndex,
                                              const std::vector<stw_types::uint32> & orc_AppIndices,
                                              std::list<QString> & orc_ExportedFiles, const bool & orq_Erase);
   static WINBOOL CALLBACK mh_EnumWindowsCallback(HWND opc_Handle, const LPARAM os32_LParam);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
