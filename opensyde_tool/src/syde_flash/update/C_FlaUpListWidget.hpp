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

   void AddFileAction(const bool oq_IsActionForExistingFile, const int32_t os32_CurrentHexFileIndex);
   void AddNewFile(const QString & orc_DialogCaption, const QString & orc_DialogFilter, const QString oc_Folder,
                   const bool oq_IsActionForExistingFile, int32_t os32_CurrentHexFileIndex);
   void DeleteItem(const int32_t os32_CurrentHexFileIndex, const bool oq_NeedToUpdateListIndex);
   void SelectFile(const int32_t os32_CurrentHexFileIndex);
   void SetLastKnownHexFilePaths(void);
   QStringList GetHexFilePaths(void) const;
   QStringList GetHexFileDeviceNames(void) const;
   bool AreAllFilesValid(void) const;
   void EnableSettings(const bool oq_Enabled);
   void ReloadCurrentFile(const QString & orc_File, const int32_t os32_CurrentHexFileIndex);
   void dropEvent(QDropEvent * const opc_Event) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   void SigUpdateFileCounter(void);

protected:
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;

private:
   void m_AddNewFile(const QString & orc_File, const bool oq_HexFile, const bool oq_IsActionForExistingFile,
                     const int32_t os32_CurrentHexFileIndex);
   bool m_CheckFileAlreadyContained(const QString & orc_File) const;
   void m_AddFile(const QString & orc_File, const bool oq_IsActionForExistingFile,
                  const int32_t os32_CurrentHexFileIndex);
   static bool mh_IsFileHexFile(const QString & orc_File);
   void m_LoadLastKnownHexFilePaths(void);
   void m_UpdateHexFileWidgetIndex(const int32_t os32_CurrentHexFileIndex) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
