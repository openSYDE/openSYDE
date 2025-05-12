//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for new data block properties (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBPROPERTIES_HPP
#define C_SDNDEDBPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OgeLeFilePath.hpp"
#include "C_OscNodeApplication.hpp"
#include "C_SdNdeDbDataPoolEntry.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDbProperties;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbProperties :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbProperties(const uint32_t ou32_NodeIndex, const int32_t os32_ApplicationIndex,
                                stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SdNdeDbProperties(void) override;

   static QSize h_GetBinaryWindowSize(void);
   static QSize h_GetDefaultWindowSize(void);
   void InitStaticNames(void) const;
   void ApplyNewData(stw::opensyde_core::C_OscNodeApplication & orc_Application) const;
   void HandleDataPools(const uint32_t ou32_ApplicationIndex) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeDbProperties * mpc_Ui;
   const uint32_t mu32_NodeIndex;
   const int32_t ms32_ApplicationIndex;
   stw::opensyde_core::C_OscNodeApplication::E_Type me_Type;
   std::vector<C_SdNdeDbDataPoolEntry *> mc_DataPoolWidgets;
   std::set<uint32_t> mc_SelectedDataPools;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   static const int32_t mhs32_VERSION_INDEX_V1;
   static const int32_t mhs32_VERSION_INDEX_V2;
   static const int32_t mhs32_VERSION_INDEX_V3;
   static const int32_t mhs32_VERSION_INDEX_V4;
   static const int32_t mhs32_VERSION_INDEX_V5;
   static const int32_t mhs32_VERSION_INDEX_V6;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   QString m_CheckName(void) const;
   QString m_CheckId(void) const;
   QString m_CheckPaths(void) const;
   QString m_CheckPath(const QString & orc_Path) const;
   QString m_GetDialogPath(const QString & orc_CurrentPath) const;

   void m_LoadData(void);
   void m_LoadDataBlock(void);
   void m_LoadOutputFilePaths(const stw::opensyde_core::C_OscNodeApplication & orc_Application,
                              const QString & orc_ProjectPath);
   void m_SetVisibilityOfContentWidgets(const stw::opensyde_core::C_OscNodeApplication::E_Type & ore_Type) const;

   void m_OnFileGenerationChanged(const int32_t os32_State);
   void m_OnClickProject(void);
   void m_OnDroppedProject(void);
   void m_OnClickOutput(void);
   void m_OnDroppedOutput(void);
   void m_OnClickGenerator(void);
   void m_OnDroppedGenerator(void);
   void m_OnClickCodeGeneration(void);
   void m_OnUpdateOutputFilePath(void);
   void m_OnDroppedCodeGeneration(void);
   void m_OnClickFileGeneration(void);
   void m_OnDroppedFileGeneration(void);
   void m_OnClickIde(void);

   void m_OnNameEdited(void) const;
   void m_OnProcessIdChanged(void) const;

   void m_CleanUpDataPoolWidgets(void);
   void m_InitDataPoolsSection(void);
   void m_HandleAddDataPools(void);
   void m_HandleRevertCodeGenerator(void) const;
   void m_HandleDeleteDataPool(C_SdNdeDbDataPoolEntry * const opc_Source, const uint32_t ou32_Index);
   void m_UpdateOwnedDpsCount(void) const;

   void m_UpdatePathsRelativeToProject(void) const;
   void m_UpdatePathsRelativeToGeneratedDir(void) const;

   void m_AskUserToSaveRelativePath(const QString & orc_Path, const QString & orc_AbsoluteReferenceDir,
                                    stw::opensyde_gui_elements::C_OgeLeFilePath * const opc_PathLineEdit);

   //Avoid call
   C_SdNdeDbProperties(const C_SdNdeDbProperties &);
   C_SdNdeDbProperties & operator =(const C_SdNdeDbProperties &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
