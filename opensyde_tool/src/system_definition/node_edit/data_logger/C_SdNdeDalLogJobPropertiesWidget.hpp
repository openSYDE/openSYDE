//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for log job properties configuration
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBPROPERTIESWIDGET_H
#define C_SDNDEDALLOGJOBPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalLogJobPropertiesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobPropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobPropertiesWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobPropertiesWidget(void) override;

   void SetNodeDataLoggerJob(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex);
   void InitStaticNames(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigReloadDataLoggerDataElements(void);

private:
   Ui::C_SdNdeDalLogJobPropertiesWidget * mpc_Ui;
   enum E_LogFileFormat ///< log file types
   {
      eLFF_CSV,
      eLFF_PARQUET,
      eNUM_OF_FORMATS
   };

   enum E_LogTriggerMode ///< local log triggers
   {
      eLLT_ON_CHANGE,
      eLLT_INTERVAL,
      eNUM_OF_LOG_TRIGGERS
   };

   void m_GetSupportedLogFileFormats() const;
   void m_GetSupportedLocalData() const;
   void m_GetSupportedClientInterfaces() const;
   void m_OnNameEdited(void);
   void m_OnCommentEdited(void);
   void m_OnFileSizeChanged(void);
   void m_OnLogFileFormatChanged(const int32_t os32_NewIndex);
   void m_OnLocalDataChanged(const int32_t os32_NewIndex);
   void m_OnClientInterfaceChanged(const int32_t os32_NewIndex);

   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataLoggerJobIndex;
   int32_t ms32_LastUsedLogFileFormatIndex;
   int32_t ms32_LastUsedLocalDataIndex;
   int32_t ms32_LastUsedInterfaceNumberIndex;

   //Avoid call
   C_SdNdeDalLogJobPropertiesWidget(const C_SdNdeDalLogJobPropertiesWidget &);
   C_SdNdeDalLogJobPropertiesWidget & operator =(const C_SdNdeDalLogJobPropertiesWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
