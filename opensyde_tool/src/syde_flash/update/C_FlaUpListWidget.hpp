//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       header file for showing list of all hex files in update widget section
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAUPLISTWIDGET_H
#define C_FLAUPLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QMouseEvent>
#include <QListWidget>
#include <QListWidgetItem>

#include "stwtypes.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_Uti.hpp"
#include "C_FlaUpHexFileInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_FlaUpListWidget :
   public QListWidget
{
   Q_OBJECT

public:
   explicit C_FlaUpListWidget(QWidget * const opc_Parent = NULL);
   ~C_FlaUpListWidget(void) override;

   void AddFileAction(const bool oq_IsActionForExistingFile, const int32_t os32_CurrentHexFileIndex,
                      const bool oq_IsFileToReplace);
   void DeleteItem(const int32_t os32_CurrentHexFileIndex, const bool oq_NeedToUpdateListIndex);
   void DeleteAllItem(void);
   void SelectFile(const int32_t os32_CurrentHexFileIndex);
   void SetLastKnownHexFilePaths(void);
   QStringList GetHexFilePaths(void) const;
   QStringList GetHexFileDeviceNames(void) const;
   bool AreAllFilesValid(void) const;
   void EnableSettings(const bool oq_Enabled);
   void ReloadCurrentFile(const QString & orc_File, C_FlaUpHexFileInfo * const opc_HexFileInfo);
   void DropEvent(QDropEvent * const opc_Event);
   void DragEnterEvent(QDragEnterEvent * const opc_Event);
   void DragMoveEvent(QDragMoveEvent * const opc_Event);
   uint32_t GetTotalHexFileSize(void) const;
   uint32_t GetHexFileSize(const uint32_t & oru32_HexFileIndex) const;
   int32_t GetAmountOfFlashableFiles() const;
   void SetStatusIcon(const int32_t & ors32_FileIndex, const uint8_t & oru8_State);

   void ResetStatusIcons(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdateFileCounter(void);
   void SigFileAdded(void);
   void SigFileRemoved(void);

private:
   void m_AddNewFile(const QString & orc_File, const bool oq_HexFile, const bool oq_IsActionForExistingFile,
                     const int32_t os32_CurrentHexFileIndex);
   void m_AddFile(const QString & orc_File, const bool oq_IsActionForExistingFile,
                  const int32_t os32_CurrentHexFileIndex, const bool oq_IsRelativePath);
   bool m_IsFileHexFile(const QString & orc_File);
   void m_LoadLastKnownHexFilePaths(void);
   void m_UpdateHexFileWidgetIndex(const int32_t os32_CurrentHexFileIndex) const;
   void m_AddFilesFromContextMenu(void);
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   bool m_AskUserToSaveRelativePath(const QString & orc_Path, const QString & orc_AbsoluteReferenceDir);
   void m_AddNewHexFile(const QString & orc_DialogCaption, const QString & orc_DialogFilter, const QString & orc_Folder,
                        const bool oq_IsActionForExistingFile, int32_t os32_CurrentHexFileIndex,
                        const bool oq_IsFileToReplace);
   QStringList m_GetHexFilePathsAsRelativeOrAbsolute(void) const;
   QString m_LastSuffixOfFileName(const QString & orc_File) const;

   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   bool mq_EnableSettings;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
