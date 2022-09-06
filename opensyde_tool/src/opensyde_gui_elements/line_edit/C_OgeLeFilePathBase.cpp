//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for line edit field with file paths (implementation)

   This class shows a minimized path if the focus is out of the edit field and a full
   path if the user is editing the path. Furthermore the complete and absolute path is
   shown in tool tip (path variables are resolved for tooltips).

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QFileInfo>
#include <QDir>
#include <QMimeData>

#include "stwtypes.h"
#include "C_Uti.h"
#include "constants.h"
#include "C_OgeLeFilePathBase.h"
#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeLeFilePathBase::C_OgeLeFilePathBase(QWidget * const opc_Parent) :
   C_OgeLeToolTipBase(opc_Parent),
   mc_Path(""),
   mc_RelativeTo(""),
   ms32_LastKnownCursorPos(-1),
   mq_DragAndDropFolderActive(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setter for full path and tool tip resolving relative path.

   For appropriate QLineEdit the minimized path is shown.

   \param[in]  orc_New           New value
   \param[in]  orc_RelativeTo    Optional directory where path is relative to if it is relative (must be absolute)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::SetPath(const QString & orc_New, const QString & orc_RelativeTo)
{
   const QDir c_Dir(this->m_ResolveVariables(orc_RelativeTo));

   // text
   this->mc_Path = orc_New;
   this->mc_Path.replace("\\", "/"); // replace backslashes
   this->m_UpdateMinimizing();

   // tool tip
   const QFileInfo c_FileInfo(c_Dir, this->m_ResolveVariables(this->mc_Path));
   this->mc_RelativeTo = c_Dir.absolutePath();
   if (this->mc_Path != "")
   {
      C_OgeLeToolTipBase::SetToolTipInformation(C_GtGetText::h_GetText("Path"), c_FileInfo.absoluteFilePath(),
                                                stw_opensyde_gui::C_NagToolTip::eDEFAULT);
   }
   else
   {
      C_OgeLeToolTipBase::SetToolTipInformation("", "", stw_opensyde_gui::C_NagToolTip::eDEFAULT);
   }

   // make sure last known cursor position is not out of range
   this->ms32_LastKnownCursorPos = std::min(static_cast<stw_types::sintn>(this->ms32_LastKnownCursorPos),
                                            this->mc_Path.size());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates drag and drop functionality for folder path

   \param[in]       oq_Active          Flag if drag and drop functionality shall be available for folder path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::SetDragAndDropActiveForFolder(const bool oq_Active)
{
   this->setDragEnabled(oq_Active);
   this->mq_DragAndDropFolderActive = oq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates drag and drop functionality for a specific file extension

   \param[in]       orc_FileExtension   Valid file extension
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::SetDragAndDropActiveForFile(const QString & orc_FileExtension)
{
   QStringList c_List;

   c_List.push_back(orc_FileExtension);

   this->SetDragAndDropActiveForFile(true, &c_List);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activates drag and drop functionality for multiple specific file extension

   \param[in]       oq_Active          Flag if drag and drop functionality shall be available
   \param[in]       opc_FileExtensions List of valid file extensions (Necessary when oq_Active is true)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::SetDragAndDropActiveForFile(const bool oq_Active,
                                                      const QStringList * const opc_FileExtensions)
{
   this->setDragEnabled(oq_Active);
   if (oq_Active == true)
   {
      if (opc_FileExtensions != NULL)
      {
         this->mc_DragAndDropFileExtensions = *opc_FileExtensions;
      }
   }
   else
   {
      this->mc_DragAndDropFileExtensions.clear();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current path

   \return
   Current path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeLeFilePathBase::GetPath(void) const
{
   return this->mc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Insert selected path variable in front of string.

   \param[in]  orc_Variable   path variable
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::InsertVariable(const QString & orc_Variable)
{
   if (this->ms32_LastKnownCursorPos > 0)
   {
      mc_Path.insert(this->ms32_LastKnownCursorPos, orc_Variable);
   }
   else
   {
      // insert at the front if last known position is invalid
      mc_Path.insert(0, orc_Variable);
   }

   // update visualization
   this->SetPath(this->mc_Path, this->mc_RelativeTo);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown text based on the current set path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::UpdateText(void)
{
   this->setText(this->mc_Path);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus in event slot

   Here: show full path

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::focusInEvent(QFocusEvent * const opc_Event)
{
   this->setText(this->mc_Path);
   C_OgeLeToolTipBase::focusInEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden focus out event slot

   Here: show minimized path if necessary

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::focusOutEvent(QFocusEvent * const opc_Event)
{
   // remember cursor position for insert functionality
   this->ms32_LastKnownCursorPos = this->cursorPosition();

   // remember complete path  (before focus out to avoid confusion of editingFinished)
   this->SetPath(this->text(), this->mc_RelativeTo);

   C_OgeLeToolTipBase::focusOutEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden key press event slot

   Here: Set new path on enter or return key press.
   This is analogue to normal line edit but often not needed because of following focus loose.

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   if ((opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<stw_types::sintn>(Qt::Key_Return)))
   {
      this->SetPath(this->text(), this->mc_RelativeTo);
   }

   // after update of complete path
   C_OgeLeToolTipBase::keyPressEvent(opc_KeyEvent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden show event slot

   Here: Reload minimized path

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::showEvent(QShowEvent * const opc_Event)
{
   C_OgeLeToolTipBase::showEvent(opc_Event);
   this->m_UpdateMinimizing();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overridden resize event.

   Here: Recalculate minimized path on resize.

   \param[in]  opc_Event   event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::resizeEvent(QResizeEvent * const opc_Event)
{
   C_OgeLeToolTipBase::resizeEvent(opc_Event);
   this->m_UpdateMinimizing();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag enter event slot

   Here: Accept external files

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();
   bool q_ValidPath = false;

   if (pc_MimeData->hasUrls() == true)
   {
      const QList<QUrl> c_UrlList = pc_MimeData->urls();

      // Handle only one path
      if (c_UrlList.size() == 1)
      {
         // extract the local path of the file
         const QString c_Path = c_UrlList.at(0).toLocalFile();
         QFileInfo c_File;

         c_File.setFile(c_Path);
         if (c_File.exists() == true)
         {
            // File case
            if (c_File.isFile() == true)
            {
               // Check file extensions
               sintn sn_Counter;

               for (sn_Counter = 0; sn_Counter < this->mc_DragAndDropFileExtensions.size(); ++sn_Counter)
               {
                  if (c_File.completeSuffix().toLower() == this->mc_DragAndDropFileExtensions.at(sn_Counter).toLower())
                  {
                     q_ValidPath = true;
                     break;
                  }
               }
            }

            // Folder case
            if ((this->mq_DragAndDropFolderActive == true) &&
                (q_ValidPath == false) &&
                (c_File.isDir() == true))
            {
               q_ValidPath = true;
            }
         }
      }
   }

   if (q_ValidPath == true)
   {
      opc_Event->accept();
   }
   else
   {
      opc_Event->ignore();
   }

   C_OgeLeToolTipBase::dragEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drop event slot

   Here: Handle dropped files

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::dropEvent(QDropEvent * const opc_Event)
{
   const QMimeData * const pc_MimeData = opc_Event->mimeData();

   if (pc_MimeData->hasUrls() == true)
   {
      const QList<QUrl> c_UrlList = pc_MimeData->urls();

      // Handle only one path
      if (c_UrlList.size() == 1)
      {
         // extract the local path of the file
         const QString c_Path = c_UrlList.at(0).toLocalFile();

         // No further check necessary, was checked in dragEnterEvent already. Set the path as result
         this->SetPath(c_Path, this->mc_RelativeTo);
         opc_Event->accept();

         // Sending both signals for different kind of usages of the interface
         Q_EMIT (this->editingFinished());
         Q_EMIT (this->SigPathDropped());
      }
   }

   // Do not call the original drop event due to problems with a further adaption of the text of the line edit
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Calls replace logic for path variables.

   Needs to be wrapped because of different path variables in different projects.
   But line edit must know resolve-stuff to adapt tool tip on editing finished and focus out.
   Default implementation does nothing.

   \param[in]  orc_Path    path that probably contains variables

   \return
   Resolved path
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_OgeLeFilePathBase::m_ResolveVariables(const QString & orc_Path)
{
   return orc_Path;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update minimized path
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLeFilePathBase::m_UpdateMinimizing()
{
   if (this->hasFocus() == false)
   {
      const QFont c_Font = C_Uti::h_GetFontPixel(mc_STYLE_GUIDE_FONT_REGULAR_13);
      this->setText(C_Uti::h_MinimizePath(this->mc_Path, c_Font, this->width(), 12));
   }
   else
   {
      // In case of focused line edit, the original text must be updated in any case (for example after drag and drop)
      this->UpdateText();
   }
}
