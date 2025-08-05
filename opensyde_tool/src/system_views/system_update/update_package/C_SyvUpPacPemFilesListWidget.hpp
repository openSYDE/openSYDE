//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPEMFILESLISTWIDGET_H
#define C_SYVUPPACPEMFILESLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacPemFilesListWidget :
   public QListWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacPemFilesListWidget(QWidget * const opc_Parent = NULL);
   void AddFileAction(void);
   void SaveUserSettings(void);
   void DeleteAllItems(void);
   QStringList GetPemFilePaths(void) const;
   void DropEvent(QDropEvent * const opc_Event);
   void DragEnterEvent(QDragEnterEvent * const opc_Event);
   void DragMoveEvent(QDragMoveEvent * const opc_Event);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   void SigUpdateFileCounter(void);

private:
   void m_AddNewPemFile(const QString & orc_DialogCaption, const QString & orc_DialogFilter,
                        const QString & orc_Folder);
   QString m_LastSuffixOfFileName(const QString & orc_File) const;
   void m_AddFile(const QString & orc_File, const int32_t os32_CurrentPemFileIndex);
   void m_LoadLastKnownPemFilePaths(void);
   void m_DeleteItem(const int32_t os32_CurrentPemFileIndex, const bool oq_NeedToUpdateListIndex);
   void m_UpdatePemFileWidgetIndex(const int32_t os32_CurrentPemFileIndex) const;
   void m_ShowHideVerticalScrollBar(const int32_t os32_Min, const int32_t os32_Max) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
