//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for configuring file logging.
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSLOGGINGWIDGET_H
#define C_CAMMOSLOGGINGWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosLoggingWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosLoggingWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamMosLoggingWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMosLoggingWidget(void);

   void LoadUserSettings(void) const;
   void PrepareForExpanded(const bool oq_Expand) const;
   void OnCommunicationStarted(const bool oq_Online);
   void OnSigLogFileAddResult(const stw_types::sint32 os32_Result);
   void OnSigSavedAsNew(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigAddLogFileAsc(const QString & orc_FilePath);
   void SigAddLogFileBlf(const QString & orc_FilePath);
   void SigRemoveAllLogFiles(void);
   void SigHide(void);

private:
   Ui::C_CamMosLoggingWidget * mpc_Ui;

   bool mq_Online;

   //Avoid call
   C_CamMosLoggingWidget(const C_CamMosLoggingWidget &);
   C_CamMosLoggingWidget & operator =(const C_CamMosLoggingWidget &);

   void m_LoadConfig(void) const;
   void m_LoadFolderConfig(void) const;
   void m_OnExpand(const bool oq_Expand) const;
   void m_OnToggled(const bool oq_Enabled);
   void m_OnFolderEdited(void);
   void m_OnFileNameEdited(void);
   void m_OnOverwriteModeSelected(const stw_types::sint32 os32_Index) const;
   void m_OnFormatSelected(const stw_types::sint32 os32_Index) const;
   void m_OnBrowse(void);
   void m_OnDroppedPath(void);
   void m_SetLoggingPath(const QString & orc_New);
   void m_InsertPathVar(const QString & orc_Variable);

   void m_DecideStartStop(void);
   void m_CheckAndStartLogging(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
